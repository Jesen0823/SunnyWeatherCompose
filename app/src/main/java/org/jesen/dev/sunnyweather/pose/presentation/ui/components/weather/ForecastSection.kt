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
