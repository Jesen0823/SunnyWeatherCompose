package org.jesen.dev.sunnyweather.pose.domain.usecase

import org.jesen.dev.sunnyweather.pose.data.network.ApiResult
import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.domain.model.PlaceResponse

class SearchPlacesUseCase(private val repository: WeatherRepository) {
    suspend operator fun invoke(query: String): ApiResult<PlaceResponse> {
        return repository.searchPlaces(query)
    }
}
