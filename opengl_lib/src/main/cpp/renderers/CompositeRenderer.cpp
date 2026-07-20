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
#include "../util/LogUtil.h"

CompositeRenderer::CompositeRenderer() 
    : m_ScreenWidth(0),
      m_ScreenHeight(0),
      m_IsInitialized(false) {}

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
    
    RainLayer *rainLayer = nullptr;
    auto rainIt = m_LayerMap.find(LAYER_TYPE_RAIN);
    if (rainIt != m_LayerMap.end()) {
        rainLayer = static_cast<RainLayer *>(rainIt->second);
    }
    
    if (rainLayer != nullptr && rainLayer->IsEnabled() && m_FBO == 0) {
        if (!InitFBO(screenW, screenH)) {
            rainLayer = nullptr;
        }
    }
    
    LightningLayer *lightningLayer = nullptr;
    auto lightningIt = m_LayerMap.find(LAYER_TYPE_LIGHTNING);
    if (lightningIt != m_LayerMap.end()) {
        lightningLayer = static_cast<LightningLayer *>(lightningIt->second);
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
        
        for (auto &layer : m_Layers) {
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
        for (auto &layer : m_Layers) {
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
    for (auto &layer : m_Layers) {
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
            return typeA < typeB;
        });
}

void CompositeRenderer::ConfigureLayers(const char *skycon) {
    if (!skycon) return;
    
    ClearLayers();
    
    std::string skyconStr(skycon);
    bool isNight = skyconStr.find("NIGHT") != std::string::npos;
    
    if (skyconStr == "CLEAR_DAY") {
        ConfigureClear(false);
    } else if (skyconStr == "CLEAR_NIGHT") {
        ConfigureClear(true);
    } else if (skyconStr == "PARTLY_CLOUDY_DAY") {
        ConfigurePartlyCloudy(false);
    } else if (skyconStr == "PARTLY_CLOUDY_NIGHT") {
        ConfigurePartlyCloudy(true);
    } else if (skyconStr == "CLOUDY") {
        ConfigureCloudy(isNight);
    } else if (skyconStr == "LIGHT_HAZE") {
        ConfigureHaze(0, isNight);
    } else if (skyconStr == "MODERATE_HAZE") {
        ConfigureHaze(1, isNight);
    } else if (skyconStr == "HEAVY_HAZE") {
        ConfigureHaze(2, isNight);
    } else if (skyconStr == "LIGHT_RAIN") {
        ConfigureRain(0, isNight);
    } else if (skyconStr == "MODERATE_RAIN") {
        ConfigureRain(1, isNight);
    } else if (skyconStr == "HEAVY_RAIN") {
        ConfigureRain(2, isNight);
    } else if (skyconStr == "STORM_RAIN") {
        ConfigureRain(3, isNight);
    } else if (skyconStr == "FOG") {
        ConfigureFog(isNight);
    } else if (skyconStr == "LIGHT_SNOW") {
        ConfigureSnow(0, isNight);
    } else if (skyconStr == "MODERATE_SNOW") {
        ConfigureSnow(1, isNight);
    } else if (skyconStr == "HEAVY_SNOW") {
        ConfigureSnow(2, isNight);
    } else if (skyconStr == "STORM_SNOW") {
        ConfigureSnow(3, isNight);
    } else if (skyconStr == "DUST") {
        ConfigureDust(isNight);
    } else if (skyconStr == "SAND") {
        ConfigureSand(isNight);
    } else if (skyconStr == "WIND") {
        ConfigureWind(isNight);
    }
    
    SortLayers();
    m_IsInitialized = false;
    
    LOGCATI("CompositeRenderer::ConfigureLayers skycon=%s, layerCount=%d", skycon, GetLayerCount());
}

void CompositeRenderer::ConfigureClear(bool isNight) {
    SkyBackgroundLayer *skyLayer = new SkyBackgroundLayer();
    skyLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.3f, 0.6f, 0.95f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.4f, 0.7f, 1.0f);
        skyLayer->SetParamFloat(PARAM_SUN_INTENSITY, 1.2f);
    } else {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.02f, 0.04f, 0.10f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.05f, 0.08f, 0.18f);
        skyLayer->SetParamFloat(PARAM_MOON_INTENSITY, 0.9f);
    }
    AddLayer(skyLayer);
    
    if (isNight) {
        StarLayer *starLayer = new StarLayer();
        AddLayer(starLayer);
    }
    
    CloudLayer *cloudLayer = new CloudLayer();
    cloudLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer->SetParamFloat(PARAM_CLOUD_COVERAGE, isNight ? 0.4f : 0.12f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.7f : 0.4f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.55f : 0.8f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_SPEED, isNight ? 0.05f : 0.15f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_ALPHA, isNight ? 25.0f : 20.0f);
    AddLayer(cloudLayer);
}

