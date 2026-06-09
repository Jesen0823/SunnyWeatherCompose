package org.jesen.dev.sunnyweather.pose.navigation_demo.ui.screens

import androidx.compose.animation.AnimatedVisibility
import androidx.compose.animation.fadeIn
import androidx.compose.animation.fadeOut
import androidx.compose.animation.slideInVertically
import androidx.compose.animation.slideOutVertically
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp

/**
 * OverlayScene 弹窗演示
 * 
 * 【OverlayScene 动画说明】
 * 
 * Navigation3 1.1.0+ 为 OverlayScene 接口新增了 `onRemoved` 挂起回调：
 * 
 * - **onRemoved**: 当与场景关联的键从返回堆栈中移除后，
 *   但在离开组合之前调用。这样退出动画会在叠加场景从合成中移除之前完成。
 * 
 * 【OverlayScene 特性】
 * 1. **叠加显示**：悬浮在主内容之上
 * 2. **动画支持**：支持进入和退出动画
 * 3. **返回手势**：支持预测性返回手势（Navigation3 1.2.0-alpha02+）
 * 4. **生命周期管理**：独立的生命周期回调
 * 
 * 【使用场景】
 * - 对话框/弹窗
 * - 底部弹出菜单（BottomSheet）
 * - 悬浮操作面板
 * - 模态对话框
 * 
 * 【NavigationEvent / rememberNavigationEventState 说明】
 * 
 * Navigation3 1.2.0-alpha02+ 引入的导航事件处理：
 * 
 * 1. **NavigationBackHandler**: 简化使用 SceneState 时对返回手势和预测性返回的处理
 * 2. **rememberNavigationEventState**: 简化将 SceneState 与预测性返回处理相关联的过程
 * 3. **onBackCompleted / onBackCancelled**: 处理返回手势完成或取消的回调
 * 
 * 【预测性返回手势】
 * Android 14+ 引入的系统级返回手势，允许用户预览返回目标页面。
 * Navigation3 通过 NavigationEvent API 提供完整支持。
 */
@Composable
fun DialogScreen(
    /** 取消按钮点击回调 */
    onDismiss: () -> Unit,
    /** 确认按钮点击回调 */
    onConfirm: () -> Unit
) {
    // 使用 AnimatedVisibility 实现进入/退出动画
    AnimatedVisibility(
        visible = true,
        // 进入动画：淡入 + 从上方滑入
        enter = fadeIn() + slideInVertically(initialOffsetY = { -it }),
        // 退出动画：淡出 + 向上滑出
        exit = fadeOut() + slideOutVertically(targetOffsetY = { -it })
    ) {
        Box(
            modifier = Modifier
                .fillMaxSize()
                .clickable { onDismiss() },
            contentAlignment = Alignment.Center
        ) {
            // 半透明背景遮罩
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .background(Color.Black.copy(alpha = 0.5f))
            )
            
            // 弹窗内容
            Surface(
                modifier = Modifier
                    .size(width = 300.dp, height = 200.dp)
                    .align(Alignment.Center)
                    .clickable(enabled = false) {},
                shape = MaterialTheme.shapes.medium,
                shadowElevation = 16.dp
            ) {
                Column(
                    modifier = Modifier
                        .fillMaxSize()
                        .padding(16.dp),
                    verticalArrangement = Arrangement.Center,
                    horizontalAlignment = Alignment.CenterHorizontally
                ) {
                    Text(
                        text = "Overlay 弹窗",
                        style = MaterialTheme.typography.headlineMedium,
                        modifier = Modifier.padding(bottom = 24.dp)
                    )
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.End
                    ) {
                        Button(
                            onClick = onDismiss,
                            modifier = Modifier.padding(end = 8.dp)
                        ) {
                            Text("取消")
                        }
                        Button(onClick = onConfirm) {
                            Text("确认")
                        }
                    }
                }
            }
        }
    }
}