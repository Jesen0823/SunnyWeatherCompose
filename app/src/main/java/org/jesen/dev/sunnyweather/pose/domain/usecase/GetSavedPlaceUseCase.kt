/**
 * 获取保存城市用例
 * 
 * 主要职责：
 * - 获取用户最近保存的城市
 * - 用于应用启动时自动加载上次查看的城市天气
 * 
 * 技术要点：
 * - 使用 operator fun invoke() 简化调用，可直接作为函数使用
 * - 返回 Flow<Place?>，支持响应式观察城市变化
 * - Place? 为 null 表示没有保存的城市
 */
package org.jesen.dev.sunnyweather.pose.domain.usecase

import kotlinx.coroutines.flow.Flow
import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.domain.model.Place

class GetSavedPlaceUseCase(private val repository: WeatherRepository) {
    operator fun invoke(): Flow<Place?> {
        return repository.getSavedPlace()
    }
}
