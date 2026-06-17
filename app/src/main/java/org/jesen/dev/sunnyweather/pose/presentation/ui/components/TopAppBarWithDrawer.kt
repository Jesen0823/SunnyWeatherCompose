/**
 * 带抽屉按钮的顶部导航栏组件
 * 
 * 主要职责：
 * - 显示页面标题
 * - 提供打开侧边栏抽屉的按钮
 * - 统一顶部导航栏样式
 * 
 * 技术要点：
 * - 基于 Material3 TopAppBar 实现
 * - 使用 Icons.Default.Menu 作为导航图标
 * - 配置容器颜色、标题颜色和导航图标颜色
 * - 使用 fontWeight = FontWeight.Bold 突出标题
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components

import androidx.compose.material.icons.filled.Menu
import androidx.compose.material.icons.Icons
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.Composable
import androidx.compose.ui.text.font.FontWeight

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TopAppBarWithDrawer(
    title: String,
    onMenuClick: () -> Unit
) {
    TopAppBar(
        title = {
            Text(
                text = title,
                fontWeight = FontWeight.Bold,
                style = MaterialTheme.typography.headlineSmall
            )
        },
        navigationIcon = {
            IconButton(onClick = onMenuClick) {
                Icon(
                    imageVector = Icons.Default.Menu,
                    contentDescription = "菜单"
                )
            }
        },
        colors = TopAppBarDefaults.topAppBarColors(
            containerColor = MaterialTheme.colorScheme.primary,
            titleContentColor = MaterialTheme.colorScheme.onPrimary,
            navigationIconContentColor = MaterialTheme.colorScheme.onPrimary
        )
    )
}