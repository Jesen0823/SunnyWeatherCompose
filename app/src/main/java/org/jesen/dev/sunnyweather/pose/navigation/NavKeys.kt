/**
 * 导航键定义类
 * 
 * 主要职责：
 * - 定义应用中所有页面的导航键
 * - 支持 Navigation3 的类型安全导航
 * 
 * 技术要点：
 * - 使用 data object 定义每个页面的唯一导航键
 * - 标注 @Serializable 支持序列化
 * - 包含 WeatherKey、PlaceKey、UnknownKey、SettingsKey 四个导航键
 */
package org.jesen.dev.sunnyweather.pose.navigation

import androidx.navigation3.runtime.NavKey
import kotlinx.serialization.Serializable

@Serializable
data object WeatherKey : NavKey

@Serializable
data object PlaceKey : NavKey

@Serializable
data object UnknownKey : NavKey

@Serializable
data object SettingsKey : NavKey

@Serializable
data object PlaceManagerKey : NavKey

@Serializable
data object AboutUsKey : NavKey
