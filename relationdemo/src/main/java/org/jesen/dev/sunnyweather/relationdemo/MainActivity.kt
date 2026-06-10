package org.jesen.dev.sunnyweather.relationdemo

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import org.jesen.dev.sunnyweather.relationdemo.appbar_demo.TestDrawerScreen
import org.jesen.dev.sunnyweather.relationdemo.modaldrawer_demo.TestPageScreen
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.Navigation3Demo
import org.jesen.dev.sunnyweather.relationdemo.ui.theme.SunnyWeatherComposeTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            SunnyWeatherComposeTheme {
                TestDrawerScreen()
            }
        }
    }
}