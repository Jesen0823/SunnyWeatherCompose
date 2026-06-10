package org.jesen.dev.sunnyweather.pose.di

import android.content.Context
import org.jesen.dev.sunnyweather.pose.data.network.KtorClient
import org.jesen.dev.sunnyweather.pose.data.network.WeatherApiService
import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.data.store.PlaceStore
import org.jesen.dev.sunnyweather.pose.domain.usecase.*
import org.jesen.dev.sunnyweather.pose.ui.locale.LocaleManager
import org.jesen.dev.sunnyweather.pose.ui.theme.ThemeManager

object AppModule {
    lateinit var context: Context

    fun init(context: Context) {
        AppModule.context = context
    }

    val weatherApiService: WeatherApiService by lazy {
        WeatherApiService()
    }

    val placeStore: PlaceStore by lazy {
        PlaceStore(context)
    }

    val weatherRepository: WeatherRepository by lazy {
        WeatherRepository(weatherApiService, placeStore)
    }

    // UseCase 注册
    val searchPlacesUseCase: SearchPlacesUseCase by lazy {
        SearchPlacesUseCase(weatherRepository)
    }

    val fetchWeatherUseCase: FetchWeatherUseCase by lazy {
        FetchWeatherUseCase(weatherRepository)
    }

    val savePlaceUseCase: SavePlaceUseCase by lazy {
        SavePlaceUseCase(weatherRepository)
    }

    val getSavedPlaceUseCase: GetSavedPlaceUseCase by lazy {
        GetSavedPlaceUseCase(weatherRepository)
    }

    val getIsPlaceSavedUseCase: GetIsPlaceSavedUseCase by lazy {
        GetIsPlaceSavedUseCase(weatherRepository)
    }

    val clearPlaceUseCase: ClearPlaceUseCase by lazy {
        ClearPlaceUseCase(weatherRepository)
    }

    // ViewModelFactory 注册
    val appViewModelFactory: AppViewModelFactory by lazy {
        AppViewModelFactory(
            searchPlacesUseCase,
            fetchWeatherUseCase,
            savePlaceUseCase,
            getSavedPlaceUseCase,
            getIsPlaceSavedUseCase,
            clearPlaceUseCase
        )
    }

    // 主题管理器
    val themeManager: ThemeManager by lazy {
        ThemeManager()
    }

    // 语言管理器
    val localeManager: LocaleManager by lazy {
        LocaleManager(context)
    }
}
