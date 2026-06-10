package org.jesen.dev.sunnyweather.relationdemo.modaldrawer_demo

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TestPageScreen(
    onBack: () -> Unit
) {
    var currentScreen by remember { mutableStateOf<Screen>(Screen.Weather) }

    AppNavigation(
        currentScreen = currentScreen,
        onScreenChange = { screen: Screen -> currentScreen = screen },
        restContent = {
            Column(
                modifier = Modifier.fillMaxSize(),
                horizontalAlignment = Alignment.CenterHorizontally,
                verticalArrangement = Arrangement.Center
            ) {
                Text(text = "当前页面: ${currentScreen.label}")
                Text(text = "路由: ${currentScreen.route}")
            }
        }
    )
}
