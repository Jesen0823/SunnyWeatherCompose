#include "HazeBuilder.h"

WeatherProfile HazeBuilder::Build(bool isNight) {
    float density[] = {0.25f, 0.45f, 0.75f};
    float visibility[] = {0.75f, 0.50f, 0.25f};
    float sunIntensity[] = {0.5f, 0.3f, 0.1f};
    float moonIntensity[] = {0.4f, 0.2f, 0.1f};
    float cloudCoverage[] = {0.4f, 0.6f, 0.8f};
    float cloudDarkness[] = {0.3f, 0.45f, 0.6f};
    float cloudLightness[] = {0.7f, 0.5f, 0.3f};

    float r[] = {0.85f, 0.75f, 0.60f};
    float g[] = {0.82f, 0.70f, 0.50f};
    float b[] = {0.75f, 0.60f, 0.40f};

    float nr[] = {0.25f, 0.20f, 0.15f};
    float ng[] = {0.24f, 0.18f, 0.13f};
    float nb[] = {0.28f, 0.22f, 0.16f};

    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, r[m_Level], g[m_Level], b[m_Level]);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, r[m_Level] * 0.9f, g[m_Level] * 0.9f, b[m_Level] * 0.9f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, sunIntensity[m_Level]);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, nr[m_Level], ng[m_Level], nb[m_Level]);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, nr[m_Level] * 1.2f, ng[m_Level] * 1.2f, nb[m_Level] * 1.2f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, moonIntensity[m_Level]);

        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 60.0f - m_Level * 20.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, cloudCoverage[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, cloudDarkness[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, cloudLightness[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, 0.015f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, 1.2f);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, 22.0f);

    LayerConfig& particleLayer = profile.AddLayer(LAYER_TYPE_PARTICLE);
    particleLayer.SetParamInt(PARAM_PARTICLE_TYPE, 0);
    particleLayer.SetParamFloat(PARAM_PARTICLE_DENSITY, density[m_Level]);
    if (!isNight) {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, r[m_Level] * 0.9f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, g[m_Level] * 0.9f);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, b[m_Level] * 0.9f);
    } else {
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, nr[m_Level]);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, ng[m_Level]);
        particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, nb[m_Level]);
    }
    particleLayer.SetParamFloat(PARAM_PARTICLE_VISIBILITY, visibility[m_Level]);

    return profile;
}

const std::string& HazeBuilder::GetSkycon() const {
    return m_Skycon;
}
