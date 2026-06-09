package org.jesen.dev.sunnyweather.pose.di

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import org.jesen.dev.sunnyweather.pose.domain.usecase.WeatherUseCases
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel

class WeatherViewModelFactory(
    private val useCases: WeatherUseCases
) : ViewModelProvider.Factory {
    @Suppress("UNCHECKED_CAST")
    override fun <T : ViewModel> create(modelClass: Class<T>): T {
        if (modelClass.isAssignableFrom(WeatherViewModel::class.java)) {
            return WeatherViewModel(useCases) as T
        }
        throw IllegalArgumentException("Unknown ViewModel class")
    }
}