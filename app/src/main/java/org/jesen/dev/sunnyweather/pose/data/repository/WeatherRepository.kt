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
    
    fun isPlaceSaved() = placeStore.isPlaceSaved()
    
    suspend fun clearPlace() {
        placeStore.clearPlace()
    }
}