#include "ClearNightBuilder.h"

WeatherProfile ClearNightBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, 1);
    skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.02f, 0.04f, 0.10f);
    skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.05f, 0.08f, 0.18f);
    skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.9f);

    LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
    starLayer.SetParamFloat(PARAM_STAR_COUNT, 180.0f);

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, 1);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.15f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, 0.55f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, 0.70f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.02f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 1.2f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, 12.0f);
    cloudLayer.SetParamVec2(PARAM_MOON_POSITION, 0.25f, 0.8f);

    return profile;
}

const std::string& ClearNightBuilder::GetSkycon() const {
    return m_Skycon;
}
