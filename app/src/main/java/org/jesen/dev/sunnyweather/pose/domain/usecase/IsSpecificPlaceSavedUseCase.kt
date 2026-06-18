package org.jesen.dev.sunnyweather.pose.domain.usecase

import kotlinx.coroutines.flow.Flow
import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository

class IsSpecificPlaceSavedUseCase(private val repository: WeatherRepository) {
    operator fun invoke(placeName: String): Flow<Boolean> {
        return repository.isSpecificPlaceSaved(placeName)
    }
}
