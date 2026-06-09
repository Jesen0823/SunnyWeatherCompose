package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import androidx.compose.animation.*
import androidx.compose.animation.core.*
import androidx.compose.ui.graphics.Color.Companion.Green
import androidx.compose.ui.graphics.Color.Companion.Yellow
import androidx.compose.ui.graphics.Color.Companion.Red
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.interaction.collectIsPressedAsState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Search
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.draw.scale
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.jesen.dev.sunnyweather.pose.domain.model.Sky
import org.jesen.dev.sunnyweather.pose.domain.model.Weather
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.PullToRefresh
import org.jesen.dev.sunnyweather.pose.presentation.ui.widget.EmptyContentView
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.UiState
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun WeatherScreen(
    viewModel: WeatherViewModel,
    placeName: String,
    lng: String,
    lat: String,
    onNavigateToPlace: () -> Unit,
    onNavigateToSettings: () -> Unit = {}
) {
    val weatherState = viewModel.weatherState.collectAsState()
    
    // 独立的刷新状态，不与 UiState 耦合
    var isRefreshing by remember { mutableStateOf(false) }
    
    // 初始化时加载数据
    LaunchedEffect(lng, lat) {
        viewModel.fetchWeather(lng, lat)
    }
    
    // 监听刷新状态，执行刷新逻辑
    LaunchedEffect(isRefreshing) {
        if (isRefreshing) {
            viewModel.fetchWeather(lng, lat)
            isRefreshing = false
        }
    }
    
    Box(modifier = Modifier.fillMaxSize()) {
        val sky = if (weatherState.value is UiState.Success) {
            Sky.getSky((weatherState.value as UiState.Success<Weather>).data.realtime.skycon)
        } else {
            Sky.getSky("CLEAR_DAY")
        }
        
        AnimatedBackground(sky = sky)
        
        Scaffold(
            topBar = {
                CenterAlignedTopAppBar(
                    title = {
                        Text(
                            text = placeName,
                            style = MaterialTheme.typography.titleMedium,
                            fontWeight = FontWeight.Medium,
                            color = Color.White
                        )
                    },
                    actions = {
                        IconButton(onClick = onNavigateToPlace) {
                            Icon(
                                imageVector = Icons.Default.Search,
                                contentDescription = "搜索城市",
                                tint = Color.White
                            )
                        }
                    },
                    colors = TopAppBarDefaults.centerAlignedTopAppBarColors(
                        containerColor = Color.Transparent,
                        scrolledContainerColor = Color.Transparent
                    ),
                    modifier = Modifier.padding(top = 16.dp)
                )
            }
        ) { innerPadding ->
            when (val state = weatherState.value) {
                is UiState.Loading -> WeatherLoadingState()
                is UiState.Error -> EmptyContentView(
                    message = state.message,
                    onRetry = { isRefreshing = true },
                    contentPadding = innerPadding
                )
                is UiState.Success -> {
                    PullToRefresh(
                        modifier = Modifier.fillMaxSize(),
                        isRefreshing = isRefreshing,
                        onRefresh = { isRefreshing = true }
                    ) {
                        WeatherContent(
                            weather = state.data,
                            placeName = placeName,
                            onNavigateToPlace = onNavigateToPlace,
                            contentPadding = innerPadding
                        )
                    }
                }
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
        val infiniteTransition = rememberInfiniteTransition(label = "loading-animation")
        val scaleState = infiniteTransition.animateFloat(
            initialValue = 0.8f,
            targetValue = 1.2f,
            animationSpec = infiniteRepeatable(
                animation = tween(1000, easing = FastOutSlowInEasing),
                repeatMode = RepeatMode.Reverse
            ),
            label = "loading-scale"
        )
        val scale = scaleState.value
        
        CircularProgressIndicator(
            modifier = Modifier
                .size(64.dp)
                .scale(scale),
            strokeWidth = 4.dp,
            color = Color.White
        )
        Spacer(modifier = Modifier.height(24.dp))
        Text(
            text = "加载中...",
            style = MaterialTheme.typography.bodyLarge,
            color = Color.White.copy(alpha = 0.8f)
        )
    }
}

@Composable
private fun WeatherContent(
    weather: Weather,
    placeName: String,
    onNavigateToPlace: () -> Unit,
    contentPadding: PaddingValues
) {
    val sky = Sky.getSky(weather.realtime.skycon)
    
    LazyColumn(
        modifier = Modifier.fillMaxSize(),
        contentPadding = PaddingValues(
            top = contentPadding.calculateTopPadding() + 16.dp,
            bottom = 24.dp
        )
    ) {
        item {
            AnimatedVisibility(
                visible = true,
                enter = slideInVertically(
                    initialOffsetY = { -60 },
                    animationSpec = TweenSpec(
                        durationMillis = 600,
                        easing = FastOutSlowInEasing
                    )
                ) + fadeIn(animationSpec = TweenSpec(500))
            ) {
                CurrentWeatherCard(
                    weather = weather,
                    placeName = placeName,
                    onNavigateToPlace = onNavigateToPlace
                )
            }
            Spacer(modifier = Modifier.height(20.dp))
        }
        
        item {
            AnimatedVisibility(
                visible = true,
                enter = slideInVertically(
                    initialOffsetY = { 60 },
                    animationSpec = tween(
                        durationMillis = 600,
                        easing = FastOutSlowInEasing,
                        delayMillis = 150
                    )
                ) + fadeIn(animationSpec = tween(500, delayMillis = 150))
            ) {
                ForecastSection(weather = weather)
            }
            Spacer(modifier = Modifier.height(20.dp))
        }
        
        item {
            AnimatedVisibility(
                visible = true,
                enter = slideInVertically(
                    initialOffsetY = { 60 },
                    animationSpec = tween(
                        durationMillis = 600,
                        easing = FastOutSlowInEasing,
                        delayMillis = 300
                    )
                ) + fadeIn(animationSpec = tween(500, delayMillis = 300))
            ) {
                LifeIndexSection(weather = weather)
            }
        }
    }
}

@Composable
private fun AnimatedBackground(sky: Sky) {
    val infiniteTransition = rememberInfiniteTransition(label = "background-animation")
    val alphaState = infiniteTransition.animateFloat(
        initialValue = 0.9f,
        targetValue = 1f,
        animationSpec = infiniteRepeatable(
            animation = tween(4000, easing = LinearEasing),
            repeatMode = RepeatMode.Reverse
        ),
        label = "background-alpha"
    )
    val alpha = alphaState.value
    
    Image(
        painter = painterResource(id = sky.bg),
        contentDescription = null,
        contentScale = ContentScale.Crop,
        modifier = Modifier
            .fillMaxSize()
            .graphicsLayer {
                this.alpha = alpha
                scaleX = 1.02f
                scaleY = 1.02f
            }
    )
}

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
                LaunchedEffect(Unit) {
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
                LaunchedEffect(Unit) {
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

@Composable
fun AnimatedForecastItem(index: Int, skycon: String, date: String, tempMin: Int, tempMax: Int) {
    val sky = Sky.getSky(skycon)
    val parsedDate = try {
        val isoFormat = java.time.format.DateTimeFormatter.ISO_OFFSET_DATE_TIME
        val dateTime = java.time.LocalDateTime.parse(date, isoFormat)
        dateTime.format(java.time.format.DateTimeFormatter.ofPattern("MM-dd"))
    } catch (e: Exception) {
        date
    }
    
    val interactionSource = remember { MutableInteractionSource() }
    val isPressed = interactionSource.collectIsPressedAsState().value
    val scale = animateFloatAsState(
        targetValue = if (isPressed) 0.98f else 1f,
        animationSpec = TweenSpec(100),
        label = "forecast-item-scale"
    )
    val alpha = remember { Animatable(0f) }
    
    LaunchedEffect(Unit) {
        alpha.animateTo(1f, tween(300, delayMillis = index * 100))
    }
    
    Surface(
        onClick = {},
        interactionSource = interactionSource,
        color = Color.Transparent,
        modifier = Modifier.scale(scale.value)
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(vertical = 10.dp)
                .graphicsLayer { this.alpha = alpha.value },
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            Text(
                text = parsedDate,
                style = MaterialTheme.typography.bodyMedium,
                color = Color.Black.copy(alpha = 0.7f)
            )
            
            Row(
                verticalAlignment = Alignment.CenterVertically
            ) {
                Image(
                    painter = painterResource(id = sky.icon),
                    contentDescription = sky.info,
                    modifier = Modifier.size(28.dp)
                )
                Spacer(modifier = Modifier.width(10.dp))
                Text(
                    text = sky.info,
                    style = MaterialTheme.typography.bodyMedium,
                    color = Color.Black.copy(alpha = 0.7f)
                )
            }
            
            Text(
                text = "$tempMin° ~ $tempMax°",
                style = MaterialTheme.typography.bodyMedium,
                color = Color.Black.copy(alpha = 0.8f),
                fontWeight = FontWeight.Medium
            )
        }
    }
}

@Composable
fun LifeIndexSection(weather: Weather) {
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
            
            val lifeIndex = weather.daily.lifeIndex
            
            AnimatedLifeIndexItem(0, "穿衣指数", lifeIndex.dressing[0].desc)
            AnimatedLifeIndexItem(1, "感冒风险", lifeIndex.coldRisk[0].desc)
            AnimatedLifeIndexItem(2, "紫外线", lifeIndex.ultraviolet[0].desc)
            AnimatedLifeIndexItem(3, "洗车指数", lifeIndex.carWashing[0].desc)
        }
    }
}

@Composable
fun AnimatedLifeIndexItem(index: Int, title: String, desc: String) {
    val alpha = remember { Animatable(0f) }
    val offsetX = remember { Animatable(20f) }
    
    LaunchedEffect(Unit) {
        alpha.animateTo(1f, tween(300, delayMillis = index * 100))
        offsetX.animateTo(0f, tween(300, delayMillis = index * 100, easing = FastOutSlowInEasing))
    }
    
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 8.dp)
            .graphicsLayer {
                this.alpha = alpha.value
                translationX = offsetX.value
            },
        horizontalArrangement = Arrangement.SpaceBetween
    ) {
        Text(
            text = title,
            style = MaterialTheme.typography.bodyMedium,
            fontWeight = FontWeight.Medium,
            color = Color.Black.copy(alpha = 0.7f)
        )
        Text(
            text = desc,
            style = MaterialTheme.typography.bodyMedium,
            color = Color.Black.copy(alpha = 0.8f)
        )
    }
}
