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
