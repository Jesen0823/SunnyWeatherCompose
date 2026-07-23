#include "SandBuilder.h"

WeatherProfile SandBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    skyLayer.SetParamInt(PARAM_SKY_MODE, 3);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.65f, 0.35f, 0.15f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.55f, 0.25f, 0.10f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.05f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.22f, 0.15f, 0.08f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.28f, 0.20f, 0.12f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.02f);

        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 5.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.75f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.65f : 0.55f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.25f : 0.45f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.09f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 0.8f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, 20.0f);

    LayerConfig& particleLayer = profile.AddLayer(LAYER_TYPE_PARTICLE);
    particleLayer.SetParamInt(PARAM_PARTICLE_TYPE, 2);
    particleLayer.SetParamFloat(PARAM_PARTICLE_DENSITY, 0.75f);
    if (!isNight) {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.65f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.40f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.18f);
    } else {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.20f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.15f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.08f);
    }
    particleLayer.SetParamFloat(PARAM_PARTICLE_VISIBILITY, 0.4f);

    LayerConfig& windLayer = profile.AddLayer(LAYER_TYPE_WIND);
    windLayer.SetParamInt(PARAM_WIND_LINES_ENABLED, 1);
    windLayer.SetParamFloat(PARAM_WIND_STRENGTH, 0.85f);

    LayerConfig& ambientLayer = profile.AddLayer(LAYER_TYPE_AMBIENT_OVERLAY);
    ambientLayer.SetParamInt(PARAM_LIGHTNING_IS_NIGHT, isNight ? 1 : 0);
    ambientLayer.SetParamInt(PARAM_OVERLAY_MODE, 3);
    ambientLayer.SetParamFloat(PARAM_FOG_INTENSITY, 2.5f);
    if (!isNight) {
        ambientLayer.SetParamFloat(PARAM_OVERLAY_COLOR_R, 0.65f);
        ambientLayer.SetParamFloat(PARAM_OVERLAY_COLOR_G, 0.40f);
        ambientLayer.SetParamFloat(PARAM_OVERLAY_COLOR_B, 0.18f);
    } else {
        ambientLayer.SetParamFloat(PARAM_OVERLAY_COLOR_R, 0.22f);
        ambientLayer.SetParamFloat(PARAM_OVERLAY_COLOR_G, 0.15f);
        ambientLayer.SetParamFloat(PARAM_OVERLAY_COLOR_B, 0.08f);
    }

    return profile;
}

const std::string& SandBuilder::GetSkycon() const {
    return m_Skycon;
}
