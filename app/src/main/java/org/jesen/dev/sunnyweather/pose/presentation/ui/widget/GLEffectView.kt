package org.jesen.dev.sunnyweather.pose.presentation.ui.widget

import android.opengl.GLSurfaceView
import android.util.Log
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.viewinterop.AndroidView
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleEventObserver
import androidx.lifecycle.compose.LocalLifecycleOwner
import com.jesen.dev.gllib.MineGLRender
import com.jesen.dev.gllib.MineGLSurfaceView

private const val TAG = "GLEffectView"

/**
 * OpenGL天气动效视图组件
 *
 * 集成OpenGL渲染到Compose界面的桥梁组件，负责管理OpenGL渲染器的完整生命周期：
 * - 组件创建时初始化渲染器
 * - 组件可见时恢复渲染
 * - 组件不可见时暂停渲染
 * - 组件销毁时释放资源
 *
 * 支持响应式天气类型切换，当skycon变化时自动更新渲染效果。
 *
 * @param skycon 天气类型字符串，如 "CLEAR_DAY", "HEAVY_SNOW", "STORM_RAIN" 等
 * @param modifier Compose修饰符，用于控制布局
 * @param renderMode OpenGL渲染模式，默认RENDERMODE_CONTINUOUSLY（持续渲染）
 *                  可选RENDERMODE_WHEN_DIRTY（按需渲染）
 */
@Composable
fun GLEffectView(
    skycon: String,
    modifier: Modifier = Modifier,
    renderMode: Int = GLSurfaceView.RENDERMODE_CONTINUOUSLY
) {
    val lifecycleOwner = LocalLifecycleOwner.current

    var glSurfaceView by remember { mutableStateOf<MineGLSurfaceView?>(null) }
    var glRender by remember { mutableStateOf<MineGLRender?>(null) }
    var lastSkycon by remember { mutableStateOf("") }
    var isPaused by remember { mutableStateOf(false) }

    AndroidView(
        factory = { ctx ->
            Log.d(TAG, "factory: creating GLEffectView, skycon = [$skycon], renderMode = [$renderMode]")
            
            val render = MineGLRender()
            Log.d(TAG, "factory: MineGLRender created, calling setContext()")
            render.setContext(ctx)
            Log.d(TAG, "factory: setContext() completed, calling init()")
            render.init()

            lastSkycon = skycon
            Log.d(TAG, "factory: calling setSkycon(skycon = [$skycon])")
            render.setSkycon(skycon)

            val view = MineGLSurfaceView(ctx, render)
            view.renderMode = renderMode
            Log.d(TAG, "factory: MineGLSurfaceView created, renderMode = [$renderMode]")

            glSurfaceView = view
            glRender = render
            Log.d(TAG, "factory: GLEffectView creation complete")
            view
        },
        modifier = modifier,
        update = { view ->
            view.renderMode = renderMode

            if (skycon != lastSkycon) {
                Log.d(TAG, "update: skycon changed from [$lastSkycon] to [$skycon]")
                glRender?.setSkycon(skycon)
                lastSkycon = skycon
                Log.d(TAG, "update: setSkycon completed")
            }
        },
        onRelease = { view ->
            Log.d(TAG, "onRelease: releasing GLEffectView, skycon = [$lastSkycon]")
            view.onPause()
            glRender?.unInit()
            Log.d(TAG, "onRelease: unInit completed")
            glSurfaceView = null
            glRender = null
        }
    )

    DisposableEffect(lifecycleOwner, glSurfaceView) {
        Log.d(TAG, "DisposableEffect: creating lifecycle observer, glSurfaceView = ${glSurfaceView != null}")
        
        val observer = LifecycleEventObserver { _, event ->
            Log.d(TAG, "DisposableEffect: lifecycle event = ${event.name}, glSurfaceView = ${glSurfaceView != null}, isPaused = [$isPaused]")
            
            when (event) {
                Lifecycle.Event.ON_RESUME -> {
                    if (isPaused) {
                        glSurfaceView?.onResume()
                        isPaused = false
                        Log.d(TAG, "DisposableEffect: called onResume(), rendering resumed")
                    }
                }
                Lifecycle.Event.ON_PAUSE -> {
                    glSurfaceView?.onPause()
                    isPaused = true
                    Log.d(TAG, "DisposableEffect: called onPause(), rendering paused")
                }
                Lifecycle.Event.ON_STOP -> {
                    glSurfaceView?.onPause()
                    isPaused = true
                    Log.d(TAG, "DisposableEffect: called onStop(), rendering stopped")
                }
                else -> {}
            }
        }

        lifecycleOwner.lifecycle.addObserver(observer)
        Log.d(TAG, "DisposableEffect: observer added")
        
        onDispose {
            Log.d(TAG, "DisposableEffect: onDispose called")
            lifecycleOwner.lifecycle.removeObserver(observer)
            Log.d(TAG, "DisposableEffect: observer removed")
            
            glSurfaceView?.onPause()
            isPaused = true
            Log.d(TAG, "DisposableEffect: rendering paused on dispose")
        }
    }
}
