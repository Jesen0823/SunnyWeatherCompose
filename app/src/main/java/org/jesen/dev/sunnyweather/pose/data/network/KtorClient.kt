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
    private const val BASE_URL = "https://api.caiyunapp.com/"
    private const val TOKEN = "cM8HddCKF2Ns1Ccm"
    
    val instance: HttpClient by lazy {
        HttpClient(OkHttp) {
            expectSuccess = true
            
            defaultRequest {
                url(BASE_URL)
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
    
    fun getBaseUrl(): String = BASE_URL
}