package org.jesen.dev.sunnyweather.pose.domain.usecase

import kotlinx.coroutines.flow.Flow
import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.domain.model.Place

class GetSavedPlaceUseCase(private val repository: WeatherRepository) {
    operator fun invoke(): Flow<Place?> {
        return repository.getSavedPlace()
    }
}
