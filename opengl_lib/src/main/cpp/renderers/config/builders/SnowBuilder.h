#ifndef GLLIB_SNOWBUILDER_H
#define GLLIB_SNOWBUILDER_H

#include "../WeatherProfileBuilder.h"

/**
 * 下雪天气配置构建器（小雪/中雪/大雪/暴雪）
 *
 * 天气特点：
 * - 天空呈灰白色或暗灰色，云层厚重
 * - 有飘落的雪花效果，雪花大小和密度随等级变化
 * - 云层覆盖率从部分覆盖到完全覆盖
 * - 雪势越大，天空越暗，云层呈暗灰色
 * - 暴雪时伴有强风，雪花随风飘动
 * - 大雪和暴雪有能见度衰减效果（雾/霾层）
 *
 * Layer组合：
 * - SkyBackgroundLayer（天空背景，灰白色调）
 * - CloudLayer（云层，雪天模式，暗灰色调）
 * - SnowLayer（雪花效果）
 * - 大雪/暴雪额外包含ParticleLayer（能见度衰减）
 * - 暴雪额外包含WindLayer（强风效果）
 *
 * @param skycon 天气类型标识（SNOW/SNOW_SHOWER/STORM_SNOW）
 * @param level 降雪等级（0-小雪，1-中雪，2-大雪，3-暴雪）
 */
class SnowBuilder : public WeatherProfileBuilder {
public:
    SnowBuilder(const std::string& skycon, int level) : m_Skycon(skycon), m_Level(level) {}
    WeatherProfile Build(bool isNight) override;
    const std::string& GetSkycon() const override;

private:
    const std::string m_Skycon;
    const int m_Level;
};

#endif // GLLIB_SNOWBUILDER_H
