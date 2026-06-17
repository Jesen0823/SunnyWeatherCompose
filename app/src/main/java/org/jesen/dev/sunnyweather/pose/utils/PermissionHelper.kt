/**
 * 权限帮助类
 * 
 * 主要职责：
 * - 管理 Android 运行时权限的检查和请求
 * - 提供权限状态监听机制
 * - 支持打开应用设置页面
 * 
 * 技术要点：
 * - PermissionStatus 密封类表示权限状态（Granted、Denied、PermanentlyDenied、Requesting、Unknown）
 * - 使用 callbackFlow 将权限状态变化转换为 Flow
 * - 支持注册 ActivityResultLauncher 处理权限请求结果
 * - PermissionConstants 提供定位权限常量和获取方法（根据 Android 版本选择）
 */
package org.jesen.dev.sunnyweather.pose.utils

import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.provider.Settings
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.core.app.ActivityCompat
import kotlinx.coroutines.channels.awaitClose
import kotlinx.coroutines.flow.callbackFlow

sealed class PermissionStatus {
    object Granted : PermissionStatus()
    object Denied : PermissionStatus()
    object PermanentlyDenied : PermissionStatus()
    object Requesting : PermissionStatus()
    object Unknown : PermissionStatus()
}

class PermissionHelper(
    private val context: Context,
    private val permission: String
) {
    private val TAG = "PermissionHelper"
    
    private var status: PermissionStatus = PermissionStatus.Unknown
    private var requestLauncher: ActivityResultLauncher<String>? = null

    init {
        Log.d(TAG, "PermissionHelper created, permission: $permission")
        checkCurrentStatus()
    }

    private fun checkCurrentStatus() {
        Log.d(TAG, "checkCurrentStatus() called")
        val isGranted = isPermissionGranted()
        Log.d(TAG, "isPermissionGranted: $isGranted")
        
        status = when {
            isGranted -> {
                Log.d(TAG, "Status: Granted")
                PermissionStatus.Granted
            }
            else -> {
                val shouldShowRationale = shouldShowRationale()
                Log.d(TAG, "shouldShowRationale: $shouldShowRationale")
                if (shouldShowRationale) {
                    Log.d(TAG, "Status: Denied")
                    PermissionStatus.Denied
                } else {
                    Log.d(TAG, "Status: PermanentlyDenied")
                    PermissionStatus.PermanentlyDenied
                }
            }
        }
    }

    fun updateStatus() {
        Log.d(TAG, "updateStatus() called")
        checkCurrentStatus()
    }

    private fun isPermissionGranted(): Boolean {
        return ActivityCompat.checkSelfPermission(
            context,
            permission
        ) == PackageManager.PERMISSION_GRANTED
    }

    fun shouldShowRationale(): Boolean {
        val activity = context as? ComponentActivity
        Log.d(TAG, "shouldShowRationale() - context is ComponentActivity: ${activity != null}")
        return if (activity != null) {
            ActivityCompat.shouldShowRequestPermissionRationale(activity, permission)
        } else {
            Log.d(TAG, "shouldShowRationale() - returning true (context is not ComponentActivity)")
            true
        }
    }

    fun getStatus(): PermissionStatus = status

    fun openAppSettings() {
        Log.d(TAG, "openAppSettings() called")
        val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS).apply {
            data = Uri.fromParts("package", context.packageName, null)
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
        }
        context.startActivity(intent)
    }

    fun registerRequestLauncher(activity: ComponentActivity) {
        Log.d(TAG, "registerRequestLauncher() called")
        requestLauncher = activity.registerForActivityResult(
            ActivityResultContracts.RequestPermission()
        ) { isGranted ->
            Log.d(TAG, "registerRequestLauncher() - result: $isGranted")
            onPermissionResult(isGranted)
        }
    }

    fun requestPermission() {
        Log.d(TAG, "requestPermission() called")
        requestLauncher?.launch(permission)
    }

    fun onPermissionResult(granted: Boolean) {
        Log.d(TAG, "onPermissionResult() called, granted: $granted")
        updateStatus()
        notifyStatusChanged(status)
    }

    fun observePermissionStatus() = callbackFlow<PermissionStatus> {
        send(status)

        val listener = object : PermissionStatusListener {
            override fun onStatusChanged(newStatus: PermissionStatus) {
                trySend(newStatus)
            }
        }

        addStatusListener(listener)
        awaitClose {
            removeStatusListener(listener)
        }
    }

    private val statusListeners = mutableListOf<PermissionStatusListener>()

    fun addStatusListener(listener: PermissionStatusListener) {
        statusListeners.add(listener)
    }

    fun removeStatusListener(listener: PermissionStatusListener) {
        statusListeners.remove(listener)
    }

    private fun notifyStatusChanged(newStatus: PermissionStatus) {
        status = newStatus
        statusListeners.forEach { it.onStatusChanged(newStatus) }
    }

    companion object {
        fun createAndRequest(
            context: Context,
            permission: String,
            onResult: (PermissionStatus) -> Unit
        ): PermissionHelper {
            val helper = PermissionHelper(context, permission)
            helper.addStatusListener(object : PermissionStatusListener {
                override fun onStatusChanged(newStatus: PermissionStatus) {
                    onResult(newStatus)
                }
            })
            return helper
        }
    }
}

interface PermissionStatusListener {
    fun onStatusChanged(newStatus: PermissionStatus)
}

object PermissionConstants {
    const val ACCESS_COARSE_LOCATION = android.Manifest.permission.ACCESS_COARSE_LOCATION
    const val ACCESS_FINE_LOCATION = android.Manifest.permission.ACCESS_FINE_LOCATION

    fun getLocationPermission(): String {
        return if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.Q) {
            ACCESS_COARSE_LOCATION
        } else {
            ACCESS_FINE_LOCATION
        }
    }
}
