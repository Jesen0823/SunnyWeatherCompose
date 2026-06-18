/**
 * 天气数据仓库类
 * 
 * 主要职责：
 * - 作为数据层与领域层的中间层，封装数据获取逻辑
 * - 协调网络请求和本地存储操作
 * - 提供统一的数据访问接口给 UseCase 使用
 * 
 * 技术要点：
 * - 组合 WeatherApiService 和 PlaceStore，实现数据来源的统一管理
 * - fetchWeather 方法合并实时天气和7日预报数据，返回统一的 Weather 对象
 * - 所有数据操作方法都是 suspend 函数或返回 Flow，支持协程
 */
package org.jesen.dev.sunnyweather.pose.data.repository

import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.data.network.WeatherApiService
import org.jesen.dev.sunnyweather.pose.data.store.PlaceStore
import org.jesen.dev.sunnyweather.pose.domain.model.DailyResponse
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.model.PlaceResponse
import org.jesen.dev.sunnyweather.pose.domain.model.RealtimeResponse
import org.jesen.dev.sunnyweather.pose.domain.model.Weather

class WeatherRepository(
    private val apiService: WeatherApiService,
    private val placeStore: PlaceStore
) {
    suspend fun searchPlaces(query: String): ApiResult<PlaceResponse> {
        return apiService.searchPlaces(query)
    }
    
    suspend fun getRealtimeWeather(lng: String, lat: String): ApiResult<RealtimeResponse> {
        return apiService.getRealtimeWeather(lng, lat)
    }
    
    suspend fun getDailyWeather(lng: String, lat: String): ApiResult<DailyResponse> {
        return apiService.getDailyWeather(lng, lat)
    }
    
    suspend fun fetchWeather(lng: String, lat: String): ApiResult<Weather> {
        return when (val realtimeResult = apiService.getRealtimeWeather(lng, lat)) {
            is ApiResult.Success -> {
                when (val dailyResult = apiService.getDailyWeather(lng, lat)) {
                    is ApiResult.Success -> {
                        ApiResult.Success(
                            Weather(
                                realtime = realtimeResult.data.result.realtime,
                                daily = dailyResult.data.result.daily
                            )
                        )
                    }
                    is ApiResult.Error -> dailyResult
                    is ApiResult.Loading -> ApiResult.Loading
                }
            }
            is ApiResult.Error -> realtimeResult
            is ApiResult.Loading -> ApiResult.Loading
        }
    }
    
    suspend fun savePlace(place: Place) {
        placeStore.savePlace(place)
    }
    
    fun getSavedPlace() = placeStore.getSavedPlace()
    
    fun getSavedPlaceList() = placeStore.getPlaceList()
    
    fun isPlaceSaved() = placeStore.isPlaceSaved()

    fun isSpecificPlaceSaved(placeName: String) = placeStore.isSpecificPlaceSaved(placeName)
    
    suspend fun clearPlace() {
        placeStore.clearPlace()
    }
}