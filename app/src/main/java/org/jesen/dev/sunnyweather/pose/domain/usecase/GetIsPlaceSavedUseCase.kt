/**
 * 检查是否有保存城市用例
 * 
 * 主要职责：
 * - 检查本地是否保存了城市数据
 * - 用于应用启动时判断是否需要进入城市选择页面
 * 
 * 技术要点：
 * - 使用 operator fun invoke() 简化调用，可直接作为函数使用
 * - 返回 Flow<Boolean>，支持响应式观察城市保存状态
 * - true 表示有保存的城市，false 表示没有
 */
package org.jesen.dev.sunnyweather.pose.domain.usecase

import kotlinx.coroutines.flow.Flow
import org.jesen.dev.sunnyweather.pose.domain.repository.WeatherRepository

class GetIsPlaceSavedUseCase(private val repository: WeatherRepository) {
    operator fun invoke(): Flow<Boolean> {
        return repository.isPlaceSaved()
    }
}
