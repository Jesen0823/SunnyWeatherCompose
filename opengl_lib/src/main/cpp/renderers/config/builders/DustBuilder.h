#ifndef GLLIB_DUSTBUILDER_H
#define GLLIB_DUSTBUILDER_H

#include "../WeatherProfileBuilder.h"

/**
 * 浮尘天气配置构建器
 *
 * 天气特点：
 * - 天空呈土黄色或黄褐色，空气中弥漫沙尘
 * - 能见度降低，有悬浮尘埃颗粒效果
 * - 太阳呈淡黄色或橙红色，光线被尘埃散射
 * - 远处景物呈土黄色调，清晰度下降
 *
 * Layer组合：
 * - SkyBackgroundLayer（天空背景，土黄色调）
 * - ParticleLayer（悬浮尘埃粒子）
 */
class DustBuilder : public WeatherProfileBuilder {
public:
    WeatherProfile Build(bool isNight) override;
    const std::string& GetSkycon() const override;

private:
    const std::string m_Skycon = "DUST";
};

#endif // GLLIB_DUSTBUILDER_H
