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