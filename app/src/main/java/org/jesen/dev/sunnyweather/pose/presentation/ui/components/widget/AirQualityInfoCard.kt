package org.jesen.dev.sunnyweather.pose.presentation.ui.components.widget

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp

/**
 * 空气质量数据项卡片组件
 * 
 * 用于展示污染物数据项（标签、数值+单位、描述）
 * 
 * @param label 污染物标签（如PM2.5）
 * @param value 污染物数值
 * @param unit 单位
 * @param description 污染物描述（如细颗粒物）
 * @param modifier 修饰符
 */
@Composable
fun AirQualityInfoCard(
    label: String,
    value: String,
    unit: String,
    description: String,
    modifier: Modifier = Modifier
) {
    Surface(
        modifier = modifier
            .clip(RoundedCornerShape(12.dp)),
        color = Color.White.copy(alpha = 0.8f),
        shadowElevation = 2.dp
    ) {
        Column(
            modifier = Modifier.padding(12.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(
                text = label,
                style = MaterialTheme.typography.bodySmall,
                color = Color.Black.copy(alpha = 0.5f)
            )
            Spacer(modifier = Modifier.height(4.dp))
            Text(
                text = "$value$unit",
                style = MaterialTheme.typography.bodyMedium,
                fontWeight = FontWeight.Medium,
                color = Color.Black.copy(alpha = 0.8f)
            )
            Spacer(modifier = Modifier.height(4.dp))
            Text(
                text = description,
                style = MaterialTheme.typography.labelSmall,
                color = Color.Black.copy(alpha = 0.4f)
            )
        }
    }
}
