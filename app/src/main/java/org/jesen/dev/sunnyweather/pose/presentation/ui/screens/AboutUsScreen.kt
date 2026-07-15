package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import android.opengl.GLSurfaceView
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.NavigateBefore
import androidx.compose.material3.CenterAlignedTopAppBar
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalLifecycleOwner
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleEventObserver
import com.jesen.dev.gllib.Constants
import com.jesen.dev.gllib.MineGLRender
import com.jesen.dev.gllib.MineGLSurfaceView
import kotlinx.coroutines.launch
import org.jesen.dev.sunnyweather.pose.utils.ResUtil

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun AboutUsScreen(onBack: () -> Unit = {}) {
    Scaffold(
        topBar = {
            CenterAlignedTopAppBar(
                title = {
                    Text(
                        text = "关于我们",
                        style = MaterialTheme.typography.titleLarge
                    )
                },
                navigationIcon = {
                    IconButton(onClick = onBack) {
                        Icon(
                            imageVector = Icons.AutoMirrored.Filled.NavigateBefore,
                            contentDescription = "返回"
                        )
                    }
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.surfaceContainer
                )
            )
        }
    ) { paddingValues ->
        AboutUsGLContent(paddingValues)
    }
}

@Composable
private fun AboutUsGLContent(paddingValues: androidx.compose.foundation.layout.PaddingValues) {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    val coroutineScope = rememberCoroutineScope()

    var glSurfaceView by remember { mutableStateOf<MineGLSurfaceView?>(null) }

    AndroidView(
        factory = { ctx ->
            val glRender = MineGLRender()
            glRender.init()

            val view = MineGLSurfaceView(ctx, glRender)
            view.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY

            coroutineScope.launch {
                val rgbaData = ResUtil.loadBitmapToRgbaAsync(context, com.jesen.dev.gllib.R.drawable.noise)

                view.queueEvent {
                    glRender.setParamsInt(Constants.SAMPLE_TYPE, Constants.SAMPLE_TYPE_KEY_CLOUD, 0)

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
        modifier = Modifier
            .fillMaxWidth()
            .height(500.dp)
            .padding(paddingValues)
            .padding(horizontal = 16.dp, vertical = 16.dp),
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