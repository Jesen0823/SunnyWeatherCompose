package org.jesen.dev.sunnyweather.pose.di

import android.content.Context
import org.jesen.dev.sunnyweather.pose.data.network.KtorClient
import org.jesen.dev.sunnyweather.pose.data.network.WeatherApiService
import org.jesen.dev.sunnyweather.pose.data.repository.WeatherRepository
import org.jesen.dev.sunnyweather.pose.data.store.PlaceStore
import org.jesen.dev.sunnyweather.pose.domain.usecase.WeatherUseCases
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel

object AppModule {
    lateinit var context: Context
    
    fun init(context: Context) {
        AppModule.context = context
    }
    
    val weatherApiService: WeatherApiService by lazy {
        WeatherApiService()
    }
    
    val placeStore: PlaceStore by lazy {
        PlaceStore(context)
    }
    
    val weatherRepository: WeatherRepository by lazy {
        WeatherRepository(weatherApiService, placeStore)
    }
    
    val weatherUseCases: WeatherUseCases by lazy {
        WeatherUseCases(weatherRepository)
    }
    
    val weatherViewModel: WeatherViewModel by lazy {
        WeatherViewModel(weatherUseCases)
    }
}