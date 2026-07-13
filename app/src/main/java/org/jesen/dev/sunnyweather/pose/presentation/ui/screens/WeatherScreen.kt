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
import androidx.compose.runtime.derivedStateOf
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberUpdatedState
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
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.WeatherAndPlaceProvider
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.PullToRefresh
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather.CurrentWeatherCard
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather.ForecastSection
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather.LifeIndexSection
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather.WeatherDetailsCard
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather.SunriseSunsetCard
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather.AirQualityCard
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.weather.HourlyForecastSection
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
 * - 使用 derivedStateOf 计算 sky 值，避免每次重组都重新计算
 * - 使用 rememberUpdatedState 捕获回调，避免 LaunchedEffect 频繁重启
 * - 使用 LaunchedEffect 处理副作用，如自动触发天气数据加载
 * - 使用 WeatherAndPlaceProvider 跨组件共享天气和城市数据
 * - 根据天气状态动态显示不同内容（加载动画、错误提示、天气数据）
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

    /**
     * 使用 rememberUpdatedState 捕获最新的 onWeatherLoaded 回调
     * 
     * 性能优化原理：
     * - 在 Compose 中，lambda 表达式每次重组都会创建新的实例
     * - 如果将 lambda 作为 LaunchedEffect 的键，会导致每次重组都重启协程
     * - rememberUpdatedState 会创建一个 State 对象，保持引用不变
     * - 但每次重组时更新内部值，这样 LaunchedEffect 不会因为 lambda 变化而重启
     * 
     * 场景说明：
     * - LaunchedEffect 的键是 place.name，当 place.name 不变时，LaunchedEffect 不会重启
     * - 如果 onWeatherLoaded 是一个 lambda（如来自父组件的回调），每次重组可能创建新实例
     * - 使用 rememberUpdatedState 可以确保 LaunchedEffect 中始终使用最新的回调引用
     * - 避免不必要的 LaunchedEffect 重启，提升性能
     * 
     * 对比示例：
     * // 未优化：每次重组都会重启 LaunchedEffect
     * LaunchedEffect(onWeatherLoaded) {
     *     onWeatherLoaded()
     * }
     * 
     * // 优化后：只有 place.name 变化时才重启
     * val onWeatherLoadedUpdated by rememberUpdatedState(onWeatherLoaded)
     * LaunchedEffect(place.name) {
     *     onWeatherLoadedUpdated()
     * }
     */
    val onWeatherLoadedUpdated by rememberUpdatedState(onWeatherLoaded)

    /**
     * 使用 rememberUpdatedState 捕获最新的 onRefresh 回调
     */
    val onRefreshUpdated by rememberUpdatedState(onRefresh)

    /**
     * 使用 derivedStateOf 计算 sky 值
     * 
     * 性能优化原理：
     * - derivedStateOf 是 Compose 提供的状态计算函数
     * - 它会缓存计算结果，只在依赖项（weatherState）变化时重新计算
     * - 如果组件因为其他状态变化而重组，但 weatherState 没变，sky 值会直接从缓存获取
     * - 避免每次重组都执行 Sky.getSky() 方法，减少不必要的计算开销
     * 
     * 对比示例：
     * // 未优化：每次重组都会执行 Sky.getSky()
     * val sky = if (weatherState is UiState.Success) {
     *     Sky.getSky(weatherState.data.realtime.skycon)
     * } else {
     *     Sky.getSky("CLEAR_DAY")
     * }
     * 
     * // 优化后：只有 weatherState 变化时才重新计算
     * val sky by remember(weatherState) {
     *     derivedStateOf {
     *         if (weatherState is UiState.Success) {
     *             Sky.getSky(weatherState.data.realtime.skycon)
     *         } else {
     *             Sky.getSky("CLEAR_DAY")
     *         }
     *     }
     * }
     * 
     * 场景说明：
     * - sky 值是基于 weatherState 计算得出的
     * - 当 weatherState 是 UiState.Loading 或 UiState.Error 时，使用默认值 CLEAR_DAY
     * - 当 weatherState 是 UiState.Success 时，从数据中获取 skycon
     */
    val sky by remember(weatherState) {
        derivedStateOf {
            if (weatherState is UiState.Success) {
                Sky.getSky(weatherState.data.realtime.skycon)
            } else {
                Sky.getSky("CLEAR_DAY")
            }
        }
    }

    /**
     * LaunchedEffect - 组件副作用
     * 
     * 场景说明：
     * - 当 place.name 变化时，自动触发天气数据加载
     * - 使用 onWeatherLoadedUpdated（rememberUpdatedState）确保使用最新回调
     * - LaunchedEffect 的键是 place.name，只有当城市名称变化时才会执行
     * - 这是一个"一次性"副作用，不会在每次重组时执行
     */
    LaunchedEffect(place.name) {
        onWeatherLoadedUpdated()
    }

    /**
     * LaunchedEffect - 处理下拉刷新
     * 
     * 场景说明：
     * - 当 isRefreshing 变为 true 时，触发刷新操作
     * - 使用 onRefreshUpdated（rememberUpdatedState）确保使用最新回调
     * - 刷新完成后重置 isRefreshing 为 false
     * - 注意：isRefreshing 在协程中被修改，需要确保线程安全
     */
    LaunchedEffect(isRefreshing) {
        if (isRefreshing) {
            onRefreshUpdated()
            isRefreshing = false
        }
    }

    Box(modifier = Modifier.fillMaxSize()) {
        AnimatedBackground(sky = sky)

        when (val state = weatherState) {
            is UiState.Loading -> WeatherLoadingState()
            is UiState.Error -> EmptyContentView(
                message = state.message,
                onRetry = { isRefreshing = true }
            )
            is UiState.Success -> WeatherSuccessContent(
                weather = state.data,
                place = place,
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
 * 使用 WeatherAndPlaceProvider 包裹子组件，实现跨组件数据共享
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
private fun WeatherSuccessContent(
    weather: Weather,
    place: Place,
    isRefreshing: Boolean,
    onRefresh: () -> Unit,
    onNavigateToPlace: () -> Unit,
    onNavigateToSettings: () -> Unit,
    onMenuClick: () -> Unit
) {
    val scrollBehavior = TopAppBarDefaults.enterAlwaysScrollBehavior()

    val alpha by remember {
        derivedStateOf {
            (1f - scrollBehavior.state.collapsedFraction).coerceIn(0.1f, 1f)
        }
    }

    /**
     * 使用 WeatherAndPlaceProvider 共享天气和城市数据
     * 
     * 场景说明：
     * - CurrentWeatherCard、ForecastSection、LifeIndexSection 都需要 weather 和 place 数据
     * - 通过 LocalProvider 提供数据，子组件可以通过 LocalWeather.current 获取
     * - 避免逐层传递参数（props drilling），提升代码可维护性
     */
    WeatherAndPlaceProvider(weather = weather, place = place) {
        Scaffold(
            modifier = Modifier.nestedScroll(scrollBehavior.nestedScrollConnection),
            topBar = {
                CenterAlignedTopAppBar(
                    title = {
                        Text(
                            text = place.name,
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
                        containerColor = MaterialTheme.colorScheme.primary.copy(alpha = alpha),
                        scrolledContainerColor = MaterialTheme.colorScheme.primary.copy(alpha = alpha),
                        titleContentColor = MaterialTheme.colorScheme.onSurface.copy(alpha = alpha),
                        actionIconContentColor = MaterialTheme.colorScheme.onSurface.copy(alpha = alpha),
                        navigationIconContentColor = MaterialTheme.colorScheme.onSurface.copy(alpha = alpha)
                    ),
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
                        top = innerPadding.calculateTopPadding(),
                        bottom = innerPadding.calculateBottomPadding() + 24.dp
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
                            CurrentWeatherCard(onNavigateToPlace = onNavigateToPlace)
                        }
                        Spacer(modifier = Modifier.height(20.dp))
                    }

                    item {
                        AnimatedVisibility(
                            visible = true,
                            enter = slideInVertically(
                                initialOffsetY = { 60 },
                                animationSpec = tween(
                                    durationMillis = 500,
                                    easing = FastOutSlowInEasing,
                                    delayMillis = 100
                                )
                            ) + fadeIn(animationSpec = tween(400, delayMillis = 100))
                        ) {
                            SunriseSunsetCard()
                        }
                        Spacer(modifier = Modifier.height(20.dp))
                    }

                    item {
                        AnimatedVisibility(
                            visible = true,
                            enter = slideInVertically(
                                initialOffsetY = { 60 },
                                animationSpec = tween(
                                    durationMillis = 500,
                                    easing = FastOutSlowInEasing,
                                    delayMillis = 200
                                )
                            ) + fadeIn(animationSpec = tween(400, delayMillis = 200))
                        ) {
                            WeatherDetailsCard()
                        }
                        Spacer(modifier = Modifier.height(20.dp))
                    }

                    item {
                        AnimatedVisibility(
                            visible = true,
                            enter = slideInVertically(
                                initialOffsetY = { 60 },
                                animationSpec = tween(
                                    durationMillis = 500,
                                    easing = FastOutSlowInEasing,
                                    delayMillis = 300
                                )
                            ) + fadeIn(animationSpec = tween(400, delayMillis = 300))
                        ) {
                            AirQualityCard()
                        }
                        Spacer(modifier = Modifier.height(20.dp))
                    }

                    item {
                        AnimatedVisibility(
                            visible = true,
                            enter = slideInVertically(
                                initialOffsetY = { 60 },
                                animationSpec = tween(
                                    durationMillis = 500,
                                    easing = FastOutSlowInEasing,
                                    delayMillis = 400
                                )
                            ) + fadeIn(animationSpec = tween(400, delayMillis = 400))
                        ) {
                            HourlyForecastSection()
                        }
                        Spacer(modifier = Modifier.height(20.dp))
                    }

                    item {
                        AnimatedVisibility(
                            visible = true,
                            enter = slideInVertically(
                                initialOffsetY = { 60 },
                                animationSpec = tween(
                                    durationMillis = 500,
                                    easing = FastOutSlowInEasing,
                                    delayMillis = 500
                                )
                            ) + fadeIn(animationSpec = tween(400, delayMillis = 500))
                        ) {
                            ForecastSection()
                        }
                        Spacer(modifier = Modifier.height(20.dp))
                    }

                    item {
                        AnimatedVisibility(
                            visible = true,
                            enter = slideInVertically(
                                initialOffsetY = { 60 },
                                animationSpec = tween(
                                    durationMillis = 500,
                                    easing = FastOutSlowInEasing,
                                    delayMillis = 600
                                )
                            ) + fadeIn(animationSpec = tween(400, delayMillis = 600))
                        ) {
                            LifeIndexSection()
                        }
                    }
                }
            }
        }
    }
}