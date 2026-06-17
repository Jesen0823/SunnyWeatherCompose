/**
 * 应用主活动
 * 
 * 主要职责：
 * - 作为 Android 应用的入口 Activity
 * - 初始化依赖注入模块
 * - 注册权限请求回调
 * - 设置 Compose 内容
 * 
 * 技术要点：
 * - 调用 AppModule.init() 初始化应用依赖
 * - 注册 ActivityResultLauncher 处理定位权限请求结果
 * - 通过 setContent 设置 SunnyWeatherComposeTheme 和 SunnyWeatherApp
 * - 启用 edge-to-edge 全屏模式
 */
package org.jesen.dev.sunnyweather.pose

import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.Scaffold
import androidx.compose.ui.Modifier
import org.jesen.dev.sunnyweather.pose.di.AppModule
import org.jesen.dev.sunnyweather.pose.presentation.ui.SunnyWeatherApp
import org.jesen.dev.sunnyweather.pose.ui.theme.SunnyWeatherComposeTheme

class MainActivity : ComponentActivity() {
    private val TAG = "MainActivity"
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Log.d(TAG, "onCreate() called")
        enableEdgeToEdge()
        
        Log.d(TAG, "Initializing AppModule")
        AppModule.init(applicationContext)
        
        Log.d(TAG, "Registering permission launcher")
        val permissionLauncher = registerForActivityResult(
            ActivityResultContracts.RequestPermission()
        ) { isGranted ->
            Log.d(TAG, "Permission result received: isGranted=$isGranted")
            AppModule.onPermissionResult(isGranted)
        }
        
        Log.d(TAG, "Registering launcher to AppModule")
        AppModule.registerPermissionLauncher(permissionLauncher)
        
        Log.d(TAG, "Setting content")
        setContent {
            SunnyWeatherComposeTheme(themeManager = AppModule.themeManager) {
                SunnyWeatherApp()
            }
        }
        
        Log.d(TAG, "onCreate() finished")
    }
}
