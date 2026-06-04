package org.jesen.dev.sunnyweather.pose

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Scaffold
import androidx.compose.ui.Modifier
import org.jesen.dev.sunnyweather.pose.navigation_demo.Navigation3Demo
import org.jesen.dev.sunnyweather.pose.ui.theme.SunnyWeatherComposeTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            SunnyWeatherComposeTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    // 临时Demo页面,可注释替换
                    Navigation3Demo(
                        modifier = Modifier.padding(innerPadding)
                    )
                }
            }
        }
    }
}

