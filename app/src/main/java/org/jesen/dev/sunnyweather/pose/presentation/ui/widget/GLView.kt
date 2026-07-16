package org.jesen.dev.sunnyweather.pose.presentation.ui.widget

import android.content.Context
import android.opengl.GLSurfaceView
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.viewinterop.AndroidView
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleEventObserver
import androidx.lifecycle.compose.LocalLifecycleOwner
import com.jesen.dev.gllib.BitmapRgbaData
import com.jesen.dev.gllib.Constants
import com.jesen.dev.gllib.GLUtils
import com.jesen.dev.gllib.MineGLRender
import com.jesen.dev.gllib.MineGLSurfaceView
import kotlinx.coroutines.launch

@Composable
fun GLView(
    rendererType: Int,
    modifier: Modifier = Modifier,
    textureResId: Int? = null,
    renderMode: Int = GLSurfaceView.RENDERMODE_CONTINUOUSLY,
    textureLoader: (suspend (Context) -> BitmapRgbaData?)? = null
) {
    val lifecycleOwner = LocalLifecycleOwner.current
    val coroutineScope = rememberCoroutineScope()

    var glSurfaceView by remember { mutableStateOf<MineGLSurfaceView?>(null) }

    AndroidView(
        factory = { ctx ->
            val glRender = MineGLRender()
            glRender.init()

            val view = MineGLSurfaceView(ctx, glRender)
            view.renderMode = renderMode

            coroutineScope.launch {
                var rgbaData: BitmapRgbaData? = null

                when {
                    textureLoader != null -> {
                        rgbaData = textureLoader(ctx)
                    }
                    textureResId != null -> {
                        rgbaData = GLUtils.loadBitmapToRgbaAsync(ctx, textureResId)
                    }
                }

                view.queueEvent {
                    glRender.setParamsInt(Constants.RENDERER_TYPE, rendererType, 0)

                    rgbaData?.let { data ->
                        glRender.setImageData(
                            Constants.IMAGE_FORMAT_RGBA,
                            data.width,
                            data.height,
                            data.rgbaBytes
                        )
                    }
                }
            }

            glSurfaceView = view
            view
        },
        modifier = modifier,
        update = { view ->
            view.renderMode = renderMode
        },
        onRelease = { view ->
            view.onPause()
            val glRender = view.getEGLRender()
            glRender.unInit()
        }
    )

    DisposableEffect(lifecycleOwner, glSurfaceView) {
        val observer = LifecycleEventObserver { _, event ->
            when (event) {
                Lifecycle.Event.ON_RESUME -> glSurfaceView?.onResume()
                Lifecycle.Event.ON_PAUSE -> glSurfaceView?.onPause()
                else -> {}
            }
        }

        lifecycleOwner.lifecycle.addObserver(observer)
        onDispose {
            lifecycleOwner.lifecycle.removeObserver(observer)
        }
    }
}
