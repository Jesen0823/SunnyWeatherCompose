package org.jesen.dev.sunnyweather.relationdemo.modaldrawer_demo

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.Map
import androidx.compose.material.icons.filled.Menu
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material3.DrawerValue
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.HorizontalDivider
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalDrawerSheet
import androidx.compose.material3.ModalNavigationDrawer
import androidx.compose.material3.NavigationDrawerItem
import androidx.compose.material3.NavigationDrawerItemDefaults
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBar
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.material3.rememberDrawerState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.launch

sealed class Screen(val route: String, val label: String) {
    object Weather : Screen("weather", "天气")
    object Place : Screen("place", "城市选择")
    object Settings : Screen("settings", "设置")
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun AppNavigation(
    currentScreen: Screen,
    onScreenChange: (Screen) -> Unit,
    restContent: @Composable () -> Unit
) {
    val drawerState = rememberDrawerState(initialValue = DrawerValue.Closed)
    val scope = rememberCoroutineScope()

    ModalNavigationDrawer(
        drawerState = drawerState,
        drawerContent = {
            ModalDrawerSheet(
                modifier = Modifier.fillMaxSize(0.85f)
            ) {
                Column(
                    modifier = Modifier
                        .padding(horizontal = 16.dp)
                        .verticalScroll(rememberScrollState())
                ) {
                    Spacer(Modifier.height(12.dp))
                    
                    // 抽屉标题
                    Text(
                        text = "Sunny Weather",
                        modifier = Modifier.padding(16.dp),
                        style = MaterialTheme.typography.titleLarge,
                        fontWeight = FontWeight.Bold
                    )
                    
                    HorizontalDivider()
                    
                    Spacer(Modifier.height(12.dp))
                    
                    // 导航项
                    NavigationDrawerContent(
                        currentScreen = currentScreen,
                        onScreenChange = { screen ->
                            onScreenChange(screen)
                            scope.launch {
                                drawerState.close()
                            }
                        }
                    )
                    
                    Spacer(Modifier.height(12.dp))
                }
            }
        },
        content = {
            Scaffold(
                modifier = Modifier.fillMaxSize(),
                topBar = {
                    TopAppBar(
                        title = {
                            Text(
                                text = currentScreen.label,
                                fontWeight = FontWeight.Medium
                            )
                        },
                        navigationIcon = {
                            IconButton(
                                onClick = {
                                    scope.launch {
                                        if (drawerState.isClosed) {
                                            drawerState.open()
                                        } else {
                                            drawerState.close()
                                        }
                                    }
                                }
                            ) {
                                Icon(
                                    imageVector = Icons.Default.Menu,
                                    contentDescription = "菜单"
                                )
                            }
                        },
                        colors = TopAppBarDefaults.topAppBarColors(
                            containerColor = Color.Transparent
                        )
                    )
                }
            ) { innerPadding ->
                Box(
                    modifier = Modifier
                        .fillMaxSize()
                        .padding(innerPadding)
                ) {
                    restContent()
                }
            }
        }
    )
}

@Composable
private fun NavigationDrawerContent(
    currentScreen: Screen,
    onScreenChange: (Screen) -> Unit
) {
    Text("Section 1", modifier = Modifier.padding(16.dp), style = MaterialTheme.typography.titleMedium)
    NavigationDrawerItem(
        icon = { Icon(Icons.Default.Home, contentDescription = null) },
        label = { Text(Screen.Weather.label) },
        selected = currentScreen == Screen.Weather,
        onClick = { onScreenChange(Screen.Weather) },
        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
    )
    NavigationDrawerItem(
        icon = { Icon(Icons.Default.Map, contentDescription = null) },
        label = { Text(Screen.Place.label) },
        selected = currentScreen == Screen.Place,
        onClick = { onScreenChange(Screen.Place) },
        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
    )
    HorizontalDivider(modifier = Modifier.padding(vertical = 8.dp))

    Text("其他操作", modifier = Modifier.padding(16.dp), style = MaterialTheme.typography.titleMedium)
    NavigationDrawerItem(
        icon = { Icon(Icons.Default.Settings, contentDescription = null) },
        label = { Text(Screen.Settings.label) },
        selected = currentScreen == Screen.Settings,
        onClick = { onScreenChange(Screen.Settings) },
        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
    )
    Spacer(Modifier.height(12.dp))
}
