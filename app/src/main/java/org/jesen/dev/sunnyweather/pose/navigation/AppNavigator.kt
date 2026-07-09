package org.jesen.dev.sunnyweather.pose.navigation

import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
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
 * 应用导航管理器
 *
 * 主要职责：
 * - 管理应用的导航栈（NavBackStack）
 * - 根据路由键（NavKey）展示不同页面
 * - 初始化和管理 ViewModel 实例
 * - 处理页面切换和状态传递
 * - 管理侧边栏抽屉的打开/关闭状态
 *
 * 技术要点：
 * - 使用 Compose Navigation3 库进行路由管理
 * - 使用 NavDisplay 和 NavEntry 定义路由与页面的映射关系
 * - 通过 viewModel 函数获取 ViewModel 实例并注入依赖
 * - 使用 collectAsState 将 Flow 转换为 Compose State
 * - 支持四种路由：WeatherKey、PlaceKey、UnknownKey、SettingsKey
 */
@Composable
fun AppNavigator(
    initialKey: NavKey,
    places: List<Place>,
    onPlaceSelected: (Place) -> Unit,
    onClearPlace: () -> Unit,
    onLanguageChanged: () -> Unit = {}
) {
    val backStack = rememberNavBackStack(initialKey)

    val weatherListViewModel: WeatherListViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val placeViewModel: PlaceViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val settingsViewModel: SettingsViewModel = viewModel(factory = AppModule.appViewModelFactory)

    val weatherStates by weatherListViewModel.weatherStates.collectAsState()

    var isDrawerOpen by remember { mutableStateOf(false) }

    NavDisplay(
        backStack = backStack,
        onBack = { backStack.removeLastOrNull() },
        entryProvider = { key ->
            when (key) {
                is WeatherKey -> NavEntry(key) {
                    AppNavigation(
                        isDrawerOpen = isDrawerOpen,
                        onDrawerClose = { isDrawerOpen = false },
                        currentScreen = Screen.Weather,
                        onScreenChange = { screen ->
                            when (screen) {
                                Screen.Place -> backStack.add(PlaceKey)
                                Screen.PlaceManager -> backStack.add(PlaceManagerKey)
                                Screen.Settings -> backStack.add(SettingsKey)
                                Screen.Weather -> {}
                            }
                            isDrawerOpen = false
                        }
                    ) {
                        if (places.isNotEmpty()) {
                            WeatherViewPager(
                                places = places,
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
                        }
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

                    androidx.compose.runtime.LaunchedEffect(locationResult) {
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
