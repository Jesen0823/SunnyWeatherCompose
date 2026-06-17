/**
 * 动画背景组件
 * 
 * 主要职责：
 * - 根据天气状况显示对应的动态背景
 * - 实现背景透明度渐变动画效果
 * 
 * 技术要点：
 * - 使用 rememberInfiniteTransition 创建无限循环动画
 * - 通过 alphaState 实现透明度在 0.9-1.0 之间循环变化
 * - 根据 Sky 枚举值选择对应的背景资源
 * - 使用 Image 组件填充整个屏幕
 */
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
