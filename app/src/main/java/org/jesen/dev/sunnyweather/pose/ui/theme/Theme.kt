/**
 * 主题管理器
 * 
 * 主要职责：
 * - 管理应用的主题模式（亮色、暗色、跟随系统）
 * - 提供主题切换功能
 * - 封装 Compose 主题配置
 * 
 * 技术要点：
 * - 使用 StateFlow 管理当前主题状态
 * - AppTheme 枚举定义三种主题模式（LIGHT、DARK、SYSTEM）
 * - ThemeManager 提供 setTheme 和 isDarkTheme 方法
 * - SunnyWeatherComposeTheme 根据主题模式配置 MaterialTheme
 * - 支持动态颜色（Android 12+）
 */
package org.jesen.dev.sunnyweather.pose.ui.theme

import android.app.Activity
import android.os.Build
import androidx.compose.foundation.isSystemInDarkTheme
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.darkColorScheme
import androidx.compose.material3.dynamicDarkColorScheme
import androidx.compose.material3.dynamicLightColorScheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.CompositionLocalProvider
import androidx.compose.runtime.ReadOnlyComposable
import androidx.compose.runtime.staticCompositionLocalOf
import androidx.compose.ui.platform.LocalContext
import androidx.lifecycle.compose.collectAsStateWithLifecycle
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue

enum class AppTheme {
    LIGHT,
    DARK,
    SYSTEM
}

class ThemeManager {
    private val _currentTheme = MutableStateFlow(AppTheme.SYSTEM)
    val currentTheme: StateFlow<AppTheme> = _currentTheme.asStateFlow()

    fun setTheme(theme: AppTheme) {
        _currentTheme.value = theme
    }

    @Composable
    fun isDarkTheme(): Boolean {
        return when (_currentTheme.collectAsState().value) {
            AppTheme.LIGHT -> false
            AppTheme.DARK -> true
            AppTheme.SYSTEM -> isSystemInDarkTheme()
        }
    }
}

val LocalThemeManager = staticCompositionLocalOf<ThemeManager> {
    error("ThemeManager not provided")
}

val LocalAppTheme = staticCompositionLocalOf<AppTheme> {
    AppTheme.SYSTEM
}

private val DarkColorScheme = darkColorScheme(
    primary = Purple80,
    secondary = PurpleGrey80,
    tertiary = Pink80
)

private val LightColorScheme = lightColorScheme(
    primary = Purple40,
    secondary = PurpleGrey40,
    tertiary = Pink40
)

@Composable
fun SunnyWeatherComposeTheme(
    themeManager: ThemeManager,
    dynamicColor: Boolean = true,
    content: @Composable () -> Unit
) {
    val currentTheme by themeManager.currentTheme.collectAsStateWithLifecycle()
    
    val darkTheme = when (currentTheme) {
        AppTheme.LIGHT -> false
        AppTheme.DARK -> true
        AppTheme.SYSTEM -> isSystemInDarkTheme()
    }

    val colorScheme = when {
        dynamicColor && Build.VERSION.SDK_INT >= Build.VERSION_CODES.S -> {
            val context = LocalContext.current
            if (darkTheme) dynamicDarkColorScheme(context) else dynamicLightColorScheme(context)
        }
        darkTheme -> DarkColorScheme
        else -> LightColorScheme
    }

    CompositionLocalProvider(
        LocalThemeManager provides themeManager,
        LocalAppTheme provides currentTheme
    ) {
        MaterialTheme(
            colorScheme = colorScheme,
            typography = Typography,
            content = content
        )
    }
}

