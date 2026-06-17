/**
 * 应用主入口组件
 * 
 * 主要职责：
 * - 作为 Compose 应用的根组件
 * - 根据是否有保存的城市决定初始页面
 * - 提供城市选择和清除的回调处理
 * 
 * 技术要点：
 * - 使用 PlaceViewModel 观察保存的城市状态
 * - 通过 isPlaceSaved 和 savedPlace 判断初始页面（WeatherKey 或 UnknownKey）
 * - 将城市选择和清除操作委托给 PlaceViewModel 处理
 * - 通过 onLanguageChanged 回调处理语言切换后的页面重建
 */
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

@Composable
fun SunnyWeatherApp() {
    val context = LocalContext.current

    val placeViewModel: PlaceViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val savedPlace by placeViewModel.savedPlace.collectAsState()
    val isPlaceSaved by placeViewModel.isPlaceSaved.collectAsState()

    // 启动时根据城市列表判断初始页面
    val initialKey = if (isPlaceSaved && savedPlace != null) {
        WeatherKey
    } else {
        UnknownKey
    }

    AppNavigator(
        initialKey = initialKey,
        selectedPlace = savedPlace,
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
