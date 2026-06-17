/**
 * 获取天气用例
 * 
 * 主要职责：
 * - 根据经纬度获取实时天气和7日预报数据
 * - 合并实时天气和预报数据，返回统一的 Weather 对象
 * 
 * 技术要点：
 * - 使用 operator fun invoke() 简化调用，可直接作为函数使用
 * - 调用 WeatherRepository 的 fetchWeather() 方法获取数据
 * - 返回 ApiResult<Weather>，统一处理成功/失败/加载状态
 * - 是一个 suspend 函数，需要在协程中调用
 */
package org.jesen.dev.sunnyweather.pose.domain.usecase

import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.domain.model.Weather
class FetchWeatherUseCase(private val repository: WeatherRepository) {
    suspend operator fun invoke(lng: String, lat: String): ApiResult<Weather> {
        return repository.fetchWeather(lng, lat)
    }
}
