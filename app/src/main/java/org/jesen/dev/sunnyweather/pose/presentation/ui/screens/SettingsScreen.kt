/**
 * 设置页面组件
 * 
 * 主要职责：
 * - 提供清除缓存、主题切换、语言切换功能
 * - 显示应用版本信息
 * 
 * 技术要点：
 * - 使用 Scaffold 提供标准的页面结构（TopAppBar）
 * - 使用 LazyColumn 实现设置列表，并正确消费 contentPadding 避免内容被顶栏遮挡
 * - 通过 DropdownMenu 实现主题和语言选择菜单
 * - 使用 AppModule.themeManager 和 AppModule.localeManager 管理主题和语言
 * - 缓存清除后触发 onCacheCleared 回调并重置状态
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import android.content.Context
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.NavigateBefore
import androidx.compose.material.icons.filled.Delete
import androidx.compose.material.icons.filled.Language
import androidx.compose.material.icons.filled.Palette
import androidx.compose.material3.CenterAlignedTopAppBar
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.ListItem
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.R
import org.jesen.dev.sunnyweather.pose.di.AppModule
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.SettingsViewModel
import org.jesen.dev.sunnyweather.pose.ui.locale.LocaleManager
import org.jesen.dev.sunnyweather.pose.ui.theme.AppTheme

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SettingsScreen(
    viewModel: SettingsViewModel,
    onCacheCleared: () -> Unit = {},
    onBack: () -> Unit = {},
    onLanguageChanged: () -> Unit = {}
) {
    val cacheCleared = viewModel.cacheCleared.collectAsState()
    val currentTheme by AppModule.themeManager.currentTheme.collectAsState()
    val context = LocalContext.current
    
    var showThemeMenu by remember { mutableStateOf(false) }
    var showLanguageMenu by remember { mutableStateOf(false) }
    
    if (cacheCleared.value) {
        onCacheCleared()
        viewModel.resetCacheCleared()
    }
    
    Scaffold(
        topBar = {
            CenterAlignedTopAppBar(
                title = {
                    Text(
                        text = stringResource(id = R.string.settings_title),
                        style = MaterialTheme.typography.titleLarge
                    )
                },
                navigationIcon = {
                    IconButton(onClick = onBack) {
                        Icon(
                            imageVector = Icons.AutoMirrored.Filled.NavigateBefore,
                            contentDescription = "返回"
                        )
                    }
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.surfaceContainer
                )
            )
        }
    ) { paddingValues ->
        LazyColumn(
            modifier = Modifier.fillMaxSize().padding(paddingValues),
            contentPadding = PaddingValues(vertical = 8.dp)
        ) {
        item {
            Surface(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(horizontal = 16.dp, vertical = 4.dp),
                onClick = { viewModel.clearPlace() }
            ) {
                ListItem(
                    headlineContent = {
                        Text(
                            text = stringResource(id = R.string.settings_clear_cache),
                            style = MaterialTheme.typography.bodyLarge
                        )
                    },
                    trailingContent = {
                        Icon(
                            imageVector = Icons.Default.Delete,
                            contentDescription = stringResource(id = R.string.clear)
                        )
                    }
                )
            }
        }
        
        item {
            Surface(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(horizontal = 16.dp, vertical = 4.dp),
                onClick = { showThemeMenu = !showThemeMenu }
            ) {
                ListItem(
                    headlineContent = {
                        Text(
                            text = stringResource(id = R.string.settings_theme),
                            style = MaterialTheme.typography.bodyLarge
                        )
                    },
                    supportingContent = {
                        Text(text = getThemeDisplayName(currentTheme))
                    },
                    trailingContent = {
                        Icon(
                            imageVector = Icons.Default.Palette,
                            contentDescription = stringResource(id = R.string.settings_theme)
                        )
                    }
                )
            }
            
            DropdownMenu(
                expanded = showThemeMenu,
                onDismissRequest = { showThemeMenu = false }
            ) {
                AppTheme.values().forEach { theme ->
                    DropdownMenuItem(
                        text = { Text(text = getThemeDisplayName(theme)) },
                        onClick = {
                            AppModule.themeManager.setTheme(theme)
                            showThemeMenu = false
                        }
                    )
                }
            }
        }
        
        item {
            Surface(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(horizontal = 16.dp, vertical = 4.dp),
                onClick = { showLanguageMenu = !showLanguageMenu }
            ) {
                ListItem(
                    headlineContent = {
                        Text(
                            text = stringResource(id = R.string.settings_language),
                            style = MaterialTheme.typography.bodyLarge
                        )
                    },
                    supportingContent = {
                        Text(text = getLanguageDisplayName(context))
                    },
                    trailingContent = {
                        Icon(
                            imageVector = Icons.Default.Language,
                            contentDescription = stringResource(id = R.string.settings_language)
                        )
                    }
                )
            }
            
            DropdownMenu(
                expanded = showLanguageMenu,
                onDismissRequest = { showLanguageMenu = false }
            ) {
                DropdownMenuItem(
                    text = { Text(text = "中文") },
                    onClick = {
                        AppModule.localeManager.setLanguage(LocaleManager.LANGUAGE_CHINESE)
                        showLanguageMenu = false
                        onLanguageChanged()
                    }
                )
                DropdownMenuItem(
                    text = { Text(text = "English") },
                    onClick = {
                        AppModule.localeManager.setLanguage(LocaleManager.LANGUAGE_ENGLISH)
                        showLanguageMenu = false
                        onLanguageChanged()
                    }
                )
            }
        }
        
        item {
            ListItem(
                headlineContent = {
                    Text(
                        text = stringResource(id = R.string.settings_about),
                        style = MaterialTheme.typography.bodyLarge
                    )
                },
                modifier = Modifier.padding(horizontal = 16.dp)
            )
        }
        
        item {
            ListItem(
                headlineContent = {
                    Text(
                        text = stringResource(id = R.string.settings_version),
                        style = MaterialTheme.typography.bodyLarge
                    )
                },
                supportingContent = {
                    Text(text = "1.0.0")
                },
                modifier = Modifier.padding(horizontal = 16.dp)
            )
        }
        }
    }
}

private fun getThemeDisplayName(theme: AppTheme): String {
    return when (theme) {
        AppTheme.LIGHT -> "亮色模式"
        AppTheme.DARK -> "暗色模式"
        AppTheme.SYSTEM -> "跟随系统"
    }
}

private fun getLanguageDisplayName(context: Context): String {
    return when (AppModule.localeManager.getCurrentLanguage()) {
        LocaleManager.LANGUAGE_CHINESE -> "中文"
        LocaleManager.LANGUAGE_ENGLISH -> "English"
        else -> "中文"
    }
}
