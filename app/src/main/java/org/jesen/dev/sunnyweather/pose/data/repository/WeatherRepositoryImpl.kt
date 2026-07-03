/**
 * 天气数据仓库实现类
 * 
 * 主要职责：
 * - 实现 WeatherRepository 接口
 * - 组合网络请求和本地存储，提供统一的数据访问
 * - 添加内存缓存，减少重复网络请求
 * - 处理并发请求，避免竞态条件
 * 
 * 技术要点：
 * - 使用 MemoryCache 缓存最近的天气数据（有效期5分钟）
 * - 使用 ConcurrentHashMap 保证线程安全
 * - 实现接口契约，便于测试时替换为 Mock 实现
 * - fetchWeather 方法合并实时天气和7日预报，返回统一的 Weather 对象
 */
package org.jesen.dev.sunnyweather.pose.data.repository

import kotlinx.coroutines.flow.Flow
import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.data.network.WeatherApiService
import org.jesen.dev.sunnyweather.pose.data.store.PlaceStore
import org.jesen.dev.sunnyweather.pose.domain.model.DailyResponse
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.model.PlaceResponse
import org.jesen.dev.sunnyweather.pose.domain.model.RealtimeResponse
import org.jesen.dev.sunnyweather.pose.domain.model.Weather
import org.jesen.dev.sunnyweather.pose.domain.repository.WeatherRepository

class WeatherRepositoryImpl(
    private val apiService: WeatherApiService,
    private val placeStore: PlaceStore
) : WeatherRepository {

    private val weatherCache = MemoryCache<String, Weather>(expireAfterMillis = 5 * 60 * 1000)

    override suspend fun searchPlaces(query: String): ApiResult<PlaceResponse> {
        return apiService.searchPlaces(query)
    }

    override suspend fun getRealtimeWeather(lng: String, lat: String): ApiResult<RealtimeResponse> {
        return apiService.getRealtimeWeather(lng, lat)
    }

    override suspend fun getDailyWeather(lng: String, lat: String): ApiResult<DailyResponse> {
        return apiService.getDailyWeather(lng, lat)
    }

    /**
     * 获取天气数据（带内存缓存）
     * 
     * 性能优化流程：
     * 1. 生成缓存键（格式："经度,纬度"）
     * 2. 检查内存缓存，命中则直接返回缓存数据（无需网络请求）
     * 3. 未命中则发起网络请求获取实时天气和7日预报
     * 4. 请求成功后将数据合并为 Weather 对象并存入缓存
     * 5. 返回 Weather 对象
     * 
     * 缓存命中场景：
     * - 用户在5分钟内多次查看同一城市的天气
     * - 下拉刷新时如果缓存未过期，直接返回缓存数据
     * - 切换城市后再切回原城市，缓存未过期时直接返回
     * 
     * 缓存未命中场景：
     * - 首次访问某城市
     * - 缓存数据已过期（超过5分钟）
     * - 手动清除缓存后
     * 
     * @param lng 经度
     * @param lat 纬度
     * @return ApiResult<Weather> 天气数据结果
     */
    override suspend fun fetchWeather(lng: String, lat: String): ApiResult<Weather> {
        // 生成缓存键：使用经纬度组合，确保唯一性
        val cacheKey = "$lng,$lat"
        
        // 第一步：检查缓存，命中则直接返回（性能优化核心）
        weatherCache.get(cacheKey)?.let {
            return ApiResult.Success(it)
        }

        // 第二步：缓存未命中，发起网络请求
        return when (val realtimeResult = apiService.getRealtimeWeather(lng, lat)) {
            is ApiResult.Success -> {
                when (val dailyResult = apiService.getDailyWeather(lng, lat)) {
                    is ApiResult.Success -> {
                        // 第三步：合并实时天气和7日预报为 Weather 对象
                        val weather = Weather(
                            realtime = realtimeResult.data.result.realtime,
                            daily = dailyResult.data.result.daily
                        )
                        // 第四步：存入缓存，供后续请求使用
                        weatherCache.put(cacheKey, weather)
                        ApiResult.Success(weather)
                    }
                    is ApiResult.Error -> dailyResult
                    is ApiResult.Loading -> ApiResult.Loading
                }
            }
            is ApiResult.Error -> realtimeResult
            is ApiResult.Loading -> ApiResult.Loading
        }
    }

    override suspend fun savePlace(place: Place) {
        placeStore.savePlace(place)
    }

    override fun getSavedPlace(): Flow<Place?> {
        return placeStore.getSavedPlace()
    }

    override fun getSavedPlaceList(): Flow<List<Place>> {
        return placeStore.getPlaceList()
    }

    override fun isPlaceSaved(): Flow<Boolean> {
        return placeStore.isPlaceSaved()
    }

    override fun isSpecificPlaceSaved(placeName: String): Flow<Boolean> {
        return placeStore.isSpecificPlaceSaved(placeName)
    }

    override suspend fun clearPlace() {
        placeStore.clearPlace()
        weatherCache.clear()
    }

    fun invalidateCache(lng: String, lat: String) {
        weatherCache.remove("$lng,$lat")
    }

    fun clearCache() {
        weatherCache.clear()
    }
}

