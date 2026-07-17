package com.jesen.dev.gllib

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet

class MineGLSurfaceView(context: Context, mineGLRender: MineGLRender?, attrs: AttributeSet?) :
    GLSurfaceView(context, attrs) {

    private val mMineGLRender: MineGLRender
    private var mRatioWidth = 0
    private var mRatioHeight = 0

    constructor(context: Context, mineGLRender: MineGLRender?) : this(context, mineGLRender, null)

    init {
        setEGLContextClientVersion(3)
        mMineGLRender = mineGLRender ?: MineGLRender()
        setEGLConfigChooser(8, 8, 8, 8, 16, 8)
        setRenderer(mMineGLRender)
        renderMode = RENDERMODE_CONTINUOUSLY
    }

    fun getEGLRender(): MineGLRender = mMineGLRender

    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec)
        val wSize = MeasureSpec.getSize(widthMeasureSpec)
        val hSize = MeasureSpec.getSize(heightMeasureSpec)
        if (mRatioWidth == 0 || mRatioHeight == 0) {
            setMeasuredDimension(wSize, hSize)
        } else {
            if (wSize < wSize * mRatioWidth / mRatioHeight) {
                setMeasuredDimension(wSize, wSize * mRatioHeight / mRatioWidth)
            } else {
                setMeasuredDimension(hSize * mRatioWidth / mRatioHeight, hSize)
            }
        }
    }

    fun setAspectRatio(w: Int, h: Int) {
        require(!(w < 0 || h < 0)) { "Size cannot be negative." }
        mRatioWidth = w
        mRatioHeight = h
        requestLayout()
    }
}