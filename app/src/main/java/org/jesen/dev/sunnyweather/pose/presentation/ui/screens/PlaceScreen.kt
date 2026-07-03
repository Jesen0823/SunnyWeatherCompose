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
import kotlinx.coroutines.delay
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
 * - 使用 rememberCoroutineScope 和 delay 实现搜索防抖（500ms）
 * - 使用单个 LaunchedEffect 处理搜索逻辑，避免多个 LaunchedEffect 嵌套
 * - 使用 derivedStateOf 计算显示状态
 * - 根据 placesState 显示默认状态、加载状态、错误状态或搜索结果
 * - 调用 SearchBar 和 PlaceList 组件
 */
@Composable
fun PlaceScreen(
    viewModel: PlaceViewModel,
    onPlaceSelected: (Place) -> Unit
) {
    var searchQuery by remember { mutableStateOf("") }
    val placesState: State<UiState<List<Place>>> = viewModel.placesState.collectAsState()
    
    /**
     * 使用 derivedStateOf 计算搜索显示状态
     * 
     * 场景说明：
     * - 是否显示加载状态取决于 placesState 和 searchQuery
     * - derivedStateOf 缓存计算结果，避免每次重组都重新计算
     */
    val showLoading by remember(placesState.value, searchQuery) {
        derivedStateOf {
            placesState.value is UiState.Loading && searchQuery.isNotEmpty()
        }
    }

    /**
     * 使用 LaunchedEffect 处理搜索逻辑（实现搜索防抖）
     * 
     * 性能优化原理 - 搜索防抖：
     * - 用户输入过程中，每次按键都会触发 searchQuery 变化
     * - 如果每次变化都发起网络请求，会造成大量不必要的请求
     * - 防抖策略：等待用户停止输入一段时间（500ms）后再发起请求
     * - 使用 delay(500) 实现等待，如果等待期间 searchQuery 变化，协程会被取消
     * 
     * LaunchedEffect 生命周期：
     * - 当 searchQuery 变化时，旧的 LaunchedEffect 协程会被取消
     * - 新的 LaunchedEffect 会启动新协程，重新开始等待
     * - 这确保了只有用户停止输入 500ms 后，才会发起真正的搜索请求
     * 
     * 对比示例：
     * // 未优化：每次输入都发起请求
     * TextField(value = query, onValueChange = {
     *     query = it
     *     viewModel.searchPlaces(it)
     * })
     * 
     * // 优化后：防抖搜索
     * var searchQuery by remember { mutableStateOf("") }
     * LaunchedEffect(searchQuery) {
     *     if (searchQuery.isNotEmpty()) {
     *         delay(500)
     *         viewModel.searchPlaces(searchQuery)
     *     }
     * }
     * 
     * 场景说明：
     * - 当 searchQuery 变化时，启动协程进行防抖搜索
     * - 使用 delay(500) 实现防抖，避免用户输入过程中频繁发起网络请求
     * - 如果在防抖期间 searchQuery 再次变化，之前的协程会被取消
     * - 只在 searchQuery 不为空时发起搜索请求
     */
    LaunchedEffect(searchQuery) {
        if (searchQuery.isNotEmpty()) {
            // 防抖延迟 500ms：等待用户停止输入后再发起请求
            // 如果用户继续输入，searchQuery 会再次变化，此协程会被取消
            delay(500)
            // 发起搜索请求
            viewModel.searchPlaces(searchQuery)
        }
    }

    Column(modifier = Modifier.fillMaxSize()) {
        SearchBar(
            query = searchQuery,
            onQueryChange = { searchQuery = it }
        )
        
        when (val state = placesState.value) {
            is UiState.Loading -> if (showLoading) {
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