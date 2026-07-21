#include "FogBuilder.h"

WeatherProfile FogBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.8f, 0.8f, 0.8f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.7f, 0.7f, 0.7f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.1f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.2f, 0.22f, 0.28f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.25f, 0.27f, 0.33f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.1f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.9f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.8f : 0.7f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.1f : 0.2f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.05f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 0.7f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, 28.0f);

    if (isNight) {
        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 40.0f);
    }

    LayerConfig& particleLayer = profile.AddLayer(LAYER_TYPE_PARTICLE);
    particleLayer.SetParamInt(PARAM_PARTICLE_TYPE, 1);
    particleLayer.SetParamFloat(PARAM_PARTICLE_DENSITY, 0.7f);
    if (!isNight) {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.85f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.85f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.85f);
    } else {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.3f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.32f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.4f);
    }
    particleLayer.SetParamFloat(PARAM_PARTICLE_VISIBILITY, 0.15f);

    return profile;
}

const std::string& FogBuilder::GetSkycon() const {
    return m_Skycon;
}
