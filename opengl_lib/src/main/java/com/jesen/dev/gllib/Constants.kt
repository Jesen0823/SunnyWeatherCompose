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
    }
}
