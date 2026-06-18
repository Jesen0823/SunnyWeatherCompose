package org.jesen.dev.sunnyweather.pose

import android.app.Application
import org.jesen.dev.sunnyweather.pose.di.AppModule

class SunnyWeatherApplication : Application() {
    override fun onCreate() {
        super.onCreate()
        AppModule.init(this)
    }
}