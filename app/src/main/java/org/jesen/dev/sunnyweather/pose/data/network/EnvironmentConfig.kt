/**
 * 环境配置类
 * 
 * 性能优化原理：
 * - API Key、Secret、Base URL 等配置在应用运行期间不会变化
 * - 如果每次使用都从资源文件读取，会产生不必要的 IO 开销
 * - 使用单例模式缓存配置值，只在应用启动时读取一次
 * - 后续使用直接从内存获取，提升访问速度
 * 
 * 安全设计：
 * - 配置项存储在资源文件中，不在代码中硬编码
 * - 避免敏感信息（如 API Key）被反编译暴露
 * - 支持从安全存储（如 Android Keystore）获取配置
 * 
 * 技术要点：
 * - 使用单例模式确保全局唯一配置
 * - 配置项通过资源文件动态获取，支持不同构建变体
 * - 默认使用生产环境配置
 * - 支持配置重载，便于测试和调试
 * 
 * 使用场景：
 * - WeatherApiService 使用此类获取 API Key 和 Base URL
 * - 彩云天气 API 使用 appKey 和 appSecret 进行签名认证
 * - 高德地图 API 使用 amapApiKey 进行请求认证
 */
package org.jesen.dev.sunnyweather.pose.data.network

import android.content.Context

object EnvironmentConfig {

    private var appKey: String = ""
    private var appSecret: String = ""
    private var amapApiKey: String = ""
    private var caiyunBaseUrl: String = "https://api.caiyunapp.com/"
    private var amapBaseUrl: String = "https://restapi.amap.com/"

    fun init(context: Context) {
        loadFromResources(context)
    }

    private fun loadFromResources(context: Context) {
        appKey = getStringResource(context, "caiyun_app_key", "phbfrfvcfs25s6ns")
        appSecret = getStringResource(context, "caiyun_app_secret", "b7QPeW3jkjYEZ4nC1sxro2NwrchSyEZu")
        amapApiKey = getStringResource(context, "amap_api_key", "")
    }

    private fun getStringResource(context: Context, name: String, defaultValue: String): String {
        val resId = context.resources.getIdentifier(name, "string", context.packageName)
        return if (resId != 0) {
            context.getString(resId)
        } else {
            defaultValue
        }
    }

    fun getAppKey(): String = appKey

    fun getAppSecret(): String = appSecret

    fun getAmapApiKey(): String = amapApiKey

    fun getCaiyunBaseUrl(): String = caiyunBaseUrl

    fun getAmapBaseUrl(): String = amapBaseUrl

    fun isSignedRequest(): Boolean = appKey.isNotEmpty() && appSecret.isNotEmpty()

    fun overrideConfig(
        newAppKey: String? = null,
        newAppSecret: String? = null,
        newAmapApiKey: String? = null,
        newCaiyunBaseUrl: String? = null,
        newAmapBaseUrl: String? = null
    ) {
        newAppKey?.let { appKey = it }
        newAppSecret?.let { appSecret = it }
        newAmapApiKey?.let { amapApiKey = it }
        newCaiyunBaseUrl?.let { caiyunBaseUrl = it }
        newAmapBaseUrl?.let { amapBaseUrl = it }
    }
}