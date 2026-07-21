#include "HazeBuilder.h"

WeatherProfile HazeBuilder::Build(bool isNight) {
    float density[] = {0.3f, 0.5f, 0.8f};
    float visibility[] = {0.7f, 0.5f, 0.2f};
    float r[] = {0.6f, 0.5f, 0.4f};
    float g[] = {0.55f, 0.45f, 0.35f};
    float b[] = {0.45f, 0.35f, 0.25f};

    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, r[m_Level], g[m_Level], b[m_Level]);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, r[m_Level] * 0.8f, g[m_Level] * 0.8f, b[m_Level] * 0.8f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.3f);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, r[m_Level] * 0.2f, g[m_Level] * 0.2f, b[m_Level] * 0.3f);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, r[m_Level] * 0.3f, g[m_Level] * 0.3f, b[m_Level] * 0.4f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.2f);

        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 60.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, 0.3f + m_Level * 0.1f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? 0.7f + m_Level * 0.1f : 0.5f + m_Level * 0.1f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? 0.3f : 0.5f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.08f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 0.9f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, 15.0f);

    LayerConfig& particleLayer = profile.AddLayer(LAYER_TYPE_PARTICLE);
    particleLayer.SetParamInt(PARAM_PARTICLE_TYPE, 0);
    particleLayer.SetParamFloat(PARAM_PARTICLE_DENSITY, density[m_Level]);
    if (!isNight) {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, r[m_Level]);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, g[m_Level]);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, b[m_Level]);
    } else {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, r[m_Level] * 0.3f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, g[m_Level] * 0.3f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, b[m_Level] * 0.4f);
    }
    particleLayer.SetParamFloat(PARAM_PARTICLE_VISIBILITY, visibility[m_Level]);

    return profile;
}

const std::string& HazeBuilder::GetSkycon() const {
    return m_Skycon;
}
