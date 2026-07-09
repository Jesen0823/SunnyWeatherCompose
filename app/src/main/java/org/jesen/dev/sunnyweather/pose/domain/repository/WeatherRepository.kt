/**
 * 天气仓库接口
 * 
 * 主要职责：
 * - 定义天气数据访问的抽象接口
 * - 作为数据层与领域层的契约
 * - 支持依赖倒置，便于测试和替换实现
 * 
 * 技术要点：
 * - 使用接口定义数据访问契约
 * - 所有方法返回 ApiResult 或 Flow，支持响应式编程
 * - 包含天气数据获取和城市存储两大功能模块
 * - 实现类可切换真实网络/模拟数据/缓存等不同数据源
 */
package org.jesen.dev.sunnyweather.pose.domain.repository

import kotlinx.coroutines.flow.Flow
import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.domain.model.DailyResponse
import org.jesen.dev.sunnyweather.pose.domain.model.HourlyResponse
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.model.PlaceResponse
import org.jesen.dev.sunnyweather.pose.domain.model.RealtimeResponse
import org.jesen.dev.sunnyweather.pose.domain.model.Weather

interface WeatherRepository {
    /**
     * 搜索城市
     * 
     * 调用高德地图搜索 API，根据关键词搜索城市列表
     * 
     * @param query 搜索关键词
     * @return 城市搜索结果
     */
    suspend fun searchPlaces(query: String): ApiResult<PlaceResponse>

    /**
     * 获取实时天气数据（对应彩云 API /realtime 接口）
     * 
     * 获取当前时刻的实时天气，包括温度、湿度、风速、风向、天气现象、空气质量等
     * 更新频率：1分钟，空间分辨率：1km×1km
     * 
     * 使用场景：
     * - 仅需要刷新实时天气（如下拉刷新只更新当前温度）
     * - 桌面小部件显示当前天气
     * - 实时天气卡片单独刷新
     * 
     * @param lng 经度
     * @param lat 纬度
     * @return 实时天气响应数据
     * 
     * @see getDailyWeather 获取天级别预报
     * @see fetchWeather 获取合并的完整天气数据
     */
    suspend fun getRealtimeWeather(lng: String, lat: String): ApiResult<RealtimeResponse>

    /**
     * 获取天级别预报数据（对应彩云 API /daily 接口）
     * 
     * 获取未来15天的逐日预报，包括最高/最低气温、天气现象、生活指数（穿衣、紫外线、洗车、感冒）等
     * 更新频率：2小时，空间分辨率：12km×12km
     * 
     * 使用场景：
     * - 仅需要更新未来几天的天气预报
     * - 生活指数卡片单独刷新
     * - 不需要实时天气的场景
     * 
     * @param lng 经度
     * @param lat 纬度
     * @return 天级别预报响应数据
     * 
     * @see getRealtimeWeather 获取实时天气
     * @see fetchWeather 获取合并的完整天气数据
     */
    suspend fun getDailyWeather(lng: String, lat: String): ApiResult<DailyResponse>

    /**
     * 获取小时级预报数据（对应彩云 API /hourly 接口）
     * 
     * 获取未来24小时的逐小时预报，包括温度、降水、风速等
     * 更新频率：近时效5-15分钟，其余1小时级
     * 
     * 使用场景：
     * - 查看未来24小时天气变化趋势
     * - 制作温度趋势图
     * - 判断何时下雨、气温变化
     * 
     * @param lng 经度
     * @param lat 纬度
     * @return 小时级预报响应数据
     * 
     * @see getRealtimeWeather 获取实时天气
     * @see getDailyWeather 获取天级别预报
     */
    suspend fun getHourlyWeather(lng: String, lat: String): ApiResult<HourlyResponse>

    /**
     * 获取完整天气数据（组合方法）
     * 
     * 内部同时调用 getRealtimeWeather 和 getDailyWeather，合并返回统一的 Weather 对象
     * 带有内存缓存（5分钟有效期），避免重复请求
     * 
     * 使用场景：
     * - 页面初始化时加载完整天气信息
     * - 切换城市后加载所有天气数据
     * - 需要同时显示实时天气和预报的场景
     * 
     * 性能优化：
     * - 优先检查内存缓存，命中则直接返回
     * - 未命中则发起网络请求，成功后存入缓存
     * 
     * @param lng 经度
     * @param lat 纬度
     * @return 合并后的完整天气数据
     * 
     * @see getRealtimeWeather 获取单独的实时天气
     * @see getDailyWeather 获取单独的天级别预报
     */
    suspend fun fetchWeather(lng: String, lat: String): ApiResult<Weather>
    suspend fun savePlace(place: Place)
    fun getSavedPlace(): Flow<Place?>
    fun getSavedPlaceList(): Flow<List<Place>>
    fun isPlaceSaved(): Flow<Boolean>
    fun isSpecificPlaceSaved(placeName: String): Flow<Boolean>
    suspend fun clearPlace()
    suspend fun savePlaceList(places: List<Place>)
}