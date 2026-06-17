/**
 * 权限弹窗组件
 * 
 * 主要职责：
 * - 提供通用权限请求弹窗
 * - 提供定位权限专用弹窗（LocationPermissionDialog）
 * 
 * 技术要点：
 * - PermissionDialog 是通用弹窗，支持自定义标题、消息和按钮
 * - LocationPermissionDialog 是定位权限专用弹窗，封装了固定的文案
 * - 基于 Material3 AlertDialog 实现
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.widget

import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.res.stringResource
import org.jesen.dev.sunnyweather.pose.R

@Composable
fun PermissionDialog(
    title: String,
    message: String,
    confirmText: String,
    dismissText: String,
    onConfirm: () -> Unit,
    onDismiss: () -> Unit
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

@Composable
fun LocationPermissionDialog(
    onGrant: () -> Unit,
    onDeny: () -> Unit,
    onGoToSettings: () -> Unit
) {
    PermissionDialog(
        title = stringResource(id = R.string.permission_dialog_title),
        message = stringResource(id = R.string.permission_dialog_message),
        confirmText = stringResource(id = R.string.permission_dialog_grant),
        dismissText = stringResource(id = R.string.permission_dialog_deny),
        onConfirm = onGrant,
        onDismiss = onDeny
    )
}
