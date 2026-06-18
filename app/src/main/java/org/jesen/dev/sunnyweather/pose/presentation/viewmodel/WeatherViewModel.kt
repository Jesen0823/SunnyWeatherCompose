/**
 * 天气 ViewModel
 * 
 * 主要职责：
 * - 管理天气页面的状态和数据
 * - 调用 UseCase 获取天气数据
 * - 将 API 结果转换为 UI 状态
 * - 在首次加载时保存城市信息（防止重复保存）
 * 
 * 技术要点：
 * - 使用 StateFlow 管理天气数据状态（Loading/Success/Error）
 * - 通过 viewModelScope 启动协程执行异步操作
 * - 调用 FetchWeatherUseCase 获取实时天气和7日预报数据
 * - 通过 saveWeatherPlace 参数控制是否保存城市，防止下拉刷新时重复保存
 * - 使用 GetIsPlaceSavedUseCase 判断城市是否已保存，避免重复操作
 */
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
import org.jesen.dev.sunnyweather.pose.domain.model.Weather
import org.jesen.dev.sunnyweather.pose.domain.usecase.FetchWeatherUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.GetIsPlaceSavedUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.SavePlaceUseCase
import org.jesen.dev.sunnyweather.pose.presentation.common.UiState

class WeatherViewModel(
    private val fetchWeatherUseCase: FetchWeatherUseCase,
    private val savePlaceUseCase: SavePlaceUseCase,
    private val getIsPlaceSavedUseCase: GetIsPlaceSavedUseCase
) : ViewModel() {
    private val _weatherState = MutableStateFlow<UiState<Weather>>(UiState.Loading)
    val weatherState: StateFlow<UiState<Weather>> = _weatherState.asStateFlow()
    
    fun fetchWeather(lng: String, lat: String, place: Place? = null, saveWeatherPlace: Boolean = false) {
        viewModelScope.launch {
            _weatherState.value = UiState.Loading
            val result = fetchWeatherUseCase(lng, lat)
            Log.d("WeatherViewModel", "fetchWeather result: $result")
            when (result) {
                is ApiResult.Success -> {
                    _weatherState.value = UiState.Success(result.data)
                    if (saveWeatherPlace && place != null) {
                        savePlaceIfNotExists(place)
                    }
                }
                is ApiResult.Error -> {
                    _weatherState.value = UiState.Error(result.message)
                }
                is ApiResult.Loading -> {}
            }
        }
    }
    
    private suspend fun savePlaceIfNotExists(place: Place) {
        getIsPlaceSavedUseCase().collect { isSaved ->
            if (!isSaved) {
                Log.d("WeatherViewModel", "Saving place: ${place.name}")
                savePlaceUseCase(place)
            } else {
                Log.d("WeatherViewModel", "Place already saved, skipping: ${place.name}")
            }
        }
    }
}
