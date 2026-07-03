package org.jesen.dev.sunnyweather.pose.data.network

import java.nio.charset.StandardCharsets
import java.security.InvalidKeyException
import java.security.NoSuchAlgorithmException
import java.util.*
import javax.crypto.Mac
import javax.crypto.spec.SecretKeySpec

object CaiyunSignature {

    fun generateNonce(): String {
        return UUID.randomUUID().toString()
    }

    fun generateTimestamp(): Long {
        return System.currentTimeMillis() / 1000
    }

    fun sign(
        appKey: String,
        appSecret: String,
        method: String,
        path: String,
        nonce: String,
        timestamp: Long,
        query: Map<String, String> = emptyMap()
    ): String {
        val sortedQuery = TreeMap(query)
        
        val queryStr = sortedQuery.entries.joinToString("&") { (key, value) ->
            "${encodeUrl(key)}=${encodeUrl(value)}"
        }

        val stringToSign = listOf(
            method,
            path,
            queryStr,
            appKey,
            nonce,
            timestamp.toString()
        ).joinToString(":")

        val hmac = try {
            Mac.getInstance("HmacSHA256")
        } catch (e: NoSuchAlgorithmException) {
            throw RuntimeException("HmacSHA256 algorithm not found", e)
        }

        val secretKey = SecretKeySpec(appSecret.toByteArray(StandardCharsets.UTF_8), "HmacSHA256")
        try {
            hmac.init(secretKey)
        } catch (e: InvalidKeyException) {
            throw RuntimeException("Invalid secret key", e)
        }

        val signatureBytes = hmac.doFinal(stringToSign.toByteArray(StandardCharsets.UTF_8))
        return Base64.getUrlEncoder().encodeToString(signatureBytes)
    }

    private fun encodeUrl(value: String): String {
        return value
            .replace("+", "%2B")
            .replace("%", "%25")
            .replace(" ", "+")
            .replace("!", "%21")
            .replace("'", "%27")
            .replace("(", "%28")
            .replace(")", "%29")
            .replace("*", "%2A")
    }
}