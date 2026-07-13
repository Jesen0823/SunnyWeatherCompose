/**
 * 未知页面组件（初始页面）
 * 
 * 主要职责：
 * - 作为应用启动时的初始页面
 * - 处理定位权限请求和位置获取
 * - 提供手动选择城市的入口
 * 
 * 技术要点：
 * - 使用 Scaffold 处理系统内边距，确保内容不被状态栏遮挡
 * - 根据 permissionStatus 显示不同的按钮状态和操作
 * - 显示定位加载动画和错误提示
 * - 根据权限状态（Granted/Denied/PermanentlyDenied）执行不同的操作
 * - 调用 LocationPermissionDialog 处理权限请求
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import android.util.Log
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Button
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.R
import org.jesen.dev.sunnyweather.pose.presentation.ui.widget.LocationPermissionDialog
import org.jesen.dev.sunnyweather.pose.utils.PermissionStatus

@Composable
fun UnknownScreen(
    permissionStatus: PermissionStatus,
    isCheckingPermission: Boolean,
    isGettingLocation: Boolean,
    showPermissionDialog: Boolean,
    locationError: String?,
    onOpenLocationSettings: () -> Unit,
    onSelectCity: () -> Unit,
    onRequestPermission: () -> Unit,
    onDismissDialog: () -> Unit,
    onGetLocation: () -> Unit = {},
    isAutoLocating: Boolean = false
) {
    Log.d("UnknownScreen", "Composing, permissionStatus: $permissionStatus")
    Log.d("UnknownScreen", "showPermissionDialog: $showPermissionDialog")
    Log.d("UnknownScreen", "isGettingLocation: $isGettingLocation")
    Log.d("UnknownScreen", "isAutoLocating: $isAutoLocating")
    
    Scaffold { paddingValues ->
        Column(
            modifier = Modifier.fillMaxSize().padding(paddingValues),
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center
        ) {
        if (isGettingLocation) {
            Column(
                horizontalAlignment = Alignment.CenterHorizontally,
                verticalArrangement = Arrangement.Center
            ) {
                CircularProgressIndicator(
                    modifier = Modifier.size(64.dp),
                    strokeWidth = 4.dp,
                    color = MaterialTheme.colorScheme.onSurface
                )
                Spacer(modifier = Modifier.size(16.dp))
                Text(
                    text = stringResource(id = R.string.unknown_getting_location),
                    style = MaterialTheme.typography.bodyMedium,
                    color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.8f)
                )
            }
        } else {
            Image(
                painter = painterResource(id = R.drawable.bg_place),
                contentDescription = null,
                modifier = Modifier.size(200.dp)
            )
            
            Spacer(modifier = Modifier.size(24.dp))
            
            Text(
                text = stringResource(id = R.string.unknown_title),
                style = MaterialTheme.typography.titleLarge,
                fontWeight = FontWeight.Bold
            )
            
            Spacer(modifier = Modifier.size(8.dp))
            
            Text(
                text = stringResource(id = R.string.unknown_message),
                style = MaterialTheme.typography.bodyMedium,
                color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.6f)
            )
            
            locationError?.let {
                Spacer(modifier = Modifier.size(8.dp))
                Text(
                    text = it,
                    style = MaterialTheme.typography.bodySmall,
                    color = MaterialTheme.colorScheme.error
                )
            }
            
            Spacer(modifier = Modifier.size(32.dp))
            
            LocationButton(
                permissionStatus = permissionStatus,
                onClick = {
                    Log.d("UnknownScreen", "LocationButton clicked, status: $permissionStatus")
                    when (permissionStatus) {
                        PermissionStatus.Granted -> {
                            Log.d("UnknownScreen", "Calling onGetLocation()")
                            onGetLocation()
                        }
                        PermissionStatus.Denied -> {
                            Log.d("UnknownScreen", "Calling onRequestPermission()")
                            onRequestPermission()
                        }
                        PermissionStatus.PermanentlyDenied -> {
                            Log.d("UnknownScreen", "Calling onOpenLocationSettings()")
                            onOpenLocationSettings()
                        }
                        else -> {
                            Log.d("UnknownScreen", "Unknown permission status, calling onRequestPermission()")
                            onRequestPermission()
                        }
                    }
                }
            )
            
            Spacer(modifier = Modifier.size(16.dp))
            
            Button(
                onClick = {
                    Log.d("UnknownScreen", "Select city button clicked")
                    onSelectCity()
                },
                modifier = Modifier.size(width = 200.dp, height = 48.dp)
            ) {
                Text(text = stringResource(id = R.string.unknown_select_city))
            }
        }
        }
    }
    
    if (showPermissionDialog) {
        Log.d("UnknownScreen", "Showing LocationPermissionDialog")
        LocationPermissionDialog(
            onGrant = onRequestPermission,
            onDeny = onDismissDialog,
            onGoToSettings = {
                Log.d("UnknownScreen", "onGoToSettings clicked")
                onDismissDialog()
                onOpenLocationSettings()
            }
        )
    }
}

@Composable
private fun LocationButton(
    permissionStatus: PermissionStatus,
    onClick: () -> Unit
) {
    val buttonText = when (permissionStatus) {
        PermissionStatus.Granted -> stringResource(id = R.string.unknown_get_location)
        PermissionStatus.Denied -> stringResource(id = R.string.unknown_request_permission)
        PermissionStatus.PermanentlyDenied -> stringResource(id = R.string.unknown_open_settings)
        else -> stringResource(id = R.string.unknown_open_location)
    }
    
    Button(
        onClick = onClick,
        modifier = Modifier.size(width = 200.dp, height = 48.dp)
    ) {
        Text(text = buttonText)
    }
}
