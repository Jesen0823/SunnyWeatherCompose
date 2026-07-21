#ifndef GLLIB_CLEARNIGHTBUILDER_H
#define GLLIB_CLEARNIGHTBUILDER_H

#include "../WeatherProfileBuilder.h"

/**
 * 晴天夜间配置构建器
 *
 * 天气特点：
 * - 天空深蓝，顶部几乎黑色，底部深蓝色
 * - 月亮微弱可见
 * - 星星闪烁，数量较多
 * - 无云层或极少量云层
 *
 * Layer组合：
 * - SkyBackgroundLayer（天空背景）
 * - StarLayer（星星）
 */
class ClearNightBuilder : public WeatherProfileBuilder {
public:
    WeatherProfile Build(bool isNight) override;
    const std::string& GetSkycon() const override;

private:
    const std::string m_Skycon = "CLEAR_NIGHT";
};

#endif // GLLIB_CLEARNIGHTBUILDER_H
