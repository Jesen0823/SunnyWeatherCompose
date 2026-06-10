package org.jesen.dev.sunnyweather.pose.presentation.ui

import android.content.Context
import android.content.Intent
import android.net.Uri
import android.provider.Settings
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.lifecycle.viewmodel.compose.viewModel
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.navigation.AppNavigator
import org.jesen.dev.sunnyweather.pose.navigation.UnknownKey
import org.jesen.dev.sunnyweather.pose.navigation.WeatherKey
import org.jesen.dev.sunnyweather.pose.di.AppModule
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceViewModel
import androidx.compose.ui.platform.LocalContext

@Composable
fun SunnyWeatherApp() {
    val context = LocalContext.current
    
    val placeViewModel: PlaceViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val savedPlace by placeViewModel.savedPlace.collectAsState()
    val isPlaceSaved by placeViewModel.isPlaceSaved.collectAsState()
    
    val initialKey = if (isPlaceSaved && savedPlace != null) WeatherKey else UnknownKey

    AppNavigator(
        initialKey = initialKey,
        selectedPlace = savedPlace,
        onOpenLocationSettings = {
            openAppSettings(context)
        },
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

private fun openAppSettings(context: Context) {
    val intent = Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS).apply {
        data = Uri.fromParts("package", context.packageName, null)
        flags = Intent.FLAG_ACTIVITY_NEW_TASK
    }
    context.startActivity(intent)
}
