/**
 * API 请求结果封装类
 * 
 * 主要职责：
 * - 统一封装网络请求的返回结果
 * - 提供安全的 API 调用包装函数
 * 
 * 技术要点：
 * - 使用密封类（sealed class）表示请求状态，支持 Success、Error、Loading 三种状态
 * - safeApiCall 函数捕获异常，将异常转换为 Error 状态，避免应用崩溃
 * - 泛型支持，可封装任意类型的响应数据
 */
package org.jesen.dev.sunnyweather.pose.data.network

sealed class ApiResult<out T> {
    data class Success<out T>(val data: T) : ApiResult<T>()
    data class Error(val message: String, val exception: Throwable? = null) : ApiResult<Nothing>()
    object Loading : ApiResult<Nothing>()
}

suspend fun <T> safeApiCall(apiCall: suspend () -> T): ApiResult<T> {
    return try {
        ApiResult.Success(apiCall())
    } catch (e: Exception) {
        ApiResult.Error(e.message ?: "Unknown error", e)
    }
}
