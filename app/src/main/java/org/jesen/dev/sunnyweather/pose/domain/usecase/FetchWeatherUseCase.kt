package org.jesen.dev.sunnyweather.pose.domain.usecase

import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.domain.model.Weather

/**
 * UseCase（用例） 是 Clean Architecture 中定义的一个核心层，位于 Domain Layer（领域层） ，它代表了 用户可以执行的一个独立业务操作 。
 * 
 * - 每个 UseCase 处理一个业务操作
 * - UseCase 应该是纯 Kotlin 类 （无 Android 依赖）
 * - 使用 operator fun invoke() 简化调用
 * - UseCase 可以组合其他 UseCase
 * 
 * UseCase 的精髓 ：
 * 1. 业务逻辑的封装容器
 * 2. 可测试的业务单元
 * 3. ViewModel 和 Repository 之间的桥梁
 * 4. 复杂业务的编排者
 * 
 * 随着业务增长，可以在 UseCase 中添加更多业务逻辑，如搜索、地点存储、地点读取等。
*/
class FetchWeatherUseCase(private val repository: WeatherRepository) {
    suspend operator fun invoke(lng: String, lat: String): ApiResult<Weather> {
        return repository.fetchWeather(lng, lat)
    }
}
