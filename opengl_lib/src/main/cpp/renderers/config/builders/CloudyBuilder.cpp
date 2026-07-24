#include "CloudyBuilder.h"

WeatherProfile CloudyBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    skyLayer.SetParamInt(PARAM_SUN_VISIBLE, 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.65f, 0.68f, 0.75f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.70f, 0.73f, 0.80f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.0f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.20f, 0.22f, 0.28f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.25f, 0.27f, 0.33f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.85f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.45f : 0.3f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.5f : 0.9f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.01f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 0.75f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, isNight ? 30.0f : 35.0f);

    return profile;
}

const std::string& CloudyBuilder::GetSkycon() const {
    return m_Skycon;
}
