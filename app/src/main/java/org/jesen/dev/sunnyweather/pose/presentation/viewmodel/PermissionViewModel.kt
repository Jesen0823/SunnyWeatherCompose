package org.jesen.dev.sunnyweather.pose.presentation.viewmodel

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import org.jesen.dev.sunnyweather.pose.di.AppModule
import org.jesen.dev.sunnyweather.pose.domain.model.Location
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.usecase.SavePlaceUseCase
import org.jesen.dev.sunnyweather.pose.utils.LocationHelper
import org.jesen.dev.sunnyweather.pose.utils.PermissionConstants
import org.jesen.dev.sunnyweather.pose.utils.PermissionHelper
import org.jesen.dev.sunnyweather.pose.utils.PermissionStatus
import kotlin.time.Duration.Companion.milliseconds

/**
 * 权限管理 ViewModel
 *
 * 主要职责：
 * - 管理定位权限的请求和状态
 * - 获取当前设备位置信息
 * - 处理权限请求结果和定位结果
 *
 * 技术要点：
 * - 使用多个 StateFlow 管理不同状态（权限状态、定位状态、错误信息等）
 * - 通过 viewModelScope 启动协程执行异步操作
 * - 使用 PermissionHelper 检查和请求定位权限
 * - 使用 LocationHelper 获取当前位置坐标
 * - 定位成功后自动保存位置信息并更新 locationResult
 */
class PermissionViewModel(
    private val savePlaceUseCase: SavePlaceUseCase
) : ViewModel() {
    
    private val TAG = "PermissionViewModel"
    
    private val _permissionStatus = MutableStateFlow<PermissionStatus>(PermissionStatus.Unknown)
    val permissionStatus: StateFlow<PermissionStatus> = _permissionStatus.asStateFlow()
    
    private val _showPermissionDialog = MutableStateFlow(false)
    val showPermissionDialog: StateFlow<Boolean> = _showPermissionDialog.asStateFlow()
    
    private val _isCheckingPermission = MutableStateFlow(false)
    val isCheckingPermission: StateFlow<Boolean> = _isCheckingPermission.asStateFlow()
    
    private val _isGettingLocation = MutableStateFlow(false)
    val isGettingLocation: StateFlow<Boolean> = _isGettingLocation.asStateFlow()
    
    private val _locationResult = MutableStateFlow<Place?>(null)
    val locationResult: StateFlow<Place?> = _locationResult.asStateFlow()
    
    private val _locationError = MutableStateFlow<String?>(null)
    val locationError: StateFlow<String?> = _locationError.asStateFlow()
    
    private val permissionHelper by lazy {
        Log.d(TAG, "Creating PermissionHelper")
        PermissionHelper(AppModule.context, PermissionConstants.getLocationPermission())
    }
    
    private val locationHelper by lazy {
        Log.d(TAG, "Creating LocationHelper")
        LocationHelper(AppModule.context)
    }
    
    fun checkPermissionStatus() {
        Log.d(TAG, "checkPermissionStatus() called")
        viewModelScope.launch {
            _isCheckingPermission.value = true
            delay(100.milliseconds)
            val status = permissionHelper.getStatus()
            Log.d(TAG, "checkPermissionStatus() - permission status: $status")
            _permissionStatus.value = status
            
            if (status == PermissionStatus.Denied) {
                Log.d(TAG, "Permission is Denied, showing dialog")
                _showPermissionDialog.value = true
                Log.d(TAG, "showPermissionDialog set to: ${_showPermissionDialog.value}")
            }
            
            _isCheckingPermission.value = false
        }
    }
    
    fun requestPermission() {
        Log.d(TAG, "requestPermission() called")
        AppModule.requestLocationPermission { granted ->
            Log.d(TAG, "requestPermission() - callback received, granted: $granted")
            onPermissionResult(granted)
        }
    }
    
    fun dismissDialog() {
        Log.d(TAG, "dismissDialog() called")
        _showPermissionDialog.value = false
    }
    
    fun openAppSettings() {
        Log.d(TAG, "openAppSettings() called")
        permissionHelper.openAppSettings()
    }
    
    fun onPermissionResult(granted: Boolean) {
        Log.d(TAG, "onPermissionResult() called, granted: $granted")
        viewModelScope.launch {
            if (granted) {
                Log.d(TAG, "Permission granted, updating status and getting location")
                _permissionStatus.value = PermissionStatus.Granted
                getLocation()
            } else {
                Log.d(TAG, "Permission denied, checking if should show rationale")
                val newStatus = if (permissionHelper.shouldShowRationale()) {
                    PermissionStatus.Denied
                } else {
                    PermissionStatus.PermanentlyDenied
                }
                Log.d(TAG, "New permission status: $newStatus")
                _permissionStatus.value = newStatus
            }
            _showPermissionDialog.value = false
        }
    }
    
    fun getLocation() {
        Log.d(TAG, "getLocation() called")
        viewModelScope.launch {
            _isGettingLocation.value = true
            _locationError.value = null
            
            locationHelper.getCurrentLocation().collect { status ->
                Log.d(TAG, "getLocation() - location status received: $status")
                when (status) {
                    is LocationHelper.LocationStatus.Success -> {
                        Log.d(TAG, "Location success: lng=${status.result.lng}, lat=${status.result.lat}")
                        val place = Place(
                            name = "当前位置",
                            location = Location(
                                lng = status.result.lng.toString(),
                                lat = status.result.lat.toString()
                            ),
                            address = "当前定位地址"
                        )
                        saveCurrentLocation(place)
                        _locationResult.value = place
                        Log.d(TAG, "Location result updated, navigating to weather screen")
                    }
                    is LocationHelper.LocationStatus.Error -> {
                        Log.e(TAG, "Location error: ${status.message}")
                        _locationError.value = status.message
                    }
                    LocationHelper.LocationStatus.Timeout -> {
                        Log.e(TAG, "Location timeout")
                        _locationError.value = "定位超时"
                    }
                    LocationHelper.LocationStatus.PermissionDenied -> {
                        Log.e(TAG, "Location permission denied")
                        _locationError.value = "定位权限被拒绝"
                        _permissionStatus.value = PermissionStatus.Denied
                        _showPermissionDialog.value = true
                    }
                }
            }
            
            _isGettingLocation.value = false
        }
    }
    
    suspend fun saveCurrentLocation(place: Place) {
        Log.d(TAG, "saveCurrentLocation() called: ${place.name}")
        savePlaceUseCase(place)
        Log.d(TAG, "saveCurrentLocation() - place saved successfully")
    }
    
    fun clearLocationResult() {
        Log.d(TAG, "clearLocationResult() called")
        _locationResult.value = null
        _locationError.value = null
    }
}
