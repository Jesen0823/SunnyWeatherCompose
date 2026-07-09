/**
 * 批量保存城市列表用例
 * 
 * 主要职责：
 * - 将多个城市一次性保存到本地存储
 * - 用于城市管理页面的批量操作（如排序后保存）
 * - 覆盖原有城市列表，保持数据一致性
 * 
 * 技术要点：
 * - 使用 operator fun invoke() 简化调用，可直接作为函数使用
 * - 调用 WeatherRepository 的 savePlaceList() 方法执行批量保存
 * - 是一个 suspend 函数，需要在协程中调用
 * - 新列表会完全替换原有列表，适合批量更新场景
 */
package org.jesen.dev.sunnyweather.pose.domain.usecase

import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.repository.WeatherRepository

class SavePlaceListUseCase(
    private val repository: WeatherRepository
) {
    suspend operator fun invoke(places: List<Place>) {
        repository.savePlaceList(places)
    }
}