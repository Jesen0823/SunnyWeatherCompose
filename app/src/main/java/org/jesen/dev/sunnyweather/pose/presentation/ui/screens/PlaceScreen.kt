package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Search
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.ListItem
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.R
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.UiState
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.WeatherViewModel

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun PlaceScreen(
    viewModel: WeatherViewModel,
    onPlaceSelected: (Place) -> Unit
) {
    var searchQuery by remember { mutableStateOf("") }
    val placesState = viewModel.placesState.collectAsState()
    
    LaunchedEffect(searchQuery) {
        if (searchQuery.isNotEmpty()) {
            viewModel.searchPlaces(searchQuery)
        }
    }
    
    Column(modifier = Modifier.fillMaxSize()) {
        SearchBar(
            query = searchQuery,
            onQueryChange = { searchQuery = it }
        )
        
        when (val state = placesState.value) {
            is UiState.Loading -> if (searchQuery.isNotEmpty()) {
                LoadingState()
            } else {
                DefaultState()
            }
            is UiState.Error -> ErrorState(state.message)
            is UiState.Success -> PlaceList(
                places = state.data,
                onPlaceSelected = onPlaceSelected
            )
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SearchBar(
    query: String,
    onQueryChange: (String) -> Unit
) {
    val keyboardController = LocalSoftwareKeyboardController.current

    OutlinedTextField(
        value = query,
        onValueChange = onQueryChange,
        leadingIcon = {
            Icon(
                imageVector = Icons.Default.Search,
                contentDescription = "搜索"
            )
        },
        placeholder = {
            Text(text = "搜索城市")
        },
        keyboardOptions = KeyboardOptions(imeAction = ImeAction.Search),
        keyboardActions = KeyboardActions(
            onSearch = { keyboardController?.hide() }
        ),
        modifier = Modifier
            .fillMaxWidth()
            .padding(16.dp)
    )
}

@Composable
fun DefaultState() {
    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Image(
            painter = painterResource(id = R.drawable.bg_place),
            contentDescription = null,
            modifier = Modifier.size(200.dp)
        )
        Text(
            text = "搜索你想查询天气的城市",
            style = MaterialTheme.typography.titleMedium,
            modifier = Modifier.padding(top = 16.dp)
        )
    }
}

@Composable
fun LoadingState() {
    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        CircularProgressIndicator()
        Text(
            text = "搜索中...",
            style = MaterialTheme.typography.bodyMedium,
            modifier = Modifier.padding(top = 8.dp)
        )
    }
}

@Composable
fun ErrorState(message: String) {
    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Text(
            text = "搜索失败",
            style = MaterialTheme.typography.titleLarge,
            fontWeight = androidx.compose.ui.text.font.FontWeight.Bold
        )
        Text(
            text = message,
            style = MaterialTheme.typography.bodyMedium,
            modifier = Modifier.padding(top = 8.dp)
        )
    }
}

@Composable
fun PlaceList(
    places: List<Place>,
    onPlaceSelected: (Place) -> Unit
) {
    LazyColumn(modifier = Modifier.fillMaxSize()) {
        items(places) { place ->
            Surface(
                modifier = Modifier
                    .fillMaxWidth()
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