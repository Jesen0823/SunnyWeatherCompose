/**
 * 天气仓库接口
 * 
 * 主要职责：
 * - 定义天气数据访问的抽象接口
 * - 作为数据层与领域层的契约
 * - 支持依赖倒置，便于测试和替换实现
 * 
 * 技术要点：
 * - 使用接口定义数据访问契约
 * - 所有方法返回 ApiResult 或 Flow，支持响应式编程
 * - 包含天气数据获取和城市存储两大功能模块
 * - 实现类可切换真实网络/模拟数据/缓存等不同数据源
 */
package org.jesen.dev.sunnyweather.pose.domain.repository

import kotlinx.coroutines.flow.Flow
import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.domain.model.DailyResponse
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.model.PlaceResponse
import org.jesen.dev.sunnyweather.pose.domain.model.RealtimeResponse
import org.jesen.dev.sunnyweather.pose.domain.model.Weather

interface WeatherRepository {
    suspend fun searchPlaces(query: String): ApiResult<PlaceResponse>
    suspend fun getRealtimeWeather(lng: String, lat: String): ApiResult<RealtimeResponse>
    suspend fun getDailyWeather(lng: String, lat: String): ApiResult<DailyResponse>
    suspend fun fetchWeather(lng: String, lat: String): ApiResult<Weather>
    suspend fun savePlace(place: Place)
    fun getSavedPlace(): Flow<Place?>
    fun getSavedPlaceList(): Flow<List<Place>>
    fun isPlaceSaved(): Flow<Boolean>
    fun isSpecificPlaceSaved(placeName: String): Flow<Boolean>
    suspend fun clearPlace()
}