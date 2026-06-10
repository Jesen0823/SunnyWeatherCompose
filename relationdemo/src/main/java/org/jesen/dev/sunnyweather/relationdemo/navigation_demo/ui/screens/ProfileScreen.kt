package org.jesen.dev.sunnyweather.relationdemo.navigation_demo.ui.screens

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.relationdemo.R

@Composable
fun ProfileScreen() {
    Column(
        modifier = Modifier.fillMaxSize(),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Image(
            painter = painterResource(id = R.drawable.bg_place),
            contentDescription = "Profile",
            modifier = Modifier
                .size(120.dp)
                .clip(androidx.compose.foundation.shape.CircleShape)
        )
        Text(
            text = "用户昵称",
            style = MaterialTheme.typography.headlineLarge,
            modifier = Modifier.padding(top = 16.dp)
        )
        Text(
            text = "用户ID: 123456",
            style = MaterialTheme.typography.bodyLarge,
            modifier = Modifier.padding(top = 8.dp)
        )
    }
}