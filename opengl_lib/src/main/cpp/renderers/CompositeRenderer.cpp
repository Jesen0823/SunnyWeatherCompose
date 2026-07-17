#include "CompositeRenderer.h"
#include "layers/SkyBackgroundLayer.h"
#include "layers/StarLayer.h"
#include "layers/CloudLayer.h"
#include "layers/PrecipitationLayer.h"
#include "layers/ParticleLayer.h"
#include "layers/EffectLayer.h"
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
        LOGCATI("CompositeRenderer::Draw: not initialized yet, calling Init()");
        Init();
        LOGCATI("CompositeRenderer::Draw: Init() completed");
    }
    if (!m_IsInitialized) {
        LOGCATE("CompositeRenderer::Draw: not initialized and no layers, skipping draw");
        return;
    }
    
    m_ScreenWidth = screenW;
    m_ScreenHeight = screenH;
    
    LOGCATI("CompositeRenderer::Draw: drawing %zu layers, screen=%dx%d", m_Layers.size(), screenW, screenH);
    
    for (size_t i = 0; i < m_Layers.size(); ++i) {
        auto &layer = m_Layers[i];
        if (layer->IsEnabled()) {
            LOGCATI("CompositeRenderer::Draw: drawing layer %zu, type=%d", i, layer->GetLayerType());
            layer->Draw(screenW, screenH);
        } else {
            LOGCATI("CompositeRenderer::Draw: layer %zu disabled, skipping", i);
        }
    }
    
    LOGCATI("CompositeRenderer::Draw: draw completed");
}

