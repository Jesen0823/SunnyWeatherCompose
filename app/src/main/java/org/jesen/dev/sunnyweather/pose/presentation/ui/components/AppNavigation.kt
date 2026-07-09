package org.jesen.dev.sunnyweather.pose.presentation.ui.components

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.BusinessCenter
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.Map
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.launch

/**
 * 应用导航组件
 *
 * 主要职责：
 * - 提供侧边栏导航抽屉功能
 * - 支持天气、城市选择、设置三个页面的切换
 * - 提供两种导航模式（自动管理抽屉状态和外部控制抽屉状态）
 *
 * 技术要点：
 * - 使用 ModalNavigationDrawer 实现侧边栏
 * - Screen 密封类定义三个导航目标（Weather、Place、Settings）
 * - 两个重载函数分别支持自动管理和外部控制抽屉状态
 * - 使用 rememberCoroutineScope 处理抽屉打开/关闭动画
 */
sealed class Screen(val route: String, val label: String) {
    object Weather : Screen("weather", "天气")
    object Place : Screen("place", "地址搜索")
    object PlaceManager : Screen("placeManager", "地址管理")
    object Settings : Screen("settings", "设置")
}

/**
 * 自动管理抽屉状态的导航组件
 *
 * 主要职责：
 * - 自动管理抽屉的打开和关闭状态
 * - 提供侧边栏导航功能
 * - 支持三个页面的切换
 */
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
            ModalDrawerSheet(
                modifier = Modifier.fillMaxSize(0.85f)
            ) {
                Column(
                    modifier = Modifier
                        .padding(horizontal = 16.dp)
                        .verticalScroll(rememberScrollState())
                ) {
                    Spacer(Modifier.height(12.dp))
                    
                    Text(
                        text = "Sunny Weather",
                        modifier = Modifier.padding(16.dp),
                        style = MaterialTheme.typography.titleLarge,
                        fontWeight = FontWeight.Bold
                    )
                    
                    HorizontalDivider()
                    
                    Spacer(Modifier.height(12.dp))
                    
                    NavigationDrawerItem(
                        icon = { Icon(Icons.Default.Home, contentDescription = null) },
                        label = { Text(Screen.Weather.label) },
                        selected = currentScreen == Screen.Weather,
                        onClick = {
                            onScreenChange(Screen.Weather)
                            scope.launch { drawerState.close() }
                        },
                        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
                    )
                    NavigationDrawerItem(
                        icon = { Icon(Icons.Default.Map, contentDescription = null) },
                        label = { Text(Screen.Place.label) },
                        selected = currentScreen == Screen.Place,
                        onClick = {
                            onScreenChange(Screen.Place)
                            scope.launch { drawerState.close() }
                        },
                        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
                    )
                    NavigationDrawerItem(
                        icon = { Icon(Icons.Default.BusinessCenter, contentDescription = null) },
                        label = { Text(Screen.PlaceManager.label) },
                        selected = currentScreen == Screen.PlaceManager,
                        onClick = {
                            onScreenChange(Screen.PlaceManager)
                            scope.launch { drawerState.close() }
                        },
                        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
                    )
                    NavigationDrawerItem(
                        icon = { Icon(Icons.Default.Settings, contentDescription = null) },
                        label = { Text(Screen.Settings.label) },
                        selected = currentScreen == Screen.Settings,
                        onClick = {
                            onScreenChange(Screen.Settings)
                            scope.launch { drawerState.close() }
                        },
                        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
                    )
                    
                    Spacer(Modifier.height(12.dp))
                }
            }
        },
        content = {
            content()
        }
    )
}

/**
 * 外部控制抽屉状态的导航组件
 *
 * 主要职责：
 * - 接受外部传入的抽屉状态（isDrawerOpen）
 * - 根据外部状态控制抽屉的打开和关闭
 * - 提供侧边栏导航功能
 * - 支持三个页面的切换
 */
@Composable
fun AppNavigation(
    isDrawerOpen: Boolean,
    onDrawerClose: () -> Unit,
    currentScreen: Screen,
    onScreenChange: (Screen) -> Unit,
    content: @Composable () -> Unit
) {
    val targetValue = if (isDrawerOpen) DrawerValue.Open else DrawerValue.Closed
    val drawerState = rememberDrawerState(initialValue = targetValue)
    
    LaunchedEffect(isDrawerOpen) {
        if (isDrawerOpen) {
            drawerState.open()
        } else {
            drawerState.close()
        }
    }

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
                    
                    Text(
                        text = "Sunny Weather",
                        modifier = Modifier.padding(16.dp),
                        style = MaterialTheme.typography.titleLarge,
                        fontWeight = FontWeight.Bold
                    )
                    
                    HorizontalDivider()
                    
                    Spacer(Modifier.height(12.dp))
                    
                    NavigationDrawerItem(
                        icon = { Icon(Icons.Default.Home, contentDescription = null) },
                        label = { Text(Screen.Weather.label) },
                        selected = currentScreen == Screen.Weather,
                        onClick = {
                            onScreenChange(Screen.Weather)
                            onDrawerClose()
                        },
                        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
                    )
                    NavigationDrawerItem(
                        icon = { Icon(Icons.Default.Map, contentDescription = null) },
                        label = { Text(Screen.Place.label) },
                        selected = currentScreen == Screen.Place,
                        onClick = {
                            onScreenChange(Screen.Place)
                            onDrawerClose()
                        },
                        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
                    )
                    NavigationDrawerItem(
                        icon = { Icon(Icons.Default.BusinessCenter, contentDescription = null) },
                        label = { Text(Screen.PlaceManager.label) },
                        selected = currentScreen == Screen.PlaceManager,
                        onClick = {
                            onScreenChange(Screen.PlaceManager)
                            onDrawerClose()
                        },
                        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
                    )
                    NavigationDrawerItem(
                        icon = { Icon(Icons.Default.Settings, contentDescription = null) },
                        label = { Text(Screen.Settings.label) },
                        selected = currentScreen == Screen.Settings,
                        onClick = {
                            onScreenChange(Screen.Settings)
                            onDrawerClose()
                        },
                        modifier = Modifier.padding(NavigationDrawerItemDefaults.ItemPadding)
                    )
                    
                    Spacer(Modifier.height(12.dp))
                }
            }
        },
        content = {
            content()
        }
    )
}
