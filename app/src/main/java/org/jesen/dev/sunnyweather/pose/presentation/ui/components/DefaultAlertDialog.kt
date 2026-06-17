/**
 * 通用弹窗组件
 * 
 * 主要职责：
 * - 提供统一风格的确认/取消弹窗
 * - 支持自定义标题、消息、按钮文字和回调
 * 
 * 技术要点：
 * - 基于 Material3 AlertDialog 实现
 * - 提供 title、message、confirmText、dismissText 四个文本参数
 * - onConfirm 和 onDismiss 分别处理确认和取消操作
 * - 使用 MaterialTheme.typography 确保风格统一
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components

import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.material3.MaterialTheme

// 公共弹窗
@Composable
fun DefaultAlertDialog(
    title: String,
    message: String,
    confirmText: String,
    dismissText: String,
    onConfirm: () -> Unit,
    onDismiss: () -> Unit,
) {
    AlertDialog(
        onDismissRequest = onDismiss,
        title = {
            Text(
                text = title,
                style = MaterialTheme.typography.titleMedium
            )
        },
        text = {
            Text(
                text = message,
                style = MaterialTheme.typography.bodyMedium
            )
        },
        confirmButton = {
            Button(onClick = onConfirm) {
                Text(text = confirmText)
            }
        },
        dismissButton = {
            Button(onClick = onDismiss) {
                Text(text = dismissText)
            }
        },
        icon = null
    )
}