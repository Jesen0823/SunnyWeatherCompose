package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import androidx.compose.foundation.gestures.detectDragGesturesAfterLongPress
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.NavigateBefore
import androidx.compose.material.icons.filled.NavigateBefore
import androidx.compose.material3.BottomAppBar
import androidx.compose.material3.Button
import androidx.compose.material3.CenterAlignedTopAppBar
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.presentation.ui.components.widget.PlaceListItem
import org.jesen.dev.sunnyweather.pose.presentation.viewmodel.PlaceManagerViewModel

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun PlaceManagerScreen(
    viewModel: PlaceManagerViewModel,
    onBack: () -> Unit = {}
) {
    val placeList by viewModel.placeList.collectAsState()
    val selectedPlaces by viewModel.selectedPlaces.collectAsState()
    val isDragging by viewModel.isDragging.collectAsState()
    val draggedIndex by viewModel.draggedIndex.collectAsState()
    val isSelectionMode by viewModel.isSelectionMode.collectAsState()

    Scaffold(
        topBar = {
            CenterAlignedTopAppBar(
                title = {
                    if (isSelectionMode) {
                        Text(
                            text = "已选择 ${selectedPlaces.size} 项",
                            style = MaterialTheme.typography.titleLarge
                        )
                    } else {
                        Text(
                            text = "地址管理",
                            style = MaterialTheme.typography.titleLarge
                        )
                    }
                },
                navigationIcon = {
                    IconButton(onClick = {
                        if (isSelectionMode) {
                            viewModel.exitSelectionMode()
                        } else {
                            onBack()
                        }
                    }) {
                        Icon(
                            imageVector = Icons.AutoMirrored.Filled.NavigateBefore,
                            contentDescription = "返回"
                        )
                    }
                },
                actions = {
                    if (placeList.isNotEmpty()) {
                        if (isSelectionMode) {
                            IconButton(onClick = { viewModel.exitSelectionMode() }) {
                                Text(
                                    text = "取消",
                                    style = MaterialTheme.typography.labelLarge
                                )
                            }
                        } else {
                            IconButton(onClick = { viewModel.enterSelectionMode() }) {
                                Text(
                                    text = "选择",
                                    style = MaterialTheme.typography.labelLarge
                                )
                            }
                        }
                    }
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.surfaceContainer
                )
            )
        },
        bottomBar = {
            if (isSelectionMode) {
                BottomAppBar(
                    containerColor = MaterialTheme.colorScheme.surfaceContainer,
                    contentPadding = PaddingValues(horizontal = 16.dp)
                ) {
                    Box(
                        modifier = Modifier.fillMaxWidth(),
                        contentAlignment = Alignment.CenterEnd
                    ) {
                        Button(
                            onClick = { viewModel.deleteSelectedPlaces() },
                            enabled = selectedPlaces.isNotEmpty(),
                            modifier = Modifier.size(width = 120.dp, height = 40.dp)
                        ) {
                            Text(
                                text = "删除(${selectedPlaces.size})",
                                style = MaterialTheme.typography.labelLarge
                            )
                        }
                    }
                }
            }
        }
    ) { paddingValues ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(paddingValues)
        ) {
            if (placeList.isEmpty()) {
                Box(
                    modifier = Modifier.fillMaxSize(),
                    contentAlignment = Alignment.Center
                ) {
                    Text(
                        text = "暂无地址",
                        style = MaterialTheme.typography.bodyLarge,
                        color = MaterialTheme.colorScheme.onSurfaceVariant
                    )
                }
            } else {
                LazyColumn(
                    modifier = Modifier.fillMaxSize(),
                    verticalArrangement = Arrangement.spacedBy(12.dp),
                    contentPadding = PaddingValues(horizontal = 16.dp, vertical = 16.dp)
                ) {
                    itemsIndexed(placeList, key = { _, place -> place.name }) { index, place ->
                        val isItemSelected = selectedPlaces.contains(place.name)
                        val isItemDragging = draggedIndex == index

                        PlaceListItem(
                            place = place,
                            isSelected = isItemSelected,
                            isDragging = isItemDragging,
                            isSelectionMode = isSelectionMode,
                            onToggleSelection = { viewModel.togglePlaceSelection(place.name) },
                            onDelete = { viewModel.deletePlace(place.name) },
                            onDragStart = { viewModel.startDrag(index) },
                            modifier = Modifier
                                .fillMaxWidth()
                                .pointerInput(place.name) {
                                    var dragOffsetY = 0f
                                    val itemHeight = 88.dp.toPx()

                                    detectDragGesturesAfterLongPress(
                                        onDrag = { change, dragAmount ->
                                            change.consume()
                                            dragOffsetY += dragAmount.y

                                            if (dragOffsetY > itemHeight && index < placeList.size - 1) {
                                                viewModel.movePlace(index, index + 1)
                                                dragOffsetY -= itemHeight
                                            } else if (dragOffsetY < -itemHeight && index > 0) {
                                                viewModel.movePlace(index, index - 1)
                                                dragOffsetY += itemHeight
                                            }
                                        },
                                        onDragEnd = {
                                            viewModel.stopDrag()
                                        },
                                        onDragCancel = {
                                            viewModel.stopDrag()
                                        }
                                    )
                                }
                        )
                    }
                }
            }
        }
    }
}