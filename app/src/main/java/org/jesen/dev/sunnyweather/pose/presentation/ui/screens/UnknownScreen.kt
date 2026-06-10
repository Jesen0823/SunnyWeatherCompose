package org.jesen.dev.sunnyweather.pose.presentation.ui.screens

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.size
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.R

@Composable
fun UnknownScreen(
    onOpenLocationSettings: () -> Unit,
    onSelectCity: () -> Unit
) {
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
        
        Spacer(modifier = Modifier.size(24.dp))
        
        Text(
            text = stringResource(id = R.string.unknown_title),
            style = MaterialTheme.typography.titleLarge,
            fontWeight = FontWeight.Bold
        )
        
        Spacer(modifier = Modifier.size(8.dp))
        
        Text(
            text = stringResource(id = R.string.unknown_message),
            style = MaterialTheme.typography.bodyMedium,
            color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.6f)
        )
        
        Spacer(modifier = Modifier.size(32.dp))
        
        Button(
            onClick = onOpenLocationSettings,
            modifier = Modifier.size(width = 200.dp, height = 48.dp)
        ) {
            Text(text = stringResource(id = R.string.unknown_open_location))
        }
        
        Spacer(modifier = Modifier.size(16.dp))
        
        Button(
            onClick = onSelectCity,
            modifier = Modifier.size(width = 200.dp, height = 48.dp),
            enabled = true
        ) {
            Text(text = stringResource(id = R.string.unknown_select_city))
        }
    }
}