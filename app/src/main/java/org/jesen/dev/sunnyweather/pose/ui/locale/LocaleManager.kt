/**
 * 语言管理器
 * 
 * 主要职责：
 * - 管理应用的语言设置
 * - 支持中文和英文切换
 * - 获取当前语言设置
 * 
 * 技术要点：
 * - 使用 Configuration.setLocale 设置应用语言
 * - 兼容 Android N (API 24) 及以上版本的多语言支持
 * - 提供 LANGUAGE_ENGLISH 和 LANGUAGE_CHINESE 常量
 * - 通过 Context.resources.configuration 获取当前语言
 */
package org.jesen.dev.sunnyweather.pose.ui.locale

import android.content.Context
import android.content.res.Configuration
import android.os.Build
import android.os.LocaleList
import java.util.Locale

class LocaleManager(private val context: Context) {
    
    companion object {
        const val LANGUAGE_ENGLISH = "en"
        const val LANGUAGE_CHINESE = "zh"
    }
    
    fun setLanguage(language: String) {
        val locale = Locale(language)
        Locale.setDefault(locale)
        
        val resources = context.resources
        val configuration = Configuration(resources.configuration)
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            configuration.setLocale(locale)
            configuration.setLocales(LocaleList(locale))
            context.createConfigurationContext(configuration)
        } else {
            @Suppress("DEPRECATION")
            configuration.setLocale(locale)
            @Suppress("DEPRECATION")
            resources.updateConfiguration(configuration, resources.displayMetrics)
        }
    }
    
    fun getCurrentLanguage(): String {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            context.resources.configuration.locales[0]?.language ?: LANGUAGE_CHINESE
        } else {
            @Suppress("DEPRECATION")
            context.resources.configuration.locale.language
        }
    }
}
