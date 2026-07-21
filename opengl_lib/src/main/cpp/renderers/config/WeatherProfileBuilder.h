#ifndef GLLIB_WEATHERPROFILEBUILDER_H
#define GLLIB_WEATHERPROFILEBUILDER_H

#include "WeatherProfile.h"

/**
 * 天气配置构建器接口
 * 
 * 使用Builder模式构建WeatherProfile对象。
 * 每个具体的天气类型都有对应的Builder实现。
 * 
 * 设计模式：Builder模式 + Strategy模式
 * - Builder模式：将复杂对象的构建过程与表示分离
 * - Strategy模式：不同天气类型使用不同的构建策略
 */
class WeatherProfileBuilder {
public:
    virtual ~WeatherProfileBuilder() {}

    /**
     * 构建天气配置
     * @param isNight 是否夜间模式
     * @return WeatherProfile对象
     */
    virtual WeatherProfile Build(bool isNight) = 0;

    /**
     * 获取对应的skycon字符串
     * @return skycon字符串
     */
    virtual const std::string& GetSkycon() const = 0;
};

#endif // GLLIB_WEATHERPROFILEBUILDER_H
