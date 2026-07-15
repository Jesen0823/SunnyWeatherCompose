/**
 * 导航路由常量定义类
 * 
 * 主要职责：
 * - 定义应用中页面的路由路径常量
 * - 便于统一管理和引用页面路由
 * 
 * 技术要点：
 * - 使用 object 单例模式定义常量
 * - 包含 PLACE_SCREEN、WEATHER_SCREEN 两个路由常量
 */
package org.jesen.dev.sunnyweather.pose.navigation

object NavigationRoutes {
    const val PLACE_SCREEN = "place_screen"
    const val WEATHER_SCREEN = "weather_screen"
}