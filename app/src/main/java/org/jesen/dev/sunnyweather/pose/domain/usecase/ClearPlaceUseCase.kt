/**
 * 清除保存城市用例
 * 
 * 主要职责：
 * - 清除本地存储的所有城市数据
 * - 用于用户退出登录或清除缓存场景
 * 
 * 技术要点：
 * - 使用 operator fun invoke() 简化调用，可直接作为函数使用
 * - 调用 WeatherRepository 的 clearPlace() 方法执行清除操作
 * - 是一个 suspend 函数，需要在协程中调用
 */
package org.jesen.dev.sunnyweather.pose.domain.usecase

import org.jesen.dev.sunnyweather.pose.domain.repository.WeatherRepository

class ClearPlaceUseCase(private val repository: WeatherRepository) {
    suspend operator fun invoke() {
        repository.clearPlace()
    }
}
