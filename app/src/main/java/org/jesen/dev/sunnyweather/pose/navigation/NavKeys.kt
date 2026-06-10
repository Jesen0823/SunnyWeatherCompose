package org.jesen.dev.sunnyweather.pose.navigation

import androidx.navigation3.runtime.NavKey
import kotlinx.serialization.KSerializer
import kotlinx.serialization.Serializable

@Serializable
data object WeatherKey : NavKey

@Serializable
data object PlaceKey : NavKey

@Serializable
data object UnknownKey : NavKey

@Serializable
data object SettingsKey : NavKey

// DrawerExamples
@Serializable
data object DrawerExamplesKey: NavKey

@Serializable
data object TestPageKey : NavKey