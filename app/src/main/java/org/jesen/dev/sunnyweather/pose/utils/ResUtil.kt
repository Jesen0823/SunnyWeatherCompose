package org.jesen.dev.sunnyweather.pose.utils

import android.content.Context
import android.graphics.Bitmap
import android.graphics.drawable.BitmapDrawable
import android.graphics.drawable.Drawable
import androidx.core.content.ContextCompat
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

data class BitmapRgbaData(
    val rgbaBytes: ByteArray,
    val width: Int,
    val height: Int
) {
    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false

        other as BitmapRgbaData

        if (!rgbaBytes.contentEquals(other.rgbaBytes)) return false
        if (width != other.width) return false
        if (height != other.height) return false

        return true
    }

    override fun hashCode(): Int {
        var result = rgbaBytes.contentHashCode()
        result = 31 * result + width
        result = 31 * result + height
        return result
    }
}

object ResUtil {

    suspend fun loadBitmapToRgbaAsync(context: Context, resId: Int): BitmapRgbaData? {
        return withContext(Dispatchers.IO) {
            loadBitmapToRgba(context, resId)
        }
    }

    fun loadBitmapToRgba(context: Context, resId: Int): BitmapRgbaData? {
        return try {
            val drawable = ContextCompat.getDrawable(context, resId)
            convertDrawableToRgba(drawable)
        } catch (e: Exception) {
            e.printStackTrace()
            null
        }
    }

    fun convertDrawableToRgba(drawable: Drawable?): BitmapRgbaData? {
        if (drawable !is BitmapDrawable) {
            return null
        }

        return try {
            val bitmap = drawable.bitmap ?: return null
            val width = bitmap.width
            val height = bitmap.height

            if (width <= 0 || height <= 0) {
                return null
            }

            val pixels = IntArray(width * height)
            bitmap.getPixels(pixels, 0, width, 0, 0, width, height)

            val rgbaBytes = ByteArray(pixels.size * 4)
            for (i in pixels.indices) {
                rgbaBytes[i * 4] = ((pixels[i] shr 16) and 0xFF).toByte()
                rgbaBytes[i * 4 + 1] = ((pixels[i] shr 8) and 0xFF).toByte()
                rgbaBytes[i * 4 + 2] = (pixels[i] and 0xFF).toByte()
                rgbaBytes[i * 4 + 3] = ((pixels[i] shr 24) and 0xFF).toByte()
            }

            BitmapRgbaData(rgbaBytes, width, height)
        } catch (e: Exception) {
            e.printStackTrace()
            null
        }
    }
}