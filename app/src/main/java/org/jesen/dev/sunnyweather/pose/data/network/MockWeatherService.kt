/**
 * 模拟天气数据服务类
 * 
 * 主要职责：
 * - 提供模拟的天气和地点数据
 * - 支持开发和测试环境，无需真实网络请求
 * 
 * 技术要点：
 * - 返回与真实 API 相同的数据结构，便于切换使用
 * - 模拟数据包含实时天气、7日预报、生活指数等
 * - 城市搜索返回固定的北京相关地点列表
 */
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
    
    fun getMockHourlyWeather(): HourlyResponse {
        return HourlyResponse(
            status = "ok",
            result = HourlyResponse.HourlyResult(
                hourly = Hourly(
                    skycon = List(24) { HourlySkyconItem(datetime = "2026-07-07T${it.toString().padStart(2, '0')}:00+08:00", value = "CLEAR_DAY") },
                    temperature = List(24) { HourlyTemperatureItem(datetime = "2026-07-07T${it.toString().padStart(2, '0')}:00+08:00", value = 25f - it * 0.5f) },
                    precipitation = List(24) { HourlyPrecipitationItem(datetime = "2026-07-07T${it.toString().padStart(2, '0')}:00+08:00", value = 0f, probability = 0f) },
                    wind = List(24) { HourlyWindItem(
                        datetime = "2026-07-07T${it.toString().padStart(2, '0')}:00+08:00",
                        speed = Wind(speed = 3f + it * 0.1f, direction = 180f),
                        direction = Wind(speed = 180f, direction = 0f)
                    )}
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
