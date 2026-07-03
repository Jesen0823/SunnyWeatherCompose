/**
 * LocalProvider 跨组件状态提供器
 * 
 * 主要职责：
 * - 使用 CompositionLocal 在组件树中共享天气数据和城市信息
 * - 避免 props drilling（逐层传递参数）
 * - 提供类型安全的跨组件访问
 * 
 * 性能优化原理：
 * - 传统方式：WeatherScreen → CurrentWeatherCard(weather, place) → 子组件(weather, place)
 *   每次传递都需要在函数签名中声明参数，修改时需要修改所有层级
 * - CompositionLocal 方式：WeatherAndPlaceProvider 提供值 → 子组件直接获取
 *   只需在 Provider 处修改一次，所有子组件自动获得最新数据
 * - 减少函数参数传递，降低耦合度，提升代码可维护性
 * 
 * 技术要点：
 * - CompositionLocal 是 Compose 提供的跨组件传值机制
 * - 适用于在多个层级的子组件间共享数据（如主题、语言、全局状态）
 * - 不建议用于频繁变化的状态，适合配置类数据
 * - 使用 LocalProvider 包裹组件树提供值，子组件通过 LocalXXX.current 获取
 * 
 * 使用场景：
 * - WeatherScreen 及其子组件（CurrentWeatherCard、ForecastSection、LifeIndexSection）
 *   需要共享 weather 和 place 数据
 * - 替代逐层传递参数的方式，提升代码可维护性
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components

import androidx.compose.runtime.Composable
import androidx.compose.runtime.CompositionLocalProvider
import androidx.compose.runtime.ReadOnlyComposable
import androidx.compose.runtime.staticCompositionLocalOf
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.model.Weather

/**
 * 天气数据 CompositionLocal
 * 
 * 使用方式：
 * 在父组件中使用 WeatherProvider 提供天气数据：
 * ```kotlin
 * WeatherProvider(weather = weather) {
 *     // 子组件可以通过 LocalWeather.current 获取天气数据
 * }
 * ```
 * 
 * 在子组件中获取：
 * ```kotlin
 * val weather = LocalWeather.current
 * ```
 */
val LocalWeather = staticCompositionLocalOf<Weather?> { null }

/**
 * 城市信息 CompositionLocal
 * 
 * 使用方式：
 * 在父组件中使用 PlaceProvider 提供城市数据：
 * ```kotlin
 * PlaceProvider(place = place) {
 *     // 子组件可以通过 LocalPlace.current 获取城市数据
 * }
 * ```
 * 
 * 在子组件中获取：
 * ```kotlin
 * val place = LocalPlace.current
 * ```
 */
val LocalPlace = staticCompositionLocalOf<Place?> { null }

/**
 * 天气数据提供者组件
 * 
 * @param weather 天气数据
 * @param content 子组件内容
 */
@Composable
fun WeatherProvider(
    weather: Weather,
    content: @Composable () -> Unit
) {
    CompositionLocalProvider(
        LocalWeather provides weather,
        content = content
    )
}

/**
 * 城市信息提供者组件
 * 
 * @param place 城市数据
 * @param content 子组件内容
 */
@Composable
fun PlaceProvider(
    place: Place,
    content: @Composable () -> Unit
) {
    CompositionLocalProvider(
        LocalPlace provides place,
        content = content
    )
}

/**
 * 同时提供天气和城市数据的组合提供者
 * 
 * @param weather 天气数据
 * @param place 城市数据
 * @param content 子组件内容
 */
@Composable
fun WeatherAndPlaceProvider(
    weather: Weather,
    place: Place,
    content: @Composable () -> Unit
) {
    CompositionLocalProvider(
        LocalWeather provides weather,
        LocalPlace provides place,
        content = content
    )
}

/**
 * 安全获取天气数据的扩展方法
 * 
 * 如果 LocalWeather 未提供值，会抛出 IllegalStateException
 * 
 * @return 天气数据
 * @throws IllegalStateException 如果未在 WeatherProvider 中使用
 */
@Composable
@ReadOnlyComposable
fun requireLocalWeather(): Weather {
    return LocalWeather.current ?: throw IllegalStateException(
        "Weather not provided! Make sure to wrap your composable with WeatherProvider."
    )
}

/**
 * 安全获取城市数据的扩展方法
 * 
 * 如果 LocalPlace 未提供值，会抛出 IllegalStateException
 * 
 * @return 城市数据
 * @throws IllegalStateException 如果未在 PlaceProvider 中使用
 */
@Composable
@ReadOnlyComposable
fun requireLocalPlace(): Place {
    return LocalPlace.current ?: throw IllegalStateException(
        "Place not provided! Make sure to wrap your composable with PlaceProvider."
    )
}