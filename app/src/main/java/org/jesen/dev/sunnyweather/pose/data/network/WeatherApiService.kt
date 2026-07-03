/**
 * 天气 API 服务类
 * 
 * 主要职责：
 * - 封装彩云天气 API v2.6 的天气数据请求
 * - 封装高德地图搜索 API 用于地点搜索
 * - 根据配置决定返回真实数据或模拟数据
 * - 统一处理 API 认证签名
 * 
 * 技术要点：
 * - 城市搜索使用高德地图搜索 API（彩云天气已移除地点搜索接口）
 * - 实时天气和7日预报使用彩云天气 API v2.6，支持 App Key & App Secret 认证
 * - 通过 ApiConfig.USE_MOCK_DATA 控制数据来源
 * - 使用 Ktor 客户端发送 HTTP 请求，自动处理 JSON 序列化
 * - 所有方法返回 ApiResult，统一错误处理
 * - 使用 EnvironmentConfig 管理 API 配置
 * - 使用 CaiyunSignature 生成签名认证头
 */
package org.jesen.dev.sunnyweather.pose.data.network

import android.util.Log
import io.ktor.client.call.body
import io.ktor.client.request.get
import io.ktor.client.request.header
import io.ktor.client.request.parameter
import org.jesen.dev.sunnyweather.pose.domain.model.DailyResponse
import org.jesen.dev.sunnyweather.pose.domain.model.PlaceResponse
import org.jesen.dev.sunnyweather.pose.domain.model.RealtimeResponse

class WeatherApiService {

    suspend fun searchPlaces(query: String): ApiResult<PlaceResponse> {
        if (ApiConfig.USE_MOCK_DATA) {
            return ApiResult.Success(MockWeatherService.getMockPlaceResponse(query))
        }
        return AmapGeocodingApiService.searchPlaces(query)
    }

    suspend fun getRealtimeWeather(lng: String, lat: String): ApiResult<RealtimeResponse> {
        if (ApiConfig.USE_MOCK_DATA) {
            return ApiResult.Success(MockWeatherService.getMockRealtimeWeather())
        }

        return safeApiCall {
            val path = "v2.6/${EnvironmentConfig.getAppKey()}/$lng,$lat/realtime"
            val response = executeSignedRequest(path)

            val rawBody = response.body<String>()
            Log.d("WeatherApiService", "getRealtimeWeather raw response: $rawBody")

            response.body()
        }
    }

    suspend fun getDailyWeather(lng: String, lat: String): ApiResult<DailyResponse> {
        if (ApiConfig.USE_MOCK_DATA) {
            return ApiResult.Success(MockWeatherService.getMockDailyWeather())
        }

        return safeApiCall {
            val path = "v2.6/${EnvironmentConfig.getAppKey()}/$lng,$lat/daily"
            val response = executeSignedRequest(path)

            val rawBody = response.body<String>()
            Log.d("WeatherApiService", "getDailyWeather raw response: $rawBody")

            response.body()
        }
    }

    private suspend fun executeSignedRequest(
        path: String,
        method: String = "GET",
        queryParams: Map<String, String> = emptyMap()
    ) = KtorClient.instance.get(path) {
        if (EnvironmentConfig.isSignedRequest()) {
            val nonce = CaiyunSignature.generateNonce()
            val timestamp = CaiyunSignature.generateTimestamp()
            val signature = CaiyunSignature.sign(
                EnvironmentConfig.getAppKey(),
                EnvironmentConfig.getAppSecret(),
                method,
                "/$path",
                nonce,
                timestamp,
                queryParams
            )
            header("x-cy-nonce", nonce)
            header("x-cy-timestamp", timestamp.toString())
            header("x-cy-signature", signature)
        }
        queryParams.forEach { (key, value) ->
            parameter(key, value)
        }
    }
}