#include "SnowBuilder.h"

WeatherProfile SnowBuilder::Build(bool isNight) {
    float coverage[] = {0.60f, 0.75f, 0.85f, 0.92f};
    float darkness[] = {0.15f, 0.25f, 0.45f, 0.55f};
    float lightness[] = {1.20f, 1.00f, 0.70f, 0.60f};
    float speed[] = {0.002f, 0.0025f, 0.003f, 0.0035f};
    float scale[] = {0.90f, 0.75f, 0.60f, 0.50f};
    float alpha[] = {10.0f, 14.0f, 18.0f, 22.0f};
    float intensity[] = {0.6f, 0.7f, 0.8f, 0.95f};
    float snowSpeed[] = {0.3f, 0.4f, 0.65f, 0.9f};
    float snowSize[] = {3.5f, 5.0f, 2.0f, 2.2f};
    float windForce[] = {0.0f, 0.05f, 0.4f, 0.8f};
    int particleCounts[] = {280, 525, 735, 1225};
    float snowShape[] = {0.0f, 0.0f, 1.0f, 1.0f};
    float fogDensity[] = {0.0f, 0.0f, 0.2f, 0.4f};
    float visibility[] = {0.0f, 0.0f, 0.6f, 0.3f};
    float r[] = {0.55f, 0.48f, 0.38f, 0.28f};
    float g[] = {0.62f, 0.52f, 0.42f, 0.30f};
    float b[] = {0.72f, 0.58f, 0.48f, 0.36f};
    float nr[] = {0.28f, 0.22f, 0.18f, 0.12f};
    float ng[] = {0.30f, 0.25f, 0.20f, 0.14f};
    float nb[] = {0.40f, 0.32f, 0.25f, 0.18f};
    float sunIntensity[] = {0.20f, 0.10f, 0.0f, 0.0f};
    int sunVisible[] = {1, 1, 0, 0};

    WeatherProfile profile;
    profile.skycon = m_Skycon;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, r[m_Level], g[m_Level], b[m_Level]);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, r[m_Level] * 0.92f, g[m_Level] * 0.92f, b[m_Level] * 0.92f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, sunIntensity[m_Level]);
        skyLayer.SetParamInt(PARAM_SUN_VISIBLE, sunVisible[m_Level]);
        skyLayer.SetParamInt(PARAM_SKY_MODE, 2);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, nr[m_Level], ng[m_Level], nb[m_Level]);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, nr[m_Level] * 1.05f, ng[m_Level] * 1.05f, nb[m_Level] * 1.05f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.2f);
        skyLayer.SetParamInt(PARAM_SKY_MODE, 2);

        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 50.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamInt(PARAM_CLOUD_MODE, 2);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, coverage[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? darkness[m_Level] * 1.1f : darkness[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? lightness[m_Level] * 0.8f : lightness[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, speed[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, scale[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, alpha[m_Level]);

    LayerConfig& snowLayer = profile.AddLayer(LAYER_TYPE_SNOW);
    snowLayer.SetParamInt(PARAM_SNOW_PARTICLE_COUNT, particleCounts[m_Level]);
    snowLayer.SetParamFloat(PARAM_SNOW_INTENSITY, intensity[m_Level]);
    snowLayer.SetParamFloat(PARAM_SNOW_SPEED, snowSpeed[m_Level]);
    snowLayer.SetParamFloat(PARAM_SNOW_SIZE, snowSize[m_Level]);
    snowLayer.SetParamFloat(PARAM_SNOW_WIND, windForce[m_Level]);
    snowLayer.SetParamFloat(PARAM_SNOW_SHAPE, snowShape[m_Level]);

    if (m_Level >= 2) {
        LayerConfig& particleLayer = profile.AddLayer(LAYER_TYPE_PARTICLE);
        particleLayer.SetParamInt(PARAM_PARTICLE_TYPE, 1);
        particleLayer.SetParamFloat(PARAM_PARTICLE_DENSITY, fogDensity[m_Level]);
        if (!isNight) {
            particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.8f);
            particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.8f);
            particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.85f);
        } else {
            particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_R, 0.25f);
            particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_G, 0.27f);
            particleLayer.SetParamFloat(PARAM_PARTICLE_COLOR_B, 0.35f);
        }
        particleLayer.SetParamFloat(PARAM_PARTICLE_VISIBILITY, visibility[m_Level]);
    }

    if (m_Level == 3) {
        LayerConfig& windLayer = profile.AddLayer(LAYER_TYPE_WIND);
        windLayer.SetParamInt(PARAM_WIND_LINES_ENABLED, 1);
        windLayer.SetParamFloat(PARAM_WIND_STRENGTH, 0.8f);
    }

    return profile;
}

const std::string& SnowBuilder::GetSkycon() const {
    return m_Skycon;
}
