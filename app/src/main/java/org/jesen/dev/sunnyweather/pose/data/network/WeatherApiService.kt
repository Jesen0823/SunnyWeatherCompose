package org.jesen.dev.sunnyweather.pose.data.network

import io.ktor.client.call.body
import io.ktor.client.request.get
import io.ktor.client.request.parameter
import org.jesen.dev.sunnyweather.pose.domain.model.DailyResponse
import org.jesen.dev.sunnyweather.pose.domain.model.PlaceResponse
import org.jesen.dev.sunnyweather.pose.domain.model.RealtimeResponse

private const val TOKEN = "TAkhjf8d1nlSlspN"

class WeatherApiService {
    suspend fun searchPlaces(query: String): ApiResult<PlaceResponse> {
        if (ApiConfig.USE_MOCK_DATA) {
            return ApiResult.Success(MockWeatherService.getMockPlaceResponse(query))
        }
        return safeApiCall {
            KtorClient.instance.get("v2/place") {
                parameter("token", TOKEN)
                parameter("lang", "zh_CN")
                parameter("query", query)
            }.body()
        }
    }
    
    suspend fun getRealtimeWeather(lng: String, lat: String): ApiResult<RealtimeResponse> {
        if (ApiConfig.USE_MOCK_DATA) {
            return ApiResult.Success(MockWeatherService.getMockRealtimeWeather())
        }
        return safeApiCall {
            KtorClient.instance.get("v2.5/$TOKEN/$lng,$lat/realtime.json").body()
        }
    }
    
    suspend fun getDailyWeather(lng: String, lat: String): ApiResult<DailyResponse> {
        if (ApiConfig.USE_MOCK_DATA) {
            return ApiResult.Success(MockWeatherService.getMockDailyWeather())
        }
        return safeApiCall {
            KtorClient.instance.get("v2.5/$TOKEN/$lng,$lat/daily.json").body()
        }
    }
}