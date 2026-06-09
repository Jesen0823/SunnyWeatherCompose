package org.jesen.dev.sunnyweather.pose.presentation.ui.components

import androidx.compose.foundation.layout.*
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.material3.pulltorefresh.PullToRefreshBox
import androidx.compose.material3.pulltorefresh.PullToRefreshDefaults
import androidx.compose.material3.pulltorefresh.PullToRefreshState
import androidx.compose.material3.pulltorefresh.rememberPullToRefreshState
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.unit.dp

/**
 * 高度封装的下拉刷新组件
 * 基于官方 Material3 PullToRefreshBox 实现
 * 支持自定义指示器
 *
 * @param isRefreshing 是否正在刷新
 * @param onRefresh 刷新回调
 * @param modifier 修饰符
 * @param state 下拉刷新状态
 * @param indicator 自定义指示器，默认为官方实现
 * @param content 内容组件，需要是可滚动组件
 */
@Composable
fun PullToRefresh(
    isRefreshing: Boolean,
    onRefresh: () -> Unit,
    modifier: Modifier = Modifier,
    state: PullToRefreshState = rememberPullToRefreshState(),
    indicator: @Composable (BoxScope.() -> Unit) = {
        PullToRefreshDefaults.Indicator(
            state = state,
            isRefreshing = isRefreshing,
            modifier = Modifier.align(Alignment.TopCenter)
        )
    },
    content: @Composable BoxScope.() -> Unit
) {
    PullToRefreshBox(
        isRefreshing = isRefreshing,
        onRefresh = onRefresh,
        modifier = modifier,
        state = state,
        indicator = indicator,
        content = content
    )
}
