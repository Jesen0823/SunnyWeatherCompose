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
import org.jesen.dev.sunnyweather.pose.navigation.AppNavigator
import org.jesen.dev.sunnyweather.pose.navigation.UnknownKey
import org.jesen.dev.sunnyweather.pose.navigation.WeatherKey
import org.jesen.dev.sunnyweather.pose.di.AppModule
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel
import androidx.compose.ui.platform.LocalContext

@Composable
fun SunnyWeatherApp() {
    val context = LocalContext.current
    var selectedPlace by remember { mutableStateOf<Place?>(null) }
    var shouldNavigateToWeather by remember { mutableStateOf(false) }
    
    val viewModel: WeatherViewModel = viewModel(factory = AppModule.weatherViewModelFactory)
    
    LaunchedEffect(Unit) {
        viewModel.isPlaceSaved.collect { isSaved ->
            if (isSaved) {
                viewModel.savedPlace.collect { place ->
                    if (place != null) {
                        selectedPlace = place
                        shouldNavigateToWeather = true
                    } else {
                        shouldNavigateToWeather = false
                    }
                }
            } else {
                shouldNavigateToWeather = false
            }
        }
    }

    val initialKey = if (shouldNavigateToWeather) WeatherKey else UnknownKey

    AppNavigator(
        initialKey = initialKey,
        selectedPlace = selectedPlace,
        onOpenLocationSettings = {
            openAppSettings(context)
        },
        onPlaceSelected = { place ->
            selectedPlace = place
            viewModel.savePlace(place)
        },
        onClearPlace = {
            viewModel.clearPlace()
            selectedPlace = null
        }
    )
}

private fun openAppSettings(context: Context) {
    val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS).apply {
        data = Uri.fromParts("package", context.packageName, null)
        flags = Intent.FLAG_ACTIVITY_NEW_TASK
    }
    context.startActivity(intent)
}