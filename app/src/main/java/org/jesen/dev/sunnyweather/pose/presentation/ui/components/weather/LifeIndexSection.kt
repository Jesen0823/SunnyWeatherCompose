/**
 * 生活指数区域组件
 * 
 * 主要职责：
 * - 显示生活指数信息（穿衣、感冒、紫外线、洗车）
 * - 封装生活指数卡片样式
 * - 通过 LocalProvider 获取天气数据
 * 
 * 技术要点：
 * - 使用 Surface 作为容器，设置圆角和阴影
 * - 使用 Grid 布局展示四个生活指数
 * - 调用 AnimatedLifeIndexItem 展示每个生活指数
 * - 使用 derivedStateOf 优化数据计算
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
fun LifeIndexSection() {
    val weather = requireLocalWeather()

    /**
     * 使用 derivedStateOf 计算生活指数数据
     * 
     * 性能优化原理：
     * - listOf() 会创建新的列表对象，每次调用都有内存分配开销
     * - firstOrNull() 方法每次调用都需要遍历列表
     * - 如果组件因为其他状态（如动画）重组，但 weather 没变，会重复计算
     * - derivedStateOf 会缓存计算结果，只有 weather 变化时才重新计算
     * - 提升渲染性能，减少不必要的内存分配和计算
     * 
     * 数据获取说明：
     * - 生活指数数据存储在 weather.daily.lifeIndex 中
     * - 每个指数类型（穿衣、感冒、紫外线、洗车）都是一个列表
     * - 使用 firstOrNull() 获取当天的指数数据（列表第一个元素）
     * - 如果数据为空，使用空字符串作为默认值
     * 
     * 对比示例：
     * // 未优化：每次重组都创建新列表
     * val lifeIndexItems = listOf(
     *     "穿衣" to (weather.daily.lifeIndex.dressing.firstOrNull()?.desc ?: ""),
     *     "感冒" to (weather.daily.lifeIndex.coldRisk.firstOrNull()?.desc ?: ""),
     *     "紫外线" to (weather.daily.lifeIndex.ultraviolet.firstOrNull()?.desc ?: ""),
     *     "洗车" to (weather.daily.lifeIndex.carWashing.firstOrNull()?.desc ?: "")
     * )
     * 
     * // 优化后：缓存计算结果
     * val lifeIndexItems by remember(weather) {
     *     derivedStateOf {
     *         listOf(...)
     *     }
     * }
     */
    val lifeIndexItems by remember(weather) {
        derivedStateOf {
            listOf(
                // 穿衣指数：获取穿衣建议描述
                "穿衣" to (weather.daily.lifeIndex.dressing.firstOrNull()?.desc ?: ""),
                // 感冒指数：获取感冒风险描述
                "感冒" to (weather.daily.lifeIndex.coldRisk.firstOrNull()?.desc ?: ""),
                // 紫外线指数：获取紫外线强度描述
                "紫外线" to (weather.daily.lifeIndex.ultraviolet.firstOrNull()?.desc ?: ""),
                // 洗车指数：获取洗车建议描述
                "洗车" to (weather.daily.lifeIndex.carWashing.firstOrNull()?.desc ?: "")
            )
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
                text = "生活指数",
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.Bold,
                color = Color.Black.copy(alpha = 0.8f)
            )
            Spacer(modifier = Modifier.height(16.dp))
            
            Column {
                lifeIndexItems.forEachIndexed { index, item ->
                    AnimatedLifeIndexItem(
                        index = index,
                        title = item.first,
                        desc = item.second
                    )
                }
            }
        }
    }
}