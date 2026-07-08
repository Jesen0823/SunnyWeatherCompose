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

/**
 * AQI（空气质量指数）
 * 
 * @param chn 中国国标 AQI 值
 * @param usa 美国标准 AQI 值（可选）
 */
@Serializable
data class AQI(
    val chn: Float,
    val usa: Float? = null
)

/**
 * 空气质量数据
 * 
 * @param pm25 PM2.5 浓度(μg/m3)
 * @param pm10 PM10 浓度(μg/m3)
 * @param o3 臭氧浓度(μg/m3)
 * @param so2 二氧化硫浓度(μg/m3)
 * @param no2 二氧化氮浓度(μg/m3)
 * @param co 一氧化碳浓度(mg/m3)
 * @param aqi AQI指数（中国/美国标准）
 */
@Serializable
data class AirQuality(
    val pm25: Float? = null,
    val pm10: Float? = null,
    val o3: Float? = null,
    val so2: Float? = null,
    val no2: Float? = null,
    val co: Float? = null,
    val aqi: AQI
)

/**
 * 风速数据
 * 
 * @param speed 风速(m/s)
 * @param direction 风向(度，0-360)
 */
@Serializable
data class Wind(
    val speed: Float,
    val direction: Float
)

/**
 * 降水数据
 * 
 * @param local 本地降水数据
 * @param nearest 最近降水数据
 */
@Serializable
data class Precipitation(
    @kotlinx.serialization.SerialName("local") val local: PrecipitationLocal,
    @kotlinx.serialization.SerialName("nearest") val nearest: PrecipitationNearest
)

/**
 * 本地降水数据
 * 
 * @param status 数据状态
 * @param datasource 数据源（如雷达）
 * @param intensity 降水强度(mm/h)
 */
@Serializable
data class PrecipitationLocal(
    val status: String,
    val datasource: String? = null,
    val intensity: Float
)

/**
 * 最近降水数据
 * 
 * @param status 数据状态
 * @param distance 最近降水距离(km)
 * @param intensity 降水强度(mm/h)
 */
@Serializable
data class PrecipitationNearest(
    val status: String,
    val distance: Float,
    val intensity: Float
)

/**
 * 生活指数单项数据
 * 
 * @param index 指数等级（1-5）
 * @param desc 指数描述（自然语言）
 */
@Serializable
data class LifeIndexItem(
    val index: Int,
    val desc: String
)

/**
 * 生活指数描述（仅包含描述字段）
 * 
 * @param desc 指数描述（自然语言）
 */
@Serializable
data class LifeDescription(
    val desc: String
)

/**
 * 实时天气数据
 * 
 * @param skycon 天气现象代码（如 CLEAR_DAY, PARTLY_CLOUDY_DAY 等）
 * @param temperature 实时温度(°C)
 * @param humidity 相对湿度(0-1)
 * @param cloudrate 云量(0-1)
 * @param visibility 能见度(km)
 * @param dswrf 向下短波辐射通量(W/M2)
 * @param wind 风速风向数据
 * @param pressure 地面气压(Pa)
 * @param apparentTemperature 体感温度(°C)
 * @param precipitation 降水数据
 * @param airQuality 空气质量数据
 */
@Serializable
data class Realtime(
    val skycon: String,
    val temperature: Float,
    val humidity: Float? = null,
    val cloudrate: Float? = null,
    val visibility: Float? = null,
    val dswrf: Float? = null,
    val wind: Wind? = null,
    val pressure: Float? = null,
    @kotlinx.serialization.SerialName("apparent_temperature") val apparentTemperature: Float? = null,
    val precipitation: Precipitation? = null,
    @kotlinx.serialization.SerialName("air_quality") 
    val airQuality: AirQuality
)

/**
 * 实时天气响应
 * 
 * @param status 返回状态（ok/failed）
 * @param result 结果数据
 */
@Serializable
data class RealtimeResponse(
    val status: String,
    val result: RealtimeResult
) {
    /**
     * 实时天气结果
     * 
     * @param realtime 实时天气数据
     */
    @Serializable
    data class RealtimeResult(
        val realtime: Realtime
    )
}

