/**
 * 动画生活指数项组件
 * 
 * 主要职责：
 * - 显示单个生活指数信息（穿衣、感冒风险、紫外线、洗车指数）
 * - 实现入场动画效果
 * 
 * 技术要点：
 * - 使用 Animatable 实现透明度和水平位移动画
 * - 根据索引延迟动画开始时间，实现依次入场效果
 * - 使用 FastOutSlowInEasing 使动画更自然流畅
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather

import androidx.compose.animation.core.*
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp

@Composable
fun AnimatedLifeIndexItem(index: Int, title: String, desc: String) {
    val alpha = remember { Animatable(0f) }
    val offsetX = remember { Animatable(20f) }
    
    androidx.compose.runtime.LaunchedEffect(Unit) {
        alpha.animateTo(1f, tween(300, delayMillis = index * 100))
        offsetX.animateTo(0f, tween(300, delayMillis = index * 100, easing = FastOutSlowInEasing))
    }
    
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 8.dp)
            .graphicsLayer {
                this.alpha = alpha.value
                translationX = offsetX.value
            },
        horizontalArrangement = Arrangement.SpaceBetween
    ) {
        Text(
            text = title,
            style = MaterialTheme.typography.bodyMedium,
            fontWeight = FontWeight.Medium,
            color = Color.Black.copy(alpha = 0.7f)
        )
        Text(
            text = desc,
            style = MaterialTheme.typography.bodyMedium,
            color = Color.Black.copy(alpha = 0.8f)
        )
    }
}