void CompositeRenderer::ConfigurePartlyCloudy(bool isNight) {
    SkyBackgroundLayer *skyLayer = new SkyBackgroundLayer();
    skyLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.35f, 0.6f, 0.9f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.15f, 0.35f, 0.55f);
        skyLayer->SetParamFloat(PARAM_SUN_INTENSITY, 0.7f);
    } else {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.08f, 0.12f, 0.2f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.12f, 0.18f, 0.3f);
        skyLayer->SetParamFloat(PARAM_MOON_INTENSITY, 0.6f);
    }
    AddLayer(skyLayer);
    
    if (isNight) {
        StarLayer *starLayer = new StarLayer();
        starLayer->SetParamFloat(PARAM_STAR_COUNT, 150.0f);
        AddLayer(starLayer);
    }
    
    CloudLayer *cloudLayer = new CloudLayer();
    cloudLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer->SetParamFloat(PARAM_CLOUD_COVERAGE, 0.5f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.5f : 0.4f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_SPEED, isNight ? 0.25f : 0.3f);
    AddLayer(cloudLayer);
}

void CompositeRenderer::ConfigureCloudy(bool isNight) {
    SkyBackgroundLayer *skyLayer = new SkyBackgroundLayer();
    skyLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.3f, 0.35f, 0.4f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.25f, 0.3f, 0.35f);
        skyLayer->SetParamFloat(PARAM_SUN_INTENSITY, 0.2f);
    } else {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.08f, 0.1f, 0.15f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.1f, 0.12f, 0.18f);
        skyLayer->SetParamFloat(PARAM_MOON_INTENSITY, 0.1f);
    }
    AddLayer(skyLayer);
    
    if (isNight) {
        StarLayer *starLayer = new StarLayer();
        starLayer->SetParamFloat(PARAM_STAR_COUNT, 30.0f);
        AddLayer(starLayer);
    }
    
    CloudLayer *cloudLayer = new CloudLayer();
    cloudLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer->SetParamFloat(PARAM_CLOUD_COVERAGE, 0.9f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.85f : 0.7f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.1f : 0.2f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_SPEED, 0.15f);
    AddLayer(cloudLayer);
}

void CompositeRenderer::ConfigureHaze(int level, bool isNight) {
    float density[] = {0.3f, 0.5f, 0.8f};
    float visibility[] = {0.7f, 0.5f, 0.2f};
    float r[] = {0.6f, 0.5f, 0.4f};
    float g[] = {0.55f, 0.45f, 0.35f};
    float b[] = {0.45f, 0.35f, 0.25f};
    
    SkyBackgroundLayer *skyLayer = new SkyBackgroundLayer();
    skyLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, r[level], g[level], b[level]);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, r[level] * 0.8f, g[level] * 0.8f, b[level] * 0.8f);
        skyLayer->SetParamFloat(PARAM_SUN_INTENSITY, 0.3f);
    } else {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, r[level] * 0.2f, g[level] * 0.2f, b[level] * 0.3f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, r[level] * 0.3f, g[level] * 0.3f, b[level] * 0.4f);
        skyLayer->SetParamFloat(PARAM_MOON_INTENSITY, 0.2f);
    }
    AddLayer(skyLayer);
    
    if (isNight) {
        StarLayer *starLayer = new StarLayer();
        starLayer->SetParamFloat(PARAM_STAR_COUNT, 60.0f);
        AddLayer(starLayer);
    }
    
    CloudLayer *cloudLayer = new CloudLayer();
    cloudLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer->SetParamFloat(PARAM_CLOUD_COVERAGE, 0.3f + level * 0.1f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.7f + level * 0.1f : 0.5f + level * 0.1f);
    AddLayer(cloudLayer);
    
    ParticleLayer *particleLayer = new ParticleLayer();
    particleLayer->SetParamInt(PARAM_PARTICLE_TYPE, 0);
    particleLayer->SetParamFloat(PARAM_PARTICLE_DENSITY, density[level]);
    if (!isNight) {
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_R, r[level]);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_G, g[level]);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_B, b[level]);
    } else {
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_R, r[level] * 0.3f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_G, g[level] * 0.3f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_B, b[level] * 0.4f);
    }
    particleLayer->SetParamFloat(PARAM_PARTICLE_VISIBILITY, visibility[level]);
    AddLayer(particleLayer);
}

