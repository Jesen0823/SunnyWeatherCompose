package org.jesen.dev.sunnyweather.pose.presentation.ui.components

import androidx.compose.animation.core.*
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.res.painterResource
import org.jesen.dev.sunnyweather.pose.domain.model.Sky

@Composable
fun AnimatedBackground(sky: Sky) {
    val infiniteTransition = rememberInfiniteTransition(label = "background-animation")
    val alphaState = infiniteTransition.animateFloat(
        initialValue = 0.9f,
        targetValue = 1f,
        animationSpec = infiniteRepeatable(
            animation = tween(4000, easing = LinearEasing),
            repeatMode = RepeatMode.Reverse
        ),
        label = "background-alpha"
    )
    val alpha = alphaState.value
    
    Image(
        painter = painterResource(id = sky.bg),
        contentDescription = null,
        contentScale = ContentScale.Crop,
        modifier = Modifier
            .fillMaxSize()
            .graphicsLayer {
                this.alpha = alpha
                scaleX = 1.02f
                scaleY = 1.02f
            }
    )
}
