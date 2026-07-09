/**
 * 检查特定城市是否已保存用例
 * 
 * 主要职责：
 * - 根据城市名称检查该城市是否已保存在本地
 * - 用于城市搜索页面判断城市是否已收藏
 * - 支持去重保存，避免重复添加相同城市
 * 
 * 技术要点：
 * - 使用 operator fun invoke() 简化调用，可直接作为函数使用
 * - 返回 Flow<Boolean>，支持响应式观察保存状态变化
 * - true 表示该城市已保存，false 表示未保存
 */
package org.jesen.dev.sunnyweather.pose.domain.usecase

import kotlinx.coroutines.flow.Flow
import org.jesen.dev.sunnyweather.pose.domain.repository.WeatherRepository

class IsSpecificPlaceSavedUseCase(private val repository: WeatherRepository) {
    operator fun invoke(placeName: String): Flow<Boolean> {
        return repository.isSpecificPlaceSaved(placeName)
    }
}