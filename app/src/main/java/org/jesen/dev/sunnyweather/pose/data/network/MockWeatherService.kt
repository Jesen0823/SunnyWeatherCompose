package org.jesen.dev.sunnyweather.pose.data.network

import android.os.Build
import androidx.annotation.RequiresApi
import org.jesen.dev.sunnyweather.pose.domain.model.*
import java.time.LocalDate

object MockWeatherService {
    
    fun getMockRealtimeWeather(): RealtimeResponse {
        return RealtimeResponse(
            status = "ok",
            result = RealtimeResponse.RealtimeResult(
                realtime = Realtime(
                    skycon = "CLEAR_DAY",
                    temperature = 26.5f,
                    airQuality = AirQuality(
                        aqi = AQI(chn = 45f)
                    )
                )
            )
        )
    }
    
    @RequiresApi(Build.VERSION_CODES.O)
    fun getMockDailyWeather(): DailyResponse {
        val today = LocalDate.now()
        return DailyResponse(
            status = "ok",
            result = DailyResponse.DailyResult(
                daily = Daily(
                    temperature = listOf(
                        Temperature(max = 30f, min = 22f),
                        Temperature(max = 29f, min = 21f),
                        Temperature(max = 31f, min = 23f),
                        Temperature(max = 28f, min = 20f),
                        Temperature(max = 27f, min = 19f),
                        Temperature(max = 29f, min = 21f),
                        Temperature(max = 30f, min = 22f)
                    ),
                    skycon = listOf(
                        Skycon(value = "CLEAR_DAY", date = today.toString()),
                        Skycon(value = "PARTLY_CLOUDY_DAY", date = today.plusDays(1).toString()),
                        Skycon(value = "CLOUDY", date = today.plusDays(2).toString()),
                        Skycon(value = "RAIN", date = today.plusDays(3).toString()),
                        Skycon(value = "CLEAR_NIGHT", date = today.plusDays(4).toString()),
                        Skycon(value = "CLEAR_DAY", date = today.plusDays(5).toString()),
                        Skycon(value = "PARTLY_CLOUDY_DAY", date = today.plusDays(6).toString())
                    ),
                    lifeIndex = LifeIndex(
                        coldRisk = listOf(LifeDescription(desc = "较冷")),
                        carWashing = listOf(LifeDescription(desc = "较适宜")),
                        ultraviolet = listOf(LifeDescription(desc = "中等")),
                        dressing = listOf(LifeDescription(desc = "短袖"))
                    )
                )
            )
        )
    }
    
    fun getMockPlaceResponse(query: String): PlaceResponse {
        return PlaceResponse(
            status = "ok",
            places = listOf(
                Place(
                    name = "北京",
                    location = Location(lng = "116.4074", lat = "39.9042"),
                    address = "北京市"
                ),
                Place(
                    name = "北京朝阳",
                    location = Location(lng = "116.4231", lat = "39.9234"),
                    address = "北京市朝阳区",
                ),
                Place(
                    name = "北京海淀",
                    location = Location(lng = "116.2993", lat = "39.9962"),
                    address = "北京市海淀区"
                )
            )
        )
    }
}
