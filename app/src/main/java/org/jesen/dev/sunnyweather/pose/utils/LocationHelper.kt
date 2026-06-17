/**
 * 位置帮助类
 * 
 * 主要职责：
 * - 获取设备当前位置坐标
 * - 处理定位权限检查和定位服务状态检查
 * - 返回定位结果状态流
 * 
 * 技术要点：
 * - 使用 callbackFlow 将回调式 API 转换为 Flow
 * - 同时请求 GPS 和网络定位，提高成功率
 * - 支持定位超时（默认30秒）
 * - LocationStatus 密封类表示定位状态（Success、Error、Timeout、PermissionDenied）
 */
package org.jesen.dev.sunnyweather.pose.utils

import android.content.Context
import android.location.Location
import android.location.LocationListener
import android.location.LocationManager
import android.os.Bundle
import android.util.Log
import androidx.core.content.ContextCompat
import kotlinx.coroutines.channels.awaitClose
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.callbackFlow
import kotlinx.coroutines.launch

class LocationHelper(private val context: Context) {
    private val TAG = "LocationHelper"
    
    private val locationManager by lazy {
        Log.d(TAG, "Getting LocationManager")
        context.getSystemService(Context.LOCATION_SERVICE) as LocationManager
    }
    
    data class LocationResult(
        val lng: Double,
        val lat: Double
    )
    
    sealed class LocationStatus {
        data class Success(val result: LocationResult) : LocationStatus()
        data class Error(val message: String) : LocationStatus()
        object Timeout : LocationStatus()
        object PermissionDenied : LocationStatus()
    }
    
    fun getCurrentLocation(timeoutMillis: Long = 30000L) = callbackFlow<LocationStatus> {
        Log.d(TAG, "getCurrentLocation() called")
        
        if (!isLocationEnabled()) {
            Log.e(TAG, "Location service is not enabled")
            trySend(LocationStatus.Error("定位服务未开启"))
            close()
            return@callbackFlow
        }
        Log.d(TAG, "Location service is enabled")
        
        val hasPermission = hasLocationPermission()
        Log.d(TAG, "hasLocationPermission: $hasPermission")
        if (!hasPermission) {
            Log.e(TAG, "Location permission denied")
            trySend(LocationStatus.PermissionDenied)
            close()
            return@callbackFlow
        }
        Log.d(TAG, "Location permission granted")
        
        val listener = object : LocationListener {
            override fun onLocationChanged(location: Location) {
                Log.d(TAG, "onLocationChanged() called: lng=${location.longitude}, lat=${location.latitude}")
                trySend(LocationStatus.Success(
                    LocationResult(
                        lng = location.longitude,
                        lat = location.latitude
                    )
                ))
                close()
            }
            
            override fun onStatusChanged(provider: String?, status: Int, extras: Bundle?) {}
            
            override fun onProviderEnabled(provider: String) {}
            
            override fun onProviderDisabled(provider: String) {
                Log.e(TAG, "onProviderDisabled(): $provider")
                trySend(LocationStatus.Error("定位服务不可用"))
                close()
            }
        }
        
        try {
            Log.d(TAG, "Requesting location updates from GPS_PROVIDER")
            locationManager.requestLocationUpdates(
                LocationManager.GPS_PROVIDER,
                0L,
                0f,
                listener
            )
        } catch (e: SecurityException) {
            Log.e(TAG, "SecurityException for GPS_PROVIDER: ${e.message}")
            trySend(LocationStatus.PermissionDenied)
            close()
            return@callbackFlow
        }
        
        try {
            Log.d(TAG, "Requesting location updates from NETWORK_PROVIDER")
            locationManager.requestLocationUpdates(
                LocationManager.NETWORK_PROVIDER,
                0L,
                0f,
                listener
            )
        } catch (e: SecurityException) {
            Log.e(TAG, "SecurityException for NETWORK_PROVIDER: ${e.message}")
        }
        
        Log.d(TAG, "Starting timeout coroutine: $timeoutMillis ms")
        launch {
            delay(timeoutMillis)
            Log.e(TAG, "Location timeout after $timeoutMillis ms")
            trySend(LocationStatus.Timeout)
            close()
        }
        
        awaitClose {
            Log.d(TAG, "Cleaning up location listener")
            locationManager.removeUpdates(listener)
        }
    }
    
    private fun hasLocationPermission(): Boolean {
        val coarsePermission = android.Manifest.permission.ACCESS_COARSE_LOCATION
        val finePermission = android.Manifest.permission.ACCESS_FINE_LOCATION
        
        val hasCoarse = ContextCompat.checkSelfPermission(context, coarsePermission) ==
                android.content.pm.PackageManager.PERMISSION_GRANTED
        val hasFine = ContextCompat.checkSelfPermission(context, finePermission) ==
                android.content.pm.PackageManager.PERMISSION_GRANTED
        
        Log.d(TAG, "hasCoarse: $hasCoarse, hasFine: $hasFine")
        return hasCoarse || hasFine
    }
    
    private fun isLocationEnabled(): Boolean {
        val gpsEnabled = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)
        val networkEnabled = locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER)
        Log.d(TAG, "gpsEnabled: $gpsEnabled, networkEnabled: $networkEnabled")
        return gpsEnabled || networkEnabled
    }
}
