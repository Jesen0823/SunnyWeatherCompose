#ifndef GLLIB_WINDBUILDER_H
#define GLLIB_WINDBUILDER_H

#include "../WeatherProfileBuilder.h"

/**
 * 大风天气配置构建器
 *
 * 天气特点：
 * - 天空有云层，云层被风吹动快速移动
 * - 有明显的风线效果，模拟气流运动
 * - 云层形态被拉伸，呈长条状
 * - 可能伴有少量沙尘或树叶等杂物飘动
 *
 * Layer组合：
 * - SkyBackgroundLayer（天空背景）
 * - CloudLayer（云层，快速移动）
 * - WindLayer（风线效果）
 */
class WindBuilder : public WeatherProfileBuilder {
public:
    WeatherProfile Build(bool isNight) override;
    const std::string& GetSkycon() const override;

private:
    const std::string m_Skycon = "WIND";
};

#endif // GLLIB_WINDBUILDER_H
