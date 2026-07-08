/**
 * Sunrise sunset card component
 * 
 * Main responsibilities:
 * - Display today's sunrise and sunset times
 * - Add sun icon and gradient background
 * - Get weather data through LocalProvider
 * 
 * Technical points:
 * - Use CompositionLocal to get weather data (requireLocalWeather)
 * - Use gradient background from orange to purple (simulating sunrise to sunset transition)
 * - Use AnimatedVisibility for entry animation
 * - Display sunrise/sunset times and daylight duration
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.core.FastOutSlowInEasing
import androidx.compose.animation.core.TweenSpec
import androidx.compose.animation.fadeIn
import androidx.compose.animation.slideInVertically
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.WbSunny
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
fun SunriseSunsetCard() {
    val weather = requireLocalWeather()
    
    val astroData = weather.daily.astro?.firstOrNull()

    if (astroData == null) {
        return
    }

    fun calculateDayLength(sunrise: String, sunset: String): String {
        val (sunriseHour, sunriseMinute) = sunrise.split(":").map { it.toInt() }
        val (sunsetHour, sunsetMinute) = sunset.split(":").map { it.toInt() }
        
        var totalMinutes = (sunsetHour * 60 + sunsetMinute) - (sunriseHour * 60 + sunriseMinute)
        if (totalMinutes < 0) {
            totalMinutes += 24 * 60
        }
        
        val hours = totalMinutes / 60
        val minutes = totalMinutes % 60
        
        return if (hours > 0 && minutes > 0) {
            "${hours}h ${minutes}m"
        } else if (hours > 0) {
            "${hours}h"
        } else {
            "${minutes}m"
        }
    }

    val dayLength = calculateDayLength(astroData.sunrise.time, astroData.sunset.time)

    Surface(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
            .clip(RoundedCornerShape(20.dp)),
        color = Color.Transparent,
        shadowElevation = 8.dp
    ) {
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .background(
                    brush = androidx.compose.ui.graphics.Brush.horizontalGradient(
                        colors = listOf(
                            Color(0xFFFF9A9E),
                            Color(0xFFFECFEF),
                            Color(0xFFA18CD1)
                        )
                    )
                )
                .padding(20.dp)
        ) {
            Column(
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                AnimatedVisibility(
                    visible = true,
                    enter = slideInVertically(
                        initialOffsetY = { 30 },
                        animationSpec = TweenSpec(
                            durationMillis = 500,
                            easing = FastOutSlowInEasing
                        )
                    ) + fadeIn(animationSpec = TweenSpec(400))
                ) {
                    Icon(
                        imageVector = Icons.Filled.WbSunny,
                        contentDescription = "Sun",
                        tint = Color.White,
                        modifier = Modifier.size(48.dp)
                    )
                }
                Spacer(modifier = Modifier.height(12.dp))
                
                AnimatedVisibility(
                    visible = true,
                    enter = slideInVertically(
                        initialOffsetY = { 20 },
                        animationSpec = TweenSpec(
                            durationMillis = 500,
                            easing = FastOutSlowInEasing,
                            delay = 100
                        )
                    ) + fadeIn(animationSpec = TweenSpec(400, delay = 100))
                ) {
                    Text(
                        text = "Sunrise & Sunset",
                        style = MaterialTheme.typography.titleMedium,
                        fontWeight = FontWeight.Bold,
                        color = Color.White
                    )
                }
                Spacer(modifier = Modifier.height(16.dp))
                
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceEvenly
                ) {
                    AnimatedVisibility(
                        visible = true,
                        enter = slideInVertically(
                            initialOffsetY = { 20 },
                            animationSpec = TweenSpec(
                                durationMillis = 400,
                                easing = FastOutSlowInEasing,
                                delay = 200
                            )
                        ) + fadeIn(animationSpec = TweenSpec(300, delay = 200))
                    ) {
                        Column(
                            horizontalAlignment = Alignment.CenterHorizontally
                        ) {
                            Icon(
                                imageVector = Icons.Filled.WbSunny,
                                contentDescription = "Sunrise",
                                tint = Color(0xFFFFD700),
                                modifier = Modifier.size(32.dp)
                            )
                            Spacer(modifier = Modifier.height(8.dp))
                            Text(
                                text = "Sunrise",
                                style = MaterialTheme.typography.bodySmall,
                                color = Color.White.copy(alpha = 0.9f)
                            )
                            Spacer(modifier = Modifier.height(4.dp))
                            Text(
                                text = astroData.sunrise.time,
                                style = MaterialTheme.typography.titleLarge,
                                fontWeight = FontWeight.Bold,
                                color = Color.White
                            )
                        }
                    }
                    
                    AnimatedVisibility(
                        visible = true,
                        enter = slideInVertically(
                            initialOffsetY = { 20 },
                            animationSpec = TweenSpec(
                                durationMillis = 400,
                                easing = FastOutSlowInEasing,
                                delay = 250
                            )
                        ) + fadeIn(animationSpec = TweenSpec(300, delay = 250))
                    ) {
                        Column(
                            horizontalAlignment = Alignment.CenterHorizontally
                        ) {
                            Icon(
                                imageVector = Icons.Filled.WbSunny,
                                contentDescription = "Sunset",
                                tint = Color(0xFFFF6B6B),
                                modifier = Modifier.size(32.dp)
                            )
                            Spacer(modifier = Modifier.height(8.dp))
                            Text(
                                text = "Sunset",
                                style = MaterialTheme.typography.bodySmall,
                                color = Color.White.copy(alpha = 0.9f)
                            )
                            Spacer(modifier = Modifier.height(4.dp))
                            Text(
                                text = astroData.sunset.time,
                                style = MaterialTheme.typography.titleLarge,
                                fontWeight = FontWeight.Bold,
                                color = Color.White
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
                            delay = 300
                        )
                    ) + fadeIn(animationSpec = TweenSpec(300, delay = 300))
                ) {
                    Text(
                        text = "Daylight: $dayLength",
                        style = MaterialTheme.typography.bodyMedium,
                        color = Color.White.copy(alpha = 0.9f)
                    )
                }
            }
        }
    }
}