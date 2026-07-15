/**
 * 实时天气详情卡片组件
 * 
 * 主要职责：
 * - 展示体感温度、湿度、风速风向、能见度、气压等实时天气详情数据
 * - 使用2列网格布局，每行显示两个数据项
 * - 添加图标和动画效果
 * - 通过LocalProvider获取天气数据
 * 
 * 技术要点：
 * - 使用CompositionLocal获取天气数据（requireLocalWeather）
 * - 使用Column+Row(chunked(2))实现网格布局展示数据项，每行两个
 * - 使用AnimatedVisibility实现入场动画
 * - 根据风速大小显示不同的描述
 * - 将湿度百分比转换为显示值（0-1 → 0%-100%）
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.core.FastOutSlowInEasing
import androidx.compose.animation.core.TweenSpec
import androidx.compose.animation.fadeIn
import androidx.compose.animation.slideInVertically
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.requireLocalWeather
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.widget.WeatherMainInfoCard

@Composable
fun WeatherDetailsCard() {
    val weather = requireLocalWeather()
    val realtime = weather.realtime

    /**
     * 根据风速获取风速描述
     * 
     * @param speed 风速(m/s)
     * @return 风速描述文字
     */
    fun getWindDescription(speed: Float): String {
        return when {
            speed < 1 -> "无风"
            speed < 6 -> "微风"
            speed < 12 -> "轻风"
            speed < 20 -> "和风"
            speed < 28 -> "清风"
            speed < 34 -> "强风"
            else -> "大风"
        }
    }

    /**
     * 根据风向角度获取风向描述
     * 
     * @param direction 风向角度(0-360)
     * @return 风向描述文字（如：东北风）
     */
    fun getWindDirection(direction: Float): String {
        val directions = listOf("北风", "东北风", "东风", "东南风", "南风", "西南风", "西风", "西北风")
        val index = ((direction % 360) / 45).toInt()
        return directions[index.coerceIn(0, 7)]
    }

    /**
     * 天气详情数据项列表
     */
    val detailItems = listOf(
        WeatherDetailItem(
            icon = Icons.Filled.Thermostat,
            label = "体感温度",
            value = "${realtime.apparentTemperature?.toInt() ?: realtime.temperature.toInt()}°C",
            tint = Color(0xFFFF6B6B),
            bgColor = Color(0xFFFFE5E5)
        ),
        WeatherDetailItem(
            icon = Icons.Filled.Water,
            label = "湿度",
            value = "${(realtime.humidity?.times(100) ?: 0).toInt()}%",
            tint = Color(0xFF4ECDC4),
            bgColor = Color(0xFFE5F7F5)
        ),
        WeatherDetailItem(
            icon = Icons.Filled.Air,
            label = "风速风向",
            value = "${getWindDirection(realtime.wind?.direction ?: 0f)} ${getWindDescription(realtime.wind?.speed ?: 0f)}",
            tint = Color(0xFF45B7D1),
            bgColor = Color(0xFFE5F2F8)
        ),
        WeatherDetailItem(
            icon = Icons.Filled.RemoveRedEye,
            label = "能见度",
            value = "${realtime.visibility ?: 0}km",
            tint = Color(0xFF96CEB4),
            bgColor = Color(0xFFEAF6F0)
        ),
        WeatherDetailItem(
            icon = Icons.Filled.Compress,
            label = "气压",
            value = "${(realtime.pressure ?: 0).toInt()}hPa",
            tint = Color(0xFFFFEAA7),
            bgColor = Color(0xFFFFFBE5)
        ),
        WeatherDetailItem(
            icon = Icons.Filled.Cloud,
            label = "云量",
            value = "${(realtime.cloudrate?.times(100) ?: 0).toInt()}%",
            tint = Color(0xFFDDA0DD),
            bgColor = Color(0xFFF5EEF8)
        )
    )

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
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "天气详情",
                    style = MaterialTheme.typography.titleMedium,
                    fontWeight = FontWeight.Bold,
                    color = Color.Black.copy(alpha = 0.8f)
                )
                Icon(
                    imageVector = Icons.Filled.Cyclone,
                    contentDescription = "详情",
                    tint = Color.Blue.copy(alpha = 0.6f),
                    modifier = Modifier.size(20.dp)
                )
            }
            Spacer(modifier = Modifier.height(16.dp))

            Column(
                modifier = Modifier.fillMaxWidth(),
                verticalArrangement = Arrangement.spacedBy(12.dp)
            ) {
                detailItems.chunked(2).forEachIndexed { rowIndex, rowItems ->
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(12.dp)
                    ) {
                        rowItems.forEachIndexed { colIndex, item ->
                            val index = rowIndex * 2 + colIndex
                            AnimatedVisibility(
                                visible = true,
                                enter = slideInVertically(
                                    initialOffsetY = { 30 },
                                    animationSpec = TweenSpec(
                                        durationMillis = 400,
                                        easing = FastOutSlowInEasing,
                                        delay = index * 50
                                    )
                                ) + fadeIn(animationSpec = TweenSpec(300)),
                                modifier = Modifier.weight(1f)
                            ) {
                                WeatherMainInfoCard(
                                    icon = item.icon,
                                    label = item.label,
                                    value = item.value,
                                    tint = item.tint,
                                    bgColor = item.bgColor,
                                    modifier = Modifier.fillMaxWidth()
                                )
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * 天气详情数据项数据类
 * 
 * @param icon 图标
 * @param label 标签文字
 * @param value 数值
 * @param tint 图标颜色
 * @param bgColor 背景颜色
 */
private data class WeatherDetailItem(
    val icon: ImageVector,
    val label: String,
    val value: String,
    val tint: Color,
    val bgColor: Color
)