void CompositeRenderer::Destroy() {
    ClearLayers();
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
            return a->GetLayerType() < b->GetLayerType();
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
        skyLayer->SetParamFloat(PARAM_STAR_DENSITY, 0.3f);
    }
    AddLayer(skyLayer);
    
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
        skyLayer->SetParamFloat(PARAM_STAR_DENSITY, 0.05f);
    }
    AddLayer(skyLayer);
    
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
        skyLayer->SetParamFloat(PARAM_STAR_DENSITY, 0.2f);
    }
    AddLayer(skyLayer);
    
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
    float darkness[] = {0.6f, 0.7f, 0.8f, 0.95f};
    float speed[] = {0.3f, 0.4f, 0.5f, 0.8f};
    float intensity[] = {0.25f, 0.5f, 0.75f, 1.0f};
    float rainSpeed[] = {0.5f, 0.7f, 0.9f, 1.0f};
    float lightningInterval[] = {0.0f, 0.0f, 8.0f, 3.0f};
    float r[] = {0.35f, 0.25f, 0.2f, 0.1f};
    float g[] = {0.4f, 0.3f, 0.25f, 0.15f};
    float b[] = {0.5f, 0.4f, 0.3f, 0.2f};
    float nr[] = {0.1f, 0.08f, 0.05f, 0.03f};
    float ng[] = {0.12f, 0.1f, 0.07f, 0.04f};
    float nb[] = {0.18f, 0.15f, 0.1f, 0.06f};
    
    SkyBackgroundLayer *skyLayer = new SkyBackgroundLayer();
    skyLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, r[level], g[level], b[level]);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, r[level] * 0.9f, g[level] * 0.9f, b[level] * 0.9f);
        skyLayer->SetParamFloat(PARAM_SUN_INTENSITY, 0.1f);
    } else {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, nr[level], ng[level], nb[level]);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, nr[level] * 1.2f, ng[level] * 1.2f, nb[level] * 1.2f);
        skyLayer->SetParamFloat(PARAM_MOON_INTENSITY, 0.05f);
        skyLayer->SetParamFloat(PARAM_STAR_DENSITY, 0.05f);
    }
    AddLayer(skyLayer);
    
    CloudLayer *cloudLayer = new CloudLayer();
    cloudLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer->SetParamFloat(PARAM_CLOUD_COVERAGE, coverage[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? darkness[level] * 1.1f : darkness[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.1f : 0.2f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_SPEED, speed[level]);
    AddLayer(cloudLayer);
    
    PrecipitationLayer *precipitationLayer = new PrecipitationLayer();
    precipitationLayer->SetParamInt(PARAM_PRECIPITATION_TYPE, 0);
    precipitationLayer->SetParamFloat(PARAM_PRECIPITATION_INTENSITY, intensity[level]);
    precipitationLayer->SetParamFloat(PARAM_PRECIPITATION_SPEED, rainSpeed[level]);
    AddLayer(precipitationLayer);
    
    if (level >= 2) {
        EffectLayer *effectLayer = new EffectLayer();
        effectLayer->SetParamInt(PARAM_EFFECT_LIGHTNING_ENABLED, 1);
        effectLayer->SetParamFloat(PARAM_EFFECT_LIGHTNING_INTERVAL, lightningInterval[level]);
        AddLayer(effectLayer);
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
        skyLayer->SetParamFloat(PARAM_STAR_DENSITY, 0.1f);
    }
    AddLayer(skyLayer);
    
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
    float coverage[] = {0.5f, 0.7f, 0.9f, 1.0f};
    float darkness[] = {0.5f, 0.6f, 0.75f, 0.9f};
    float speed[] = {0.2f, 0.25f, 0.3f, 0.6f};
    float intensity[] = {0.2f, 0.45f, 0.7f, 1.0f};
    float snowSpeed[] = {0.3f, 0.4f, 0.5f, 0.7f};
    float fogDensity[] = {0.0f, 0.0f, 0.2f, 0.4f};
    float visibility[] = {0.0f, 0.0f, 0.6f, 0.3f};
    float r[] = {0.6f, 0.5f, 0.4f, 0.3f};
    float g[] = {0.65f, 0.55f, 0.45f, 0.35f};
    float b[] = {0.75f, 0.65f, 0.55f, 0.45f};
    float nr[] = {0.15f, 0.12f, 0.08f, 0.05f};
    float ng[] = {0.18f, 0.15f, 0.1f, 0.06f};
    float nb[] = {0.25f, 0.2f, 0.15f, 0.1f};
    
    SkyBackgroundLayer *skyLayer = new SkyBackgroundLayer();
    skyLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, r[level], g[level], b[level]);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, r[level] * 0.9f, g[level] * 0.9f, b[level] * 0.9f);
        skyLayer->SetParamFloat(PARAM_SUN_INTENSITY, 0.2f);
    } else {
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_TOP, nr[level], ng[level], nb[level]);
        skyLayer->SetParamVec3(PARAM_SKY_COLOR_BOTTOM, nr[level] * 1.1f, ng[level] * 1.1f, nb[level] * 1.1f);
        skyLayer->SetParamFloat(PARAM_MOON_INTENSITY, 0.3f);
        skyLayer->SetParamFloat(PARAM_STAR_DENSITY, 0.3f);
    }
    AddLayer(skyLayer);
    
    CloudLayer *cloudLayer = new CloudLayer();
    cloudLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer->SetParamFloat(PARAM_CLOUD_COVERAGE, coverage[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? darkness[level] * 1.1f : darkness[level]);
    cloudLayer->SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.15f : 0.25f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_SPEED, speed[level]);
    AddLayer(cloudLayer);
    
    PrecipitationLayer *precipitationLayer = new PrecipitationLayer();
    precipitationLayer->SetParamInt(PARAM_PRECIPITATION_TYPE, 1);
    precipitationLayer->SetParamFloat(PARAM_PRECIPITATION_INTENSITY, intensity[level]);
    precipitationLayer->SetParamFloat(PARAM_PRECIPITATION_SPEED, snowSpeed[level]);
    AddLayer(precipitationLayer);
    
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
        EffectLayer *effectLayer = new EffectLayer();
        effectLayer->SetParamInt(PARAM_EFFECT_WIND_LINES_ENABLED, 1);
        effectLayer->SetParamFloat(PARAM_EFFECT_WIND_STRENGTH, 0.8f);
        AddLayer(effectLayer);
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
        skyLayer->SetParamFloat(PARAM_STAR_DENSITY, 0.15f);
    }
    AddLayer(skyLayer);
    
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
        skyLayer->SetParamFloat(PARAM_STAR_DENSITY, 0.1f);
    }
    AddLayer(skyLayer);
    
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
    
    EffectLayer *effectLayer = new EffectLayer();
    effectLayer->SetParamInt(PARAM_EFFECT_WIND_LINES_ENABLED, 1);
    effectLayer->SetParamFloat(PARAM_EFFECT_WIND_STRENGTH, 0.6f);
    AddLayer(effectLayer);
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
        skyLayer->SetParamFloat(PARAM_STAR_DENSITY, 0.4f);
    }
    AddLayer(skyLayer);
    
    CloudLayer *cloudLayer = new CloudLayer();
    cloudLayer->SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer->SetParamFloat(PARAM_CLOUD_COVERAGE, 0.4f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.5f : 0.4f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.2f : 0.5f);
    cloudLayer->SetParamFloat(PARAM_CLOUD_SPEED, 0.9f);
    AddLayer(cloudLayer);
    
    EffectLayer *effectLayer = new EffectLayer();
    effectLayer->SetParamInt(PARAM_EFFECT_WIND_LINES_ENABLED, 1);
    effectLayer->SetParamFloat(PARAM_EFFECT_WIND_STRENGTH, 0.7f);
    AddLayer(effectLayer);
}