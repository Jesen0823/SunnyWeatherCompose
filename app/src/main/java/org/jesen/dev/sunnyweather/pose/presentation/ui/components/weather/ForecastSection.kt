/**
 * 天气预报区域组件
 * 
 * 主要职责：
 * - 显示未来7天的天气预报
 * - 封装天气预报卡片样式
 * 
 * 技术要点：
 * - 使用 Surface 作为容器，设置圆角和阴影
 * - 遍历 weather.daily.skycon 和 weather.daily.temperature 数据
 * - 调用 AnimatedForecastItem 展示每一天的预报
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.domain.model.Weather

@Composable
fun ForecastSection(weather: Weather) {
    Surface(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
            .clip(RoundedCornerShape(20.dp)),
        color = Color.White.copy(alpha = 0.9f),
        shadowElevation = 8.dp
    ) {
        Column(
            modifier = Modifier.padding(20.dp)
        ) {
            Text(
                text = "未来天气预报",
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.Bold,
                color = Color.Black.copy(alpha = 0.8f)
            )
            Spacer(modifier = Modifier.height(16.dp))
            
            Column {
                weather.daily.skycon.forEachIndexed { index, skycon ->
                    val temperature = weather.daily.temperature[index]
                    AnimatedForecastItem(
                        index = index,
                        skycon = skycon.value,
                        date = skycon.date,
                        tempMin = temperature.min.toInt(),
                        tempMax = temperature.max.toInt()
                    )
                }
            }
        }
    }
}
