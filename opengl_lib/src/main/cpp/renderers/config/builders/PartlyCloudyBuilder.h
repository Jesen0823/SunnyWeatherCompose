#ifndef GLLIB_PARTLYCLOUDYBUILDER_H
#define GLLIB_PARTLYCLOUDYBUILDER_H

#include "../WeatherProfileBuilder.h"

/**
 * 多云天气配置构建器（白天/夜间）
 *
 * 天气特点：
 * - 天空部分被云层覆盖，约30%-60%覆盖率
 * - 太阳/月亮部分可见，被云层遮挡
 * - 云层呈白色或灰白色，有层次感
 * - 光照较晴天减弱，但仍有明显光源
 *
 * Layer组合：
 * - SkyBackgroundLayer（天空背景）
 * - CloudLayer（云层）
 * - 夜间额外包含StarLayer（星星，少量）
 *
 * @param skycon 天气类型标识（PARTLY_CLOUDY_DAY 或 PARTLY_CLOUDY_NIGHT）
 */
class PartlyCloudyBuilder : public WeatherProfileBuilder {
public:
    PartlyCloudyBuilder(const std::string& skycon) : m_Skycon(skycon) {}
    WeatherProfile Build(bool isNight) override;
    const std::string& GetSkycon() const override;

private:
    const std::string m_Skycon;
};

#endif // GLLIB_PARTLYCLOUDYBUILDER_H
