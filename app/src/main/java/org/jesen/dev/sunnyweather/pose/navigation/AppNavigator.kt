/**
 * 应用导航组件
 * 
 * 主要职责：
 * - 管理应用的页面导航逻辑
 * - 根据导航键（NavKey）渲染对应的屏幕组件
 * - 协调 ViewModel 和屏幕之间的数据传递
 * 
 * 技术要点：
 * - 使用 Navigation3 库实现声明式导航
 * - 通过 NavDisplay 和 NavEntry 定义页面路由
 * - 支持 WeatherScreen、PlaceScreen、UnknownScreen、SettingsScreen 四个页面
 * - 通过 LaunchedEffect 监听定位结果并自动跳转
 */
package org.jesen.dev.sunnyweather.pose.navigation

import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.LaunchedEffect
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.navigation3.runtime.NavEntry
import androidx.navigation3.runtime.NavKey
import androidx.navigation3.runtime.rememberNavBackStack
import androidx.navigation3.ui.NavDisplay
import org.jesen.dev.sunnyweather.pose.di.AppModule
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.PlaceScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.SettingsScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.UnknownScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.WeatherScreen
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PermissionViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.SettingsViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel

@Composable
fun AppNavigator(
    initialKey: NavKey,
    selectedPlace: Place?,
    onPlaceSelected: (Place) -> Unit,
    onClearPlace: () -> Unit,
    onLanguageChanged: () -> Unit = {}
) {
    val backStack = rememberNavBackStack(initialKey)

    val weatherViewModel: WeatherViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val placeViewModel: PlaceViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val settingsViewModel: SettingsViewModel = viewModel(factory = AppModule.appViewModelFactory)

    NavDisplay(
        backStack = backStack,
        onBack = { backStack.removeLastOrNull() },
        entryProvider = { key ->
            when (key) {
                is WeatherKey -> NavEntry(key) {
                    val place = selectedPlace
                    if (place != null) {
                        WeatherScreen(
                            viewModel = weatherViewModel,
                            placeName = place.name,
                            lng = place.location.lng,
                            lat = place.location.lat,
                            onNavigateToPlace = { backStack.add(PlaceKey) },
                            onNavigateToSettings = { backStack.add(SettingsKey) }
                        )
                    }
                }

                is PlaceKey -> NavEntry(key) {
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
                    val permissionViewModel: PermissionViewModel = viewModel(
                        factory = AppModule.appViewModelFactory
                    )
                    val permissionStatus by permissionViewModel.permissionStatus.collectAsState()
                    val isCheckingPermission by permissionViewModel.isCheckingPermission.collectAsState()
                    val isGettingLocation by permissionViewModel.isGettingLocation.collectAsState()
                    val showPermissionDialog by permissionViewModel.showPermissionDialog.collectAsState()
                    val locationResult by permissionViewModel.locationResult.collectAsState()
                    val locationError by permissionViewModel.locationError.collectAsState()

                    LaunchedEffect(locationResult) {
                        locationResult?.let { place ->
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
                        onGetLocation = { permissionViewModel.getLocation() }
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
