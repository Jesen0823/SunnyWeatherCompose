#include "DustBuilder.h"

WeatherProfile DustBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.6f, 0.5f, 0.35f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.5f, 0.4f, 0.25f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.3f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.15f, 0.12f, 0.08f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.2f, 0.16f, 0.1f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.15f);

        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 50.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.7f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.65f : 0.55f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.2f : 0.4f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.1f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 0.8f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, 20.0f);

    LayerConfig& particleLayer = profile.AddLayer(LAYER_TYPE_PARTICLE);
    particleLayer.SetParamInt(PARAM_PARTICLE_TYPE, 2);
    particleLayer.SetParamFloat(PARAM_PARTICLE_DENSITY, 0.4f);
    if (!isNight) {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.6f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.5f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.35f);
    } else {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.2f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.16f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.1f);
    }
    particleLayer.SetParamFloat(PARAM_PARTICLE_VISIBILITY, 0.5f);

    return profile;
}

const std::string& DustBuilder::GetSkycon() const {
    return m_Skycon;
}
