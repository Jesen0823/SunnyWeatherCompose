package org.jesen.dev.sunnyweather.pose.presentation.ui

import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.platform.LocalContext
import androidx.lifecycle.viewmodel.compose.viewModel
import org.jesen.dev.sunnyweather.pose.di.AppModule
import org.jesen.dev.sunnyweather.pose.navigation.AppNavigator
import org.jesen.dev.sunnyweather.pose.navigation.UnknownKey
import org.jesen.dev.sunnyweather.pose.navigation.WeatherKey
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceViewModel
import org.jesen.dev.sunnyweather.pose.utils.PermissionConstants
import org.jesen.dev.sunnyweather.pose.utils.PermissionHelper
import org.jesen.dev.sunnyweather.pose.utils.PermissionStatus

enum class LaunchState {
    Loading,
    Weather,
    Unknown,
    AutoLocating
}

@Composable
fun SunnyWeatherApp() {
    val context = LocalContext.current

    val placeViewModel: PlaceViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val savedPlaces by placeViewModel.savedPlaceList.collectAsState()
    val isPlaceSaved by placeViewModel.isPlaceSaved.collectAsState()

    var launchState by remember { mutableStateOf(LaunchState.Loading) }

    LaunchedEffect(savedPlaces, isPlaceSaved) {
        if (launchState == LaunchState.Loading) {
            when {
                savedPlaces.isNotEmpty() -> {
                    launchState = LaunchState.Weather
                }
                else -> {
                    val permissionHelper = PermissionHelper(context, PermissionConstants.getLocationPermission())
                    val permissionStatus = permissionHelper.getStatus()
                    
                    if (permissionStatus == PermissionStatus.Granted) {
                        launchState = LaunchState.AutoLocating
                    } else {
                        launchState = LaunchState.Unknown
                    }
                }
            }
        }
    }

    val initialKey = when (launchState) {
        LaunchState.Loading -> UnknownKey
        LaunchState.Weather -> WeatherKey
        LaunchState.AutoLocating -> UnknownKey
        LaunchState.Unknown -> UnknownKey
    }

    AppNavigator(
        initialKey = initialKey,
        places = savedPlaces,
        isAutoLocating = launchState == LaunchState.AutoLocating,
        onPlaceSelected = { place ->
            placeViewModel.savePlace(place)
        },
        onClearPlace = {
            placeViewModel.clearPlace()
        },
        onLanguageChanged = {
            val activity = context as? android.app.Activity
            activity?.recreate()
        }
    )
}