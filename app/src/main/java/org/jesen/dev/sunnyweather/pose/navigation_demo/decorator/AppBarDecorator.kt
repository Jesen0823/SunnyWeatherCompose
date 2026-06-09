package org.jesen.dev.sunnyweather.pose.navigation_demo.decorator

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.ArrowBack
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import org.jesen.dev.sunnyweather.pose.navigation_demo.metadata.NavMetadataKeys

/**
 * AppBar 装饰器实现
 * 
 * 【SceneDecoratorStrategy 说明】
 * 
 * Navigation3 1.1.0+ 引入的场景装饰器策略，用于：
 * 
 * 1. **共享 UI 组件**：跨多个场景共享顶部导航栏、底部导航栏等通用组件
 * 2. **共享状态**：在不同场景间共享状态（如主题、用户信息等）
 * 3. **统一装饰**：为所有场景提供一致的布局结构
 * 
 * 【使用场景】
 * - 多个页面需要相同的顶部 AppBar
 * - 需要在页面间共享某些状态
 * - 需要统一的页面布局结构
 * 
 * 【优势】
 * - 减少代码重复
 * - 保持 UI 一致性
 * - 便于统一修改
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun AppBarDecorator(
    /** 页面标题 */
    title: String,
    /** 是否显示返回按钮 */
    showBackButton: Boolean,
    /** 返回按钮点击回调 */
    onBack: () -> Unit,
    /** 页面内容 */
    content: @Composable () -> Unit
) {
    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text(text = title) },
                navigationIcon = {
                    if (showBackButton) {
                        IconButton(onClick = onBack) {
                            Icon(
                                imageVector = Icons.AutoMirrored.Filled.ArrowBack,
                                contentDescription = "Back"
                            )
                        }
                    }
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.primaryContainer,
                    titleContentColor = MaterialTheme.colorScheme.onPrimaryContainer
                )
            )
        },
        content = { padding ->
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(padding)
            ) {
                content()
            }
        }
    )
}