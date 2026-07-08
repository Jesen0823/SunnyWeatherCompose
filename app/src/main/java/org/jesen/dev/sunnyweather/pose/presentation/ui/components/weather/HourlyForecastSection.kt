/**
 * 24小时预报组件
 * 
 * 主要职责：
 * - 展示未来24小时的逐小时天气变化
 * - 使用横向滚动布局
 * - 展示时间、天气图标、温度、降水概率
 * - 添加温度趋势连接线
 * - 通过LocalProvider获取天气数据
 * 
 * 技术要点：
 * - 使用CompositionLocal获取天气数据（requireLocalWeather）
 * - 使用LazyRow实现横向滚动
 * - 根据skycon显示对应的天气图标
 * - 使用Canvas绘制温度趋势连接线
 * - 使用AnimatedVisibility实现入场动画
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.core.FastOutSlowInEasing
import androidx.compose.animation.core.TweenSpec
import androidx.compose.animation.fadeIn
import androidx.compose.animation.slideInHorizontally
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyRow
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.Path
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.domain.model.HourlyPrecipitationItem
import org.jesen.dev.sunnyweather.pose.domain.model.HourlySkyconItem
import org.jesen.dev.sunnyweather.pose.domain.model.HourlyTemperatureItem
import org.jesen.dev.sunnyweather.pose.domain.model.Sky
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.requireLocalWeather

@Composable
fun HourlyForecastSection() {
    val weather = requireLocalWeather()
    val hourly = weather.hourly

    if (hourly == null || hourly.skycon.isNullOrEmpty()) {
        return
    }

    val skyconList = hourly.skycon!!
    val tempList = hourly.temperature ?: emptyList()
    val precipList = hourly.precipitation ?: emptyList()

    val hourlyItems = skyconList.mapIndexed { index, skyconItem ->
        HourlyForecastItem(
            index = index,
            datetime = skyconItem.datetime,
            skycon = skyconItem.value,
            temperature = tempList.getOrNull(index)?.value ?: 0f,
            precipitationProbability = precipList.getOrNull(index)?.probability ?: 0f
        )
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
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "24小时预报",
                    style = MaterialTheme.typography.titleMedium,
                    fontWeight = FontWeight.Bold,
                    color = Color.Black.copy(alpha = 0.8f)
                )
                if (!hourly.description.isNullOrEmpty()) {
                    Text(
                        text = hourly.description,
                        style = MaterialTheme.typography.labelSmall,
                        color = Color.Black.copy(alpha = 0.5f),
                        maxLines = 1
                    )
                }
            }
            Spacer(modifier = Modifier.height(16.dp))

            Box(modifier = Modifier.fillMaxWidth()) {
                HourlyTemperatureTrendLine(hourlyItems.take(24))

                LazyRow(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.spacedBy(16.dp),
                    contentPadding = PaddingValues(end = 16.dp)
                ) {
                    itemsIndexed(hourlyItems.take(24)) { index, item ->
                        HourlyForecastItemWithAnimation(item = item, index = index)
                    }
                }
            }
        }
    }
}

/**
 * 带动画的小时预报卡片
 * 
 * @param item 小时预报数据
 * @param index 索引（用于动画延迟）
 */
@Composable
private fun HourlyForecastItemWithAnimation(item: HourlyForecastItem, index: Int) {
    AnimatedVisibility(
        visible = true,
        enter = slideInHorizontally(
            initialOffsetX = { 50 },
            animationSpec = TweenSpec(
                durationMillis = 400,
                easing = FastOutSlowInEasing,
                delay = index * 30
            )
        ) + fadeIn(animationSpec = TweenSpec(300))
    ) {
        HourlyForecastItemCard(item = item)
    }
}

/**
 * 温度趋势连接线
 * 
 * 使用Canvas绘制温度变化趋势线和温度点标记
 * 
 * @param items 小时预报数据列表
 */
