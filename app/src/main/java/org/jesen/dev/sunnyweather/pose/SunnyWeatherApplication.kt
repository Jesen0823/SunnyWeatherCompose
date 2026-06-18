package org.jesen.dev.sunnyweather.pose

import android.app.Application

class SunnyWeatherApplication : Application() {
    override fun onCreate() {
        super.onCreate()
        AppModule.init(this)
    }
}