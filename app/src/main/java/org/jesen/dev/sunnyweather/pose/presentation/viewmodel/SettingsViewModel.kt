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
