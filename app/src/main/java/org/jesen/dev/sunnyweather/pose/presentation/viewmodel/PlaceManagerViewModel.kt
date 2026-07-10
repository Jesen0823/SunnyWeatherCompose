package org.jesen.dev.sunnyweather.pose.presentation.viewmodel

import android.util.Log
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import org.jesen.dev.sunnyweather.pose.domain.usecase.GetSavedPlaceListUseCase
import org.jesen.dev.sunnyweather.pose.domain.usecase.SavePlaceListUseCase

private const val TAG = "PlaceManagerViewModel"

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
        Log.d(TAG, "========== PlaceManagerViewModel initialized ==========")
        loadPlaceList()
    }

    private fun loadPlaceList() {
        Log.d(TAG, "loadPlaceList() called")
        viewModelScope.launch {
            Log.d(TAG, "Launching coroutine for getSavedPlaceListUseCase")
            getSavedPlaceListUseCase().collect { places ->
                Log.d(TAG, "getSavedPlaceListUseCase emitted: ${places.size} places")
                if (places.isNotEmpty()) {
                    val placeNames = places.joinToString(", ") { it.name }
                    Log.d(TAG, "Loaded places: [$placeNames]")
                } else {
                    Log.d(TAG, "Loaded places: empty")
                }
                _placeList.value = places
            }
        }
    }

    fun togglePlaceSelection(placeName: String) {
        Log.d(TAG, "togglePlaceSelection() - placeName: $placeName")
        _selectedPlaces.value = if (_selectedPlaces.value.contains(placeName)) {
            _selectedPlaces.value - placeName
        } else {
            _selectedPlaces.value + placeName
        }
        Log.d(TAG, "togglePlaceSelection() - selectedPlaces now: ${_selectedPlaces.value.size}")
    }

    fun clearSelection() {
        Log.d(TAG, "clearSelection() called")
        _selectedPlaces.value = emptySet()
    }

    fun enterSelectionMode() {
        Log.d(TAG, "enterSelectionMode() called")
        _isSelectionMode.value = true
    }

    fun exitSelectionMode() {
        Log.d(TAG, "exitSelectionMode() called")
        _isSelectionMode.value = false
        _selectedPlaces.value = emptySet()
    }

    fun deleteSelectedPlaces() {
        Log.d(TAG, "deleteSelectedPlaces() - deleting ${_selectedPlaces.value.size} places")
        val newList = _placeList.value.filter { !_selectedPlaces.value.contains(it.name) }
        _placeList.value = newList
        _selectedPlaces.value = emptySet()
        _isSelectionMode.value = false
        saveToCache(newList)
        Log.d(TAG, "deleteSelectedPlaces() - done, remaining: ${newList.size} places")
    }

    fun deletePlace(placeName: String) {
        Log.d(TAG, "deletePlace() - deleting: $placeName")
        val newList = _placeList.value.filter { it.name != placeName }
        _placeList.value = newList
        _selectedPlaces.value = _selectedPlaces.value - placeName
        saveToCache(newList)
        Log.d(TAG, "deletePlace() - done, remaining: ${newList.size} places")
    }

    fun startDrag(index: Int) {
        Log.d(TAG, "startDrag() - index: $index")
        _isDragging.value = true
        _draggedIndex.value = index
    }

    fun stopDrag() {
        Log.d(TAG, "stopDrag()")
        _isDragging.value = false
        _draggedIndex.value = null
    }

    fun movePlace(fromIndex: Int, toIndex: Int) {
        Log.d(TAG, "movePlace() - fromIndex: $fromIndex, toIndex: $toIndex")
        val list = _placeList.value.toMutableList()
        val item = list.removeAt(fromIndex)
        list.add(toIndex, item)
        _placeList.value = list
        _draggedIndex.value = toIndex
        saveToCache(list)
        Log.d(TAG, "movePlace() - done")
    }

    private fun saveToCache(places: List<Place>) {
        Log.d(TAG, "saveToCache() - saving ${places.size} places")
        viewModelScope.launch {
            savePlaceListUseCase(places)
            Log.d(TAG, "saveToCache() - completed")
        }
    }

    fun isPlaceSelected(placeName: String): Boolean {
        return _selectedPlaces.value.contains(placeName)
    }
}