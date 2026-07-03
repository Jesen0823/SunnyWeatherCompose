/**
 * 搜索城市用例
 * 
 * 主要职责：
 * - 根据关键词搜索城市
 * - 返回匹配的城市列表供用户选择
 * 
 * 技术要点：
 * - 使用 operator fun invoke() 简化调用，可直接作为函数使用
 * - 返回 ApiResult<PlaceResponse>，统一处理成功/失败/加载状态
 * - 是一个 suspend 函数，需要在协程中调用
 */
package org.jesen.dev.sunnyweather.pose.domain.usecase

import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.domain.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.domain.model.PlaceResponse

class SearchPlacesUseCase(private val repository: WeatherRepository) {
    suspend operator fun invoke(query: String): ApiResult<PlaceResponse> {
        return repository.searchPlaces(query)
    }
}
