package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.automirrored.filled.NavigateBefore
import androidx.compose.material3.CenterAlignedTopAppBar
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.jesen.dev.gllib.Constants
import org.jesen.dev.sunnyweather.pose.presentation.ui.widget.GLView

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun AboutUsScreen(onBack: () -> Unit = {}) {
    Scaffold(
        topBar = {
            CenterAlignedTopAppBar(
                title = {
                    Text(
                        text = "关于我们",
                        style = MaterialTheme.typography.titleLarge
                    )
                },
                navigationIcon = {
                    IconButton(onClick = onBack) {
                        Icon(
                            imageVector = Icons.AutoMirrored.Filled.NavigateBefore,
                            contentDescription = "返回"
                        )
                    }
                },
                colors = TopAppBarDefaults.topAppBarColors(
                    containerColor = MaterialTheme.colorScheme.surfaceContainer
                )
            )
        }
    ) { paddingValues ->
        GLView(
            rendererType = Constants.RENDERER_TYPE_KEY_CLOUD,
            textureResId = com.jesen.dev.gllib.R.drawable.noise,
            modifier = Modifier
                .fillMaxWidth()
                .height(500.dp)
                .padding(paddingValues)
                .padding(horizontal = 16.dp, vertical = 16.dp)
        )
    }
}