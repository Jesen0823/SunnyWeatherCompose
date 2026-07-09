package org.jesen.dev.sunnyweather.pose.domain.usecase

import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.repository.WeatherRepository

class SavePlaceListUseCase(
    private val repository: WeatherRepository
) {
    suspend operator fun invoke(places: List<Place>) {
        repository.savePlaceList(places)
    }
}
