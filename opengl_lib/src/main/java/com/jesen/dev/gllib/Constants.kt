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

        /** 轻度雾霾
         * 空气污染细颗粒物（PM2.5）是形成霾天气的重要原因
         * 天空淡黄色或土黄色
         * 能见度较低
         * 太阳亮度微弱
         */
        const val SKYCON_LIGHT_HAZE = "LIGHT_HAZE"

        /** 中度雾霾
         * 空气污染细颗粒物（PM2.5）是形成霾天气的重要原因
         * 空气较为浑浊，天空黄色
         * 能见度较低
         * 太阳亮度极弱
         */
        const val SKYCON_MODERATE_HAZE = "MODERATE_HAZE"

        /** 重度雾霾
         * 空气污染细颗粒物（PM2.5）是形成霾天气的重要原因
         * 空气浑浊，天空深黄接近红色
         * 能见度极低
         * ，看不见太阳或没有太阳
         */
        const val SKYCON_HEAVY_HAZE = "HEAVY_HAZE"

        /** 小雨 */
        const val SKYCON_LIGHT_RAIN = "LIGHT_RAIN"

        /** 中雨 */
        const val SKYCON_MODERATE_RAIN = "MODERATE_RAIN"

        /** 大雨 */
        const val SKYCON_HEAVY_RAIN = "HEAVY_RAIN"

        /** 暴雨 */
        const val SKYCON_STORM_RAIN = "STORM_RAIN"

        /** 雾
         * 大量微小水汽悬浮空气造成
         * 乳白色，能见度低
         * 空气湿度高
         */
        const val SKYCON_FOG = "FOG"

        /** 小雪 */
        const val SKYCON_LIGHT_SNOW = "LIGHT_SNOW"

        /** 中雪 */
        const val SKYCON_MODERATE_SNOW = "MODERATE_SNOW"

        /** 大雪 */
        const val SKYCON_HEAVY_SNOW = "HEAVY_SNOW"

        /** 暴雪 */
        const val SKYCON_STORM_SNOW = "STORM_SNOW"

        /** 浮尘
         * 浮尘天气,细粒浮游空中形成“落黄沙”
         * 能见度差
         * 空气湿度低
         * 出现时远方物体呈土黄色，太阳呈苍白色或淡黄色
         */
        const val SKYCON_DUST = "DUST"

        /** 沙尘
         * 能见度更低，而且风力很大
         * 空气湿度低
         * 地面大量尘沙吹起，所以空气相当浑浊
         *
         */
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