void CompositeRenderer::ConfigureRain(int level, bool isNight) {
    float coverage[] = {0.6f, 0.8f, 0.9f, 1.0f};
    float darkness[] = {0.30f, 0.40f, 0.50f, 0.65f};
    float lightness[] = {0.75f, 0.65f, 0.50f, 0.35f};
    float speed[] = {0.08f, 0.06f, 0.04f, 0.02f};
    float scale[] = {0.90f, 0.80f, 0.70f, 0.55f};
    float alpha[] = {14.0f, 18.0f, 22.0f, 28.0f};
    float intensity[] = {0.3f, 0.7f, 1.3f, 1.8f};
    float rainSpeed[] = {0.5f, 0.7f, 0.9f, 1.0f};
    float lightningInterval[] = {0.0f, 0.0f, 0.0f, 3.0f};
    float r[] = {0.48f, 0.38f, 0.28f, 0.20f};
    float g[] = {0.52f, 0.44f, 0.33f, 0.25f};
    float b[] = {0.68f, 0.58f, 0.48f, 0.38f};
    float nr[] = {0.28f, 0.24f, 0.20f, 0.17f};
    float ng[] = {0.32f, 0.28f, 0.24f, 0.20f};
    float nb[] = {0.48f, 0.42f, 0.36f, 0.30f};
    
    LOGCATI("CompositeRenderer::ConfigureRain: level=%d, isNight=%d, coverage=%.2f, darkness=%.2f, intensity=%.2f, rainSpeed=%.2f", 
            level, isNight, coverage[level], darkness[level], intensity[level], rainSpeed[level]);
    LOGCATI("CompositeRenderer::ConfigureRain: skyColor=(%.2f,%.2f,%.2f)", r[level], g[level], b[level]);
    
    SkyBackgroundLayer *skyLayer = new SkyBackgroundLayer();
    skyLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, r[level], g[level], b[level]);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, r[level] * 0.9f, g[level] * 0.9f, b[level] * 0.9f);
        skyLayer->SetParamFloat(PARAM_SUN_INTENSITY, 0.1f);
        skyLayer->SetParamInt(PARAM_SUN_VISIBLE, 0);
        skyLayer->SetParamInt(PARAM_SKY_MODE, 0);
    } else {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, nr[level], ng[level], nb[level]);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, nr[level] * 1.2f, ng[level] * 1.2f, nb[level] * 1.2f);
        skyLayer->SetParamFloat(PARAM_MOON_INTENSITY, 0.05f);
        skyLayer->SetParamInt(PARAM_SKY_MODE, 1);
    }
    AddLayer(skyLayer);
    
    if (isNight) {
        StarLayer *starLayer = new StarLayer();
        starLayer->SetParamFloat(PARAM_STAR_COUNT, 20.0f);
        AddLayer(starLayer);
    }
    
    CloudLayer *cloudLayer = new CloudLayer();
    cloudLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer->SetParamFloat(PARAM_CLOUD_COVERAGE, coverage[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? darkness[level] * 1.1f : darkness[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? lightness[level] * 0.8f : lightness[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_SPEED, speed[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_SCALE, scale[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_ALPHA, alpha[level]);
    cloudLayer->SetParamInt(PARAM_CLOUD_MODE, 1);
    AddLayer(cloudLayer);
    
    RainLayer *rainLayer = new RainLayer();
    float rainIntensity = intensity[level];
    if (level == 3) rainIntensity = 2.2f;
    rainLayer->SetParamFloat(PARAM_RAIN_INTENSITY, rainIntensity);
    rainLayer->SetParamFloat(PARAM_RAIN_WIND_SPEED, level == 3 ? 0.8f : 0.2f + (float)level * 0.15f);
    rainLayer->SetParamFloat(PARAM_RAIN_REFRACTION, 0.3f + (float)level * 0.25f);
    AddLayer(rainLayer);
    
    ParticleLayer *particleLayer = new ParticleLayer();
    particleLayer->SetParamInt(PARAM_PARTICLE_TYPE, 1);
    particleLayer->SetParamFloat(PARAM_PARTICLE_DENSITY, level == 3 ? 0.6f : 0.3f);
    particleLayer->SetParamFloat(PARAM_PARTICLE_VISIBILITY, level == 3 ? 0.3f : 0.5f);
    particleLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, r[level] * 0.8f, g[level] * 0.8f, b[level] * 0.8f);
    AddLayer(particleLayer);
    
    if (level == 3) {
        LightningLayer *lightningLayer = new LightningLayer();
        lightningLayer->SetParamInt(PARAM_LIGHTNING_ENABLED, 1);
        lightningLayer->SetParamFloat(PARAM_LIGHTNING_INTERVAL, lightningInterval[level]);
        lightningLayer->SetParamInt(PARAM_LIGHTNING_IS_NIGHT, isNight ? 1 : 0);
        AddLayer(lightningLayer);
        
        AmbientOverlayLayer *ambientLayer = new AmbientOverlayLayer();
        ambientLayer->SetParamInt(PARAM_LIGHTNING_IS_NIGHT, isNight ? 1 : 0);
        AddLayer(ambientLayer);
    }
}

void CompositeRenderer::ConfigureFog(bool isNight) {
    SkyBackgroundLayer *skyLayer = new SkyBackgroundLayer();
    skyLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.8f, 0.8f, 0.8f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.7f, 0.7f, 0.7f);
        skyLayer->SetParamFloat(PARAM_SUN_INTENSITY, 0.1f);
    } else {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.2f, 0.22f, 0.28f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.25f, 0.27f, 0.33f);
        skyLayer->SetParamFloat(PARAM_MOON_INTENSITY, 0.1f);
    }
    AddLayer(skyLayer);
    
    if (isNight) {
        StarLayer *starLayer = new StarLayer();
        starLayer->SetParamFloat(PARAM_STAR_COUNT, 40.0f);
        AddLayer(starLayer);
    }
    
    ParticleLayer *particleLayer = new ParticleLayer();
    particleLayer->SetParamInt(PARAM_PARTICLE_TYPE, 1);
    particleLayer->SetParamFloat(PARAM_PARTICLE_DENSITY, 0.7f);
    if (!isNight) {
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.85f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.85f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.85f);
    } else {
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.3f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.32f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.4f);
    }
    particleLayer->SetParamFloat(PARAM_PARTICLE_VISIBILITY, 0.15f);
    AddLayer(particleLayer);
}

