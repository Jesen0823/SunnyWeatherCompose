package org.jesen.dev.sunnyweather.pose.navigation

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
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.navigation3.runtime.NavEntry
import androidx.navigation3.runtime.NavKey
import androidx.navigation3.runtime.rememberNavBackStack
import androidx.navigation3.ui.NavDisplay
import org.jesen.dev.sunnyweather.pose.di.AppModule
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.AppNavigation
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.Screen
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.WeatherViewPager
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.PlaceManagerScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.PlaceScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.SettingsScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.UnknownScreen
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PermissionViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceManagerViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.SettingsViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherListViewModel

/**
 * 应用导航控制器
 *
 * 主要职责：
 * - 管理应用所有页面的导航逻辑
 * - 使用 Navigation3 框架进行页面路由管理
 * - 协调各页面之间的数据传递和状态同步
 * - 处理天气页面、城市搜索页面、城市管理页面和设置页面的导航
 *
 * 技术要点：
 * - 使用 NavBackStack 管理页面栈
 * - 通过 NavDisplay 和 entryProvider 定义页面路由映射
 * - 集成多个 ViewModel 管理各页面状态
 * - 监听 locationResult 自动跳转天气页面
 */
private const val TAG = "AppNavigator"

@Composable
fun AppNavigator(
    initialKey: NavKey,
    isAutoLocating: Boolean = false,
    onPlaceSelected: (Place) -> Unit,
    onClearPlace: () -> Unit,
    onLanguageChanged: () -> Unit = {}
) {
    Log.d(TAG, "AppNavigator started - initialKey: $initialKey, isAutoLocating: $isAutoLocating")
    
    val backStack = rememberNavBackStack(initialKey)

    val weatherListViewModel: WeatherListViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val placeViewModel: PlaceViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val settingsViewModel: SettingsViewModel = viewModel(factory = AppModule.appViewModelFactory)

    val weatherStates by weatherListViewModel.weatherStates.collectAsState()
    val savedPlaces by placeViewModel.savedPlaceList.collectAsState()
    val isInitialLoading by placeViewModel.isInitialLoading.collectAsState()

    var isDrawerOpen by remember { mutableStateOf(false) }

    LaunchedEffect(savedPlaces, isInitialLoading) {
        if (!isInitialLoading && savedPlaces.isNotEmpty()) {
            val currentTopKey = backStack.lastOrNull()
            if (currentTopKey !is WeatherKey) {
                Log.d(TAG, "Auto-navigating to WeatherScreen - savedPlaces loaded: ${savedPlaces.size} places")
                backStack.clear()
                backStack.add(WeatherKey)
            }
        }
    }

    NavDisplay(
        backStack = backStack,
        onBack = { backStack.removeLastOrNull() },
        entryProvider = { key ->
            Log.d(TAG, "NavDisplay entryProvider - key: $key")
            when (key) {
                is WeatherKey -> NavEntry(key) {
                    Log.d(TAG, "Navigating to WeatherScreen - places: ${savedPlaces.size}")
                    AppNavigation(
                        isDrawerOpen = isDrawerOpen,
                        onDrawerClose = { isDrawerOpen = false },
                        currentScreen = Screen.Weather,
                        onScreenChange = { screen ->
                            Log.d(TAG, "Screen change requested: $screen")
                            when (screen) {
                                Screen.Place -> {
                                    Log.d(TAG, "Navigating to PlaceKey")
                                    backStack.add(PlaceKey)
                                }
                                Screen.PlaceManager -> {
                                    Log.d(TAG, "Navigating to PlaceManagerKey")
                                    backStack.add(PlaceManagerKey)
                                }
                                Screen.Settings -> {
                                    Log.d(TAG, "Navigating to SettingsKey")
                                    backStack.add(SettingsKey)
                                }
                                Screen.Weather -> {}
                            }
                            isDrawerOpen = false
                        }
                    ) {
                        if (savedPlaces.isNotEmpty()) {
                            WeatherViewPager(
                                places = savedPlaces,
                                weatherStates = weatherStates,
                                onWeatherLoaded = { place ->
                                    weatherListViewModel.fetchWeather(place, saveWeatherPlace = true)
                                },
                                onRefresh = { place ->
                                    weatherListViewModel.refreshWeather(place)
                                },
                                onNavigateToPlace = { backStack.add(PlaceKey) },
                                onNavigateToSettings = { backStack.add(SettingsKey) },
                                onMenuClick = { isDrawerOpen = !isDrawerOpen }
                            )
                        } else {
                            Log.w(TAG, "WeatherScreen but places is empty, showing loading")
                            Box(modifier = Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
                                CircularProgressIndicator(
                                    modifier = Modifier.size(48.dp),
                                    color = MaterialTheme.colorScheme.primary
                                )
                            }
                        }
                    }
                }

                is PlaceKey -> NavEntry(key) {
                    Log.d(TAG, "Navigating to PlaceScreen")
                    PlaceScreen(
                        viewModel = placeViewModel,
                        onPlaceSelected = { place ->
                            onPlaceSelected(place)
                            backStack.clear()
                            backStack.add(WeatherKey)
                        }
                    )
                }

                is UnknownKey -> NavEntry(key) {
                    Log.d(TAG, "Navigating to UnknownScreen - isAutoLocating: $isAutoLocating")
                    
                    val permissionViewModel: PermissionViewModel = viewModel(
                        factory = AppModule.appViewModelFactory
                    )
                    val permissionStatus by permissionViewModel.permissionStatus.collectAsState()
                    val isCheckingPermission by permissionViewModel.isCheckingPermission.collectAsState()
                    val isGettingLocation by permissionViewModel.isGettingLocation.collectAsState()
                    val showPermissionDialog by permissionViewModel.showPermissionDialog.collectAsState()
                    val locationResult by permissionViewModel.locationResult.collectAsState()
                    val locationError by permissionViewModel.locationError.collectAsState()

                    LaunchedEffect(isAutoLocating) {
                        if (isAutoLocating) {
                            Log.d(TAG, "AutoLocating mode detected, triggering getLocation()")
                            permissionViewModel.getLocation()
                        } else {
                            Log.d(TAG, "Not in AutoLocating mode, user needs to manually trigger")
                        }
                    }

                    LaunchedEffect(locationResult) {
                        locationResult?.let { place ->
                            Log.d(TAG, "Location result received: ${place.name}, navigating to WeatherScreen")
                            onPlaceSelected(place)
                            backStack.clear()
                            backStack.add(WeatherKey)
                            permissionViewModel.clearLocationResult()
                        }
                    }

                    UnknownScreen(
                        permissionStatus = permissionStatus,
                        isCheckingPermission = isCheckingPermission,
                        isGettingLocation = isGettingLocation,
                        showPermissionDialog = showPermissionDialog,
                        locationError = locationError,
                        onOpenLocationSettings = { permissionViewModel.openAppSettings() },
                        onSelectCity = { backStack.add(PlaceKey) },
                        onRequestPermission = { permissionViewModel.requestPermission() },
                        onDismissDialog = { permissionViewModel.dismissDialog() },
                        onGetLocation = { permissionViewModel.getLocation() },
                        isAutoLocating = isAutoLocating
                    )
                }

                is PlaceManagerKey -> NavEntry(key) {
                    val placeManagerViewModel: PlaceManagerViewModel = viewModel(
                        factory = AppModule.appViewModelFactory
                    )
                    PlaceManagerScreen(
                        viewModel = placeManagerViewModel,
                        onBack = { backStack.removeLastOrNull() }
                    )
                }

                is SettingsKey -> NavEntry(key) {
                    SettingsScreen(
                        viewModel = settingsViewModel,
                        onCacheCleared = {
                            onClearPlace()
                            backStack.clear()
                            backStack.add(UnknownKey)
                        },
                        onBack = { backStack.removeLastOrNull() },
                        onLanguageChanged = onLanguageChanged
                    )
                }

                else -> error("Unknown navigation key: $key")
            }
        }
    )
}