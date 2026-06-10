package org.jesen.dev.sunnyweather.relationdemo.appbar_demo

import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.runtime.*
import org.jesen.dev.sunnyweather.relationdemo.modaldrawer_demo.AppNavigation
import org.jesen.dev.sunnyweather.relationdemo.modaldrawer_demo.Screen

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TestDrawerScreen() {
    var currentScreen by remember { mutableStateOf<Screen>(Screen.Weather) }

    AppNavigation(
        currentScreen = currentScreen,
        onScreenChange = { screen: Screen -> currentScreen = screen },
        restContent = {
            SimpleCenterAlignedTopAppBar()
        }
    )
}
