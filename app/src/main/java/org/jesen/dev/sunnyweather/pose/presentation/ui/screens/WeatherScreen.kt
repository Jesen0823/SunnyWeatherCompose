package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.jesen.dev.sunnyweather.pose.R
import org.jesen.dev.sunnyweather.pose.domain.model.Sky
import org.jesen.dev.sunnyweather.pose.domain.model.Weather
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.PullToRefresh
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.UiState
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

@Composable
fun WeatherScreen(
    viewModel: WeatherViewModel,
    placeName: String,
    lng: String,
    lat: String,
    onNavigateToPlace: () -> Unit
) {
    val weatherState = viewModel.weatherState.collectAsState()
    
    LaunchedEffect(lng, lat) {
        viewModel.fetchWeather(lng, lat)
    }
    
    PullToRefresh(
        isRefreshing = weatherState.value is UiState.Loading,
        onRefresh = { viewModel.fetchWeather(lng, lat) }
    ) {
        Box(modifier = Modifier.fillMaxSize()) {
            when (val state = weatherState.value) {
                is UiState.Loading -> WeatherLoadingState()
                is UiState.Error -> WeatherErrorState(state.message)
                is UiState.Success -> WeatherContent(
                    weather = state.data,
                    placeName = placeName,
                    onNavigateToPlace = onNavigateToPlace
                )
            }
        }
    }
}

@Composable
private fun WeatherLoadingState() {
    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        CircularProgressIndicator()
        Spacer(modifier = Modifier.height(16.dp))
        Text(
            text = "加载中...",
            style = MaterialTheme.typography.bodyLarge
        )
    }
}

@Composable
private fun WeatherErrorState(message: String) {
    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Text(
            text = "加载失败",
            style = MaterialTheme.typography.titleLarge,
            fontWeight = FontWeight.Bold
        )
        Spacer(modifier = Modifier.height(8.dp))
        Text(
            text = message,
            style = MaterialTheme.typography.bodyMedium,
            color = Color.Gray
        )
    }
}

@Composable
private fun WeatherContent(
    weather: Weather,
    placeName: String,
    onNavigateToPlace: () -> Unit
) {
    val sky = Sky.getSky(weather.realtime.skycon)
    
    Box(
        modifier = Modifier.fillMaxSize()
    ) {
        Image(
            painter = painterResource(id = sky.bg),
            contentDescription = null,
            modifier = Modifier.fillMaxSize()
        )
        
        LazyColumn(
            modifier = Modifier.fillMaxSize(),
            contentPadding = PaddingValues(top = 16.dp, bottom = 16.dp)
        ) {
            item {
                CurrentWeatherCard(
                    weather = weather,
                    placeName = placeName,
                    onNavigateToPlace = onNavigateToPlace
                )
                Spacer(modifier = Modifier.height(16.dp))
            }
            
            item {
                ForecastSection(weather = weather)
                Spacer(modifier = Modifier.height(16.dp))
            }
            
            item {
                LifeIndexSection(weather = weather)
            }
        }
    }
}

@Composable
fun CurrentWeatherCard(
    weather: Weather,
    placeName: String,
    onNavigateToPlace: () -> Unit
) {
    val sky = Sky.getSky(weather.realtime.skycon)
    val realtime = weather.realtime
    
    Surface(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
            .clip(RoundedCornerShape(16.dp)),
        color = Color.White.copy(alpha = 0.8f)
    ) {
        Column(
            modifier = Modifier.padding(16.dp),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Surface(
                onClick = onNavigateToPlace,
                color = Color.Transparent
            ) {
                Text(
                    text = placeName,
                    style = MaterialTheme.typography.titleMedium,
                    fontWeight = FontWeight.Bold
                )
            }
            Spacer(modifier = Modifier.height(8.dp))
            
            Row(
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "${realtime.temperature.toInt()}",
                    fontSize = 64.sp,
                    fontWeight = FontWeight.Bold
                )
                Text(
                    text = "°C",
                    fontSize = 24.sp
                )
                Spacer(modifier = Modifier.width(16.dp))
                Image(
                    painter = painterResource(id = sky.icon),
                    contentDescription = sky.info,
                    modifier = Modifier.size(64.dp)
                )
            }
            Spacer(modifier = Modifier.height(8.dp))
            
            Text(
                text = sky.info,
                style = MaterialTheme.typography.titleLarge,
                fontWeight = FontWeight.Medium
            )
            Spacer(modifier = Modifier.height(4.dp))
            
            Text(
                text = "空气指数 ${realtime.airQuality.aqi.chn.toInt()}",
                style = MaterialTheme.typography.bodyMedium
            )
        }
    }
}

@Composable
fun ForecastSection(weather: Weather) {
    Surface(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
            .clip(RoundedCornerShape(16.dp)),
        color = Color.White.copy(alpha = 0.8f)
    ) {
        Column(
            modifier = Modifier.padding(16.dp)
        ) {
            Text(
                text = "未来天气预报",
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(12.dp))
            
            LazyColumn {
                items(weather.daily.skycon) { skycon ->
                    val index = weather.daily.skycon.indexOf(skycon)
                    val temperature = weather.daily.temperature[index]
                    ForecastItem(
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

@Composable
fun ForecastItem(skycon: String, date: Long, tempMin: Int, tempMax: Int) {
    val sky = Sky.getSky(skycon)
    val sdf = SimpleDateFormat("MM-dd", Locale.getDefault())
    
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 8.dp),
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.SpaceBetween
    ) {
        Text(
            text = sdf.format(Date(date)),
            style = MaterialTheme.typography.bodyMedium
        )
        
        Row(
            verticalAlignment = Alignment.CenterVertically
        ) {
            Image(
                painter = painterResource(id = sky.icon),
                contentDescription = sky.info,
                modifier = Modifier.size(24.dp)
            )
            Spacer(modifier = Modifier.width(8.dp))
            Text(
                text = sky.info,
                style = MaterialTheme.typography.bodyMedium
            )
        }
        
        Text(
            text = "$tempMin° ~ $tempMax°",
            style = MaterialTheme.typography.bodyMedium
        )
    }
}

@Composable
fun LifeIndexSection(weather: Weather) {
    Surface(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
            .clip(RoundedCornerShape(16.dp)),
        color = Color.White.copy(alpha = 0.8f)
    ) {
        Column(
            modifier = Modifier.padding(16.dp)
        ) {
            Text(
                text = "生活指数",
                style = MaterialTheme.typography.titleMedium,
                fontWeight = FontWeight.Bold
            )
            Spacer(modifier = Modifier.height(12.dp))
            
            val lifeIndex = weather.daily.lifeIndex
            
            LifeIndexItem("穿衣指数", lifeIndex.dressing[0].desc)
            LifeIndexItem("感冒风险", lifeIndex.coldRisk[0].desc)
            LifeIndexItem("紫外线", lifeIndex.ultraviolet[0].desc)
            LifeIndexItem("洗车指数", lifeIndex.carWashing[0].desc)
        }
    }
}

@Composable
fun LifeIndexItem(title: String, desc: String) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 6.dp),
        horizontalArrangement = Arrangement.SpaceBetween
    ) {
        Text(
            text = title,
            style = MaterialTheme.typography.bodyMedium,
            fontWeight = FontWeight.Medium
        )
        Text(
            text = desc,
            style = MaterialTheme.typography.bodyMedium
        )
    }
}