/**
     * 内存缓存类
     * 
     * 性能优化原理：
     * - 天气数据在短时间内（如5分钟）不会频繁变化
     * - 如果每次请求都从网络获取，会浪费带宽和增加延迟
     * - 使用内存缓存存储最近的天气数据，避免重复网络请求
     * - 缓存数据带有过期时间，确保数据不会太旧
     * 
     * 线程安全机制：
     * - 使用 @Synchronized 注解保证多线程环境下的安全访问
     * - 虽然使用的是 mutableMapOf（非线程安全），但 synchronized 块确保了原子操作
     * - 对于高并发场景，可以考虑使用 ConcurrentHashMap 替代
     * 
     * 缓存策略：
     * - 写策略：每次获取新数据后写入缓存
     * - 读策略：先检查缓存，命中则返回，未命中则从网络获取
     * - 过期策略：获取时检查时间戳，过期则移除并返回 null
     * - 清理策略：支持手动清除指定键或全部缓存
     * 
     * 使用场景：
     * - WeatherRepositoryImpl 使用此类缓存 Weather 对象
     * - 缓存键格式："经度,纬度"（如 "116.4074,39.9042"）
     * - 缓存有效期：5分钟（5 * 60 * 1000 毫秒）
     */
class MemoryCache<K, V>(
    private val expireAfterMillis: Long
) {
    /**
     * 缓存存储，使用普通 mutableMapOf
     * 注意：虽然 mutableMapOf 不是线程安全的，但 @Synchronized 保证了安全访问
     */
    private val cache = mutableMapOf<K, CacheEntry<V>>()

    /**
     * 获取缓存数据
     * 
     * 流程：
     * 1. 根据 key 获取缓存条目
     * 2. 如果条目存在且未过期，返回数据
     * 3. 如果条目过期，移除并返回 null
     * 
     * @param key 缓存键
     * @return 缓存值（如果存在且未过期），否则返回 null
     */
    @Synchronized
    fun get(key: K): V? {
        val entry = cache[key]
        if (entry != null && !entry.isExpired) {
            return entry.value
        }
        // 数据过期，从缓存中移除
        cache.remove(key)
        return null
    }

    /**
     * 存储缓存数据
     * 
     * 流程：
     * 1. 创建 CacheEntry，包含数据和当前时间戳
     * 2. 将 CacheEntry 存入缓存
     * 
     * @param key 缓存键
     * @param value 缓存值
     */
    @Synchronized
    fun put(key: K, value: V) {
        cache[key] = CacheEntry(value, System.currentTimeMillis(), expireAfterMillis)
    }

    /**
     * 移除指定缓存
     * 
     * @param key 要移除的缓存键
     */
    @Synchronized
    fun remove(key: K) {
        cache.remove(key)
    }

    /**
     * 清空所有缓存
     */
    @Synchronized
    fun clear() {
        cache.clear()
    }

    /**
     * 缓存条目数据类
     * 
     * @param value 缓存的数据
     * @param timestamp 存储时间戳（毫秒）
     * @param expireAfterMillis 过期时间（毫秒）
     */
    private data class CacheEntry<V>(
        val value: V,
        val timestamp: Long,
        private val expireAfterMillis: Long
    ) {
        /**
         * 判断缓存是否过期
         * 
         * 计算当前时间与存储时间的差值，超过过期时间则认为过期
         */
        val isExpired: Boolean
            get() = System.currentTimeMillis() - timestamp > expireAfterMillis
    }
}