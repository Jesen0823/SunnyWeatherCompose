/**
 * 应用依赖注入模块
 * 
 * 主要职责：
 * - 管理应用级别的单例依赖
 * - 提供 Repository、UseCase、ViewModelFactory 等核心组件
 * - 处理权限请求的全局协调
 * 
 * 技术要点：
 * - 使用 object 单例模式确保全局唯一实例
 * - 使用 lazy 延迟初始化，避免不必要的资源消耗
 * - 管理 ActivityResultLauncher 的注册和权限结果回调
 * - 提供 ThemeManager 和 LocaleManager 支持主题和多语言切换
 */
package org.jesen.dev.sunnyweather.pose.di

import android.content.Context
import android.util.Log
import androidx.activity.result.ActivityResultLauncher
import org.jesen.dev.sunnyweather.pose.data.network.WeatherApiService
import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.data.store.PlaceStore
import org.jesen.dev.sunnyweather.pose.domain.usecase.*
import org.jesen.dev.sunnyweather.pose.ui.locale.LocaleManager
import org.jesen.dev.sunnyweather.pose.ui.theme.ThemeManager
import org.jesen.dev.sunnyweather.pose.utils.PermissionConstants

object AppModule {
    private val TAG = "AppModule"
    
    lateinit var context: Context
    private var permissionLauncher: ActivityResultLauncher<String>? = null
    private var permissionCallback: ((Boolean) -> Unit)? = null

    fun init(context: Context) {
        Log.d(TAG, "init() called with context: ${context.packageName}")
        AppModule.context = context
    }

    fun registerPermissionLauncher(launcher: ActivityResultLauncher<String>) {
        Log.d(TAG, "registerPermissionLauncher() called")
        permissionLauncher = launcher
        Log.d(TAG, "permissionLauncher registered: ${launcher != null}")
    }

    fun requestLocationPermission(callback: (Boolean) -> Unit) {
        Log.d(TAG, "requestLocationPermission() called")
        Log.d(TAG, "permissionLauncher is null: ${permissionLauncher == null}")
        permissionCallback = callback
        permissionLauncher?.launch(PermissionConstants.getLocationPermission())
        Log.d(TAG, "launch() called, permission: ${PermissionConstants.getLocationPermission()}")
    }

    fun onPermissionResult(isGranted: Boolean) {
        Log.d(TAG, "onPermissionResult() called, isGranted: $isGranted")
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
        WeatherRepository(weatherApiService, placeStore)
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

    val getSavedPlaceListUseCase: GetSavedPlaceListUseCase by lazy {
        GetSavedPlaceListUseCase(weatherRepository)
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
            getSavedPlaceListUseCase,
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
