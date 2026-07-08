/**
 * 空气质量详情卡片组件
 * 
 * 主要职责：
 * - 展示AQI指数、PM2.5、PM10、O3、SO2、NO2、CO污染物浓度
 * - 根据AQI值设置颜色指示器（绿/黄/橙/红/紫）
 * - 添加AQI进度条和动态背景颜色
 * - 通过LocalProvider获取天气数据
 * 
 * 技术要点：
 * - 使用CompositionLocal获取天气数据（requireLocalWeather）
 * - 使用Column+Row(chunked(2))实现2列网格布局展示所有污染物
 * - 根据AQI等级设置不同的颜色和背景
 * - 使用LinearProgressIndicator展示AQI进度条
 * - 使用AnimatedVisibility实现入场动画
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
import androidx.compose.material.icons.filled.WindPower
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.requireLocalWeather

@Composable
fun AirQualityCard() {
    val weather = requireLocalWeather()
    val airQuality = weather.realtime.airQuality

    /**
     * 根据AQI值获取空气质量信息
     * 
     * @param aqi AQI值
     * @return Triple(空气质量等级描述, AQI颜色, 背景颜色)
     */
    fun getAqiInfo(aqi: Float): Triple<String, Color, Color> {
        return when (aqi.toInt()) {
            in 0..50 -> Triple("优", Color(0xFF52C41A), Color(0xFFE6F7FF))
            in 51..100 -> Triple("良", Color(0xFFFAAD14), Color(0xFFFFF7E6))
            in 101..150 -> Triple("轻度污染", Color(0xFFFF7A45), Color(0xFFFFF0E6))
            in 151..200 -> Triple("中度污染", Color(0xFFFF4D4F), Color(0xFFFFEBE6))
            in 201..300 -> Triple("重度污染", Color(0xFF722ED1), Color(0xFFF9F0FF))
            else -> Triple("严重污染", Color(0xFF1F1F1F), Color(0xFFF5F5F5))
        }
    }

    val (aqiLevel, aqiColor, bgColor) = getAqiInfo(airQuality.aqi.chn)
    val aqiValue = airQuality.aqi.chn.toInt()
    val aqiProgress = (aqiValue.coerceIn(0, 500) / 500f).coerceIn(0f, 1f)

    val airQualityItems = listOf(
        AirQualityItem(
            label = "PM2.5",
            value = "${airQuality.pm25?.toInt() ?: 0}",
            unit = "μg/m³",
            description = "细颗粒物"
        ),
        AirQualityItem(
            label = "PM10",
            value = "${airQuality.pm10?.toInt() ?: 0}",
            unit = "μg/m³",
            description = "可吸入颗粒物"
        ),
        AirQualityItem(
            label = "O₃",
            value = "${airQuality.o3?.toInt() ?: 0}",
            unit = "μg/m³",
            description = "臭氧"
        ),
        AirQualityItem(
            label = "SO₂",
            value = "${airQuality.so2?.toInt() ?: 0}",
            unit = "μg/m³",
            description = "二氧化硫"
        ),
        AirQualityItem(
            label = "NO₂",
            value = "${airQuality.no2?.toInt() ?: 0}",
            unit = "μg/m³",
            description = "二氧化氮"
        ),
        AirQualityItem(
            label = "CO",
            value = "${airQuality.co ?: 0}",
            unit = "mg/m³",
            description = "一氧化碳"
        )
    )

    Surface(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
            .clip(RoundedCornerShape(20.dp)),
        color = bgColor,
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
                Row(
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        imageVector = Icons.Filled.WindPower,
                        contentDescription = "空气质量",
                        tint = aqiColor,
                        modifier = Modifier.size(24.dp)
                    )
                    Spacer(modifier = Modifier.width(12.dp))
                    Text(
                        text = "空气质量",
                        style = MaterialTheme.typography.titleMedium,
                        fontWeight = FontWeight.Bold,
                        color = Color.Black.copy(alpha = 0.8f)
                    )
                }
                AnimatedVisibility(
                    visible = true,
                    enter = slideInVertically(
                        initialOffsetY = { -20 },
                        animationSpec = TweenSpec(
                            durationMillis = 400,
                            easing = FastOutSlowInEasing
                        )
                    ) + fadeIn(animationSpec = TweenSpec(300))
                ) {
                    Row(
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text(
                            text = "$aqiValue",
                            style = MaterialTheme.typography.titleLarge,
                            fontWeight = FontWeight.Bold,
                            color = aqiColor
                        )
                        Spacer(modifier = Modifier.width(8.dp))
                        Text(
                            text = aqiLevel,
                            style = MaterialTheme.typography.bodyMedium,
                            color = aqiColor
                        )
                    }
                }
            }
            Spacer(modifier = Modifier.height(12.dp))

            AnimatedVisibility(
                visible = true,
                enter = slideInVertically(
                    initialOffsetY = { 20 },
                    animationSpec = TweenSpec(
                        durationMillis = 400,
                        easing = FastOutSlowInEasing,
                        delay = 100
                    )
                ) + fadeIn(animationSpec = TweenSpec(300, delay = 100))
            ) {
                Column {
                    AqiProgressBar(aqiValue = aqiValue)
                    Spacer(modifier = Modifier.height(4.dp))
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween
                    ) {
                        listOf("0", "50", "100", "150", "200", "300", "400", "500").forEach {
                            Text(
                                text = it,
                                style = MaterialTheme.typography.labelSmall,
                                color = Color.Black.copy(alpha = 0.4f)
                            )
                        }
                    }
                }
            }

            Spacer(modifier = Modifier.height(16.dp))

            Column(
                modifier = Modifier.fillMaxWidth(),
                verticalArrangement = Arrangement.spacedBy(12.dp)
            ) {
                airQualityItems.chunked(2).forEachIndexed { rowIndex, rowItems ->
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(16.dp)
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
                                        delay = (index + 2) * 50
                                    )
                                ) + fadeIn(animationSpec = TweenSpec(300))
                            ) {
                                Surface(
                                    modifier = Modifier
                                        .weight(1f)
                                        .clip(RoundedCornerShape(12.dp)),
                                    color = Color.White.copy(alpha = 0.8f),
                                    shadowElevation = 2.dp
                                ) {
                                    Column(
                                        modifier = Modifier.padding(12.dp),
                                        horizontalAlignment = Alignment.CenterHorizontally
                                    ) {
                                        Text(
                                            text = item.label,
                                            style = MaterialTheme.typography.bodySmall,
                                            color = Color.Black.copy(alpha = 0.5f)
                                        )
                                        Spacer(modifier = Modifier.height(4.dp))
                                        Text(
                                            text = "${item.value}${item.unit}",
                                            style = MaterialTheme.typography.bodyMedium,
                                            fontWeight = FontWeight.Medium,
                                            color = Color.Black.copy(alpha = 0.8f)
                                        )
                                        Spacer(modifier = Modifier.height(4.dp))
                                        Text(
                                            text = item.description,
                                            style = MaterialTheme.typography.labelSmall,
                                            color = Color.Black.copy(alpha = 0.4f)
                                        )
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * 空气质量数据项数据类
 * 
 * @param label 污染物标签
 * @param value 污染物数值
 * @param unit 单位
 * @param description 污染物描述
 */
private data class AirQualityItem(
    val label: String,
    val value: String,
    val unit: String,
    val description: String
)

/**
 * AQI进度条组件
 * 
 * 显示空气质量指数的可视化进度条，采用中国标准AQI颜色分段：
 * - 0-50: 优 (绿色)
 * - 51-100: 良 (黄色)
 * - 101-150: 轻度污染 (橙色)
 * - 151-200: 中度污染 (红色)
 * - 201-300: 重度污染 (紫色)
 * - >300: 严重污染 (深红色)
 * 
 * @param aqiValue AQI数值
 */
@Composable
private fun AqiProgressBar(aqiValue: Int) {
    val aqiProgress = (aqiValue.coerceIn(0, 500) / 500f).coerceIn(0f, 1f)
    
    val color = when (aqiValue) {
        in 0..50 -> Color(0xFF52C41A)
        in 51..100 -> Color(0xFFFAAD14)
        in 101..150 -> Color(0xFFFF7A45)
        in 151..200 -> Color(0xFFFF4D4F)
        in 201..300 -> Color(0xFF722ED1)
        else -> Color(0xFF1F1F1F)
    }

    Surface(
        modifier = Modifier
            .fillMaxWidth()
            .height(8.dp)
            .clip(RoundedCornerShape(4.dp)),
        color = Color.Black.copy(alpha = 0.1f)
    ) {
        Box(
            modifier = Modifier
                .fillMaxHeight()
                .fillMaxWidth(aqiProgress)
        ) {
            Surface(color = color) {}
        }
    }
}