/**
 * 温度数据（每日）
 * 
 * @param max 最高温度(°C)
 * @param min 最低温度(°C)
 */
@Serializable
data class Temperature(
    val max: Float,
    val min: Float
)

/**
 * 天气现象（每日）
 * 
 * @param value 天气现象代码（如 CLEAR_DAY, PARTLY_CLOUDY_DAY 等）
 * @param date 日期
 */
@Serializable
data class Skycon(
    val value: String,
    val date: String
)

/**
 * 生活指数数据
 * 
 * @param coldRisk 感冒风险指数
 * @param carWashing 洗车指数
 * @param ultraviolet 紫外线指数
 * @param dressing 穿衣指数
 * @param comfort 舒适度指数（可选）
 */
@Serializable
data class LifeIndex(
    val coldRisk: List<LifeDescription>,
    val carWashing: List<LifeDescription>,
    val ultraviolet: List<LifeDescription>,
    val dressing: List<LifeDescription>,
    @kotlinx.serialization.SerialName("comfort") val comfort: List<LifeDescription>? = null
)

/**
 * 降水概率数据（每日）
 * 
 * @param max 最大降水量(mm)
 * @param min 最小降水量(mm)
 * @param avg 平均降水量(mm)
 * @param probability 降水概率(%)
 */
@Serializable
data class PrecipitationProbability(
    val max: Float,
    val min: Float,
    val avg: Float,
    val probability: Float? = null
)

/**
 * 每日风速数据（包含最大/最小/平均风速）
 * 
 * @param date 日期
 * @param max 最大风速
 * @param min 最小风速
 * @param avg 平均风速
 */
@Serializable
data class DailyWindItem(
    val date: String,
    val max: Wind,
    val min: Wind,
    val avg: Wind
)

/**
 * 日出时间数据
 * 
 * @param time 日出时间（格式：HH:mm，当地时区）
 */
@Serializable
data class Sunrise(
    val time: String
)

/**
 * 日落时间数据
 * 
 * @param time 日落时间（格式：HH:mm，当地时区）
 */
@Serializable
data class Sunset(
    val time: String
)

/**
 * 天文数据（日出日落）
 * 
 * @param date 日期
 * @param sunrise 日出时间
 * @param sunset 日落时间
 */
@Serializable
data class Astro(
    val date: String,
    val sunrise: Sunrise,
    val sunset: Sunset
)

/**
 * 每日预报数据
 * 
 * @param temperature 全天地表2米气温（最高/最低）
 * @param skycon 全天主要天气现象
 * @param lifeIndex 生活指数
 * @param precipitation 全天降水数据（最大/最小/平均/概率）
 * @param temperature08h20h 白天（08-20时）地表2米气温
 * @param temperature20h32h 夜晚（20-次日08时）地表2米气温
 * @param wind 全天地表10米风速（最大/最小/平均）
 * @param wind08h20h 白天（08-20时）地表10米风速
 * @param wind20h32h 夜晚（20-次日08时）地表10米风速
 * @param precipitation08h20h 白天（08-20时）降水数据
 * @param precipitation20h32h 夜晚（20-次日08时）降水数据
 * @param humidity 地表2米相对湿度（最大/最小/平均，0-1）
 * @param cloudrate 云量（最大/最小/平均，0-1）
 * @param pressure 地面气压（最大/最小/平均，Pa）
 * @param visibility 地表水平能见度（最大/最小/平均，km）
 * @param dswrf 向下短波辐射通量（最大/最小/平均，W/M2）
 * @param skycon08h20h 白天（08-20时）主要天气现象
 * @param skycon20h32h 夜晚（20-次日08时）主要天气现象
 * @param astro 日出日落时间数据
 */
