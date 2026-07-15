/**
 * 当前天气卡片组件
 * 
 * 主要职责：
 * - 显示实时天气信息（温度、天气状况、空气质量）
 * - 通过 LocalProvider 获取天气和城市数据
 * - 点击城市名称可跳转到城市选择页面
 * 
 * 技术要点：
 * - 使用 LaunchedEffect + Animatable 实现温度文字和图标的入场缩放动画
 * - 根据 AQI 值动态设置空气质量颜色（绿/黄/橙/红/紫）
 * - 使用 Card 组件替代 Surface，更符合 Material3 设计规范
 * - 半透明背景（alpha=0.1）配合 OpenGL 背景效果更佳
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather

import androidx.compose.animation.core.*
import androidx.compose.foundation.Image
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.graphics.Color.Companion.Green
import androidx.compose.ui.graphics.Color.Companion.Yellow
import androidx.compose.ui.graphics.Color.Companion.Red
import org.jesen.dev.sunnyweather.pose.domain.model.Sky
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.requireLocalPlace
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.requireLocalWeather

@Composable
fun CurrentWeatherCard() {
    val weather = requireLocalWeather()
    val place = requireLocalPlace()
    
    val sky = Sky.getSky(weather.realtime.skycon)
    val realtime = weather.realtime
    
    val tempScale = remember { Animatable(0f) }
    val iconScale = remember { Animatable(0.5f) }

    /**
     * LaunchedEffect - 启动入场动画（管理动画生命周期）
     * 
     * 性能优化原理 - 动画生命周期管理：
     * - Animatable 是 Compose 的动画状态对象，需要在协程中执行 animateTo
     * - LaunchedEffect 提供了与组件生命周期绑定的协程作用域
     * - 当组件进入组合时，LaunchedEffect 启动协程，执行动画
     * - 当组件退出组合时，LaunchedEffect 自动取消协程，停止动画
     * - 这避免了组件销毁后动画仍然运行的内存泄漏问题
     * 
     * LaunchedEffect 的 key 说明：
     * - key = Unit 表示只在组件首次进入组合时执行一次
     * - 如果 key 是其他值（如 weather），则当 key 变化时会重启协程
     * - 对于入场动画，使用 Unit 确保只播放一次
     */
    LaunchedEffect(Unit) {
        // 启动温度文字的缩放动画（从0到1，800ms）
        tempScale.animateTo(1f, tween(800, easing = FastOutSlowInEasing))
        // 启动天气图标的缩放动画（从0.5到1，600ms）
        iconScale.animateTo(1f, tween(600, easing = FastOutSlowInEasing))
    }

    Card(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp, vertical = 12.dp),
        shape = RoundedCornerShape(20.dp),
        colors = CardDefaults.cardColors(
            containerColor = Color.White.copy(alpha = 0.2f)
        ),
        elevation = CardDefaults.cardElevation(defaultElevation = 0.dp)
    ) {
        Column(
            modifier = Modifier.padding(24.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(
                text = place.name,
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.Bold,
                color = Color.Black.copy(alpha = 0.8f)
            )
            Spacer(modifier = Modifier.height(12.dp))
            
            Row(
                modifier = Modifier.fillMaxWidth(),
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = Arrangement.Center
            ) {
                Text(
                    text = "${realtime.temperature.toInt()}",
                    fontSize = 72.sp,
                    fontWeight = FontWeight.Bold,
                    color = Color.Black.copy(alpha = 0.9f),
                    modifier = Modifier.graphicsLayer { scaleX = tempScale.value }
                )
                Text(
                    text = "°C",
                    fontSize = 28.sp,
                    color = Color.Black.copy(alpha = 0.6f)
                )
                Spacer(modifier = Modifier.width(20.dp))
                
                Image(
                    painter = painterResource(id = sky.icon),
                    contentDescription = sky.info,
                    modifier = Modifier
                        .size(72.dp)
                        .graphicsLayer { scaleX = iconScale.value; scaleY = iconScale.value }
                )
            }
            Spacer(modifier = Modifier.height(12.dp))
            
            Text(
                text = sky.info,
                style = MaterialTheme.typography.titleLarge,
                fontWeight = FontWeight.Medium,
                color = Color.Black.copy(alpha = 0.8f)
            )
            Spacer(modifier = Modifier.height(6.dp))
            
            val aqiColor = when (realtime.airQuality.aqi.chn.toInt()) {
                in 0..50 -> Green
                in 51..100 -> Yellow
                in 101..150 -> Color(0xFFFFA500)
                in 151..200 -> Red
                else -> Color(0xFF800080)
            }
            Text(
                text = "空气指数 ${realtime.airQuality.aqi.chn.toInt()}",
                style = MaterialTheme.typography.bodyMedium,
                color = aqiColor
            )
        }
    }
}