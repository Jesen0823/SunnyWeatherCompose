/**
 * 动画天气预报项组件
 * 
 * 主要职责：
 * - 显示单天的天气预报信息
 * - 实现入场动画和点击缩放效果
 * 
 * 技术要点：
 * - 使用 animateFloatAsState 实现点击缩放动画
 * - 使用 Animatable 实现入场透明度动画
 * - 根据索引延迟动画开始时间，实现瀑布流效果
 * - 解析 ISO 格式日期为 MM-dd 格式
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather

import androidx.compose.animation.core.*
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.interaction.collectIsPressedAsState
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.scale
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.domain.model.Sky

@Composable
fun AnimatedForecastItem(index: Int, skycon: String, date: String, tempMin: Int, tempMax: Int) {
    val sky = Sky.getSky(skycon)
    val parsedDate = try {
        val isoFormat = java.time.format.DateTimeFormatter.ISO_OFFSET_DATE_TIME
        val dateTime = java.time.LocalDateTime.parse(date, isoFormat)
        dateTime.format(java.time.format.DateTimeFormatter.ofPattern("MM-dd"))
    } catch (e: Exception) {
        date
    }
    
    val interactionSource = remember { MutableInteractionSource() }
    val isPressed = interactionSource.collectIsPressedAsState().value
    val scale = animateFloatAsState(
        targetValue = if (isPressed) 0.98f else 1f,
        animationSpec = TweenSpec(100),
        label = "forecast-item-scale"
    )
    val alpha = remember { Animatable(0f) }
    
    androidx.compose.runtime.LaunchedEffect(Unit) {
        alpha.animateTo(1f, tween(300, delayMillis = index * 100))
    }
    
    Surface(
        onClick = {},
        interactionSource = interactionSource,
        color = Color.Transparent,
        modifier = Modifier.scale(scale.value)
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(vertical = 10.dp)
                .graphicsLayer { this.alpha = alpha.value },
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            Text(
                text = parsedDate,
                style = MaterialTheme.typography.bodyMedium,
                color = Color.Black.copy(alpha = 0.7f)
            )
            
            Row(
                verticalAlignment = Alignment.CenterVertically
            ) {
                Image(
                    painter = painterResource(id = sky.icon),
                    contentDescription = sky.info,
                    modifier = Modifier.size(28.dp)
                )
                Spacer(modifier = Modifier.width(10.dp))
                Text(
                    text = sky.info,
                    style = MaterialTheme.typography.bodyMedium,
                    color = Color.Black.copy(alpha = 0.7f)
                )
            }
            
            Text(
                text = "$tempMin° ~ $tempMax°",
                style = MaterialTheme.typography.bodyMedium,
                color = Color.Black.copy(alpha = 0.8f),
                fontWeight = FontWeight.Medium
            )
        }
    }
}
