#include "CompositeRenderer.h"
#include "layers/SkyBackgroundLayer.h"
#include "layers/StarLayer.h"
#include "layers/CloudLayer.h"
#include "layers/SnowLayer.h"
#include "layers/RainLayer.h"
#include "layers/ParticleLayer.h"
#include "layers/LightningLayer.h"
#include "layers/WindLayer.h"
#include "layers/AmbientOverlayLayer.h"
#include "config/ProfileBuilderRegistry.h"
#include "../util/LogUtil.h"

CompositeRenderer::CompositeRenderer()
        : m_ScreenWidth(0),
          m_ScreenHeight(0),
          m_IsInitialized(false) {
}

CompositeRenderer::~CompositeRenderer() {
    ClearLayers();
}

bool CompositeRenderer::Init() {
    if (m_IsInitialized) {
        LOGCATI("CompositeRenderer::Init: already initialized, skipping");
        return true;
    }

    LOGCATI("CompositeRenderer::Init: initializing %zu layers", m_Layers.size());

    bool allSuccess = true;
    for (size_t i = 0; i < m_Layers.size(); ++i) {
        LOGCATI("CompositeRenderer::Init: initializing layer %zu, type=%d", i, m_Layers[i]->GetLayerType());
        bool success = m_Layers[i]->Init();
        if (!success) {
            LOGCATE("CompositeRenderer::Init: layer %zu initialization failed", i);
            allSuccess = false;
        }
        LOGCATI("CompositeRenderer::Init: layer %zu initialized", i);
    }

    m_IsInitialized = true;
    LOGCATI("CompositeRenderer::Init: all %zu layers initialized, success=%d", m_Layers.size(), allSuccess);
    return allSuccess;
}

