/**
 * 获取保存城市列表用例
 * 
 * 主要职责：
 * - 获取本地保存的所有城市列表
 * - 支持多城市管理，用于天气页面切换城市
 * 
 * 技术要点：
 * - 使用 operator fun invoke() 简化调用，可直接作为函数使用
 * - 返回 Flow<List<Place>>，支持响应式观察城市列表变化
 * - 城市列表按添加时间排序，最新添加的城市在首位
 */
package org.jesen.dev.sunnyweather.pose.domain.usecase

import kotlinx.coroutines.flow.Flow
import org.jesen.dev.sunnyweather.pose.domain.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.domain.model.Place

class GetSavedPlaceListUseCase(private val repository: WeatherRepository) {
    operator fun invoke(): Flow<List<Place>> {
        return repository.getSavedPlaceList()
    }
}
