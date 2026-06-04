package org.jesen.dev.sunnyweather.pose.domain.usecase

import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.model.PlaceResponse
import org.jesen.dev.sunnyweather.pose.domain.model.Weather

class WeatherUseCases(private val repository: WeatherRepository) {
    suspend fun searchPlaces(query: String): ApiResult<PlaceResponse> {
        return repository.searchPlaces(query)
    }
    
    suspend fun fetchWeather(lng: String, lat: String): ApiResult<Weather> {
        return repository.fetchWeather(lng, lat)
    }
    
    suspend fun savePlace(place: Place) {
        repository.savePlace(place)
    }
    
    fun getSavedPlace() = repository.getSavedPlace()
    
    fun isPlaceSaved() = repository.isPlaceSaved()
    
    suspend fun clearPlace() {
        repository.clearPlace()
    }
}