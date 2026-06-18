/**
 * ViewModel 工厂类
 * 
 * 主要职责：
 * - 根据 ViewModel 类型创建对应的 ViewModel 实例
 * - 注入必要的 UseCase 依赖
 * 
 * 技术要点：
 * - 实现 ViewModelProvider.Factory 接口
 * - 使用 when 表达式匹配不同的 ViewModel 类型
 * - 支持 PlaceViewModel、WeatherViewModel、SettingsViewModel、PermissionViewModel
 * - 使用 @Suppress("UNCHECKED_CAST") 抑制类型转换警告
 */
package org.jesen.dev.sunnyweather.pose.di

import androidx.lifecycle.ViewModel
import androidx.lifecycle.ViewModelProvider
import org.jesen.dev.sunnyweather.pose.domain.usecase.*
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PermissionViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.SettingsViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel

class AppViewModelFactory(
    private val searchPlacesUseCase: SearchPlacesUseCase,
    private val fetchWeatherUseCase: FetchWeatherUseCase,
    private val savePlaceUseCase: SavePlaceUseCase,
    private val getSavedPlaceUseCase: GetSavedPlaceUseCase,
    private val getIsPlaceSavedUseCase: GetIsPlaceSavedUseCase,
    private val getSavedPlaceListUseCase: GetSavedPlaceListUseCase,
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
            modelClass.isAssignableFrom(SettingsViewModel::class.java) ->
                SettingsViewModel(clearPlaceUseCase) as T
            modelClass.isAssignableFrom(PermissionViewModel::class.java) ->
                PermissionViewModel(savePlaceUseCase) as T
            else -> throw IllegalArgumentException("Unknown ViewModel class: $modelClass")
        }
    }
}
