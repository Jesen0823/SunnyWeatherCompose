package org.jesen.dev.sunnyweather.relationdemo.navigation_demo.deeplink

import androidx.navigation3.runtime.NavKey
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.route.HomeRoute
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.route.SettingsRoute
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.route.WeatherRoute

/**
 * DeepLink 匹配器实现
 * 
 * 【DeepLinkRequest / DeepLinkUri / DeepLinkMatcher 说明】
 * 
 * Navigation3 1.2.0-alpha03+ 引入了完整的深层链接支持：
 * 
 * 1. **DeepLinkUri**: android.net.Uri 的 Kotlin 多平台版本，
 *    用于解析深层链接 URL
 * 
 * 2. **DeepLinkRequest**: 表示请求的深层链接，包含 DeepLinkUri 和可选的 
 *    mimeType 或 action（仅限 Android）
 * 
 * 3. **DeepLinkMatcher**: 表示可与 DeepLinkRequest 匹配的深层链接，
 *    每个匹配器与一个导航键关联。匹配成功返回包含导航键的 MatchResult
 * 
 * 【UriDeepLinkMatcher】
 * DeepLinkMatcher 的一个实现，通过将 DeepLinkUri 与可序列化的导航键匹配
 * 来处理深层链接，支持基元类型或基元集合的深层链接参数。
 * 
 * 【使用场景】
 * - 从外部 URL 跳转到应用内特定页面
 * - 支持应用间跳转
 * - 支持通知点击跳转
 * 
 * 【示例 URL】
 * - sunnyweather://home
 * - sunnyweather://settings
 * - sunnyweather://weather?city=Beijing
 */
object DeepLinkMatchers {
    
    /**
     * 根据 URL 字符串匹配对应的导航键
     * 
     * @param uriString 深层链接 URL 字符串
     * @return 匹配到的 NavKey，如果不匹配返回 null
     */
    fun match(uriString: String): NavKey? {
        // 使用 Android 原生 Uri 解析（实际项目中可使用 Navigation3 的 DeepLinkUri）
        val uri = android.net.Uri.parse(uriString)
        val scheme = uri.scheme
        val host = uri.host
        
        // 验证 scheme 是否匹配
        if (scheme != "sunnyweather") {
            return null
        }
        
        // 根据 host 匹配不同的路由
        return when (host) {
            "home" -> HomeRoute
            "settings" -> SettingsRoute
            "weather" -> {
                // 从 URL 参数中提取城市名称
                val city = uri.getQueryParameter("city") ?: "Unknown"
                WeatherRoute(
                    city
                )
            }
            else -> null
        }
    }
    
    /**
     * 【Navigation3 1.2.0-alpha03+ 推荐用法】
     * 使用 UriDeepLinkMatcher 进行类型安全的深层链接匹配
     * 
     * 注意：当前示例使用简化实现，完整实现需要使用：
     * 
     * val weatherMatcher = UriDeepLinkMatcher(
     *     uriPattern = "sunnyweather://weather?city={city}",
     *     serializer = WeatherRoute.serializer()
     * )
     * 
     * val request = DeepLinkRequest.fromUri(DeepLinkUri.parse(uriString))
     * val result = weatherMatcher.match(request)
     * return result?.key
     */
}