void CompositeRenderer::Draw(int screenW, int screenH) {
    if (!m_IsInitialized && !m_Layers.empty()) {
        Init();
    }
    if (!m_IsInitialized) {
        return;
    }

    m_ScreenWidth = screenW;
    m_ScreenHeight = screenH;

    static int drawFrameCount = 0;
    drawFrameCount++;

    bool requiresFBO = m_RenderFlags.requiresFBO;
    RainLayer *rainLayer = nullptr;
    if (requiresFBO) {
        auto rainIt = m_LayerMap.find(LAYER_TYPE_RAIN);
        if (rainIt != m_LayerMap.end()) {
            rainLayer = static_cast<RainLayer *>(rainIt->second);
        }
    }

    if (rainLayer != nullptr && rainLayer->IsEnabled() && m_FBO == 0) {
        if (!InitFBO(screenW, screenH)) {
            rainLayer = nullptr;
        }
    }

    LightningLayer *lightningLayer = nullptr;
    if (m_RenderFlags.requiresLightningLink) {
        auto lightningIt = m_LayerMap.find(LAYER_TYPE_LIGHTNING);
        if (lightningIt != m_LayerMap.end()) {
            lightningLayer = static_cast<LightningLayer *>(lightningIt->second);
        }
    }

    AmbientOverlayLayer *ambientLayer = nullptr;
    auto ambientIt = m_LayerMap.find(LAYER_TYPE_AMBIENT_OVERLAY);
    if (ambientIt != m_LayerMap.end()) {
        ambientLayer = static_cast<AmbientOverlayLayer *>(ambientIt->second);
    }

    if (rainLayer != nullptr && m_FBO != 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glViewport(0, 0, screenW, screenH);
        glClearColor(0.45f, 0.47f, 0.50f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        for (auto &layer: m_Layers) {
            LayerType type = layer->GetLayerType();
            if (layer->IsEnabled() &&
                    type != LAYER_TYPE_RAIN &&
                    type != LAYER_TYPE_LIGHTNING &&
                    type != LAYER_TYPE_AMBIENT_OVERLAY) {
                layer->Draw(screenW, screenH);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenW, screenH);

        rainLayer->SetBackgroundTexture(m_FBOTexture);
        rainLayer->Draw(screenW, screenH);
    } else {
        for (auto &layer: m_Layers) {
            LayerType type = layer->GetLayerType();
            if (layer->IsEnabled() &&
                    type != LAYER_TYPE_LIGHTNING &&
                    type != LAYER_TYPE_AMBIENT_OVERLAY) {
                layer->Draw(screenW, screenH);
            }
        }
    }

    if (lightningLayer != nullptr && lightningLayer->IsEnabled()) {
        lightningLayer->Draw(screenW, screenH);

        float flashIntensity = lightningLayer->GetFlashIntensity();
        if (ambientLayer != nullptr && ambientLayer->IsEnabled()) {
            ambientLayer->SetFlashIntensity(flashIntensity);
            ambientLayer->Draw(screenW, screenH);
        }
    } else if (ambientLayer != nullptr && ambientLayer->IsEnabled()) {
        ambientLayer->Draw(screenW, screenH);
    }
}

void CompositeRenderer::Destroy() {
    ClearLayers();
    DestroyFBO();
    m_IsInitialized = false;
}

void CompositeRenderer::AddLayer(GLLayerBase *layer) {
    if (!layer) return;

    LayerType type = layer->GetLayerType();

    if (m_LayerMap.find(type) != m_LayerMap.end()) {
        RemoveLayer(type);
    }

    m_Layers.push_back(layer);
    m_LayerMap[type] = layer;

    SortLayers();

    if (m_IsInitialized) {
        layer->Init();
    }
}

void CompositeRenderer::RemoveLayer(LayerType layerType) {
    auto it = m_LayerMap.find(layerType);
    if (it != m_LayerMap.end()) {
        GLLayerBase *layer = it->second;

        auto vecIt = std::find(m_Layers.begin(), m_Layers.end(), layer);
        if (vecIt != m_Layers.end()) {
            m_Layers.erase(vecIt);
        }

        layer->Destroy();
        delete layer;
        m_LayerMap.erase(it);
    }
}

void CompositeRenderer::ClearLayers() {
    for (auto &layer: m_Layers) {
        layer->Destroy();
        delete layer;
    }
    m_Layers.clear();
    m_LayerMap.clear();
}

GLLayerBase *CompositeRenderer::GetLayer(LayerType layerType) {
    auto it = m_LayerMap.find(layerType);
    return (it != m_LayerMap.end()) ? it->second : nullptr;
}

bool CompositeRenderer::HasLayer(LayerType layerType) {
    return m_LayerMap.find(layerType) != m_LayerMap.end();
}

int CompositeRenderer::GetLayerCount() const {
    return m_Layers.size();
}

void CompositeRenderer::SortLayers() {
    std::sort(m_Layers.begin(), m_Layers.end(),
            [](const GLLayerBase *a, const GLLayerBase *b) {
                LayerType typeA = a->GetLayerType();
                LayerType typeB = b->GetLayerType();
                if (typeA == LAYER_TYPE_RAIN) return false;
                if (typeB == LAYER_TYPE_RAIN) return true;
                if (typeA == LAYER_TYPE_AMBIENT_OVERLAY) return false;
                if (typeB == LAYER_TYPE_AMBIENT_OVERLAY) return true;
                if (typeA == LAYER_TYPE_WIND && typeB == LAYER_TYPE_SNOW) return true;
                if (typeB == LAYER_TYPE_WIND && typeA == LAYER_TYPE_SNOW) return false;
                return typeA < typeB;
            });
}

void CompositeRenderer::ConfigureLayers(const char *skycon) {
    if (!skycon) return;

    std::string skyconStr(skycon);
    bool isNight = skyconStr.find("NIGHT") != std::string::npos;

    WeatherProfile profile = ProfileBuilderRegistry::GetInstance()->BuildProfile(skyconStr, isNight);
    ConfigureLayers(profile);

    LOGCATI("CompositeRenderer::ConfigureLayers skycon=%s, layerCount=%d", skycon, GetLayerCount());
}

void CompositeRenderer::ConfigureLayers(const WeatherProfile& profile) {
    ClearLayers();

    m_RenderFlags = profile.flags;

    for (const auto& layerConfig : profile.layers) {
        GLLayerBase* layer = CreateLayer(layerConfig);
        if (layer != nullptr) {
            ApplyLayerParams(layer, layerConfig);
            AddLayer(layer);
        }
    }

    SortLayers();
    m_IsInitialized = false;
}

GLLayerBase* CompositeRenderer::CreateLayer(const LayerConfig& config) {
    GLLayerBase* layer = nullptr;

    switch (config.layerType) {
        case LAYER_TYPE_SKY_BACKGROUND:
            layer = new SkyBackgroundLayer();
            break;
        case LAYER_TYPE_STAR:
            layer = new StarLayer();
            break;
        case LAYER_TYPE_CLOUD:
            layer = new CloudLayer();
            break;
        case LAYER_TYPE_SNOW:
            layer = new SnowLayer();
            break;
        case LAYER_TYPE_PARTICLE:
            layer = new ParticleLayer();
            break;
        case LAYER_TYPE_RAIN:
            layer = new RainLayer();
            break;
        case LAYER_TYPE_LIGHTNING:
            layer = new LightningLayer();
            break;
        case LAYER_TYPE_WIND:
            layer = new WindLayer();
            break;
        case LAYER_TYPE_AMBIENT_OVERLAY:
            layer = new AmbientOverlayLayer();
            break;
        default:
            LOGCATE("CompositeRenderer::CreateLayer: unknown layer type %d", config.layerType);
            break;
    }

    return layer;
}

void CompositeRenderer::ApplyLayerParams(GLLayerBase* layer, const LayerConfig& config) {
    if (layer == nullptr) return;

    for (const auto& paramPair : config.params) {
        LayerParamType paramType = paramPair.first;
        const LayerParamValue& value = paramPair.second;

        switch (value.type) {
            case PARAM_VALUE_INT:
                layer->SetParamInt(paramType, value.intValue);
                break;
            case PARAM_VALUE_FLOAT:
                layer->SetParamFloat(paramType, value.floatValue);
                break;
            case PARAM_VALUE_VEC3:
                layer->SetParamVec3(paramType, value.vec3Value.x, value.vec3Value.y, value.vec3Value.z);
                break;
            default:
                LOGCATE("CompositeRenderer::ApplyLayerParams: unknown param value type %d", value.type);
                break;
        }
    }
}

bool CompositeRenderer::InitFBO(int width, int height) {
    LOGCATI("CompositeRenderer::InitFBO: width=%d, height=%d", width, height);

    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    glGenTextures(1, &m_FBOTexture);
    glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBOTexture, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        LOGCATE("CompositeRenderer::InitFBO: FBO creation failed, status=%d", status);
        glDeleteTextures(1, &m_FBOTexture);
        glDeleteFramebuffers(1, &m_FBO);
        m_FBO = 0;
        m_FBOTexture = 0;
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    LOGCATI("CompositeRenderer::InitFBO: success, FBO=%d, texture=%d", m_FBO, m_FBOTexture);
    return true;
}

void CompositeRenderer::DestroyFBO() {
    if (m_FBOTexture != 0) {
        glDeleteTextures(1, &m_FBOTexture);
        m_FBOTexture = 0;
    }
    if (m_FBO != 0) {
        glDeleteFramebuffers(1, &m_FBO);
        m_FBO = 0;
    }
    LOGCATI("CompositeRenderer::DestroyFBO: completed");
}
