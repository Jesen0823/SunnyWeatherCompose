package org.jesen.dev.sunnyweather.pose.presentation.viewmodel

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.launch
import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.usecase.ClearPlaceUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.GetIsPlaceSavedUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.GetSavedPlaceListUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.GetSavedPlaceUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.SavePlaceUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.SearchPlacesUseCase
import org.jesen.dev.sunnyweather.pose.presentation.common.UiState

/**
 * 城市选择 ViewModel
 *
 * 主要职责：
 * - 管理城市搜索页面的状态和数据
 * - 调用 UseCase 搜索城市、保存城市、清除城市
 * - 维护保存的城市列表和当前城市状态
 *
 * 技术要点：
 * - 使用多个 StateFlow 管理不同状态（搜索结果、保存城市、城市列表等）
 * - 通过 viewModelScope 启动协程执行异步操作
 * - 初始化时自动加载保存的城市数据
 * - 调用 SearchPlacesUseCase 搜索城市并处理结果
 */

private const val TAG = "PlaceViewModel"

class PlaceViewModel(
    private val searchPlacesUseCase: SearchPlacesUseCase,
    private val savePlaceUseCase: SavePlaceUseCase,
    private val clearPlaceUseCase: ClearPlaceUseCase,
    private val getSavedPlaceUseCase: GetSavedPlaceUseCase,
    private val getIsPlaceSavedUseCase: GetIsPlaceSavedUseCase,
    private val getSavedPlaceListUseCase: GetSavedPlaceListUseCase
) : ViewModel() {
    private val _placesState = MutableStateFlow<UiState<List<Place>>>(UiState.Loading)
    val placesState: StateFlow<UiState<List<Place>>> = _placesState.asStateFlow()
    
    private val _savedPlace = MutableStateFlow<Place?>(null)
    val savedPlace: StateFlow<Place?> = _savedPlace.asStateFlow()
    
    private val _savedPlaceList = MutableStateFlow<List<Place>>(emptyList())
    val savedPlaceList: StateFlow<List<Place>> = _savedPlaceList.asStateFlow()
    
    private val _isPlaceSaved = MutableStateFlow(false)
    val isPlaceSaved: StateFlow<Boolean> = _isPlaceSaved.asStateFlow()
    
    private val _isInitialLoading = MutableStateFlow(true)
    val isInitialLoading: StateFlow<Boolean> = _isInitialLoading.asStateFlow()
    
    init {
        Log.d(TAG, "========== PlaceViewModel initialized ==========")
        loadSavedPlace()
        loadSavedPlaceList()
    }
    
    private fun loadSavedPlace() {
        Log.d(TAG, "loadSavedPlace() called - starting to load saved place")
        
        viewModelScope.launch {
            Log.d(TAG, "Launching coroutine for getSavedPlaceUseCase")
            getSavedPlaceUseCase().collect { place ->
                Log.d(TAG, "getSavedPlaceUseCase emitted: ${place?.name ?: "null"}")
                _savedPlace.value = place
            }
        }
        
        viewModelScope.launch {
            Log.d(TAG, "Launching coroutine for getIsPlaceSavedUseCase")
            getIsPlaceSavedUseCase().collect { saved ->
                Log.d(TAG, "getIsPlaceSavedUseCase emitted: $saved")
                _isPlaceSaved.value = saved
            }
        }
    }
    
    private fun loadSavedPlaceList() {
        Log.d(TAG, "loadSavedPlaceList() called - starting to load saved place list")
        
        viewModelScope.launch {
            Log.d(TAG, "Launching coroutine for getSavedPlaceListUseCase")
            getSavedPlaceListUseCase().collect { places ->
                Log.d(TAG, "getSavedPlaceListUseCase emitted: ${places.size} places")
                if (places.isNotEmpty()) {
                    val placeNames = places.joinToString(", ") { it.name }
                    Log.d(TAG, "Saved places: [$placeNames]")
                }
                _savedPlaceList.value = places
                if (_isInitialLoading.value) {
                    _isInitialLoading.value = false
                    Log.d(TAG, "isInitialLoading set to false - initial data loading complete")
                }
            }
        }
    }
    
    fun searchPlaces(query: String) {
        viewModelScope.launch {
            _placesState.value = UiState.Loading
            val result = searchPlacesUseCase(query)
            Log.d(TAG, "searchPlaces result: $result")
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
        Log.d(TAG, "savePlace() called with place: ${place.name}")
        val currentList = _savedPlaceList.value.toMutableList()
        if (!currentList.any { it.name == place.name }) {
            currentList.add(place)
        } else {
            val index = currentList.indexOfFirst { it.name == place.name }
            currentList[index] = place
        }
        _savedPlaceList.value = currentList
        _savedPlace.value = place
        _isPlaceSaved.value = true
        Log.d(TAG, "savePlace() - local state updated immediately: ${currentList.size} places")
        
        viewModelScope.launch {
            savePlaceUseCase(place)
            val updatedList = getSavedPlaceListUseCase().first()
            if (updatedList.isNotEmpty()) {
                _savedPlaceList.value = updatedList
                _savedPlace.value = updatedList.first()
            }
            Log.d(TAG, "savePlace() completed - database sync finished: ${updatedList.size} places")
        }
    }

    fun clearPlace(){
        Log.d(TAG, "clearPlace() called")
        viewModelScope.launch {
            clearPlaceUseCase()
            Log.d(TAG, "clearPlace() completed")
        }
    }
}