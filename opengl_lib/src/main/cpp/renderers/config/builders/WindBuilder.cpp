#include "WindBuilder.h"

WeatherProfile WindBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.4f, 0.65f, 0.9f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.2f, 0.45f, 0.7f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.5f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.08f, 0.12f, 0.22f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.12f, 0.18f, 0.32f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.4f);

        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 180.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.4f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.5f : 0.4f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.2f : 0.5f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.9f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 0.8f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, 18.0f);

    LayerConfig& windLayer = profile.AddLayer(LAYER_TYPE_WIND);
    windLayer.SetParamInt(PARAM_WIND_LINES_ENABLED, 1);
    windLayer.SetParamFloat(PARAM_WIND_STRENGTH, 0.7f);

    return profile;
}

const std::string& WindBuilder::GetSkycon() const {
    return m_Skycon;
}
