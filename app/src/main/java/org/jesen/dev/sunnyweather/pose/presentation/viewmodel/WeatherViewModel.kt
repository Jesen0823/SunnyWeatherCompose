/**
 * 天气 ViewModel
 * 
 * 主要职责：
 * - 管理天气页面的状态和数据
 * - 调用 UseCase 获取天气数据
 * - 将 API 结果转换为 UI 状态
 * 
 * 技术要点：
 * - 使用 StateFlow 管理天气数据状态（Loading/Success/Error）
 * - 通过 viewModelScope 启动协程执行异步操作
 * - 调用 FetchWeatherUseCase 获取实时天气和7日预报数据
 * - 天气数据获取成功后更新 weatherState
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
