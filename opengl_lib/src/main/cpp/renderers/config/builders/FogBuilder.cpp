#include "FogBuilder.h"

WeatherProfile FogBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.80f, 0.82f, 0.86f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.85f, 0.87f, 0.90f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.001f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.20f, 0.22f, 0.28f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.25f, 0.27f, 0.33f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.001f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 1.0f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.5f : 0.45f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.2f : 0.3f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.0003f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 3.5f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, 8.0f);

    if (isNight) {
        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 1.0f);
    }

    LayerConfig& particleLayer = profile.AddLayer(LAYER_TYPE_PARTICLE);
    particleLayer.SetParamInt(PARAM_PARTICLE_TYPE, 1);
    particleLayer.SetParamFloat(PARAM_PARTICLE_DENSITY, 0.998f);
    if (!isNight) {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.82f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.84f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.88f);
    } else {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.16f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.18f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.23f);
    }
    particleLayer.SetParamFloat(PARAM_PARTICLE_VISIBILITY, 0.01f);

    LayerConfig& ambientLayer = profile.AddLayer(LAYER_TYPE_AMBIENT_OVERLAY);
    ambientLayer.SetParamInt(PARAM_LIGHTNING_IS_NIGHT, isNight ? 1 : 0);
    ambientLayer.SetParamInt(PARAM_FOG_MODE, 1);
    ambientLayer.SetParamFloat(PARAM_FOG_INTENSITY, 3.0f);

    return profile;
}

const std::string& FogBuilder::GetSkycon() const {
    return m_Skycon;
}
