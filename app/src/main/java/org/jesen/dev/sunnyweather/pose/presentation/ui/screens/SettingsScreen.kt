package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Delete
import androidx.compose.material3.Icon
import androidx.compose.material3.ListItem
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp

@Composable
fun SettingsScreen(
    onClearPlace: () -> Unit = {},
    onBack: () -> Unit = {}
) {
    androidx.compose.foundation.lazy.LazyColumn(modifier = Modifier.fillMaxSize()) {
        item {
            Surface(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(horizontal = 16.dp, vertical = 4.dp),
                onClick = onClearPlace
            ) {
                ListItem(
                    headlineContent = {
                        Text(
                            text = "清除缓存",
                            style = MaterialTheme.typography.bodyLarge
                        )
                    },
                    trailingContent = {
                        Icon(
                            imageVector = Icons.Default.Delete,
                            contentDescription = "清除"
                        )
                    }
                )
            }
        }
        item {
            ListItem(
                headlineContent = {
                    Text(
                        text = "关于我们",
                        style = MaterialTheme.typography.bodyLarge
                    )
                },
                modifier = Modifier.padding(horizontal = 16.dp)
            )
        }
        item {
            ListItem(
                headlineContent = {
                    Text(
                        text = "版本信息",
                        style = MaterialTheme.typography.bodyLarge
                    )
                },
                supportingContent = {
                    Text(text = "1.0.0")
                },
                modifier = Modifier.padding(horizontal = 16.dp)
            )
        }
    }
}