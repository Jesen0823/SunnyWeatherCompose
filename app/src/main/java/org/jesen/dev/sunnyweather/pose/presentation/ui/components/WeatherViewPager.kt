package org.jesen.dev.sunnyweather.pose.presentation.ui.components

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.wrapContentSize
import androidx.compose.foundation.pager.HorizontalPager
import androidx.compose.foundation.pager.rememberPagerState
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.model.Weather
import org.jesen.dev.sunnyweather.pose.presentation.common.UiState
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.WeatherScreen

/**
 * 多城市天气页面滑动组件
 *
 * 主要职责：
 * - 使用 HorizontalPager 实现多城市天气页面的横向滑动切换
 * - 显示页面指示器，展示当前城市名称和总页数
 * - 管理页面切换状态，更新指示器显示
 * - 为每个城市创建对应的 WeatherScreen 子组件
 *
 * 技术要点：
 * - 使用 Compose Foundation 官方 HorizontalPager（替代已弃用的 Accompanist）
 * - 使用 rememberPagerState 管理页面状态
 * - 通过 LaunchedEffect 监听页面切换事件
 * - 支持空列表保护，避免越界异常
 * - 仅当城市数量大于1时显示指示器
 */
@Composable
fun WeatherViewPager(
    places: List<Place>,
    weatherStates: Map<String, UiState<Weather>>,
    onWeatherLoaded: (Place) -> Unit,
    onRefresh: (Place) -> Unit,
    onNavigateToPlace: () -> Unit,
    onNavigateToSettings: () -> Unit = {},
    onMenuClick: () -> Unit = {}
) {
    if (places.isEmpty()) {
        return
    }

    val pagerState = rememberPagerState(pageCount = { places.size })
    var currentPage by remember { mutableIntStateOf(0) }

    LaunchedEffect(pagerState.currentPage) {
        if (pagerState.currentPage != currentPage) {
            currentPage = pagerState.currentPage
        }
    }

    Box(modifier = Modifier.fillMaxSize()) {
        HorizontalPager(
            state = pagerState,
            modifier = Modifier.fillMaxSize()
        ) { page ->
            val place = places[page]
            
            WeatherScreen(
                place = place,
                weatherState = weatherStates[place.name] ?: UiState.Loading,
                onWeatherLoaded = { onWeatherLoaded(place) },
                onRefresh = { onRefresh(place) },
                onNavigateToPlace = onNavigateToPlace,
                onNavigateToSettings = onNavigateToSettings,
                onMenuClick = onMenuClick
            )
        }

        if (places.size > 1) {
            WeatherPagerIndicator(
                currentPage = currentPage,
                places = places,
                modifier = Modifier
                    .align(Alignment.TopCenter)
                    .padding(top = 100.dp)
            )
        }
    }
}

/**
 * 页面指示器组件
 *
 * 主要职责：
 * - 显示当前城市名称和总页数（如：上海 1/3）
 * - 使用半透明背景和圆角样式，提高可视性
 * - 支持空列表和越界保护
 */
@Composable
fun WeatherPagerIndicator(
    currentPage: Int,
    places: List<Place>,
    modifier: Modifier = Modifier
) {
    if (places.isEmpty() || currentPage >= places.size) {
        return
    }
    
    Surface(
        modifier = modifier
            .wrapContentSize(),
        shape = MaterialTheme.shapes.medium,
        color = Color.Black.copy(alpha = 0.4f),
        tonalElevation = 4.dp
    ) {
        Text(
            text = "${places[currentPage].name} ${currentPage + 1}/${places.size}",
            style = MaterialTheme.typography.bodyMedium,
            fontWeight = FontWeight.Medium,
            color = Color.White,
            fontSize = 14.sp,
            modifier = Modifier.padding(horizontal = 16.dp, vertical = 8.dp)
        )
    }
}
