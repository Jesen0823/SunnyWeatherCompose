/**
 * 城市数据存储类
 * 
 * 主要职责：
 * - 使用 DataStore 持久化存储用户选择的城市列表
 * - 提供城市数据的增删查操作接口
 * 
 * 技术要点：
 * - 基于 Jetpack DataStore (Preferences) 实现轻量级数据存储
 * - 使用 JSON 序列化存储 List<Place>，支持多城市管理
 * - 新添加的城市自动放在列表首位，最新选择的城市优先显示
 * - 返回 Flow 类型，支持响应式数据观察
 */
package org.jesen.dev.sunnyweather.pose.data.store

import android.content.Context
import androidx.datastore.core.DataStore
import androidx.datastore.preferences.core.Preferences
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.stringPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.map
import kotlinx.serialization.decodeFromString
import kotlinx.serialization.encodeToString
import kotlinx.serialization.json.Json
import org.jesen.dev.sunnyweather.pose.domain.model.Place

private val Context.placeDataStore: DataStore<Preferences> by preferencesDataStore(name = "place_prefs")

class PlaceStore(context: Context) {
    private val dataStore = context.placeDataStore
    private val PLACE_KEY = stringPreferencesKey("place")
    private val PLACE_LIST_KEY = stringPreferencesKey("place_list")
    private val json = Json { ignoreUnknownKeys = true }

    suspend fun savePlace(place: Place) {
        dataStore.edit { preferences ->
            val currentList = preferences[PLACE_LIST_KEY]?.let {
                json.decodeFromString<List<Place>>(it)
            } ?: emptyList()
            
            val newList = listOf(place) + currentList.filter { it.name != place.name }
            preferences[PLACE_LIST_KEY] = json.encodeToString(newList)
        }
    }

    fun getSavedPlace(): Flow<Place?> {
        return getPlaceList().map { it.firstOrNull() }
    }

    suspend fun clearPlace() {
        dataStore.edit { preferences ->
            preferences.remove(PLACE_KEY)
            preferences.remove(PLACE_LIST_KEY)
        }
    }

    fun getPlaceList(): Flow<List<Place>> {
        return dataStore.data.map { preferences ->
            preferences[PLACE_LIST_KEY]?.let {
                json.decodeFromString<List<Place>>(it)
            } ?: emptyList()
        }
    }

    fun isPlaceSaved(): Flow<Boolean> {
        return getPlaceList().map { it.isNotEmpty() }
    }
}
