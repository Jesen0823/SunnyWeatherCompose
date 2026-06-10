package org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather

import androidx.compose.animation.core.*
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.interaction.collectIsPressedAsState
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.scale
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
import org.jesen.dev.sunnyweather.pose.domain.model.Weather

@Composable
fun CurrentWeatherCard(
    weather: Weather,
    placeName: String,
    onNavigateToPlace: () -> Unit
) {
    val sky = Sky.getSky(weather.realtime.skycon)
    val realtime = weather.realtime
    
    val interactionSource = remember { MutableInteractionSource() }
    val isPressed = interactionSource.collectIsPressedAsState().value
    val scale = animateFloatAsState(
        targetValue = if (isPressed) 0.98f else 1f,
        animationSpec = TweenSpec(150),
        label = "card-scale"
    )
    val elevation = animateDpAsState(
        targetValue = if (isPressed) 4.dp else 12.dp,
        animationSpec = TweenSpec(150),
        label = "card-elevation"
    )
    
    Surface(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
            .clip(RoundedCornerShape(20.dp))
            .scale(scale.value),
        color = Color.White.copy(alpha = 0.9f),
        shadowElevation = elevation.value
    ) {
        Column(
            modifier = Modifier.padding(24.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Surface(
                onClick = onNavigateToPlace,
                color = Color.Transparent,
                interactionSource = interactionSource
            ) {
                Text(
                    text = placeName,
                    style = MaterialTheme.typography.titleMedium,
                    fontWeight = FontWeight.Bold,
                    color = Color.Black.copy(alpha = 0.8f)
                )
            }
            Spacer(modifier = Modifier.height(12.dp))
            
            Row(
                verticalAlignment = Alignment.CenterVertically
            ) {
                val tempScale = remember { Animatable(0f) }
                androidx.compose.runtime.LaunchedEffect(Unit) {
                    tempScale.animateTo(1f, tween(800, easing = FastOutSlowInEasing))
                }
                
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
                
                val iconScale = remember { Animatable(0.5f) }
                androidx.compose.runtime.LaunchedEffect(Unit) {
                    iconScale.animateTo(1f, tween(600, easing = FastOutSlowInEasing))
                }
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
