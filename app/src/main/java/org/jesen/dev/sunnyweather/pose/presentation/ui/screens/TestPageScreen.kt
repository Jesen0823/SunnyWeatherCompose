package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowBack
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.AppNavigation
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.Screen

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TestPageScreen(
    onBack: () -> Unit
) {
    var currentScreen by remember { mutableStateOf<Screen>(Screen.Weather) }

    AppNavigation(
        currentScreen = currentScreen,
        onScreenChange = { screen: Screen -> currentScreen = screen },
        content = {
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
