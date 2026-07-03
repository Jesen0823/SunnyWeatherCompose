/**
 * Ktor HTTP 客户端封装类
 * 
 * 主要职责：
 * - 初始化和配置 Ktor HTTP 客户端
 * - 提供统一的网络请求基础配置
 * - 支持多个 Base URL（彩云天气、高德地图）
 * 
 * 技术要点：
 * - 使用 OkHttp 作为底层引擎，提供稳定的 HTTP 客户端实现
 * - 配置 JSON 序列化/反序列化，支持忽略未知字段
 * - 设置超时时间（请求30秒，连接10秒，socket30秒）
 * - 启用日志记录，便于调试网络请求
 * - 使用 lazy 延迟初始化，避免不必要的资源消耗
 * - 使用 EnvironmentConfig 获取 Base URL 配置
 */
package org.jesen.dev.sunnyweather.pose.data.network

import io.ktor.client.HttpClient
import io.ktor.client.engine.okhttp.OkHttp
import io.ktor.client.plugins.HttpTimeout
import io.ktor.client.plugins.contentnegotiation.ContentNegotiation
import io.ktor.client.plugins.defaultRequest
import io.ktor.client.plugins.logging.LogLevel
import io.ktor.client.plugins.logging.Logging
import io.ktor.serialization.kotlinx.json.json
import kotlinx.serialization.json.Json
import java.util.concurrent.TimeUnit

object KtorClient {
    private const val DEFAULT_BASE_URL = "https://api.caiyunapp.com/"

    val instance: HttpClient by lazy {
        HttpClient(OkHttp) {
            expectSuccess = true

            defaultRequest {
                url(EnvironmentConfig.getCaiyunBaseUrl().ifEmpty { DEFAULT_BASE_URL })
            }

            install(ContentNegotiation) {
                json(Json {
                    prettyPrint = true
                    isLenient = true
                    ignoreUnknownKeys = true
                    encodeDefaults = true
                    coerceInputValues = true
                })
            }

            install(HttpTimeout) {
                requestTimeoutMillis = 30_000
                connectTimeoutMillis = 10_000
                socketTimeoutMillis = 30_000
            }

            install(Logging) {
                level = LogLevel.ALL
            }

            engine {
                config {
                    connectTimeout(30, TimeUnit.SECONDS)
                    readTimeout(30, TimeUnit.SECONDS)
                    writeTimeout(30, TimeUnit.SECONDS)
                }
            }
        }
    }

    fun getBaseUrl(): String = EnvironmentConfig.getCaiyunBaseUrl().ifEmpty { DEFAULT_BASE_URL }
}