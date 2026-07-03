/**
 * 天气预报区域组件
 * 
 * 主要职责：
 * - 显示未来7天的天气预报
 * - 封装天气预报卡片样式
 * - 通过 LocalProvider 获取天气数据
 * 
 * 技术要点：
 * - 使用 Surface 作为容器，设置圆角和阴影
 * - 遍历 weather.daily.skycon 和 weather.daily.temperature 数据
 * - 调用 AnimatedForecastItem 展示每一天的预报
 * - 使用 derivedStateOf 优化列表渲染
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.derivedStateOf
import androidx.compose.runtime.getValue
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.requireLocalWeather

@Composable
fun ForecastSection() {
    val weather = requireLocalWeather()

    /**
     * 使用 derivedStateOf 计算天气预报列表数据
     * 
     * 性能优化原理：
     * - mapIndexed 会创建新的列表对象，每次调用都有内存分配开销
     * - 如果组件因为其他状态（如动画）重组，但 weather 没变，会重复创建相同的列表
     * - derivedStateOf 会缓存计算结果，只有 weather 变化时才重新计算
     * - 子组件使用 forecastItems 时，只有当列表内容真正变化时才会触发重组
     * 
     * 数据组合说明：
     * - 天气预报需要 skycon（天气状况）和 temperature（温度）数据
     * - 这两个数据都在 weather.daily 中，但存储在不同列表中
     * - 使用 mapIndexed 将它们组合成 Triple(index, skycon, temperature)
     * - index 用于动画延迟，skycon 用于天气图标，temperature 用于温度显示
     * 
     * 对比示例：
     * // 未优化：每次重组都创建新列表
     * val forecastItems = weather.daily.skycon.mapIndexed { index, skycon ->
     *     Triple(index, skycon, weather.daily.temperature[index])
     * }
     * 
     * // 优化后：缓存计算结果
     * val forecastItems by remember(weather) {
     *     derivedStateOf {
     *         weather.daily.skycon.mapIndexed { index, skycon ->
     *             Triple(index, skycon, weather.daily.temperature[index])
     *         }
     *     }
     * }
     */
    val forecastItems by remember(weather) {
        derivedStateOf {
            // 将 skycon 和 temperature 组合成列表数据
            weather.daily.skycon.mapIndexed { index, skycon ->
                Triple(
                    index,          // 用于动画延迟索引
                    skycon,         // 天气状况数据
                    weather.daily.temperature[index]  // 对应日期的温度数据
                )
            }
        }
    }

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
                forecastItems.forEach { (index, skycon, temperature) ->
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