package org.jesen.dev.sunnyweather.pose.presentation.ui.components

import androidx.compose.foundation.gestures.Orientation
import androidx.compose.foundation.gestures.draggable
import androidx.compose.foundation.gestures.rememberDraggableState
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.offset
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clipToBounds
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.dp
import kotlin.math.roundToInt

@Composable
fun PullToRefresh(
    isRefreshing: Boolean,
    onRefresh: () -> Unit,
    refreshThreshold: Float = 80f,
    content: @Composable () -> Unit
) {
    var offset by remember { mutableStateOf(0f) }
    val isDragEnabled = !isRefreshing

    Box(
        modifier = Modifier
            .fillMaxSize()
            .clipToBounds()
            .draggable(
                state = rememberDraggableState { delta ->
                    if (isDragEnabled) {
                        offset = (offset + delta).coerceAtLeast(0f)
                    }
                },
                orientation = Orientation.Vertical,
                onDragStopped = { velocity ->
                    if (offset >= refreshThreshold) {
                        offset = refreshThreshold
                        onRefresh()
                    } else {
                        offset = 0f
                    }
                }
            )
    ) {
        content()

        if (offset > 0f || isRefreshing) {
            Box(
                modifier = Modifier
                    .height(refreshThreshold.dp)
                    .fillMaxSize()
                    .offset { IntOffset(0, offset.roundToInt()) },
                contentAlignment = Alignment.Center
            ) {
                if (isRefreshing) {
                    CircularProgressIndicator()
                } else {
                    val pullProgress = (offset / refreshThreshold).coerceIn(0f, 1f)
                    Text(
                        text = if (pullProgress >= 1f) "松开刷新" else "下拉刷新",
                        style = MaterialTheme.typography.bodyLarge,
                        color = MaterialTheme.colorScheme.onSurface
                    )
                }
            }
        }
    }
}
