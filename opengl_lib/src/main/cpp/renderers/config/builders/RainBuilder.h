#ifndef GLLIB_RAINBUILDER_H
#define GLLIB_RAINBUILDER_H

#include "../WeatherProfileBuilder.h"

/**
 * 下雨天气配置构建器（小雨/中雨/大雨/暴雨）
 *
 * 天气特点：
 * - 天空呈灰蓝色或深灰色，云层厚重
 * - 有垂直下落的雨滴效果
 * - 云层覆盖率高，从部分覆盖到完全覆盖
 * - 雨势越大，天空越暗，雨滴密度越高
 * - 暴雨时可能伴有闪电效果
 *
 * Layer组合：
 * - SkyBackgroundLayer（天空背景，灰蓝色调）
 * - CloudLayer（云层，高覆盖率）
 * - RainLayer（雨滴效果）
 * - 暴雨额外包含ParticleLayer（雨雾效果）和LightningLayer（闪电）
 *
 * @param skycon 天气类型标识（RAIN/SHOWER_RAIN/THUNDER_SHOWER）
 * @param level 降雨等级（0-小雨，1-中雨，2-大雨，3-暴雨）
 */
class RainBuilder : public WeatherProfileBuilder {
public:
    RainBuilder(const std::string& skycon, int level) : m_Skycon(skycon), m_Level(level) {}
    WeatherProfile Build(bool isNight) override;
    const std::string& GetSkycon() const override;

private:
    const std::string m_Skycon;
    const int m_Level;
};

#endif // GLLIB_RAINBUILDER_H