@Composable
private fun HourlyTemperatureTrendLine(items: List<HourlyForecastItem>) {
    if (items.size < 2) return

    val minTemp = items.minOf { it.temperature } - 5
    val maxTemp = items.maxOf { it.temperature } + 5
    val tempRange = maxTemp - minTemp

    Canvas(
        modifier = Modifier
            .fillMaxWidth()
            .height(100.dp)
            .padding(start = 30.dp)
    ) {
        val itemWidth = 60.dp.toPx()
        val spacing = 16.dp.toPx()
        val totalSpacing = spacing
        val cardWidth = itemWidth

        val path = Path().apply {
            items.forEachIndexed { index, item ->
                val x = index * (cardWidth + totalSpacing) + cardWidth / 2
                val y = size.height - ((item.temperature - minTemp) / tempRange) * (size.height - 20.dp.toPx()) - 10.dp.toPx()
                
                if (index == 0) {
                    moveTo(x, y)
                } else {
                    lineTo(x, y)
                }
            }
        }

        drawPath(
            path = path,
            color = Color(0xFFFF6B6B),
            style = Stroke(
                width = 3.dp.toPx(),
                cap = StrokeCap.Round
            )
        )

        items.forEachIndexed { index, item ->
            val x = index * (cardWidth + totalSpacing) + cardWidth / 2
            val y = size.height - ((item.temperature - minTemp) / tempRange) * (size.height - 20.dp.toPx()) - 10.dp.toPx()
            
            drawCircle(
                color = Color(0xFFFF6B6B),
                radius = 6.dp.toPx(),
                center = androidx.compose.ui.geometry.Offset(x, y)
            )
            
            drawCircle(
                color = Color.White,
                radius = 3.dp.toPx(),
                center = androidx.compose.ui.geometry.Offset(x, y)
            )
        }
    }
}

/**
 * 小时预报数据项数据类
 * 
 * @param index 索引
 * @param datetime 日期时间
 * @param skycon 天气现象代码
 * @param temperature 温度
 * @param precipitationProbability 降水概率
 */
private data class HourlyForecastItem(
    val index: Int,
    val datetime: String,
    val skycon: String,
    val temperature: Float,
    val precipitationProbability: Float
)

/**
 * 小时预报卡片
 * 
 * @param item 小时预报数据
 */
@Composable
private fun HourlyForecastItemCard(item: HourlyForecastItem) {
    val sky = Sky.getSky(item.skycon)
    
    /**
     * 获取时间显示文本
     * 
     * @return 时间显示文本（当前显示"现在"，其他显示HH:MM）
     */
    fun getTimeDisplay(): String {
        return if (item.index == 0) {
            "现在"
        } else {
            val timePart = item.datetime.split("T").getOrNull(1)?.split("+")?.getOrNull(0)
            timePart?.substring(0, 5) ?: "${(item.index % 24).toString().padStart(2, '0')}:00"
        }
    }

    Column(
        horizontalAlignment = Alignment.CenterHorizontally,
        modifier = Modifier.width(60.dp)
    ) {
        Text(
            text = getTimeDisplay(),
            style = MaterialTheme.typography.bodySmall,
            color = Color.Black.copy(alpha = 0.6f)
        )
        Spacer(modifier = Modifier.height(8.dp))
        
        Image(
            painter = painterResource(id = sky.icon),
            contentDescription = sky.info,
            modifier = Modifier.size(32.dp)
        )
        Spacer(modifier = Modifier.height(8.dp))
        
        Text(
            text = "${item.temperature.toInt()}°",
            style = MaterialTheme.typography.bodyMedium,
            fontWeight = FontWeight.Medium,
            color = Color.Black.copy(alpha = 0.8f)
        )
        
        if (item.precipitationProbability > 0) {
            Spacer(modifier = Modifier.height(4.dp))
            Text(
                text = "${item.precipitationProbability.toInt()}%",
                style = MaterialTheme.typography.labelSmall,
                color = Color(0xFF4ECDC4)
            )
        }
    }
}
