#include "ClearDayBuilder.h"

WeatherProfile ClearDayBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, 0);
    skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.3f, 0.6f, 0.95f);
    skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.4f, 0.7f, 1.0f);
    skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 1.2f);

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.30f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, 0.08f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, 1.10f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.02f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 0.9f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, 14.0f);

    return profile;
}

const std::string& ClearDayBuilder::GetSkycon() const {
    return m_Skycon;
}
