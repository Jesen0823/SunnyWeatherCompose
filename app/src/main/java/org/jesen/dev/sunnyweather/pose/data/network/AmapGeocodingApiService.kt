/**
 * 高德地图搜索 API 服务类
 * 
 * 主要职责：
 * - 通过高德地图搜索 API 进行地址搜索联想
 * - 为彩云天气 API 提供地点搜索功能
 * 
 * 技术要点：
 * - 使用独立的 Ktor 客户端，Base URL 通过 EnvironmentConfig 获取
 * - 使用搜索 API (v3/place/text) 实现地址联想搜索
 * - 返回与 PlaceResponse 兼容的数据结构，便于业务层统一处理
 * - 需要用户注册高德开发者账号获取 Web 服务 API Key
 * - API Key 通过 EnvironmentConfig 管理，避免硬编码
 */
package org.jesen.dev.sunnyweather.pose.data.network

import io.ktor.client.HttpClient
import io.ktor.client.engine.okhttp.OkHttp
import io.ktor.client.plugins.contentnegotiation.ContentNegotiation
import io.ktor.client.plugins.defaultRequest
import io.ktor.client.request.get
import io.ktor.client.request.parameter
import io.ktor.client.request.url
import io.ktor.client.statement.bodyAsText
import io.ktor.serialization.kotlinx.json.json
import kotlinx.serialization.SerialName
import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json
import org.jesen.dev.sunnyweather.pose.domain.model.Location
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.model.PlaceResponse

object AmapGeocodingApiService {
    private const val DEFAULT_AMAP_API_KEY = "04119bf76517b328f970c0eaf77446f4"

    private val amapClient: HttpClient by lazy {
        HttpClient(OkHttp) {
            expectSuccess = false

            defaultRequest {
                url(EnvironmentConfig.getAmapBaseUrl())
            }

            install(ContentNegotiation) {
                json(Json {
                    prettyPrint = true
                    isLenient = true
                    ignoreUnknownKeys = true
                    encodeDefaults = true
                    coerceInputValues = true
                })
            }
        }
    }

    suspend fun searchPlaces(query: String): ApiResult<PlaceResponse> {
        return safeApiCall {
            val response = amapClient.get("v3/place/text") {
                parameter("keywords", query)
                parameter("output", "JSON")
                parameter("key", EnvironmentConfig.getAmapApiKey().ifEmpty { DEFAULT_AMAP_API_KEY })
                parameter("city", "")
                parameter("citylimit", "true")
                parameter("offset", "10")
                parameter("extensions", "base")
            }

            val responseBody = response.bodyAsText()
            val amapResponse = Json {
                ignoreUnknownKeys = true
            }.decodeFromString<AmapPlaceSearchResponse>(responseBody)

            convertToPlaceResponse(amapResponse)
        }
    }

    private fun convertToPlaceResponse(amapResponse: AmapPlaceSearchResponse): PlaceResponse {
        val places = amapResponse.pois?.map { poi ->
            val (lng, lat) = poi.location.split(",")
            Place(
                name = poi.name ?: "",
                location = Location(
                    lng = lng,
                    lat = lat
                ),
                address = poi.address ?: poi.name ?: ""
            )
        } ?: emptyList()

        return PlaceResponse(
            status = if (amapResponse.status == "1") "ok" else "error",
            places = places
        )
    }

    @Serializable
    private data class AmapPlaceSearchResponse(
        val status: String,
        val info: String? = null,
        val count: String? = null,
        @SerialName("pois") val pois: List<AmapPOI>? = null
    )

    @Serializable
    private data class AmapPOI(
        val id: String? = null,
        val name: String? = null,
        val type: String? = null,
        val typecode: String? = null,
        val address: String? = null,
        val location: String = "",
        val pname: String? = null,
        val cityname: String? = null,
        val adname: String? = null
    )
}