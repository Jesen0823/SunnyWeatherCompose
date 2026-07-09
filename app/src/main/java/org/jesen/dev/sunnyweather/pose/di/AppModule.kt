/**
 * 应用依赖注入模块
 *
 * 主要职责：
 * - 管理应用级别的单例依赖
 * - 提供 Application 上下文和权限管理
 * - 创建并缓存 UseCase、Repository、Store 等实例
 * - 提供 ViewModel 工厂
 * - 初始化环境配置
 *
 * 技术要点：
 * - 使用 object 单例模式
 * - 使用 Application 上下文避免内存泄漏
 * - 使用 lazy 延迟初始化，避免提前创建不必要的实例
 * - 管理权限请求的回调机制
 * - 所有依赖通过属性提供，便于测试和替换
 * - 依赖倒置：使用接口而不是具体实现
 */
package org.jesen.dev.sunnyweather.pose.di

import android.app.Application
import android.content.Context
import androidx.activity.result.ActivityResultLauncher
import org.jesen.dev.sunnyweather.pose.data.network.EnvironmentConfig
import org.jesen.dev.sunnyweather.pose.data.network.WeatherApiService
import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepositoryImpl
import org.jesen.dev.sunnyweather.pose.data.store.PlaceStore
import org.jesen.dev.sunnyweather.pose.domain.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.domain.usecase.*
import org.jesen.dev.sunnyweather.pose.ui.locale.LocaleManager
import org.jesen.dev.sunnyweather.pose.ui.theme.ThemeManager
import org.jesen.dev.sunnyweather.pose.utils.PermissionConstants

object AppModule {
    private val TAG = "AppModule"

    lateinit var application: Application
    private var permissionLauncher: ActivityResultLauncher<String>? = null
    private var permissionCallback: ((Boolean) -> Unit)? = null

    fun init(application: Application) {
        AppModule.application = application
        EnvironmentConfig.init(application)
    }

    val context: Context
        get() = application.applicationContext

    fun registerPermissionLauncher(launcher: ActivityResultLauncher<String>) {
        permissionLauncher = launcher
    }

    fun requestLocationPermission(callback: (Boolean) -> Unit) {
        permissionCallback = callback
        permissionLauncher?.launch(PermissionConstants.getLocationPermission())
    }

    fun onPermissionResult(isGranted: Boolean) {
        permissionCallback?.invoke(isGranted)
        permissionCallback = null
    }

    val weatherApiService: WeatherApiService by lazy {
        WeatherApiService()
    }

    val placeStore: PlaceStore by lazy {
        PlaceStore(context)
    }

    val weatherRepository: WeatherRepository by lazy {
        WeatherRepositoryImpl(weatherApiService, placeStore)
    }

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

    val isSpecificPlaceSavedUseCase: IsSpecificPlaceSavedUseCase by lazy {
        IsSpecificPlaceSavedUseCase(weatherRepository)
    }

    val getSavedPlaceListUseCase: GetSavedPlaceListUseCase by lazy {
        GetSavedPlaceListUseCase(weatherRepository)
    }

    val savePlaceListUseCase: SavePlaceListUseCase by lazy {
        SavePlaceListUseCase(weatherRepository)
    }

    val clearPlaceUseCase: ClearPlaceUseCase by lazy {
        ClearPlaceUseCase(weatherRepository)
    }

    val appViewModelFactory: AppViewModelFactory by lazy {
        AppViewModelFactory(
            searchPlacesUseCase,
            fetchWeatherUseCase,
            savePlaceUseCase,
            getSavedPlaceUseCase,
            getIsPlaceSavedUseCase,
            isSpecificPlaceSavedUseCase,
            getSavedPlaceListUseCase,
            savePlaceListUseCase,
            clearPlaceUseCase
        )
    }

    val themeManager: ThemeManager by lazy {
        ThemeManager()
    }

    val localeManager: LocaleManager by lazy {
        LocaleManager(context)
    }
}