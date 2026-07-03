/**
 * 天气状况枚举类
 * 
 * 主要职责：
 * - 定义各种天气状况（晴、多云、阴、雨、雪等）
 * - 映射天气图标和背景资源
 * - 提供根据 skycon 字符串获取对应天气状况的方法
 * 
 * 技术要点：
 * - 使用枚举类封装天气类型、描述、图标和背景资源
 * - getSky() 方法通过 skycon 字符串匹配对应的天气枚举值
 * - 每个枚举值包含天气描述、图标资源和背景资源
 * - 天气现象优先级：降雪 > 降雨 > 雾 > 沙尘 > 浮尘 > 雾霾 > 大风 > 阴 > 多云 > 晴
 */
package org.jesen.dev.sunnyweather.pose.domain.model

import org.jesen.dev.sunnyweather.pose.R

enum class Sky(
    val skycon: String, 
    val info: String, 
    val icon: Int, 
    val bg: Int
) {
    /** 晴（白天）- 云量 < 0.2 */
    CLEAR_DAY("CLEAR_DAY", "晴", R.drawable.ic_clear_day, R.drawable.bg_clear_day),
    
    /** 晴（夜间）- 云量 < 0.2 */
    CLEAR_NIGHT("CLEAR_NIGHT", "晴", R.drawable.ic_clear_night, R.drawable.bg_clear_night),
    
    /** 多云（白天）- 0.8 >= 云量 > 0.2 */
    PARTLY_CLOUDY_DAY("PARTLY_CLOUDY_DAY", "多云", R.drawable.ic_partly_cloud_day, R.drawable.bg_partly_cloudy_day),
    
    /** 多云（夜间）- 0.8 >= 云量 > 0.2 */
    PARTLY_CLOUDY_NIGHT("PARTLY_CLOUDY_NIGHT", "多云", R.drawable.ic_partly_cloud_night, R.drawable.bg_partly_cloudy_night),
    
    /** 阴 - 云量 > 0.8 */
    CLOUDY("CLOUDY", "阴", R.drawable.ic_cloudy, R.drawable.bg_cloudy),
    
    /** 轻度雾霾 - PM2.5 100~150 */
    LIGHT_HAZE("LIGHT_HAZE", "轻度雾霾", R.drawable.ic_fog, R.drawable.bg_fog),
    
    /** 中度雾霾 - PM2.5 150~200 */
    MODERATE_HAZE("MODERATE_HAZE", "中度雾霾", R.drawable.ic_fog, R.drawable.bg_fog),
    
    /** 重度雾霾 - PM2.5 > 200 */
    HEAVY_HAZE("HEAVY_HAZE", "重度雾霾", R.drawable.ic_fog, R.drawable.bg_fog),
    
    /** 小雨 */
    LIGHT_RAIN("LIGHT_RAIN", "小雨", R.drawable.ic_light_rain, R.drawable.bg_rain),
    
    /** 中雨 */
    MODERATE_RAIN("MODERATE_RAIN", "中雨", R.drawable.ic_light_rain, R.drawable.bg_rain),
    
    /** 大雨 */
    HEAVY_RAIN("HEAVY_RAIN", "大雨", R.drawable.ic_light_rain, R.drawable.bg_rain),
    
    /** 暴雨 */
    STORM_RAIN("STORM_RAIN", "暴雨", R.drawable.ic_light_rain, R.drawable.bg_rain),
    
    /** 雾 */
    FOG("FOG", "雾", R.drawable.ic_fog, R.drawable.bg_fog),
    
    /** 小雪 */
    LIGHT_SNOW("LIGHT_SNOW", "小雪", R.drawable.ic_light_snow, R.drawable.bg_snow),
    
    /** 中雪 */
    MODERATE_SNOW("MODERATE_SNOW", "中雪", R.drawable.ic_light_snow, R.drawable.bg_snow),
    
    /** 大雪 */
    HEAVY_SNOW("HEAVY_SNOW", "大雪", R.drawable.ic_light_snow, R.drawable.bg_snow),
    
    /** 暴雪 */
    STORM_SNOW("STORM_SNOW", "暴雪", R.drawable.ic_light_snow, R.drawable.bg_snow),
    
    /** 浮尘 */
    DUST("DUST", "浮尘", R.drawable.ic_fog, R.drawable.bg_fog),
    
    /** 沙尘 */
    SAND("SAND", "沙尘", R.drawable.ic_fog, R.drawable.bg_fog),
    
    /** 大风 */
    WIND("WIND", "大风", R.drawable.ic_cloudy, R.drawable.bg_wind);

    companion object {
        /**
         * 根据 skycon 字符串获取对应的天气状况枚举
         * 
         * @param skycon 天气现象代码（来自API）
         * @return 对应的天气状况枚举，未匹配到时返回 CLEAR_DAY
         */
        fun getSky(skycon: String): Sky {
            return values().find { it.skycon == skycon } ?: CLEAR_DAY
        }
    }
}