package org.jesen.dev.sunnyweather.pose.presentation.viewmodel

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.collectLatest
import kotlinx.coroutines.launch
import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.model.Weather
import org.jesen.dev.sunnyweather.pose.domain.usecase.WeatherUseCases

sealed class UiState<out T> {
    object Loading : UiState<Nothing>()
    data class Success<out T>(val data: T) : UiState<T>()
    data class Error(val message: String) : UiState<Nothing>()
}

class WeatherViewModel(private val useCases: WeatherUseCases) : ViewModel() {
    private val _placesState = MutableStateFlow<UiState<List<Place>>>(UiState.Loading)
    val placesState: StateFlow<UiState<List<Place>>> = _placesState.asStateFlow()
    
    private val _weatherState = MutableStateFlow<UiState<Weather>>(UiState.Loading)
    val weatherState: StateFlow<UiState<Weather>> = _weatherState.asStateFlow()
    
    private val _savedPlace = MutableStateFlow<Place?>(null)
    val savedPlace: StateFlow<Place?> = _savedPlace.asStateFlow()
    
    private val _isPlaceSaved = MutableStateFlow(false)
    val isPlaceSaved: StateFlow<Boolean> = _isPlaceSaved.asStateFlow()
    
    init {
        loadSavedPlace()
    }
    
    private fun loadSavedPlace() {
        viewModelScope.launch {
            useCases.getSavedPlace().collectLatest { place ->
                _savedPlace.value = place
            }
        }
        
        viewModelScope.launch {
            useCases.isPlaceSaved().collectLatest { saved ->
                _isPlaceSaved.value = saved
            }
        }
    }
    
    fun searchPlaces(query: String) {
        viewModelScope.launch {
            _placesState.value = UiState.Loading
            val result = useCases.searchPlaces(query)
            Log.d("WeatherViewModel,result",result.toString())
            when (result) {
                is ApiResult.Success -> {
                    if (result.data.status == "ok") {
                        _placesState.value = UiState.Success(result.data.places)
                    } else {
                        _placesState.value = UiState.Error("请求失败: ${result.data.status}")
                    }
                }
                is ApiResult.Error -> {
                    _placesState.value = UiState.Error(result.message)
                }
                is ApiResult.Loading -> {}
            }
        }
    }
    
    fun fetchWeather(lng: String, lat: String) {
        viewModelScope.launch {
            _weatherState.value = UiState.Loading
            val result = useCases.fetchWeather(lng, lat)
            Log.d("WeatherViewModel,fetchWeather",result.toString())
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
    
    fun savePlace(place: Place) {
        viewModelScope.launch {
            useCases.savePlace(place)
        }
    }
    
    fun clearPlace() {
        viewModelScope.launch {
            useCases.clearPlace()
        }
    }
}