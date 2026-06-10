package org.jesen.dev.sunnyweather.pose.navigation

import androidx.compose.runtime.Composable
import androidx.navigation3.runtime.NavEntry
import androidx.navigation3.runtime.NavKey
import androidx.navigation3.runtime.rememberNavBackStack
import androidx.navigation3.ui.NavDisplay
import androidx.lifecycle.viewmodel.compose.viewModel
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.di.AppModule
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.NavigationDrawerExamples
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.PlaceScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.SettingsScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.SimpleCenterAlignedTopAppBar
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.TestPageScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.UnknownScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.WeatherScreen
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.SettingsViewModel
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel

@Composable
fun AppNavigator(
    initialKey: NavKey,
    selectedPlace: Place?,
    onOpenLocationSettings: () -> Unit,
    onPlaceSelected: (Place) -> Unit,
    onClearPlace: () -> Unit,
    onLanguageChanged: () -> Unit = {}
) {
    val backStack = rememberNavBackStack(initialKey)
    
    // 使用统一的 ViewModelFactory 创建 ViewModel
    val weatherViewModel: WeatherViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val placeViewModel: PlaceViewModel = viewModel(factory = AppModule.appViewModelFactory)
    val settingsViewModel: SettingsViewModel = viewModel(factory = AppModule.appViewModelFactory)

    NavDisplay(
        backStack = backStack,
        onBack = { backStack.removeLastOrNull() },
        entryProvider = { key ->
            when (key) {
                is WeatherKey -> NavEntry(key) {
                    selectedPlace?.let { place ->
                        WeatherScreen(
                            viewModel = weatherViewModel,
                            placeName = place.name,
                            lng = place.location.lng,
                            lat = place.location.lat,
                            onNavigateToPlace = { backStack.add(PlaceKey) },
                            onNavigateToSettings = { backStack.add(TestPageKey) }
                        )
                    } ?: UnknownScreen(
                        onOpenLocationSettings = onOpenLocationSettings,
                        onSelectCity = { backStack.add(PlaceKey) }
                    )
                }

                is PlaceKey -> NavEntry(key) {
                    PlaceScreen(
                        viewModel = placeViewModel,
                        onPlaceSelected = { place ->
                            onPlaceSelected(place)
                            backStack.clear()
                            backStack.add(WeatherKey)
                        },
                        onNavigateToTestPage = {
                            backStack.add(TestPageKey)
                            //backStack.add(DrawerExamplesKey)
                        }
                    )
                }

                is UnknownKey -> NavEntry(key) {
                    UnknownScreen(
                        onOpenLocationSettings = onOpenLocationSettings,
                        onSelectCity = { backStack.add(PlaceKey) }
                    )
                }

                is SettingsKey -> NavEntry(key) {
                    SettingsScreen(
                        viewModel = settingsViewModel,
                        onCacheCleared = {
                            onClearPlace()
                            backStack.clear()
                            backStack.add(UnknownKey)
                        },
                        onBack = { backStack.removeLastOrNull() },
                        onLanguageChanged = onLanguageChanged
                    )
                }

                is TestPageKey -> NavEntry(key) {
                    // SimpleCenterAlignedTopAppBar()
                    TestPageScreen(
                        onBack = { backStack.removeLastOrNull() }
                    )
                }

                is DrawerExamplesKey -> NavEntry(key){
                    //NavigationDrawerExamples()

                }

                else -> error("Unknown navigation key: $key")
            }
        }
    )
}
