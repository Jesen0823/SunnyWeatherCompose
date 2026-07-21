#ifndef GLLIB_PROFILEBUILDERREGISTRY_H
#define GLLIB_PROFILEBUILDERREGISTRY_H

#include "WeatherProfileBuilder.h"
#include "../base/GLLayerBase.h"
#include <unordered_map>
#include <memory>
#include <string>

/**
 * 天气配置构建器注册中心
 * 
 * 使用Registry模式管理所有WeatherProfileBuilder的注册和查找。
 * 提供线程安全的访问接口。
 * 
 * 设计模式：Registry模式 + Singleton模式
 * - Registry模式：集中管理对象的注册和查找
 * - Singleton模式：全局唯一的注册中心实例
 */
class ProfileBuilderRegistry {
public:
    /**
     * 获取单例实例
     * @return 注册中心实例
     */
    static ProfileBuilderRegistry* GetInstance();

    /**
     * 销毁单例实例
     */
    static void DestroyInstance();

    /**
     * 注册Builder
     * @param skycon 天气类型字符串
     * @param builder Builder实例（所有权转移）
     */
    void RegisterBuilder(const std::string& skycon, WeatherProfileBuilder* builder);

    /**
     * 根据skycon获取Builder
     * @param skycon 天气类型字符串
     * @return Builder指针，未找到返回nullptr
     */
    WeatherProfileBuilder* GetBuilder(const std::string& skycon);

    /**
     * 根据skycon构建WeatherProfile
     * @param skycon 天气类型字符串
     * @param isNight 是否夜间模式
     * @return WeatherProfile对象，未找到返回默认配置
     */
    WeatherProfile BuildProfile(const std::string& skycon, bool isNight);

    /**
     * 检查skycon是否已注册
     * @param skycon 天气类型字符串
     * @return 是否已注册
     */
    bool IsRegistered(const std::string& skycon);

private:
    ProfileBuilderRegistry() {}
    ~ProfileBuilderRegistry();

    static ProfileBuilderRegistry* m_pInstance;
    std::unordered_map<std::string, WeatherProfileBuilder*> m_Builders;
};

#endif // GLLIB_PROFILEBUILDERREGISTRY_H
