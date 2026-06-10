package org.jesen.dev.sunnyweather.relationdemo.navigation_demo.route

import androidx.navigation3.runtime.NavKey
import kotlinx.serialization.Serializable

/**
 * Navigation3 路由键定义
 * 
 * Navigation3 使用 `@Serializable` 标记的类作为路由键（NavKey），
 * 支持类型安全的参数传递和 DeepLink 解析。
 * 
 * 【版本支持】Navigation3 1.0.0+
 * 
 * 【使用场景】
 * - 定义应用的各个页面路由
 * - 支持带参数的路由（如 WeatherRoute 需要城市名称参数）
 * - 支持 DeepLink 深层链接跳转
 */

/**
 * 首页路由
 * 无参数的路由使用 data object 声明
 */
@Serializable
data object HomeRoute : NavKey

/**
 * 天气页面路由
 * 带参数的路由使用 data class 声明，参数会被自动序列化/反序列化
 * 
 * @param city 城市名称，用于显示天气信息
 */
@Serializable
data class WeatherRoute(val city: String) : NavKey

/**
 * 设置页面路由
 */
@Serializable
data object SettingsRoute : NavKey

/**
 * 个人中心路由
 */
@Serializable
data object ProfileRoute : NavKey

/**
 * 弹窗路由（OverlayScene）
 * 
 * 【OverlayScene 说明】
 * OverlayScene 是 Navigation3 1.1.0+ 引入的叠加场景概念，
 * 用于实现弹窗、对话框等悬浮在主内容之上的 UI 元素。
 * 
 * 【特性】
 * - 支持进入/退出动画
 * - 支持 onRemoved 回调，在场景从返回栈移除但离开组合之前调用
 * - 可以通过 SceneStrategy 配置叠加行为
 */
@Serializable
data object DialogRoute : NavKey