/**
 * 城市地点数据模型
 * 
 * 主要职责：
 * - 定义城市信息的数据结构，包括名称、经纬度、地址等
 * - 支持 JSON 序列化，用于网络请求和本地存储
 * 
 * 技术要点：
 * - 使用 @Serializable 注解支持 Kotlinx 序列化
 * - 使用 @Immutable 注解标记不可变数据类，帮助 Compose 优化重组判断
 * - Location 独立定义，便于复用
 * - PlaceResponse 封装 API 返回的城市列表响应
 */
package org.jesen.dev.sunnyweather.pose.domain.model

import androidx.compose.runtime.Immutable
import kotlinx.serialization.Serializable

@Immutable
@Serializable
data class Location(
    val lng: String,
    val lat: String
)

@Immutable
@Serializable
data class Place(
    val name: String,
    val location: Location,
    @kotlinx.serialization.SerialName("formatted_address") val address: String
)

@Immutable
@Serializable
data class PlaceResponse(
    val status: String,
    val places: List<Place>
)