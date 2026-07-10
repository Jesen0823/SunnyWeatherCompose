package org.jesen.dev.sunnyweather.pose.presentation.ui

import android.util.Log
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.size
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import org.jesen.dev.sunnyweather.pose.di.AppModule
import org.jesen.dev.sunnyweather.pose.navigation.AppNavigator
import org.jesen.dev.sunnyweather.pose.navigation.UnknownKey
import org.jesen.dev.sunnyweather.pose.navigation.WeatherKey
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PermissionViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceViewModel
import org.jesen.dev.sunnyweather.pose.utils.PermissionConstants
import org.jesen.dev.sunnyweather.pose.utils.PermissionHelper
import org.jesen.dev.sunnyweather.pose.utils.PermissionStatus

enum class LaunchState {
    Loading,
    Weather,
    Unknown,
    AutoLocating,
    FirstTimePermissionRequest,
    PermissionRequested
}

/**
 * 应用主入口组件
 *
 * 主要职责：
 * - 管理应用启动时的路由逻辑
 * - 优先检查缓存的城市列表，有数据直接展示天气页面
 * - 无缓存时检查定位权限，有权限自动获取位置
 * - 无权限时进入UnknownScreen引导用户选择城市
 *
 * 技术要点：
 * - 使用 LaunchState 枚举管理启动流程状态
 * - 使用 rememberSaveable 确保 Activity 重建时状态不丢失
 * - 通过 LaunchedEffect 监听状态变化并触发路由切换
 * - 集成 PlaceViewModel 和 PermissionViewModel 管理数据和权限
 */
private const val TAG = "SunnyWeatherApp"

@Composable
fun SunnyWeatherApp() {
    Log.d(TAG, "========== SunnyWeatherApp Composing ==========")
    
    val context = LocalContext.current

    val placeViewModel: PlaceViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val savedPlaces by placeViewModel.savedPlaceList.collectAsState()
    val isPlaceSaved by placeViewModel.isPlaceSaved.collectAsState()
    val isInitialLoading by placeViewModel.isInitialLoading.collectAsState()

    val permissionViewModel: PermissionViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val permissionStatus by permissionViewModel.permissionStatus.collectAsState()

    Log.d(TAG, "Initial savedPlaces: ${savedPlaces.size} places, isPlaceSaved: $isPlaceSaved, isInitialLoading: $isInitialLoading")

    var launchState by rememberSaveable { mutableStateOf(LaunchState.Loading) }
    var hasRequestedPermission by rememberSaveable { mutableStateOf(false) }

    LaunchedEffect(savedPlaces, isPlaceSaved, permissionStatus, isInitialLoading) {
        Log.d(TAG, "LaunchedEffect triggered - savedPlaces.size: ${savedPlaces.size}, isPlaceSaved: $isPlaceSaved, permissionStatus: $permissionStatus, isInitialLoading: $isInitialLoading, current launchState: $launchState, hasRequestedPermission: $hasRequestedPermission")
        
        if (isInitialLoading) {
            Log.d(TAG, "Data is still loading, skipping route decision")
            return@LaunchedEffect
        }
        
        when {
            savedPlaces.isNotEmpty() -> {
                Log.d(TAG, "Route decision: savedPlaces is NOT empty (${savedPlaces.size} places), navigating to WeatherScreen")
                launchState = LaunchState.Weather
            }
            launchState != LaunchState.Weather && permissionStatus == PermissionStatus.Granted -> {
                Log.d(TAG, "Route decision: Permission granted, entering AutoLocating mode")
                launchState = LaunchState.AutoLocating
            }
            launchState == LaunchState.Loading -> {
                val permissionHelper = PermissionHelper(context, PermissionConstants.getLocationPermission())
                val status = permissionHelper.getStatus()
                Log.d(TAG, "Location permission status: $status")
                
                when (status) {
                    PermissionStatus.FirstTimeRequest -> {
                        Log.d(TAG, "Route decision: First time requesting permission, entering FirstTimePermissionRequest mode")
                        launchState = LaunchState.FirstTimePermissionRequest
                    }
                    else -> {
                        Log.d(TAG, "Route decision: Permission not granted ($status), navigating to UnknownScreen")
                        launchState = LaunchState.Unknown
                    }
                }
            }
            launchState == LaunchState.PermissionRequested -> {
                val permissionHelper = PermissionHelper(context, PermissionConstants.getLocationPermission())
                val actualStatus = permissionHelper.getStatus()
                Log.d(TAG, "Activity recreated, checking actual permission status: $actualStatus")
                
                if (actualStatus == PermissionStatus.Granted) {
                    Log.d(TAG, "Route decision: Permission actually granted after recreation, entering AutoLocating mode")
                    launchState = LaunchState.AutoLocating
                } else {
                    Log.d(TAG, "Route decision: Refreshing permission status in ViewModel")
                    permissionViewModel.refreshPermissionStatus()
                }
            }
        }
    }

    LaunchedEffect(launchState) {
        if (launchState == LaunchState.FirstTimePermissionRequest && !hasRequestedPermission) {
            Log.d(TAG, "LaunchedEffect: FirstTimePermissionRequest detected, calling requestPermission()")
            hasRequestedPermission = true
            launchState = LaunchState.PermissionRequested
            permissionViewModel.requestPermission()
        }
    }

    val initialKey = when (launchState) {
        LaunchState.Loading -> {
            Log.d(TAG, "Initial key: Loading -> UnknownKey (temporary)")
            UnknownKey
        }
        LaunchState.Weather -> {
            Log.d(TAG, "Initial key: Weather -> WeatherKey")
            WeatherKey
        }
        LaunchState.AutoLocating -> {
            Log.d(TAG, "Initial key: AutoLocating -> UnknownKey (will auto-locate)")
            UnknownKey
        }
        LaunchState.FirstTimePermissionRequest -> {
            Log.d(TAG, "Initial key: FirstTimePermissionRequest -> UnknownKey (will request permission)")
            UnknownKey
        }
        LaunchState.PermissionRequested -> {
            Log.d(TAG, "Initial key: PermissionRequested -> UnknownKey (waiting for permission result)")
            UnknownKey
        }
        LaunchState.Unknown -> {
            Log.d(TAG, "Initial key: Unknown -> UnknownKey")
            UnknownKey
        }
    }

    Log.d(TAG, "========== AppNavigator starting with initialKey: $initialKey ==========")
    
    if (isInitialLoading) {
        Box(modifier = Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
            CircularProgressIndicator(
                modifier = Modifier.size(48.dp),
                color = MaterialTheme.colorScheme.primary
            )
        }
    } else {
        AppNavigator(
            initialKey = initialKey,
            isAutoLocating = launchState == LaunchState.AutoLocating,
            onPlaceSelected = { place ->
                Log.d(TAG, "onPlaceSelected called: ${place.name}")
                placeViewModel.savePlace(place)
            },
            onClearPlace = {
                Log.d(TAG, "onClearPlace called")
                placeViewModel.clearPlace()
            },
            onLanguageChanged = {
                Log.d(TAG, "onLanguageChanged called")
                val activity = context as? android.app.Activity
                activity?.recreate()
            }
        )
    }
}