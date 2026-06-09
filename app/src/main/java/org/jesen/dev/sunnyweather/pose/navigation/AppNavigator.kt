package org.jesen.dev.sunnyweather.pose.navigation

import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.navigation3.runtime.NavEntry
import androidx.navigation3.runtime.NavKey
import androidx.navigation3.runtime.rememberNavBackStack
import androidx.navigation3.ui.NavDisplay
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.PlaceScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.SettingsScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.UnknownScreen
import org.jesen.dev.sunnyweather.pose.presentation.ui.screens.WeatherScreen
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel
import androidx.lifecycle.viewmodel.compose.viewModel

@Composable
fun AppNavigator(
    initialKey: NavKey,
    selectedPlace: Place?,
    onOpenLocationSettings: () -> Unit,
    onPlaceSelected: (Place) -> Unit,
    onClearPlace: () -> Unit
) {
    val backStack = rememberNavBackStack(initialKey)
    val viewModel: WeatherViewModel = viewModel()

    NavDisplay(
        backStack = backStack,
        onBack = { backStack.removeLastOrNull() },
        entryProvider = { key ->
            when (key) {
                is WeatherKey -> NavEntry(key) {
                    selectedPlace?.let { place ->
                        WeatherScreen(
                            viewModel = viewModel,
                            placeName = place.name,
                            lng = place.location.lng,
                            lat = place.location.lat,
                            onNavigateToPlace = { backStack.add(PlaceKey) },
                            onNavigateToSettings = { backStack.add(SettingsKey) }
                        )
                    } ?: UnknownScreen(
                        onOpenLocationSettings = onOpenLocationSettings,
                        onSelectCity = { backStack.add(PlaceKey) }
                    )
                }

                is PlaceKey -> NavEntry(key) {
                    PlaceScreen(
                        viewModel = viewModel,
                        onPlaceSelected = { place ->
                            onPlaceSelected(place)
                            backStack.clear()
                            backStack.add(WeatherKey)
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
                        onClearPlace = {
                            onClearPlace()
                            backStack.clear()
                            backStack.add(UnknownKey)
                        },
                        onBack = { backStack.removeLastOrNull() }
                    )
                }

                else -> error("Unknown navigation key: $key")
            }
        }
    )
}