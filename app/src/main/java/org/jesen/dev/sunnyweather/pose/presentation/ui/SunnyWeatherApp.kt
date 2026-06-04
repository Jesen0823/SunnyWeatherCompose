package org.jesen.dev.sunnyweather.pose.presentation.ui

import android.content.Context
import android.content.Intent
import android.net.Uri
import android.provider.Settings
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.lifecycle.viewmodel.compose.viewModel
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.AppNavigation
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.Screen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.PlaceScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.SettingsScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.UnknownScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.WeatherScreen
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel
import androidx.compose.ui.platform.LocalContext

sealed class AppScreen {
    object Weather : AppScreen()
    object Place : AppScreen()
    object Unknown : AppScreen()
    object Settings : AppScreen()
}

@Composable
fun SunnyWeatherApp() {
    val context = LocalContext.current
    var currentScreen: AppScreen by remember { mutableStateOf(AppScreen.Unknown) }
    var selectedPlace by remember { mutableStateOf<Place?>(null) }
    var hasTriedPermission by remember { mutableStateOf(false) }
    var permissionRequested by remember { mutableStateOf(false) }
    
    val viewModel: WeatherViewModel = viewModel()
    
    LaunchedEffect(Unit) {
        viewModel.isPlaceSaved.collect { isSaved ->
            if (isSaved) {
                viewModel.savedPlace.collect { place ->
                    if (place != null) {
                        selectedPlace = place
                        currentScreen = AppScreen.Weather
                    } else if (!hasTriedPermission) {
                        requestLocationPermission(context)
                        hasTriedPermission = true
                        permissionRequested = true
                    }
                }
            } else if (!hasTriedPermission) {
                requestLocationPermission(context)
                hasTriedPermission = true
                permissionRequested = true
            }
        }
    }
    
    AppNavigation(
        currentScreen = when (currentScreen) {
            AppScreen.Weather -> Screen.Weather
            AppScreen.Place -> Screen.Place
            AppScreen.Settings -> Screen.Settings
            AppScreen.Unknown -> Screen.Weather
        },
        onScreenChange = { screen ->
            currentScreen = when (screen) {
                Screen.Weather -> AppScreen.Weather
                Screen.Place -> AppScreen.Place
                Screen.Settings -> AppScreen.Settings
            }
        }
    ) {
        when (currentScreen) {
            AppScreen.Weather -> {
                selectedPlace?.let { place ->
                    WeatherScreen(
                        viewModel = viewModel,
                        placeName = place.name,
                        lng = place.location.lng,
                        lat = place.location.lat,
                        onNavigateToPlace = {
                            currentScreen = AppScreen.Place
                        }
                    )
                } ?: UnknownScreen(
                    onOpenLocationSettings = {
                        openAppSettings(context)
                    },
                    onSelectCity = {
                        currentScreen = AppScreen.Place
                    }
                )
            }
            AppScreen.Place -> PlaceScreen(
                viewModel = viewModel,
                onPlaceSelected = { place ->
                    selectedPlace = place
                    viewModel.savePlace(place)
                    currentScreen = AppScreen.Weather
                }
            )
            AppScreen.Settings -> SettingsScreen(
                onClearPlace = {
                    viewModel.clearPlace()
                    selectedPlace = null
                    currentScreen = AppScreen.Unknown
                    hasTriedPermission = false
                    permissionRequested = false
                }
            )
            AppScreen.Unknown -> UnknownScreen(
                onOpenLocationSettings = {
                    openAppSettings(context)
                },
                onSelectCity = {
                    currentScreen = AppScreen.Place
                }
            )
        }
    }
}

private fun requestLocationPermission(context: Context) {
    // 在实际应用中，这里应该使用 Activity Result API 请求权限
    // 由于这是一个简化实现，我们直接检查权限状态
    // 实际实现需要在 Activity 中注册权限请求
}

private fun openAppSettings(context: Context) {
    val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS).apply {
        data = Uri.fromParts("package", context.packageName, null)
        flags = Intent.FLAG_ACTIVITY_NEW_TASK
    }
    context.startActivity(intent)
}