void CompositeRenderer::ConfigureSnow(int level, bool isNight) {
    float coverage[] = {0.6f, 0.8f, 0.95f, 1.0f};
    float darkness[] = {0.35f, 0.55f, 0.75f, 0.90f};
    float lightness[] = {0.70f, 0.55f, 0.40f, 0.25f};
    float speed[] = {0.002f, 0.003f, 0.004f, 0.005f};
    float scale[] = {0.90f, 0.75f, 0.60f, 0.45f};
    float alpha[] = {14.0f, 20.0f, 26.0f, 32.0f};
    float intensity[] = {0.6f, 0.7f, 0.8f, 0.95f};
    float snowSpeed[] = {0.3f, 0.4f, 0.65f, 0.9f};
    float snowSize[] = {3.5f, 5.0f, 2.0f, 2.2f};
    float windForce[] = {0.0f, 0.05f, 0.4f, 0.8f};
    int particleCounts[] = {280, 525, 735, 1225};
    float snowShape[] = {0.0f, 0.0f, 1.0f, 1.0f};
    float fogDensity[] = {0.0f, 0.0f, 0.2f, 0.4f};
    float visibility[] = {0.0f, 0.0f, 0.6f, 0.3f};
    float r[] = {0.50f, 0.42f, 0.35f, 0.22f};
    float g[] = {0.58f, 0.46f, 0.38f, 0.24f};
    float b[] = {0.70f, 0.52f, 0.42f, 0.28f};
    float nr[] = {0.25f, 0.20f, 0.15f, 0.10f};
    float ng[] = {0.28f, 0.23f, 0.18f, 0.12f};
    float nb[] = {0.38f, 0.30f, 0.22f, 0.15f};
    float sunIntensity[] = {0.15f, 0.05f, 0.0f, 0.0f};
    int sunVisible[] = {1, 1, 0, 0};
    
    SkyBackgroundLayer *skyLayer = new SkyBackgroundLayer();
    skyLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, r[level], g[level], b[level]);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, r[level] * 0.92f, g[level] * 0.92f, b[level] * 0.92f);
        skyLayer->SetParamFloat(PARAM_SUN_INTENSITY, sunIntensity[level]);
        skyLayer->SetParamInt(PARAM_SUN_VISIBLE, sunVisible[level]);
        skyLayer->SetParamInt(PARAM_SKY_MODE, 2);
    } else {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, nr[level], ng[level], nb[level]);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, nr[level] * 1.05f, ng[level] * 1.05f, nb[level] * 1.05f);
        skyLayer->SetParamFloat(PARAM_MOON_INTENSITY, 0.2f);
        skyLayer->SetParamInt(PARAM_SKY_MODE, 2);
    }
    AddLayer(skyLayer);
    
    if (isNight) {
        StarLayer *starLayer = new StarLayer();
        starLayer->SetParamFloat(PARAM_STAR_COUNT, 50.0f);
        AddLayer(starLayer);
    }
    
    CloudLayer *cloudLayer = new CloudLayer();
    cloudLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer->SetParamFloat(PARAM_CLOUD_COVERAGE, coverage[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? darkness[level] * 1.1f : darkness[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? lightness[level] * 0.8f : lightness[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_SPEED, speed[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_SCALE, scale[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_ALPHA, alpha[level]);
    cloudLayer->SetParamInt(PARAM_CLOUD_MODE, 2);
    AddLayer(cloudLayer);
    
    SnowLayer *snowLayer = new SnowLayer();
    snowLayer->SetParamInt(PARAM_SNOW_PARTICLE_COUNT, particleCounts[level]);
    snowLayer->SetParamFloat(PARAM_SNOW_INTENSITY, intensity[level]);
    snowLayer->SetParamFloat(PARAM_SNOW_SPEED, snowSpeed[level]);
    snowLayer->SetParamFloat(PARAM_SNOW_SIZE, snowSize[level]);
    snowLayer->SetParamFloat(PARAM_SNOW_WIND, windForce[level]);
    snowLayer->SetParamFloat(PARAM_SNOW_SHAPE, snowShape[level]);
    AddLayer(snowLayer);
    
    if (level >= 2) {
        ParticleLayer *particleLayer = new ParticleLayer();
        particleLayer->SetParamInt(PARAM_PARTICLE_TYPE, 1);
        particleLayer->SetParamFloat(PARAM_PARTICLE_DENSITY, fogDensity[level]);
        if (!isNight) {
            particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.8f);
            particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.8f);
            particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.85f);
        } else {
            particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.25f);
            particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.27f);
            particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.35f);
        }
        particleLayer->SetParamFloat(PARAM_PARTICLE_VISIBILITY, visibility[level]);
        AddLayer(particleLayer);
    }
    
    if (level == 3) {
        WindLayer *windLayer = new WindLayer();
        windLayer->SetParamInt(PARAM_WIND_LINES_ENABLED, 1);
        windLayer->SetParamFloat(PARAM_WIND_STRENGTH, 0.8f);
        AddLayer(windLayer);
    }
}

