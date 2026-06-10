package org.jesen.dev.sunnyweather.relationdemo.navigation_demo

import androidx.compose.material3.adaptive.ExperimentalMaterial3AdaptiveApi
import androidx.compose.runtime.Composable
import androidx.navigation3.runtime.NavEntry
import androidx.navigation3.runtime.NavKey
import androidx.navigation3.runtime.rememberNavBackStack
import androidx.navigation3.ui.NavDisplay
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.decorator.AppBarDecorator
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.route.DialogRoute
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.route.HomeRoute
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.route.ProfileRoute
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.route.SettingsRoute
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.route.WeatherRoute
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.ui.screens.DialogScreen
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.ui.screens.HomeScreen
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.ui.screens.ProfileScreen
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.ui.screens.SettingsScreen
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.ui.screens.WeatherScreen

/**
 * Navigation3 高级特性演示主入口
 * 
 * 【Navigation3 核心概念】
 * 
 * 1. **NavKey**: 导航键，使用 `@Serializable` 标记的类，支持类型安全的参数传递
 * 
 * 2. **NavBackStack**: 导航返回栈，管理导航历史
 *    - rememberNavBackStack(initialKey):
 *    创建并记住导航栈
 *    - backStack.add(key): 导航到新页面
 *    - backStack.removeLastOrNull(): 返回上一页
 *    - backStack.clear(): 清空返回栈
 * 
 * 3. **NavDisplay**: 导航显示组件，负责根据当前栈状态渲染对应页面
 *    - backStack: 导航栈
 *    - onBack: 返回按钮回调
 *    - entryProvider: 根据 NavKey 提供对应的 NavEntry
 * 
 * 4. **NavEntry**: 导航条目，封装页面内容和元数据
 * 
 * 【版本支持】
 * - Navigation3 1.0.0+: 基础导航功能
 * - Navigation3 1.1.0+: SceneDecoratorStrategy、NavMetadata、OverlayScene 动画
 * - Navigation3 1.2.0-alpha02+: NavigationEvent、rememberNavigationEventState
 * - Navigation3 1.2.0-alpha03+: DeepLinkRequest、DeepLinkUri、DeepLinkMatcher
 * 
 * 【项目结构】
 * - route/: 路由键定义（NavKey）
 * - deeplink/: 深层链接匹配器
 * - decorator/: 场景装饰器
 * - metadata/: 元数据键定义
 * - ui/screens/: 页面组件
 */
@OptIn(ExperimentalMaterial3AdaptiveApi::class)
@Composable
fun Navigation3Demo(
    /** 初始路由键，默认为首页 */
    initialKey: NavKey = HomeRoute,
) {
    // 创建导航返回栈
    val backStack = rememberNavBackStack(initialKey)

    // 导航显示组件
    NavDisplay(
        backStack = backStack,
        onBack = { backStack.removeLastOrNull() },
        entryProvider = { key ->
            // 根据路由键提供对应的页面
            when (key) {
                is HomeRoute -> NavEntry(key) {
                    AppBarDecorator(
                        title = "Navigation3 Demo",
                        showBackButton = false,
                        onBack = { backStack.removeLastOrNull() }
                    ) {
                        HomeScreen(
                            onNavigateToWeather = { city ->
                                backStack.add(WeatherRoute(city))
                            },
                            onNavigateToSettings = {
                                backStack.add(SettingsRoute)
                            },
                            onNavigateToProfile = {
                                backStack.add(ProfileRoute)
                            },
                            onShowDialog = {
                                backStack.add(DialogRoute)
                            }
                        )
                    }
                }

                is WeatherRoute -> NavEntry(key) {
                    AppBarDecorator(
                        title = "${key.city} 天气",
                        showBackButton = true,
                        onBack = { backStack.removeLastOrNull() }
                    ) {
                        WeatherScreen(city = key.city)
                    }
                }

                is SettingsRoute -> NavEntry(
                    key
                ) {
                    AppBarDecorator(
                        title = "设置",
                        showBackButton = true,
                        onBack = { backStack.removeLastOrNull() }
                    ) {
                        SettingsScreen()
                    }
                }

                is ProfileRoute -> NavEntry(
                    key
                ) {
                    AppBarDecorator(
                        title = "个人中心",
                        showBackButton = true,
                        onBack = { backStack.removeLastOrNull() }
                    ) {
                        ProfileScreen()
                    }
                }

                is DialogRoute -> NavEntry(
                    key
                ) {
                    DialogScreen(
                        onDismiss = { backStack.removeLastOrNull() },
                        onConfirm = { backStack.removeLastOrNull() }
                    )
                }

                else -> error("Unknown key: $key")
            }
        }
    )
}