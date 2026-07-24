#include "WindBuilder.h"

WeatherProfile WindBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.45f, 0.68f, 0.92f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.25f, 0.50f, 0.75f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.4f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.06f, 0.10f, 0.20f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.10f, 0.16f, 0.30f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.35f);

        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 120.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.55f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.6f : 0.5f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.15f : 0.4f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.55f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 0.75f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, 20.0f);

    LayerConfig& windLayer = profile.AddLayer(LAYER_TYPE_WIND);
    windLayer.SetParamInt(PARAM_WIND_LINES_ENABLED, 1);
    windLayer.SetParamFloat(PARAM_WIND_STRENGTH, 0.85f);

    return profile;
}

const std::string& WindBuilder::GetSkycon() const {
    return m_Skycon;
}