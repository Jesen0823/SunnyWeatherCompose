#include "DustBuilder.h"

WeatherProfile DustBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    skyLayer.SetParamInt(PARAM_SKY_MODE, 3);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.55f, 0.48f, 0.30f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.45f, 0.40f, 0.22f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.15f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.18f, 0.15f, 0.08f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.22f, 0.18f, 0.10f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.05f);

        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 20.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.4f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.55f : 0.45f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.25f : 0.50f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.05f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 0.9f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, 18.0f);

    LayerConfig& particleLayer = profile.AddLayer(LAYER_TYPE_PARTICLE);
    particleLayer.SetParamInt(PARAM_PARTICLE_TYPE, 2);
    particleLayer.SetParamFloat(PARAM_PARTICLE_DENSITY, 0.5f);
    if (!isNight) {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.60f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.52f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.35f);
    } else {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.20f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.16f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.10f);
    }
    particleLayer.SetParamFloat(PARAM_PARTICLE_VISIBILITY, 0.6f);

    LayerConfig& ambientLayer = profile.AddLayer(LAYER_TYPE_AMBIENT_OVERLAY);
    ambientLayer.SetParamInt(PARAM_LIGHTNING_IS_NIGHT, isNight ? 1 : 0);
    ambientLayer.SetParamInt(PARAM_OVERLAY_MODE, 2);
    ambientLayer.SetParamFloat(PARAM_FOG_INTENSITY, 2.5f);
    if (!isNight) {
        ambientLayer.SetParamFloat(PARAM_OVERLAY_COLOR_R, 0.75f);
        ambientLayer.SetParamFloat(PARAM_OVERLAY_COLOR_G, 0.68f);
        ambientLayer.SetParamFloat(PARAM_OVERLAY_COLOR_B, 0.50f);
    } else {
        ambientLayer.SetParamFloat(PARAM_OVERLAY_COLOR_R, 0.25f);
        ambientLayer.SetParamFloat(PARAM_OVERLAY_COLOR_G, 0.20f);
        ambientLayer.SetParamFloat(PARAM_OVERLAY_COLOR_B, 0.12f);
    }

    return profile;
}

const std::string& DustBuilder::GetSkycon() const {
    return m_Skycon;
}
