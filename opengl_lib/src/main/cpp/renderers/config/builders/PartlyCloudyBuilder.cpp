#include "PartlyCloudyBuilder.h"

WeatherProfile PartlyCloudyBuilder::Build(bool isNight) {
    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.28f, 0.48f, 0.75f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.45f, 0.62f, 0.85f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.50f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, 0.06f, 0.09f, 0.15f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, 0.10f, 0.15f, 0.25f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.40f);

        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 50.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.60f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.6f : 0.15f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.55f : 1.05f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, isNight ? 0.015f : 0.025f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 1.0f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, isNight ? 14.0f : 16.0f);
    
    if (isNight) {
        cloudLayer.SetParamVec2(PARAM_MOON_POSITION, 0.25f, 0.8f);
    }

    return profile;
}

const std::string& PartlyCloudyBuilder::GetSkycon() const {
    return m_Skycon;
}
