#ifndef GLLIB_CLOUDYBUILDER_H
#define GLLIB_CLOUDYBUILDER_H

#include "../WeatherProfileBuilder.h"

/**
 * 阴天配置构建器
 *
 * 天气特点：
 * - 天空完全被云层覆盖，云层厚重
 * - 太阳/月亮不可见，光照均匀柔和
 * - 云层呈灰白色，颜色均匀
 * - 天空整体亮度降低，无明显明暗对比
 *
 * Layer组合：
 * - SkyBackgroundLayer（天空背景，灰蓝色调）
 * - CloudLayer（云层，高覆盖率）
 */
class CloudyBuilder : public WeatherProfileBuilder {
public:
    WeatherProfile Build(bool isNight) override;
    const std::string& GetSkycon() const override;

private:
    const std::string m_Skycon = "CLOUDY";
};

#endif // GLLIB_CLOUDYBUILDER_H
