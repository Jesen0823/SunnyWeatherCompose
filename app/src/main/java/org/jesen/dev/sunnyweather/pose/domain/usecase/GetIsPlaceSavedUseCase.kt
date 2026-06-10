package org.jesen.dev.sunnyweather.pose.domain.usecase

import kotlinx.coroutines.flow.Flow
import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository

class GetIsPlaceSavedUseCase(private val repository: WeatherRepository) {
    operator fun invoke(): Flow<Boolean> {
        return repository.isPlaceSaved()
    }
}
