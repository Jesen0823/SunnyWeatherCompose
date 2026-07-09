package org.jesen.dev.sunnyweather.pose.presentation.viewmodel

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.usecase.GetSavedPlaceListUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.SavePlaceListUseCase

class PlaceManagerViewModel(
    private val getSavedPlaceListUseCase: GetSavedPlaceListUseCase,
    private val savePlaceListUseCase: SavePlaceListUseCase
) : ViewModel() {

    private val _placeList = MutableStateFlow<List<Place>>(emptyList())
    val placeList: StateFlow<List<Place>> = _placeList.asStateFlow()

    private val _selectedPlaces = MutableStateFlow<Set<String>>(emptySet())
    val selectedPlaces: StateFlow<Set<String>> = _selectedPlaces.asStateFlow()

    private val _isDragging = MutableStateFlow(false)
    val isDragging: StateFlow<Boolean> = _isDragging.asStateFlow()

    private val _draggedIndex = MutableStateFlow<Int?>(null)
    val draggedIndex: StateFlow<Int?> = _draggedIndex.asStateFlow()

    private val _isSelectionMode = MutableStateFlow(false)
    val isSelectionMode: StateFlow<Boolean> = _isSelectionMode.asStateFlow()

    init {
        loadPlaceList()
    }

    private fun loadPlaceList() {
        viewModelScope.launch {
            getSavedPlaceListUseCase().collect { places ->
                _placeList.value = places
            }
        }
    }

    fun togglePlaceSelection(placeName: String) {
        _selectedPlaces.value = if (_selectedPlaces.value.contains(placeName)) {
            _selectedPlaces.value - placeName
        } else {
            _selectedPlaces.value + placeName
        }
    }

    fun clearSelection() {
        _selectedPlaces.value = emptySet()
    }

    fun enterSelectionMode() {
        _isSelectionMode.value = true
    }

    fun exitSelectionMode() {
        _isSelectionMode.value = false
        _selectedPlaces.value = emptySet()
    }

    fun deleteSelectedPlaces() {
        val newList = _placeList.value.filter { !_selectedPlaces.value.contains(it.name) }
        _placeList.value = newList
        _selectedPlaces.value = emptySet()
        _isSelectionMode.value = false
        saveToCache(newList)
    }

    fun deletePlace(placeName: String) {
        val newList = _placeList.value.filter { it.name != placeName }
        _placeList.value = newList
        _selectedPlaces.value = _selectedPlaces.value - placeName
        saveToCache(newList)
    }

    fun startDrag(index: Int) {
        _isDragging.value = true
        _draggedIndex.value = index
    }

    fun stopDrag() {
        _isDragging.value = false
        _draggedIndex.value = null
    }

    fun movePlace(fromIndex: Int, toIndex: Int) {
        val list = _placeList.value.toMutableList()
        val item = list.removeAt(fromIndex)
        list.add(toIndex, item)
        _placeList.value = list
        _draggedIndex.value = toIndex
        saveToCache(list)
    }

    private fun saveToCache(places: List<Place>) {
        viewModelScope.launch {
            savePlaceListUseCase(places)
        }
    }

    fun isPlaceSelected(placeName: String): Boolean {
        return _selectedPlaces.value.contains(placeName)
    }
}
