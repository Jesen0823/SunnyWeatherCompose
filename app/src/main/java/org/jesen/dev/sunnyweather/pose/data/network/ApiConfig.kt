/**
 * API 配置类
 * 
 * 主要职责：
 * - 管理网络请求的全局配置参数
 * - 控制是否使用模拟数据模式
 * 
 * 技术要点：
 * - 使用单例模式（object）确保全局唯一配置
 * - USE_MOCK_DATA 为 true 时，所有 API 请求返回模拟数据，便于开发调试
 */
package org.jesen.dev.sunnyweather.pose.data.network

object ApiConfig {
    const val USE_MOCK_DATA = false
}