void CompositeRenderer::ConfigureDust(bool isNight) {
    SkyBackgroundLayer *skyLayer = new SkyBackgroundLayer();
    skyLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.6f, 0.5f, 0.35f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.5f, 0.4f, 0.25f);
        skyLayer->SetParamFloat(PARAM_SUN_INTENSITY, 0.3f);
    } else {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.15f, 0.12f, 0.08f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.2f, 0.16f, 0.1f);
        skyLayer->SetParamFloat(PARAM_MOON_INTENSITY, 0.15f);
    }
    AddLayer(skyLayer);
    
    if (isNight) {
        StarLayer *starLayer = new StarLayer();
        starLayer->SetParamFloat(PARAM_STAR_COUNT, 50.0f);
        AddLayer(starLayer);
    }
    
    ParticleLayer *particleLayer = new ParticleLayer();
    particleLayer->SetParamInt(PARAM_PARTICLE_TYPE, 2);
    particleLayer->SetParamFloat(PARAM_PARTICLE_DENSITY, 0.4f);
    if (!isNight) {
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.6f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.5f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.35f);
    } else {
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.2f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.16f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.1f);
    }
    particleLayer->SetParamFloat(PARAM_PARTICLE_VISIBILITY, 0.5f);
    AddLayer(particleLayer);
}

