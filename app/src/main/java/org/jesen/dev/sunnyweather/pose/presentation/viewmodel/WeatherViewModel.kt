package org.jesen.dev.sunnyweather.pose.presentation.viewmodel

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.domain.model.Weather
import org.jesen.dev.sunnyweather.pose.domain.usecase.FetchWeatherUseCase
import org.jesen.dev.sunnyweather.pose.presentation.common.UiState

class WeatherViewModel(private val fetchWeatherUseCase: FetchWeatherUseCase) : ViewModel() {
    private val _weatherState = MutableStateFlow<UiState<Weather>>(UiState.Loading)
    val weatherState: StateFlow<UiState<Weather>> = _weatherState.asStateFlow()
    
    fun fetchWeather(lng: String, lat: String) {
        viewModelScope.launch {
            _weatherState.value = UiState.Loading
            val result = fetchWeatherUseCase(lng, lat)
            Log.d("WeatherViewModel", "fetchWeather result: $result")
            when (result) {
                is ApiResult.Success -> {
                    _weatherState.value = UiState.Success(result.data)
                }
                is ApiResult.Error -> {
                    _weatherState.value = UiState.Error(result.message)
                }
                is ApiResult.Loading -> {}
            }
        }
    }
}
