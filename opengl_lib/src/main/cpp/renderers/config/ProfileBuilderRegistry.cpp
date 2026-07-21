#include "ProfileBuilderRegistry.h"
#include "../base/GLLayerBase.h"

ProfileBuilderRegistry* ProfileBuilderRegistry::m_pInstance = nullptr;

ProfileBuilderRegistry* ProfileBuilderRegistry::GetInstance() {
    if (m_pInstance == nullptr) {
        m_pInstance = new ProfileBuilderRegistry();
    }
    return m_pInstance;
}

void ProfileBuilderRegistry::DestroyInstance() {
    if (m_pInstance != nullptr) {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

ProfileBuilderRegistry::~ProfileBuilderRegistry() {
    for (auto& pair : m_Builders) {
        delete pair.second;
    }
    m_Builders.clear();
}

void ProfileBuilderRegistry::RegisterBuilder(const std::string& skycon, WeatherProfileBuilder* builder) {
    if (builder == nullptr) return;

    auto it = m_Builders.find(skycon);
    if (it != m_Builders.end()) {
        delete it->second;
    }
    m_Builders[skycon] = builder;
}

WeatherProfileBuilder* ProfileBuilderRegistry::GetBuilder(const std::string& skycon) {
    auto it = m_Builders.find(skycon);
    if (it != m_Builders.end()) {
        return it->second;
    }
    return nullptr;
}

WeatherProfile ProfileBuilderRegistry::BuildProfile(const std::string& skycon, bool isNight) {
    WeatherProfileBuilder* builder = GetBuilder(skycon);
    if (builder != nullptr) {
        WeatherProfile profile = builder->Build(isNight);
        profile.skycon = skycon;
        return profile;
    }

    WeatherProfile defaultProfile;
    defaultProfile.skycon = skycon;

    LayerConfig& skyLayer = defaultProfile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.3f, 0.6f, 0.95f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.4f, 0.7f, 1.0f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 1.2f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.02f, 0.04f, 0.10f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.05f, 0.08f, 0.18f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.9f);

        LayerConfig& starLayer = defaultProfile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 300.0f);
    }

    LayerConfig& cloudLayer = defaultProfile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, isNight ? 0.35f : 0.20f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.65f : 0.15f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.50f : 0.98f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, isNight ? 0.02f : 0.025f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, isNight ? 1.1f : 1.0f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, isNight ? 20.0f : 10.0f);

    return defaultProfile;
}

bool ProfileBuilderRegistry::IsRegistered(const std::string& skycon) {
    return m_Builders.find(skycon) != m_Builders.end();
}
