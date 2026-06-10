package org.jesen.dev.sunnyweather.pose.presentation.viewmodel

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.usecase.ClearPlaceUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.GetIsPlaceSavedUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.GetSavedPlaceUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.SavePlaceUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.SearchPlacesUseCase
import org.jesen.dev.sunnyweather.pose.presentation.common.UiState

class PlaceViewModel(
    private val searchPlacesUseCase: SearchPlacesUseCase,
    private val savePlaceUseCase: SavePlaceUseCase,
    private val clearPlaceUseCase: ClearPlaceUseCase,
    private val getSavedPlaceUseCase: GetSavedPlaceUseCase,
    private val getIsPlaceSavedUseCase: GetIsPlaceSavedUseCase
) : ViewModel() {
    private val _placesState = MutableStateFlow<UiState<List<Place>>>(UiState.Loading)
    val placesState: StateFlow<UiState<List<Place>>> = _placesState.asStateFlow()
    
    private val _savedPlace = MutableStateFlow<Place?>(null)
    val savedPlace: StateFlow<Place?> = _savedPlace.asStateFlow()
    
    private val _isPlaceSaved = MutableStateFlow(false)
    val isPlaceSaved: StateFlow<Boolean> = _isPlaceSaved.asStateFlow()
    
    init {
        loadSavedPlace()
    }
    
    private fun loadSavedPlace() {
        viewModelScope.launch {
            getSavedPlaceUseCase().collect { place ->
                _savedPlace.value = place
            }
        }
        
        viewModelScope.launch {
            getIsPlaceSavedUseCase().collect { saved ->
                _isPlaceSaved.value = saved
            }
        }
    }
    
    fun searchPlaces(query: String) {
        viewModelScope.launch {
            _placesState.value = UiState.Loading
            val result = searchPlacesUseCase(query)
            Log.d("PlaceViewModel", "searchPlaces result: $result")
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
    
    fun savePlace(place: Place) {
        viewModelScope.launch {
            savePlaceUseCase(place)
        }
    }

    fun  clearPlace(){
        viewModelScope.launch {
            clearPlaceUseCase()
        }
    }
}
