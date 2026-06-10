package org.jesen.dev.sunnyweather.pose.domain.usecase

import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.domain.model.Place

class SavePlaceUseCase(private val repository: WeatherRepository) {
    suspend operator fun invoke(place: Place) {
        repository.savePlace(place)
    }
}
