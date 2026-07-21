#include "PartlyCloudyBuilder.h"

WeatherProfile PartlyCloudyBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.35f, 0.6f, 0.9f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.15f, 0.35f, 0.55f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.7f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.08f, 0.12f, 0.2f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.12f, 0.18f, 0.3f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.6f);

        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 150.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.5f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.5f : 0.4f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.6f : 0.8f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, isNight ? 0.25f : 0.3f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 0.85f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, isNight ? 18.0f : 22.0f);

    return profile;
}

const std::string& PartlyCloudyBuilder::GetSkycon() const {
    return m_Skycon;
}
