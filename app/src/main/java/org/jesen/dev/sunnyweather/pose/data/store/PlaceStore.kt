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
    private val json = Json { ignoreUnknownKeys = true }

    suspend fun savePlace(place: Place) {
        dataStore.edit { preferences ->
            preferences[PLACE_KEY] = json.encodeToString(place)
        }
    }

    fun getSavedPlace(): Flow<Place?> {
        return dataStore.data.map { preferences ->
            preferences[PLACE_KEY]?.let { json.decodeFromString<Place>(it) }
        }
    }

    suspend fun clearPlace() {
        dataStore.edit { preferences ->
            preferences.remove(PLACE_KEY)
        }
    }

    fun isPlaceSaved(): Flow<Boolean> {
        return dataStore.data.map { preferences ->
            preferences.contains(PLACE_KEY)
        }
    }
}