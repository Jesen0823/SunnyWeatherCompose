package com.jesen.dev.gllib

interface Constants {
    companion object {
        const val TAG = "OpenGLRender"
        const val ANGLE_TYPE = 300
        const val RENDERER_TYPE = 200

        const val RENDERER_TYPE_KEY_BEATING_HEART = RENDERER_TYPE + 0
        const val RENDERER_TYPE_KEY_CLOUD = RENDERER_TYPE + 1
        const val RENDERER_TYPE_KEY_BEZIER_CURVE = RENDERER_TYPE + 2
        const val RENDERER_TYPE_KEY_COORD_SYSTEM = RENDERER_TYPE + 3

        const val IMAGE_FORMAT_RGBA = 0x01
        const val IMAGE_FORMAT_NV21 = 0x02
        const val IMAGE_FORMAT_NV12 = 0x03
        const val IMAGE_FORMAT_I420 = 0x04
        const val IMAGE_FORMAT_YUYV = 0x05
        const val IMAGE_FORMAT_GARY = 0x06
        const val IMAGE_FORMAT_I444 = 0x07

        // ==================== 天气类型常量 ====================
        // 对应彩云 API 的 skycon 值

        /** 晴（白天）*/
        const val SKYCON_CLEAR_DAY = "CLEAR_DAY"

        /** 晴（夜间）*/
        const val SKYCON_CLEAR_NIGHT = "CLEAR_NIGHT"

        /** 多云（白天）*/
        const val SKYCON_PARTLY_CLOUDY_DAY = "PARTLY_CLOUDY_DAY"

        /** 多云（夜间）*/
        const val SKYCON_PARTLY_CLOUDY_NIGHT = "PARTLY_CLOUDY_NIGHT"

        /** 阴 */
        const val SKYCON_CLOUDY = "CLOUDY"

        /** 轻度雾霾 */
        const val SKYCON_LIGHT_HAZE = "LIGHT_HAZE"

        /** 中度雾霾 */
        const val SKYCON_MODERATE_HAZE = "MODERATE_HAZE"

        /** 重度雾霾 */
        const val SKYCON_HEAVY_HAZE = "HEAVY_HAZE"

        /** 小雨 */
        const val SKYCON_LIGHT_RAIN = "LIGHT_RAIN"

        /** 中雨 */
        const val SKYCON_MODERATE_RAIN = "MODERATE_RAIN"

        /** 大雨 */
        const val SKYCON_HEAVY_RAIN = "HEAVY_RAIN"

        /** 暴雨 */
        const val SKYCON_STORM_RAIN = "STORM_RAIN"

        /** 雾 */
        const val SKYCON_FOG = "FOG"

        /** 小雪 */
        const val SKYCON_LIGHT_SNOW = "LIGHT_SNOW"

        /** 中雪 */
        const val SKYCON_MODERATE_SNOW = "MODERATE_SNOW"

        /** 大雪 */
        const val SKYCON_HEAVY_SNOW = "HEAVY_SNOW"

        /** 暴雪 */
        const val SKYCON_STORM_SNOW = "STORM_SNOW"

        /** 浮尘 */
        const val SKYCON_DUST = "DUST"

        /** 沙尘 */
        const val SKYCON_SAND = "SAND"

        /** 大风 */
        const val SKYCON_WIND = "WIND"

        /**
         * 全部天气类型列表，用于验证 skycon 是否有效
         */
        val ALL_SKYCONS = listOf(
            SKYCON_CLEAR_DAY,
            SKYCON_CLEAR_NIGHT,
            SKYCON_PARTLY_CLOUDY_DAY,
            SKYCON_PARTLY_CLOUDY_NIGHT,
            SKYCON_CLOUDY,
            SKYCON_LIGHT_HAZE,
            SKYCON_MODERATE_HAZE,
            SKYCON_HEAVY_HAZE,
            SKYCON_LIGHT_RAIN,
            SKYCON_MODERATE_RAIN,
            SKYCON_HEAVY_RAIN,
            SKYCON_STORM_RAIN,
            SKYCON_FOG,
            SKYCON_LIGHT_SNOW,
            SKYCON_MODERATE_SNOW,
            SKYCON_HEAVY_SNOW,
            SKYCON_STORM_SNOW,
            SKYCON_DUST,
            SKYCON_SAND,
            SKYCON_WIND
        )

        /**
         * 验证 skycon 是否有效
         * @param skycon 天气类型字符串
         * @return 是否有效
         */
        fun isValidSkycon(skycon: String): Boolean {
            return ALL_SKYCONS.contains(skycon)
        }
    }
}
