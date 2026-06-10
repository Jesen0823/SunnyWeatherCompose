package org.jesen.dev.sunnyweather.relationdemo.appbar_demo

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Favorite
import androidx.compose.material.icons.filled.Menu
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.input.nestedscroll.nestedScroll
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp

/**
 * A sample for a simple use of [CenterAlignedTopAppBar].
 *
 * The top app bar here does not react to any scroll events in the content under it.
 */
@OptIn(ExperimentalMaterial3Api::class)
@Preview
@Composable
fun SimpleCenterAlignedTopAppBar() {
    val scrollBehavior = TopAppBarDefaults.enterAlwaysScrollBehavior()
    Scaffold(
        modifier = Modifier.nestedScroll(scrollBehavior.nestedScrollConnection),
        topBar = {
            CenterAlignedTopAppBar(
                title = { Text("TopAppBar", maxLines = 1, overflow = TextOverflow.Ellipsis) },
                navigationIcon = {
                    TooltipBox(
                        positionProvider =
                            TooltipDefaults.rememberTooltipPositionProvider(
                                TooltipAnchorPosition.Above
                            ),
                        tooltip = { PlainTooltip { Text("Menu") } },
                        state = rememberTooltipState(),
                    ) {
                        IconButton(onClick = { /* doSomething() */ }) {
                            Icon(imageVector = Icons.Filled.Menu, contentDescription = "Menu")
                        }
                    }
                },
                actions = {
                    TooltipBox(
                        positionProvider =
                            TooltipDefaults.rememberTooltipPositionProvider(
                                TooltipAnchorPosition.Above
                            ),
                        tooltip = { PlainTooltip { Text("Add to favorites") } },
                        state = rememberTooltipState(),
                    ) {
                        IconButton(onClick = { /* doSomething() */ }) {
                            Icon(
                                imageVector = Icons.Filled.Favorite,
                                contentDescription = "Add to favorites",
                            )
                        }
                    }
                },
                scrollBehavior = scrollBehavior,
            )
        },
        content = { innerPadding ->
            LazyColumn(
                contentPadding = innerPadding,
                verticalArrangement = Arrangement.spacedBy(8.dp),
            ) {
                val list = (0..75).map { it.toString() }
                items(count = list.size) {
                    Text(
                        text = list[it],
                        style = MaterialTheme.typography.bodyLarge,
                        modifier = Modifier.fillMaxWidth().padding(horizontal = 16.dp),
                    )
                }
            }
        },
    )
}