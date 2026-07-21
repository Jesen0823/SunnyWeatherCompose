#ifndef GLLIB_HAZEBUILDER_H
#define GLLIB_HAZEBUILDER_H

#include "../WeatherProfileBuilder.h"

/**
 * 雾霾天气配置构建器
 *
 * 天气特点：
 * - 天空呈灰白色或黄色，能见度低
 * - 有明显的悬浮颗粒效果，空气中弥漫薄雾
 * - 太阳/月亮朦胧可见，呈圆形光晕
 * - 远处景物模糊，有深度感衰减
 *
 * Layer组合：
 * - SkyBackgroundLayer（天空背景，灰黄色调）
 * - ParticleLayer（悬浮颗粒，模拟雾霾）
 *
 * @param skycon 天气类型标识（HAZE）
 * @param level 雾霾等级（0-轻度，1-中度，2-重度）
 */
class HazeBuilder : public WeatherProfileBuilder {
public:
    HazeBuilder(const std::string& skycon, int level) : m_Skycon(skycon), m_Level(level) {}
    WeatherProfile Build(bool isNight) override;
    const std::string& GetSkycon() const override;

private:
    const std::string m_Skycon;
    const int m_Level;
};

#endif // GLLIB_HAZEBUILDER_H
