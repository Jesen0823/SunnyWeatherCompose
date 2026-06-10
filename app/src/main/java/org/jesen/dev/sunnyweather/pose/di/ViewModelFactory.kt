package org.jesen.dev.sunnyweather.pose.di

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import org.jesen.dev.sunnyweather.pose.domain.usecase.*
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.SettingsViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel

class AppViewModelFactory(
    private val searchPlacesUseCase: SearchPlacesUseCase,
    private val fetchWeatherUseCase: FetchWeatherUseCase,
    private val savePlaceUseCase: SavePlaceUseCase,
    private val getSavedPlaceUseCase: GetSavedPlaceUseCase,
    private val getIsPlaceSavedUseCase: GetIsPlaceSavedUseCase,
    private val clearPlaceUseCase: ClearPlaceUseCase
) : ViewModelProvider.Factory {

    @Suppress("UNCHECKED_CAST")
    override fun <T : ViewModel> create(modelClass: Class<T>): T {
        return when {
            modelClass.isAssignableFrom(PlaceViewModel::class.java) ->
                PlaceViewModel(
                    searchPlacesUseCase,
                    savePlaceUseCase,
                    clearPlaceUseCase,
                    getSavedPlaceUseCase,
                    getIsPlaceSavedUseCase
                ) as T
            modelClass.isAssignableFrom(WeatherViewModel::class.java) ->
                WeatherViewModel(fetchWeatherUseCase) as T
            modelClass.isAssignableFrom(SettingsViewModel::class.java) ->
                SettingsViewModel(clearPlaceUseCase) as T
            else -> throw IllegalArgumentException("Unknown ViewModel class: $modelClass")
        }
    }
}
