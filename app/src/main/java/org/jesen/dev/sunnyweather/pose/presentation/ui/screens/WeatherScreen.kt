package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.core.FastOutSlowInEasing
import androidx.compose.animation.core.RepeatMode
import androidx.compose.animation.core.TweenSpec
import androidx.compose.animation.core.animateFloat
import androidx.compose.animation.core.infiniteRepeatable
import androidx.compose.animation.core.rememberInfiniteTransition
import androidx.compose.animation.core.tween
import androidx.compose.animation.fadeIn
import androidx.compose.animation.slideInVertically
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Menu
import androidx.compose.material.icons.filled.Search
import androidx.compose.material3.CenterAlignedTopAppBar
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.PlainTooltip
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TooltipAnchorPosition
import androidx.compose.material3.TooltipBox
import androidx.compose.material3.TooltipDefaults
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.material3.rememberTooltipState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.scale
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.input.nestedscroll.nestedScroll
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.model.Sky
import org.jesen.dev.sunnyweather.pose.domain.model.Weather
import org.jesen.dev.sunnyweather.pose.presentation.common.UiState
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.AnimatedBackground
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.PullToRefresh
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather.CurrentWeatherCard
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather.ForecastSection
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather.LifeIndexSection
import org.jesen.dev.sunnyweather.pose.presentation.ui.widget.EmptyContentView

/**
 * 天气页面组件
 *
 * 主要职责：
 * - 展示单个城市的天气信息
 * - 处理加载状态、错误状态和成功状态的UI展示
 * - 管理下拉刷新功能
 * - 提供导航到城市选择和设置页面的入口
 *
 * 技术要点：
 * - 根据天气状态动态显示不同内容（加载动画、错误提示、天气数据）
 * - 使用 AnimatedBackground 根据天气类型显示动态背景
 * - 使用 Scaffold 和 TopAppBar 构建页面骨架
 * - 支持下拉刷新和点击刷新两种方式
 * - 通过 LaunchedEffect 自动触发天气数据加载
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun WeatherScreen(
    place: Place,
    weatherState: UiState<Weather>,
    onWeatherLoaded: () -> Unit,
    onRefresh: () -> Unit,
    onNavigateToPlace: () -> Unit,
    onNavigateToSettings: () -> Unit = {},
    onMenuClick: () -> Unit = {}
) {
    var isRefreshing by remember { mutableStateOf(false) }

    LaunchedEffect(place.name) {
        if (weatherState is UiState.Loading || weatherState is UiState.Error) {
            onWeatherLoaded()
        }
    }

    LaunchedEffect(isRefreshing) {
        if (isRefreshing) {
            onRefresh()
            isRefreshing = false
        }
    }

    Box(modifier = Modifier.fillMaxSize()) {
        val sky = if (weatherState is UiState.Success) {
            Sky.getSky(weatherState.data.realtime.skycon)
        } else {
            Sky.getSky("CLEAR_DAY")
        }

        AnimatedBackground(sky = sky)

        when (val state = weatherState) {
            is UiState.Loading -> WeatherLoadingState()
            is UiState.Error -> EmptyContentView(
                message = state.message,
                onRetry = { isRefreshing = true }
            )
            is UiState.Success -> WeatherSuccessContent(
                weather = state.data,
                placeName = place.name,
                isRefreshing = isRefreshing,
                onRefresh = { isRefreshing = true },
                onNavigateToPlace = onNavigateToPlace,
                onNavigateToSettings = onNavigateToSettings,
                onMenuClick = onMenuClick
            )
        }
    }
}

/**
 * 加载状态组件
 *
 * 主要职责：
 * - 显示加载动画（缩放的 CircularProgressIndicator）
 * - 显示"加载中..."提示文字
 */
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
            color = MaterialTheme.colorScheme.onSurface
        )
        Spacer(modifier = Modifier.height(24.dp))
        Text(
            text = "加载中...",
            style = MaterialTheme.typography.bodyLarge,
            color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.8f)
        )
    }
}

/**
 * 天气成功内容组件
 *
 * 主要职责：
 * - 展示天气成功加载后的完整内容
 * - 包含 TopAppBar、当前天气卡片、预报列表和生活指数
 * - 支持下拉刷新功能
 * - 添加入场动画效果
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun WeatherSuccessContent(
    weather: Weather,
    placeName: String,
    isRefreshing: Boolean,
    onRefresh: () -> Unit,
    onNavigateToPlace: () -> Unit,
    onNavigateToSettings: () -> Unit,
    onMenuClick: () -> Unit
) {
    val scrollBehavior = TopAppBarDefaults.enterAlwaysScrollBehavior()

    Scaffold(
        modifier = Modifier.nestedScroll(scrollBehavior.nestedScrollConnection),
        topBar = {
            CenterAlignedTopAppBar(
                title = {
                    Text(
                        text = placeName,
                        style = MaterialTheme.typography.titleMedium,
                        fontWeight = FontWeight.Medium,
                        color = MaterialTheme.colorScheme.onSurface
                    )
                },
                navigationIcon = {
                    TooltipBox(
                        positionProvider =
                        TooltipDefaults.rememberTooltipPositionProvider(
                            TooltipAnchorPosition.Above
                        ),
                        tooltip = { PlainTooltip { Text("Menu") } },
                        state = rememberTooltipState(),
                    ) {
                        IconButton(onClick = onMenuClick) {
                            Icon(imageVector = Icons.Filled.Menu, contentDescription = "Menu")
                        }
                    }
                },
                actions = {
                    TooltipBox(
                        positionProvider =
                        TooltipDefaults.rememberTooltipPositionProvider(
                            TooltipAnchorPosition.Above
                        ),
                        tooltip = { PlainTooltip { Text("Search your city") } },
                        state = rememberTooltipState(),
                    ) {
                        IconButton(onClick = onNavigateToPlace) {
                            Icon(
                                imageVector = Icons.Default.Search,
                                contentDescription = "搜索城市",
                                tint = MaterialTheme.colorScheme.onSurface
                            )
                        }
                    }
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = Color.Transparent,
                    scrolledContainerColor = Color.Transparent,
                    titleContentColor = MaterialTheme.colorScheme.onSurface,
                    actionIconContentColor = MaterialTheme.colorScheme.onSurface
                ),
                modifier = Modifier.padding(top = 16.dp),
                scrollBehavior = scrollBehavior
            )
        }
    ) { innerPadding ->
        PullToRefresh(
            modifier = Modifier.fillMaxSize(),
            isRefreshing = isRefreshing,
            onRefresh = onRefresh
        ) {
            LazyColumn(
                modifier = Modifier.fillMaxSize(),
                contentPadding = PaddingValues(
                    top = innerPadding.calculateTopPadding() + 16.dp,
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
    }
}
