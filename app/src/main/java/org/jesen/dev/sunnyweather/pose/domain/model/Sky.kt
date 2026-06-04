package org.jesen.dev.sunnyweather.pose.domain.model

import org.jesen.dev.sunnyweather.pose.R

enum class Sky(val skycon: String, val info: String, val icon: Int, val bg: Int) {
    CLEAR_DAY("CLEAR_DAY", "晴", R.drawable.ic_clear_day, R.drawable.bg_clear_day),
    CLEAR_NIGHT("CLEAR_NIGHT", "晴", R.drawable.ic_clear_night, R.drawable.bg_clear_night),
    PARTLY_CLOUDY_DAY("PARTLY_CLOUDY_DAY", "多云", R.drawable.ic_partly_cloud_day, R.drawable.bg_partly_cloudy_day),
    PARTLY_CLOUDY_NIGHT("PARTLY_CLOUDY_NIGHT", "多云", R.drawable.ic_partly_cloud_night, R.drawable.bg_partly_cloudy_night),
    CLOUDY("CLOUDY", "阴", R.drawable.ic_cloudy, R.drawable.bg_cloudy),
    RAIN("RAIN", "雨", R.drawable.ic_light_rain, R.drawable.bg_rain),
    SNOW("SNOW", "雪", R.drawable.ic_light_snow, R.drawable.bg_snow),
    WIND("WIND", "风", R.drawable.ic_cloudy, R.drawable.bg_wind),
    HAZE("HAZE", "雾霾", R.drawable.ic_fog, R.drawable.bg_fog);

    companion object {
        fun getSky(skycon: String): Sky {
            return values().find { it.skycon == skycon } ?: CLEAR_DAY
        }
    }
}