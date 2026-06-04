package org.jesen.dev.sunnyweather.pose.presentation.ui.components

import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.Map
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material.icons.Icons
import androidx.compose.material3.DrawerValue
import androidx.compose.material3.Icon
import androidx.compose.material3.ModalNavigationDrawer
import androidx.compose.material3.NavigationDrawerItem
import androidx.compose.material3.NavigationDrawerItemDefaults
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.rememberDrawerState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Modifier
import kotlinx.coroutines.launch

sealed class Screen(val route: String, val label: String) {
    object Weather : Screen("weather", "天气")
    object Place : Screen("place", "城市选择")
    object Settings : Screen("settings", "设置")
}

@Composable
fun AppNavigation(
    currentScreen: Screen,
    onScreenChange: (Screen) -> Unit,
    content: @Composable () -> Unit
) {
    val drawerState = rememberDrawerState(initialValue = DrawerValue.Closed)
    val scope = rememberCoroutineScope()

    ModalNavigationDrawer(
        drawerState = drawerState,
        drawerContent = {
            NavigationDrawerContent(
                currentScreen = currentScreen,
                onScreenChange = { screen ->
                    onScreenChange(screen)
                    scope.launch {
                        drawerState.close()
                    }
                }
            )
        },
        content = {
            Scaffold(
                modifier = Modifier.fillMaxSize(),
                topBar = {
                    TopAppBarWithDrawer(
                        title = currentScreen.label,
                        onMenuClick = {
                            scope.launch {
                                drawerState.open()
                            }
                        }
                    )
                }
            ) { innerPadding ->
                content()
            }
        }
    )
}

@Composable
fun NavigationDrawerContent(
    currentScreen: Screen,
    onScreenChange: (Screen) -> Unit
) {
    NavigationDrawerItem(
        icon = { Icon(Icons.Default.Home, contentDescription = Screen.Weather.label) },
        label = { Text(Screen.Weather.label) },
        selected = currentScreen == Screen.Weather,
        onClick = { onScreenChange(Screen.Weather) },
        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
    )
    NavigationDrawerItem(
        icon = { Icon(Icons.Default.Map, contentDescription = Screen.Place.label) },
        label = { Text(Screen.Place.label) },
        selected = currentScreen == Screen.Place,
        onClick = { onScreenChange(Screen.Place) },
        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
    )
    NavigationDrawerItem(
        icon = { Icon(Icons.Default.Settings, contentDescription = Screen.Settings.label) },
        label = { Text(Screen.Settings.label) },
        selected = currentScreen == Screen.Settings,
        onClick = { onScreenChange(Screen.Settings) },
        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
    )
}