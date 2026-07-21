#include "SandBuilder.h"

WeatherProfile SandBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.7f, 0.5f, 0.25f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.6f, 0.4f, 0.2f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.2f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.18f, 0.12f, 0.06f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.22f, 0.15f, 0.08f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.1f);

        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 35.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.8f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.7f : 0.6f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.15f : 0.35f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.15f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 0.75f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, 22.0f);

    LayerConfig& particleLayer = profile.AddLayer(LAYER_TYPE_PARTICLE);
    particleLayer.SetParamInt(PARAM_PARTICLE_TYPE, 3);
    particleLayer.SetParamFloat(PARAM_PARTICLE_DENSITY, 0.7f);
    if (!isNight) {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.7f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.5f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.25f);
    } else {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.22f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.15f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.08f);
    }
    particleLayer.SetParamFloat(PARAM_PARTICLE_VISIBILITY, 0.3f);

    LayerConfig& windLayer = profile.AddLayer(LAYER_TYPE_WIND);
    windLayer.SetParamInt(PARAM_WIND_LINES_ENABLED, 1);
    windLayer.SetParamFloat(PARAM_WIND_STRENGTH, 0.6f);

    return profile;
}

const std::string& SandBuilder::GetSkycon() const {
    return m_Skycon;
}
