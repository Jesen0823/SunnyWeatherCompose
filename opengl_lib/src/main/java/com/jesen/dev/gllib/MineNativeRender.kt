package com.jesen.dev.gllib

class MineNativeRender {

    init {
        System.loadLibrary("gllib")
    }

    external fun native_OnInit()
    external fun native_OnUnInit()
    external fun native_SetParamsInt(paramType: Int, value0: Int, value1: Int)
    external fun native_UpdateTransformMatrix(
        rotateX: Float,
        rotateY: Float,
        scaleX: Float,
        scaleY: Float
    )

    external fun native_SetImageData(format: Int, width: Int, height: Int, bytes: ByteArray)

    external fun native_OnSurfaceCreated()
    external fun native_OnSurfaceChanged(width: Int, height: Int)
    external fun native_OnDrawFrame()
    external fun native_SetSkycon(skycon: String)
}