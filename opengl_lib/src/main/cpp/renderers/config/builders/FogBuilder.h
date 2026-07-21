#ifndef GLLIB_FOGBUILDER_H
#define GLLIB_FOGBUILDER_H

#include "../WeatherProfileBuilder.h"

/**
 * 雾天配置构建器
 *
 * 天气特点：
 * - 能见度极低，远处景物完全被雾遮挡
 * - 天空呈灰白色，上下颜色差异小
 * - 有明显的距离衰减效果，近处清晰远处模糊
 * - 整体光线柔和，无明显光源
 *
 * Layer组合：
 * - SkyBackgroundLayer（天空背景，灰白色调）
 * - ParticleLayer（雾气粒子，模拟雾的效果）
 */
class FogBuilder : public WeatherProfileBuilder {
public:
    WeatherProfile Build(bool isNight) override;
    const std::string& GetSkycon() const override;

private:
    const std::string m_Skycon = "FOG";
};

#endif // GLLIB_FOGBUILDER_H
