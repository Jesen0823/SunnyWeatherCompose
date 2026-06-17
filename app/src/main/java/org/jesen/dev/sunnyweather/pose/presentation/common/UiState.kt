/**
 * UI 状态密封类
 * 
 * 主要职责：
 * - 定义通用的 UI 状态，用于表示数据加载的不同阶段
 * - 统一处理 Loading、Success、Error 三种状态
 * 
 * 技术要点：
 * - 使用密封类（sealed class）表示有限的状态集合
 * - 泛型支持，可封装任意类型的数据
 * - Loading 表示数据加载中，Success 表示加载成功并携带数据，Error 表示加载失败并携带错误信息
 */
package org.jesen.dev.sunnyweather.pose.presentation.common

sealed class UiState<out T> {
    object Loading : UiState<Nothing>()
    data class Success<out T>(val data: T) : UiState<T>()
    data class Error(val message: String) : UiState<Nothing>()
}
