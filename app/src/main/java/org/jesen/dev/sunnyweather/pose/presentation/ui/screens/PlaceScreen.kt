package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.*
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import androidx.compose.ui.text.font.FontWeight
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.debounce
import kotlinx.coroutines.flow.receiveAsFlow
import org.jesen.dev.sunnyweather.pose.R
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.search.PlaceList
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.search.SearchBar
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceViewModel
import org.jesen.dev.sunnyweather.pose.presentation.common.UiState

@OptIn(kotlinx.coroutines.FlowPreview::class)
@Composable
fun PlaceScreen(
    viewModel: PlaceViewModel,
    onPlaceSelected: (Place) -> Unit
) {
    var searchQuery by remember { mutableStateOf("") }
    val placesState: State<UiState<List<Place>>> = viewModel.placesState.collectAsState()
    
    val searchChannel = remember { Channel<String>(Channel.CONFLATED) }
    val searchFlow: Flow<String> = remember { 
        @OptIn(kotlinx.coroutines.FlowPreview::class)
        searchChannel.receiveAsFlow().debounce(500) 
    }
    
    LaunchedEffect(searchQuery) {
        searchChannel.trySend(searchQuery)
    }
    
    LaunchedEffect(Unit) {
        searchFlow.collect { query ->
            if (query.isNotEmpty()) {
                viewModel.searchPlaces(query)
            }
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

@Composable
private fun DefaultState() {
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
            text = stringResource(id = R.string.search_default),
            style = MaterialTheme.typography.titleMedium,
            modifier = Modifier.padding(top = 16.dp)
        )
    }
}

@Composable
private fun LoadingState() {
    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        CircularProgressIndicator()
        Text(
            text = stringResource(id = R.string.search_loading),
            style = MaterialTheme.typography.bodyMedium,
            modifier = Modifier.padding(top = 8.dp)
        )
    }
}

@Composable
private fun ErrorState(message: String) {
    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Text(
            text = stringResource(id = R.string.search_error),
            style = MaterialTheme.typography.titleLarge,
            fontWeight = FontWeight.Bold
        )
        Text(
            text = message,
            style = MaterialTheme.typography.bodyMedium,
            modifier = Modifier.padding(top = 8.dp)
        )
    }
}
