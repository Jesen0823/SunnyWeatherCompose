package org.jesen.dev.sunnyweather.pose.presentation.ui

import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.platform.LocalContext
import androidx.lifecycle.viewmodel.compose.viewModel
import org.jesen.dev.sunnyweather.pose.di.AppModule
import org.jesen.dev.sunnyweather.pose.navigation.AppNavigator
import org.jesen.dev.sunnyweather.pose.navigation.UnknownKey
import org.jesen.dev.sunnyweather.pose.navigation.WeatherKey
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceViewModel

/**
 * 应用主组件
 *
 * 主要职责：
 * - 作为应用的根 Compose 组件
 * - 初始化 PlaceViewModel 并获取保存的城市列表
 * - 根据是否有保存的城市决定初始路由
 * - 启动 AppNavigator 管理应用导航
 *
 * 技术要点：
 * - 使用 viewModel 函数获取 PlaceViewModel 实例
 * - 使用 collectAsState 将 Flow 转换为 Compose State
 * - 根据 isPlaceSaved 和 savedPlaces 决定初始路由
 * - 提供城市选择、清除、语言切换等回调
 */
@Composable
fun SunnyWeatherApp() {
    val context = LocalContext.current

    val placeViewModel: PlaceViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val savedPlaces by placeViewModel.savedPlaceList.collectAsState()
    val isPlaceSaved by placeViewModel.isPlaceSaved.collectAsState()

    val initialKey = if (isPlaceSaved && savedPlaces.isNotEmpty()) {
        WeatherKey
    } else {
        UnknownKey
    }

    AppNavigator(
        initialKey = initialKey,
        places = savedPlaces,
        onPlaceSelected = { place ->
            placeViewModel.savePlace(place)
        },
        onClearPlace = {
            placeViewModel.clearPlace()
        },
        onLanguageChanged = {
            val activity = context as? android.app.Activity
            activity?.recreate()
        }
    )
}
