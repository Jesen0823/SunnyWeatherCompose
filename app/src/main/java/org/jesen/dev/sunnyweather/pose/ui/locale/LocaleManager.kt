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
