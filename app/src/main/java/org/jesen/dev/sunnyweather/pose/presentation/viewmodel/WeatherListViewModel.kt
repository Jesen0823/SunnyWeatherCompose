package org.jesen.dev.sunnyweather.pose.presentation.viewmodel

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewmodelScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.launch
import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.model.Weather
import org.jesen.dev.sunnyweather.pose.domain.usecase.FetchWeatherUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.IsSpecificPlaceSavedUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.SavePlaceUseCase
import org.jesen.dev.sunnyweather.pose.presentation.common.UiState

/**
 * 多城市天气 ViewModel
 *
 * 主要职责：
 * - 管理多个城市的天气状态
 * - 支持并发获取不同城市的天气数据
 * - 防止重复请求和重复保存城市
 * - 提供刷新指定城市天气的方法
 *
 * 技术要点：
 * - 使用 Map<String, UiState<Weather>> 存储多个城市的天气状态
 * - 使用 StateFlow 管理状态，支持响应式数据监听
 * - 通过 saveWeatherPlace 参数控制是否保存城市
 * - 使用 IsSpecificPlaceSavedUseCase 检查特定城市是否已保存
 * - 使用 first() 获取单次 Flow 结果，避免协程泄漏
 */
class WeatherListViewModel(
    private val fetchWeatherUseCase: FetchWeatherUseCase,
    private val savePlaceUseCase: SavePlaceUseCase,
    private val isSpecificPlaceSavedUseCase: IsSpecificPlaceSavedUseCase
) : ViewModel() {
    private val _weatherStates = MutableStateFlow<Map<String, UiState<Weather>>>(emptyMap())
    val weatherStates: StateFlow<Map<String, UiState<Weather>>> = _weatherStates.asStateFlow()

    fun fetchWeather(place: Place, saveWeatherPlace: Boolean = false) {
        val currentState = _weatherStates.value[place.name]
        if (currentState is UiState.Loading) {
            return
        }

        viewModelScope.launch {
            _weatherStates.value = _weatherStates.value + (place.name to UiState.Loading)
            
            val result = fetchWeatherUseCase(place.location.lng, place.location.lat)
            
            when (result) {
                is ApiResult.Success -> {
                    _weatherStates.value = _weatherStates.value + (place.name to UiState.Success(result.data))
                    if (saveWeatherPlace) {
                        savePlaceIfNotExists(place)
                    }
                }
                is ApiResult.Error -> {
                    _weatherStates.value = _weatherStates.value + (place.name to UiState.Error(result.message))
                }
                is ApiResult.Loading -> {}
            }
        }
    }

    fun refreshWeather(place: Place) {
        fetchWeather(place, saveWeatherPlace = false)
    }

    private suspend fun savePlaceIfNotExists(place: Place) {
        val isSaved = isSpecificPlaceSavedUseCase(place.name).first()
        if (!isSaved) {
            savePlaceUseCase(place)
        }
    }
}
