package org.jesen.dev.sunnyweather.pose

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.Scaffold
import androidx.compose.ui.Modifier
import org.jesen.dev.sunnyweather.pose.di.AppModule
import org.jesen.dev.sunnyweather.pose.presentation.ui.SunnyWeatherApp
import org.jesen.dev.sunnyweather.pose.ui.theme.SunnyWeatherComposeTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        
        AppModule.init(applicationContext)
        
        setContent {
            SunnyWeatherComposeTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    SunnyWeatherApp()
                }
            }
        }
    }
}