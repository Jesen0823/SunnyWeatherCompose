package org.jesen.dev.sunnyweather.pose.domain.usecase

import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository

class ClearPlaceUseCase(private val repository: WeatherRepository) {
    suspend operator fun invoke() {
        repository.clearPlace()
    }
}
