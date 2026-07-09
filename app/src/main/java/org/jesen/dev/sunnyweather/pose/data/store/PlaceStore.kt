package org.jesen.dev.sunnyweather.pose.data.store

import android.content.Context
import androidx.datastore.core.DataStore
import androidx.datastore.preferences.core.Preferences
import androidx.datastore.preferences.core.edit
import androidx.datastore.preferences.core.stringPreferencesKey
import androidx.datastore.preferences.preferencesDataStore
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.map
import kotlinx.serialization.json.Json
import org.jesen.dev.sunnyweather.pose.domain.model.Place

private val Context.placeDataStore: DataStore<Preferences> by preferencesDataStore(name = "place_prefs")

/**
 * 城市数据存储类
 *
 * 主要职责：
 * - 使用 DataStore 持久化存储城市列表数据
 * - 提供保存城市、获取城市、清除城市的方法
 * - 检查城市是否已保存的方法
 * - 支持城市去重（按名称）
 *
 * 技术要点：
 * - 使用 Preferences DataStore 存储 JSON 序列化的城市列表
 * - 使用 kotlinx.serialization 进行 JSON 序列化/反序列化
 * - 保存城市时自动去重，保持最新城市在列表首位
 * - 返回 Flow 类型，支持响应式数据监听
 */
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

    fun isSpecificPlaceSaved(placeName: String): Flow<Boolean> {
        return getPlaceList().map { list ->
            list.any { it.name == placeName }
        }
    }

    suspend fun savePlaceList(places: List<Place>) {
        dataStore.edit { preferences ->
            preferences[PLACE_LIST_KEY] = json.encodeToString(places)
        }
    }
}
