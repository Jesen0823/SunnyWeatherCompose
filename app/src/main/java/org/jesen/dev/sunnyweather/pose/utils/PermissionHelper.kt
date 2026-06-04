package org.jesen.dev.sunnyweather.pose.utils

import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import android.net.Uri
import android.provider.Settings
import androidx.activity.ComponentActivity
import androidx.core.app.ActivityCompat

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
    private var status: PermissionStatus = PermissionStatus.Unknown
    
    init {
        checkCurrentStatus()
    }
    
    private fun checkCurrentStatus() {
        status = when {
            isPermissionGranted() -> PermissionStatus.Granted
            shouldShowRationale() -> PermissionStatus.Denied
            else -> PermissionStatus.PermanentlyDenied
        }
    }
    
    fun updateStatus() {
        checkCurrentStatus()
    }
    
    private fun isPermissionGranted(): Boolean {
        return ActivityCompat.checkSelfPermission(
            context,
            permission
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    private fun shouldShowRationale(): Boolean {
        val activity = context as? ComponentActivity
        return activity?.let {
            ActivityCompat.shouldShowRequestPermissionRationale(it, permission)
        } ?: false
    }
    
    fun getStatus(): PermissionStatus = status
    
    fun openAppSettings() {
        val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS).apply {
            data = Uri.fromParts("package", context.packageName, null)
            flags = Intent.FLAG_ACTIVITY_NEW_TASK
        }
        context.startActivity(intent)
    }
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