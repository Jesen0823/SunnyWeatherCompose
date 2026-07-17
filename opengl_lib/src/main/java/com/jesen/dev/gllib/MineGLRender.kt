package com.jesen.dev.gllib

import android.opengl.GLSurfaceView
import android.util.Log
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MineGLRender : GLSurfaceView.Renderer {
    private val TAG = "MineGLRender"
    private val nativeRender = MineNativeRender()
    private var mSampleType = 0
    private var pendingSkycon: String? = null
    @Volatile
    private var isSurfaceCreated = false

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        Log.d(TAG, "onSurfaceCreated() called with: gl = [$gl], config = [$config], GL_VERSION = ${gl?.glGetString(GL10.GL_VERSION) ?: ""}")
        Log.d(TAG, "onSurfaceCreated: current pendingSkycon = [${pendingSkycon ?: "null"}], isSurfaceCreated = [$isSurfaceCreated]")
        
        pendingSkycon?.let { skycon ->
            Log.d(TAG, "onSurfaceCreated: applying pending skycon = [$skycon]")
            nativeRender.native_SetSkycon(skycon)
            Log.d(TAG, "onSurfaceCreated: native_SetSkycon() returned")
            pendingSkycon = null
            Log.d(TAG, "onSurfaceCreated: pendingSkycon cleared")
        }
        
        Log.d(TAG, "onSurfaceCreated: calling native_OnSurfaceCreated()")
        nativeRender.native_OnSurfaceCreated()
        Log.d(TAG, "onSurfaceCreated: native_OnSurfaceCreated() returned")
        
        isSurfaceCreated = true
        Log.d(TAG, "onSurfaceCreated: isSurfaceCreated set to true")
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        Log.d(TAG, "onSurfaceChanged() called with: gl = [$gl], width = [$width], height = [$height]")
        nativeRender.native_OnSurfaceChanged(width, height)
    }

    override fun onDrawFrame(gl: GL10?) {
        nativeRender.native_OnDrawFrame()
    }

    fun setParamsInt(paramType: Int, value0: Int, value1: Int) {
        if (paramType == Constants.RENDERER_TYPE) {
            mSampleType = value0
        }
        nativeRender.native_SetParamsInt(paramType, value0, value1)
    }

    fun updateTransformMatrix(rotateX: Float, rotateY: Float, scaleX: Float, scaleY: Float) {
        nativeRender.native_UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY)
    }

    fun setImageData(format: Int, width: Int, height: Int, bytes: ByteArray) {
        nativeRender.native_SetImageData(format, width, height, bytes)
    }

    fun init() {
        nativeRender.native_OnInit()
    }

    fun unInit() {
        nativeRender.native_OnUnInit()
    }

    fun getSampleType() = mSampleType

    fun setSkycon(skycon: String) {
        Log.d(TAG, "setSkycon() called with: skycon = [$skycon], isSurfaceCreated = [$isSurfaceCreated], thread = [${Thread.currentThread().name}]")
        if (isSurfaceCreated) {
            Log.d(TAG, "setSkycon: surface already created, calling native_SetSkycon()")
            nativeRender.native_SetSkycon(skycon)
            Log.d(TAG, "setSkycon: native_SetSkycon() returned")
        } else {
            Log.d(TAG, "setSkycon: surface not created yet, saving to pendingSkycon = [$skycon]")
            pendingSkycon = skycon
        }
    }
}