#ifndef GLLIB_CLEARDAYBUILDER_H
#define GLLIB_CLEARDAYBUILDER_H

#include "../WeatherProfileBuilder.h"

/**
 * 晴天白天配置构建器
 * 
 * 天气特点：
 * - 天空湛蓝，顶部深蓝色，底部浅蓝色
 * - 太阳明亮，强度较高
 * - 云层稀少，覆盖率低，偏白色
 * - 无星星、雨雪、颗粒等效果
 * 
 * Layer组合：
 * - SkyBackgroundLayer（天空背景）
 * - CloudLayer（云层）
 */
class ClearDayBuilder : public WeatherProfileBuilder {
public:
    WeatherProfile Build(bool isNight) override;
    const std::string& GetSkycon() const override;

private:
    const std::string m_Skycon = "CLEAR_DAY";
};

#endif // GLLIB_CLEARDAYBUILDER_H
