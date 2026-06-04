package org.jesen.dev.sunnyweather.pose.domain.model

import kotlinx.serialization.Serializable

@Serializable
data class Location(
    val lng: String,
    val lat: String
)

@Serializable
data class Place(
    val name: String,
    val location: Location,
    val address: String
)

@Serializable
data class PlaceResponse(
    val status: String,
    val places: List<Place>
)