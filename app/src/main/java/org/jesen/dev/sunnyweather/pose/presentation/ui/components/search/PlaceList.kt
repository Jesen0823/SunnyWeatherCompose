package org.jesen.dev.sunnyweather.pose.presentation.ui.components.search

import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.ListItem
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.domain.model.Place

@Composable
fun PlaceList(
    places: List<Place>,
    onPlaceSelected: (Place) -> Unit
) {
    LazyColumn(modifier = Modifier.fillMaxSize()) {
        items(places) { place ->
            Surface(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(horizontal = 16.dp, vertical = 4.dp),
                onClick = { onPlaceSelected(place) }
            ) {
                ListItem(
                    headlineContent = {
                        Text(
                            text = place.name,
                            style = MaterialTheme.typography.bodyLarge
                        )
                    },
                    supportingContent = {
                        Text(
                            text = place.address,
                            style = MaterialTheme.typography.bodyMedium
                        )
                    }
                )
            }
        }
    }
}