@Serializable
data class Daily(
    val temperature: List<Temperature>,
    val skycon: List<Skycon>,
    @kotlinx.serialization.SerialName("life_index") val lifeIndex: LifeIndex,
    val precipitation: List<PrecipitationProbability>? = null,
    @kotlinx.serialization.SerialName("temperature_08h_20h") val temperature08h20h: List<Temperature>? = null,
    @kotlinx.serialization.SerialName("temperature_20h_32h") val temperature20h32h: List<Temperature>? = null,
    val wind: List<DailyWindItem>? = null,
    @kotlinx.serialization.SerialName("wind_08h_20h") val wind08h20h: List<DailyWindItem>? = null,
    @kotlinx.serialization.SerialName("wind_20h_32h") val wind20h32h: List<DailyWindItem>? = null,
    @kotlinx.serialization.SerialName("precipitation_08h_20h") val precipitation08h20h: List<PrecipitationProbability>? = null,
    @kotlinx.serialization.SerialName("precipitation_20h_32h") val precipitation20h32h: List<PrecipitationProbability>? = null,
    val humidity: List<PrecipitationProbability>? = null,
    val cloudrate: List<PrecipitationProbability>? = null,
    val pressure: List<PrecipitationProbability>? = null,
    val visibility: List<PrecipitationProbability>? = null,
    val dswrf: List<PrecipitationProbability>? = null,
    @kotlinx.serialization.SerialName("skycon_08h_20h") val skycon08h20h: List<Skycon>? = null,
    @kotlinx.serialization.SerialName("skycon_20h_32h") val skycon20h32h: List<Skycon>? = null,
    val astro: List<Astro>? = null
)

/**
 * 每日预报响应
 * 
 * @param status 返回状态（ok/failed）
 * @param result 结果数据
 */
@Serializable
data class DailyResponse(
    val status: String,
    val result: DailyResult
) {
    /**
     * 每日预报结果
     * 
     * @param daily 每日预报数据
     */
    @Serializable
    data class DailyResult(
        val daily: Daily
    )
}

/**
 * 小时级降水数据项
 */
@Serializable
data class HourlyPrecipitationItem(
    val datetime: String,
    val value: Float,
    val probability: Float? = null
)

/**
 * 小时级温度数据项
 */
@Serializable
data class HourlyTemperatureItem(
    val datetime: String,
    val value: Float
)

/**
 * 小时级天气现象数据项
 */
@Serializable
data class HourlySkyconItem(
    val datetime: String,
    val value: String
)

/**
 * 小时级风速数据项（彩云天气API v2.6格式）
 * 
 * 与温度等数据不同，风速数据包含speed和direction字段
 */
@Serializable
data class HourlyWindItem(
    val datetime: String,
    val speed: Wind,
    val direction: Wind
)

/**
 * 小时级空气质量数据
 */
@Serializable
data class HourlyAirQuality(
    val aqi: HourlyAQI? = null,
    val pm25: Float? = null
)

/**
 * 小时级AQI数据
 */
@Serializable
data class HourlyAQI(
    val chn: Float? = null,
    val usa: Float? = null
)

/**
 * 小时级预报数据（彩云天气API v2.6格式）
 * 
 * API返回的是分开的数组，每个数组元素包含datetime字段
 */
@Serializable
data class Hourly(
    val precipitation: List<HourlyPrecipitationItem>? = null,
    val temperature: List<HourlyTemperatureItem>? = null,
    val skycon: List<HourlySkyconItem>? = null,
    val humidity: List<HourlyTemperatureItem>? = null,
    val wind: List<HourlyWindItem>? = null,
    val cloudrate: List<HourlyTemperatureItem>? = null,
    val pressure: List<HourlyTemperatureItem>? = null,
    val visibility: List<HourlyTemperatureItem>? = null,
    val dswrf: List<HourlyTemperatureItem>? = null,
    @kotlinx.serialization.SerialName("air_quality") 
    val airQuality: HourlyAirQuality? = null,
    val description: String? = null
)

/**
 * 小时级预报响应
 */
@Serializable
data class HourlyResponse(
    val status: String,
    val result: HourlyResult
) {
    @Serializable
    data class HourlyResult(
        val hourly: Hourly
    )
}

/**
 * 天气数据聚合模型
 * 
 * @param realtime 实时天气数据
 * @param daily 每日预报数据
 * @param hourly 小时级预报数据（可选）
 */
data class Weather(
    val realtime: Realtime,
    val daily: Daily,
    val hourly: Hourly? = null
)