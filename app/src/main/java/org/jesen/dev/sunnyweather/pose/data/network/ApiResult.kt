package org.jesen.dev.sunnyweather.pose.data.network

import android.util.Log

sealed class ApiResult<out T> {
    data class Success<out T>(val data: T) : ApiResult<T>()
    data class Error(
        val message: String,
        val exception: Throwable? = null,
        val errorCode: Int? = null
    ) : ApiResult<Nothing>()
    object Loading : ApiResult<Nothing>()
}

private const val MAX_RETRY_COUNT = 3
private const val RETRY_DELAY_MS = 1000L
private const val BACKOFF_MULTIPLIER = 2.0

suspend fun <T> safeApiCall(apiCall: suspend () -> T): ApiResult<T> {
    var retryCount = 0
    var delayMs = RETRY_DELAY_MS
    
    while (retryCount < MAX_RETRY_COUNT) {
        try {
            return ApiResult.Success(apiCall())
        } catch (e: Exception) {
            Log.d("ApiResult", "safeApiCall,${e.message}")
            
            val errorCode = extractErrorCode(e.message)
            
            if (errorCode == 429 && retryCount < MAX_RETRY_COUNT - 1) {
                retryCount++
                delayMs = (delayMs * BACKOFF_MULTIPLIER).toLong()
                kotlinx.coroutines.delay(delayMs)
                Log.d("ApiResult", "Rate limited, retrying in $delayMs ms (attempt $retryCount)")
            } else {
                val errorMessage = when (errorCode) {
                    429 -> "请求过于频繁，请稍后重试"
                    401 -> "API 认证失败，请检查配置"
                    403 -> "权限不足"
                    500 -> "服务器内部错误"
                    else -> e.message ?: "未知错误"
                }
                return ApiResult.Error(errorMessage, e, errorCode)
            }
        }
    }
    
    return ApiResult.Error("请求失败：达到最大重试次数")
}

private fun extractErrorCode(message: String?): Int? {
    if (message == null) return null
    
    val errorCodePattern = Regex("invalid: (\\d+)")
    val matchResult = errorCodePattern.find(message)
    return matchResult?.groupValues?.get(1)?.toIntOrNull()
}