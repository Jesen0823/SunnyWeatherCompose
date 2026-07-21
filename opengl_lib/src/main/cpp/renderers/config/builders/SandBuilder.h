#ifndef GLLIB_SANDBUILDER_H
#define GLLIB_SANDBUILDER_H

#include "../WeatherProfileBuilder.h"

/**
 * 沙尘/沙尘暴天气配置构建器
 *
 * 天气特点：
 * - 天空呈深黄色或橙黄色，沙尘浓度高
 * - 能见度极低，严重时天空昏暗
 * - 有明显的风沙流动效果，水平方向的沙尘运动
 * - 太阳被沙尘遮蔽，呈暗红色
 * - 整体色调偏暖，黄色和橙色为主
 *
 * Layer组合：
 * - SkyBackgroundLayer（天空背景，深黄色调）
 * - ParticleLayer（沙尘粒子）
 * - WindLayer（风沙流动效果）
 */
class SandBuilder : public WeatherProfileBuilder {
public:
    WeatherProfile Build(bool isNight) override;
    const std::string& GetSkycon() const override;

private:
    const std::string m_Skycon = "SAND";
};

#endif // GLLIB_SANDBUILDER_H
