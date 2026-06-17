/**
 * 保存城市用例
 * 
 * 主要职责：
 * - 将用户选择的城市保存到本地存储
 * - 支持城市去重，相同城市不会重复保存
 * 
 * 技术要点：
 * - 使用 operator fun invoke() 简化调用，可直接作为函数使用
 * - 调用 WeatherRepository 的 savePlace() 方法执行保存
 * - 是一个 suspend 函数，需要在协程中调用
 * - 新保存的城市自动放在列表首位
 */
package org.jesen.dev.sunnyweather.pose.domain.usecase

import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.domain.model.Place

class SavePlaceUseCase(private val repository: WeatherRepository) {
    suspend operator fun invoke(place: Place) {
        repository.savePlace(place)
    }
}