void CompositeRenderer::ConfigureSand(bool isNight) {
    SkyBackgroundLayer *skyLayer = new SkyBackgroundLayer();
    skyLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.7f, 0.5f, 0.25f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.6f, 0.4f, 0.2f);
        skyLayer->SetParamFloat(PARAM_SUN_INTENSITY, 0.2f);
    } else {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.18f, 0.12f, 0.06f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.22f, 0.15f, 0.08f);
        skyLayer->SetParamFloat(PARAM_MOON_INTENSITY, 0.1f);
    }
    AddLayer(skyLayer);
    
    if (isNight) {
        StarLayer *starLayer = new StarLayer();
        starLayer->SetParamFloat(PARAM_STAR_COUNT, 35.0f);
        AddLayer(starLayer);
    }
    
    ParticleLayer *particleLayer = new ParticleLayer();
    particleLayer->SetParamInt(PARAM_PARTICLE_TYPE, 3);
    particleLayer->SetParamFloat(PARAM_PARTICLE_DENSITY, 0.7f);
    if (!isNight) {
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.7f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.5f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.25f);
    } else {
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.22f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.15f);
        particleLayer->SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.08f);
    }
    particleLayer->SetParamFloat(PARAM_PARTICLE_VISIBILITY, 0.3f);
    AddLayer(particleLayer);
    
    WindLayer *windLayer = new WindLayer();
    windLayer->SetParamInt(PARAM_WIND_LINES_ENABLED, 1);
    windLayer->SetParamFloat(PARAM_WIND_STRENGTH, 0.6f);
    AddLayer(windLayer);
}

void CompositeRenderer::ConfigureWind(bool isNight) {
    SkyBackgroundLayer *skyLayer = new SkyBackgroundLayer();
    skyLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.4f, 0.65f, 0.9f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.2f, 0.45f, 0.7f);
        skyLayer->SetParamFloat(PARAM_SUN_INTENSITY, 0.5f);
    } else {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, 0.08f, 0.12f, 0.22f);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.12f, 0.18f, 0.32f);
        skyLayer->SetParamFloat(PARAM_MOON_INTENSITY, 0.4f);
    }
    AddLayer(skyLayer);
    
    if (isNight) {
        StarLayer *starLayer = new StarLayer();
        starLayer->SetParamFloat(PARAM_STAR_COUNT, 180.0f);
        AddLayer(starLayer);
    }
    
    CloudLayer *cloudLayer = new CloudLayer();
    cloudLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer->SetParamFloat(PARAM_CLOUD_COVERAGE, 0.4f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.5f : 0.4f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.2f : 0.5f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_SPEED, 0.9f);
    AddLayer(cloudLayer);
    
    WindLayer *windLayer = new WindLayer();
    windLayer->SetParamInt(PARAM_WIND_LINES_ENABLED, 1);
    windLayer->SetParamFloat(PARAM_WIND_STRENGTH, 0.7f);
    AddLayer(windLayer);
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