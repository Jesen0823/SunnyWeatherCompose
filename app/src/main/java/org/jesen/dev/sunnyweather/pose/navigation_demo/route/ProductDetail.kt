package org.jesen.dev.sunnyweather.pose.navigation_demo.route

import androidx.navigation3.runtime.NavKey
import kotlinx.serialization.Serializable

// 所有作为NavEntry Key的data class都必须实现NavKey接口
// 所有作为NavEntry Key的data class都必须添加@Serializable注解，因为横竖屏旋转后的状态恢复要求数据是可序列化的才行
@Serializable
data class ProductDetail (
    val name:String,
    val description:String
): NavKey