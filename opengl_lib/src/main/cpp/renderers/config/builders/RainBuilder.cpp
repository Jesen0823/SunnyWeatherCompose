#include "RainBuilder.h"

WeatherProfile RainBuilder::Build(bool isNight) {
    float coverage[] = {0.55f, 0.65f, 0.78f, 0.88f};
    float darkness[] = {0.13f, 0.28f, 0.58f, 0.70f};
    float lightness[] = {1.25f, 1.00f, 0.55f, 0.40f};
    float speed[] = {0.005f, 0.010f, 0.020f, 0.035f};
    float scale[] = {1.10f, 0.95f, 0.75f, 0.60f};
    float alpha[] = {12.0f, 16.0f, 20.0f, 24.0f};
    float intensity[] = {0.3f, 0.7f, 1.3f, 1.8f};
    float rainSpeed[] = {0.5f, 0.7f, 0.9f, 1.0f};
    float r[] = {0.62f, 0.52f, 0.35f, 0.28f};
    float g[] = {0.65f, 0.57f, 0.40f, 0.32f};
    float b[] = {0.72f, 0.65f, 0.50f, 0.42f};
    float nr[] = {0.35f, 0.30f, 0.22f, 0.18f};
    float ng[] = {0.39f, 0.34f, 0.26f, 0.22f};
    float nb[] = {0.50f, 0.45f, 0.35f, 0.30f};

    WeatherProfile profile;
    profile.skycon = m_Skycon;
    profile.flags.requiresFBO = true;

    LayerConfig& skyLayer = profile.AddLayer(LAYER_TYPE_SKY_BACKGROUND);
    skyLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    if (!isNight) {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, r[m_Level], g[m_Level], b[m_Level]);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, r[m_Level] * 0.9f, g[m_Level] * 0.9f, b[m_Level] * 0.9f);
        skyLayer.SetParamFloat(PARAM_SUN_INTENSITY, 0.1f);
        skyLayer.SetParamInt(PARAM_SUN_VISIBLE, 0);
        skyLayer.SetParamInt(PARAM_SKY_MODE, 0);
    } else {
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, nr[m_Level], ng[m_Level], nb[m_Level]);
        skyLayer.SetParamVec3(PARAM_SKY_COLOR_BOTTOM, nr[m_Level] * 1.2f, ng[m_Level] * 1.2f, nb[m_Level] * 1.2f);
        skyLayer.SetParamFloat(PARAM_MOON_INTENSITY, 0.05f);
        skyLayer.SetParamInt(PARAM_SKY_MODE, 1);

        LayerConfig& starLayer = profile.AddLayer(LAYER_TYPE_STAR);
        starLayer.SetParamFloat(PARAM_STAR_COUNT, 20.0f);
    }

    LayerConfig& cloudLayer = profile.AddLayer(LAYER_TYPE_CLOUD);
    cloudLayer.SetParamInt(PARAM_TIME_OF_DAY, isNight ? 1 : 0);
    cloudLayer.SetParamInt(PARAM_CLOUD_MODE, 1);
    cloudLayer.SetParamFloat(PARAM_CLOUD_COVERAGE, coverage[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_DARKNESS, isNight ? darkness[m_Level] * 1.1f : darkness[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_LIGHTNESS, isNight ? lightness[m_Level] * 0.8f : lightness[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SPEED, speed[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_SCALE, scale[m_Level]);
    cloudLayer.SetParamFloat(PARAM_CLOUD_ALPHA, alpha[m_Level]);

    LayerConfig& rainLayer = profile.AddLayer(LAYER_TYPE_RAIN);
    float rainIntensity = intensity[m_Level];
    if (m_Level == 3) rainIntensity = 2.2f;
    rainLayer.SetParamFloat(PARAM_RAIN_INTENSITY, rainIntensity);
    rainLayer.SetParamFloat(PARAM_RAIN_WIND_SPEED, m_Level == 3 ? 0.8f : 0.2f + (float)m_Level * 0.15f);
    rainLayer.SetParamFloat(PARAM_RAIN_REFRACTION, 0.3f + (float)m_Level * 0.25f);

    LayerConfig& particleLayer = profile.AddLayer(LAYER_TYPE_PARTICLE);
    particleLayer.SetParamInt(PARAM_PARTICLE_TYPE, 1);
    particleLayer.SetParamFloat(PARAM_PARTICLE_DENSITY, m_Level == 3 ? 0.6f : 0.3f);
    particleLayer.SetParamFloat(PARAM_PARTICLE_VISIBILITY, m_Level == 3 ? 0.3f : 0.5f);
    particleLayer.SetParamVec3(PARAM_SKY_COLOR_TOP, r[m_Level] * 0.8f, g[m_Level] * 0.8f, b[m_Level] * 0.8f);

    if (m_Level == 3) {
        profile.flags.requiresLightningLink = true;

        LayerConfig& lightningLayer = profile.AddLayer(LAYER_TYPE_LIGHTNING);
        lightningLayer.SetParamInt(PARAM_LIGHTNING_ENABLED, 1);
        lightningLayer.SetParamFloat(PARAM_LIGHTNING_INTERVAL, 3.0f);
        lightningLayer.SetParamInt(PARAM_LIGHTNING_IS_NIGHT, isNight ? 1 : 0);

        LayerConfig& ambientLayer = profile.AddLayer(LAYER_TYPE_AMBIENT_OVERLAY);
        ambientLayer.SetParamInt(PARAM_LIGHTNING_IS_NIGHT, isNight ? 1 : 0);
    }

    return profile;
}

const std::string& RainBuilder::GetSkycon() const {
    return m_Skycon;
}
