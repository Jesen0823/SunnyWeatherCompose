package org.jesen.dev.sunnyweather.pose.di

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import org.jesen.dev.sunnyweather.pose.domain.usecase.*
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PermissionViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceManagerViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.SettingsViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherListViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel

/**
 * ViewModel 工厂类
 *
 * 主要职责：
 * - 根据 ViewModel 类名创建对应的 ViewModel 实例
 * - 注入所需的 UseCase 依赖
 * - 支持多个 ViewModel 的创建
 *
 * 技术要点：
 * - 实现 ViewModelProvider.Factory 接口
 * - 使用 when 表达式匹配不同的 ViewModel 类型
 * - 通过构造函数注入所有 UseCase 依赖
 * - 使用 @Suppress("UNCHECKED_CAST") 抑制类型转换警告
 */
class AppViewModelFactory(
    private val searchPlacesUseCase: SearchPlacesUseCase,
    private val fetchWeatherUseCase: FetchWeatherUseCase,
    private val savePlaceUseCase: SavePlaceUseCase,
    private val getSavedPlaceUseCase: GetSavedPlaceUseCase,
    private val getIsPlaceSavedUseCase: GetIsPlaceSavedUseCase,
    private val isSpecificPlaceSavedUseCase: IsSpecificPlaceSavedUseCase,
    private val getSavedPlaceListUseCase: GetSavedPlaceListUseCase,
    private val savePlaceListUseCase: SavePlaceListUseCase,
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
                    getIsPlaceSavedUseCase,
                    getSavedPlaceListUseCase
                ) as T
            modelClass.isAssignableFrom(WeatherViewModel::class.java) ->        
                WeatherViewModel(fetchWeatherUseCase, savePlaceUseCase, getIsPlaceSavedUseCase) as T
            modelClass.isAssignableFrom(WeatherListViewModel::class.java) ->
                WeatherListViewModel(fetchWeatherUseCase, savePlaceUseCase, isSpecificPlaceSavedUseCase) as T
            modelClass.isAssignableFrom(SettingsViewModel::class.java) ->
                SettingsViewModel(clearPlaceUseCase) as T
            modelClass.isAssignableFrom(PermissionViewModel::class.java) ->
                PermissionViewModel(savePlaceUseCase) as T
            modelClass.isAssignableFrom(PlaceManagerViewModel::class.java) ->
                PlaceManagerViewModel(getSavedPlaceListUseCase, savePlaceListUseCase) as T
            else -> throw IllegalArgumentException("Unknown ViewModel class: $modelClass")
        }
    }
}
