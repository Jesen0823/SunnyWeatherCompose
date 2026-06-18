package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.*
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import androidx.compose.ui.text.font.FontWeight
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.debounce
import kotlinx.coroutines.flow.receiveAsFlow
import org.jesen.dev.sunnyweather.pose.R
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.search.PlaceList
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.search.SearchBar
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceViewModel
import org.jesen.dev.sunnyweather.pose.presentation.common.UiState

/**
 * 城市选择页面组件
 *
 * 主要职责：
 * - 提供城市搜索功能
 * - 显示搜索结果列表
 * - 处理加载状态和错误状态
 *
 * 技术要点：
 * - 使用 Channel 和 debounce(500ms) 实现搜索防抖
 * - 根据 placesState 显示默认状态、加载状态、错误状态或搜索结果
 * - 调用 SearchBar 和 PlaceList 组件
 * - 使用 LaunchedEffect 监听搜索关键词变化
 */
@OptIn(kotlinx.coroutines.FlowPreview::class)
@Composable
fun PlaceScreen(
    viewModel: PlaceViewModel,
    onPlaceSelected: (Place) -> Unit
) {
    var searchQuery by remember { mutableStateOf("") }
    val placesState: State<UiState<List<Place>>> = viewModel.placesState.collectAsState()
    
    val searchChannel = remember { Channel<String>(Channel.CONFLATED) }
    val searchFlow: Flow<String> = remember { 
        @OptIn(kotlinx.coroutines.FlowPreview::class)
        searchChannel.receiveAsFlow().debounce(500) 
    }
    
    LaunchedEffect(searchQuery) {
        searchChannel.trySend(searchQuery)
    }
    
    LaunchedEffect(Unit) {
        searchFlow.collect { query ->
            if (query.isNotEmpty()) {
                viewModel.searchPlaces(query)
            }
        }
    }
    
    Column(modifier = Modifier.fillMaxSize()) {
        SearchBar(
            query = searchQuery,
            onQueryChange = { searchQuery = it }
        )
        
        when (val state = placesState.value) {
            is UiState.Loading -> if (searchQuery.isNotEmpty()) {
                LoadingState()
            } else {
                DefaultState()
            }
            is UiState.Error -> ErrorState(state.message)
            is UiState.Success -> PlaceList(
                places = state.data,
                onPlaceSelected = onPlaceSelected
            )
        }
    }
}

/**
 * 默认状态组件
 *
 * 主要职责：
 * - 显示搜索图标和提示文字
 * - 当用户未输入搜索关键词时显示
 */
@Composable
private fun DefaultState() {
    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Image(
            painter = painterResource(id = R.drawable.bg_place),
            contentDescription = null,
            modifier = Modifier.size(200.dp)
        )
        Text(
            text = stringResource(id = R.string.search_default),
            style = MaterialTheme.typography.titleMedium,
            modifier = Modifier.padding(top = 16.dp)
        )
    }
}

/**
 * 加载状态组件
 *
 * 主要职责：
 * - 显示加载动画和提示文字
 * - 当搜索请求正在进行时显示
 */
@Composable
private fun LoadingState() {
    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        CircularProgressIndicator()
        Text(
            text = stringResource(id = R.string.search_loading),
            style = MaterialTheme.typography.bodyMedium,
            modifier = Modifier.padding(top = 8.dp)
        )
    }
}

/**
 * 错误状态组件
 *
 * 主要职责：
 * - 显示错误标题和错误信息
 * - 当搜索请求失败时显示
 */
@Composable
private fun ErrorState(message: String) {
    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Text(
            text = stringResource(id = R.string.search_error),
            style = MaterialTheme.typography.titleLarge,
            fontWeight = FontWeight.Bold
        )
        Text(
            text = message,
            style = MaterialTheme.typography.bodyMedium,
            modifier = Modifier.padding(top = 8.dp)
        )
    }
}
