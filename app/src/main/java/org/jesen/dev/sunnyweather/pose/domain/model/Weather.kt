/**
 * 天气数据模型
 * 
 * 主要职责：
 * - 定义天气相关的数据结构，包括实时天气、7日预报、空气质量等
 * - 支持 JSON 序列化，用于解析 API 响应数据
 * 
 * 技术要点：
 * - 使用 @Serializable 注解支持 Kotlinx 序列化
 * - Realtime 包含实时天气数据（天气状况、温度、空气质量）
 * - Daily 包含7日预报数据（温度、天气状况、生活指数）
 * - Weather 作为聚合模型，合并实时天气和7日预报数据
 * - 使用 @SerialName 处理 API 响应中的下划线命名
 */
package org.jesen.dev.sunnyweather.pose.domain.model

import kotlinx.serialization.Serializable
import java.util.Date

@Serializable
data class AQI(
    val chn: Float
)

@Serializable
data class AirQuality(
    val aqi: AQI
)

@Serializable
data class Realtime(
    val skycon: String,
    val temperature: Float,
    @kotlinx.serialization.SerialName("air_quality") 
    val airQuality: AirQuality
)

@Serializable
data class RealtimeResponse(
    val status: String,
    val result: RealtimeResult
) {
    @Serializable
    data class RealtimeResult(
        val realtime: Realtime
    )
}

@Serializable
data class Temperature(
    val max: Float,
    val min: Float
)

@Serializable
data class Skycon(
    val value: String,
    val date: String
)

@Serializable
data class LifeDescription(
    val desc: String
)

@Serializable
data class LifeIndex(
    val coldRisk: List<LifeDescription>,
    val carWashing: List<LifeDescription>,
    val ultraviolet: List<LifeDescription>,
    val dressing: List<LifeDescription>
)

@Serializable
data class Daily(
    val temperature: List<Temperature>,
    val skycon: List<Skycon>,
    @kotlinx.serialization.SerialName("life_index") val lifeIndex: LifeIndex
)

@Serializable
data class DailyResponse(
    val status: String,
    val result: DailyResult
) {
    @Serializable
    data class DailyResult(
        val daily: Daily
    )
}

data class Weather(
    val realtime: Realtime,
    val daily: Daily
)