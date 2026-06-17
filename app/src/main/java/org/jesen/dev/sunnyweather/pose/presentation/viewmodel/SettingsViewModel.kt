/**
 * 设置页面 ViewModel
 * 
 * 主要职责：
 * - 管理设置页面的状态和操作
 * - 处理清除缓存操作
 * - 通知 UI 缓存清除完成状态
 * 
 * 技术要点：
 * - 使用 StateFlow 管理缓存清除状态（cacheCleared）
 * - 通过 viewModelScope 启动协程执行异步操作
 * - 调用 ClearPlaceUseCase 清除本地保存的城市数据
 * - 缓存清除后短暂设置 cacheCleared 为 true，然后自动重置
 */
package org.jesen.dev.sunnyweather.pose.presentation.viewmodel

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import org.jesen.dev.sunnyweather.pose.domain.usecase.ClearPlaceUseCase
import kotlin.time.Duration.Companion.milliseconds

class SettingsViewModel(private val clearPlaceUseCase: ClearPlaceUseCase) : ViewModel() {
    private val _cacheCleared = MutableStateFlow(false)
    val cacheCleared: StateFlow<Boolean> = _cacheCleared.asStateFlow()
    
    fun clearPlace() {
        viewModelScope.launch {
            clearPlaceUseCase()
            Log.d("SettingsViewModel", "Place cache cleared")
            _cacheCleared.value = true
            delay(100.milliseconds)
            _cacheCleared.value = false
        }
    }
    
    fun resetCacheCleared() {
        _cacheCleared.value = false
    }
}
