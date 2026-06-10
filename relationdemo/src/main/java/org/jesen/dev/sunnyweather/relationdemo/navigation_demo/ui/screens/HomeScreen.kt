package org.jesen.dev.sunnyweather.relationdemo.navigation_demo.ui.screens

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.unit.dp

@Composable
fun HomeScreen(
    onNavigateToWeather: (String) -> Unit,
    onNavigateToSettings: () -> Unit,
    onNavigateToProfile: () -> Unit,
    onShowDialog: () -> Unit
) {
    val cityInput = remember { mutableStateOf("") }
    
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text(
            text = "Navigation3 高级特性演示",
            style = MaterialTheme.typography.headlineLarge,
            modifier = Modifier.padding(bottom = 32.dp)
        )
        
        OutlinedTextField(
            value = cityInput.value,
            onValueChange = { cityInput.value = it },
            label = { Text("请输入城市名称") },
            placeholder = { Text("例如：Beijing, Shanghai") },
            keyboardOptions = KeyboardOptions(
                imeAction = ImeAction.Done
            ),
            keyboardActions = KeyboardActions(
                onDone = {
                    if (cityInput.value.isNotBlank()) {
                        onNavigateToWeather(cityInput.value.trim())
                    }
                }
            ),
            modifier = Modifier
                .fillMaxWidth()
                .padding(bottom = 16.dp),
            singleLine = true
        )
        
        Button(
            onClick = {
                val city = cityInput.value.trim()
                if (city.isNotBlank()) {
                    onNavigateToWeather(city)
                }
            },
            enabled = cityInput.value.isNotBlank(),
            modifier = Modifier.padding(vertical = 8.dp)
        ) {
            Text("跳转到天气页面 (带参数)")
        }
        
        Button(
            onClick = onNavigateToSettings,
            modifier = Modifier.padding(vertical = 8.dp)
        ) {
            Text("跳转到设置页面")
        }
        
        Button(
            onClick = onNavigateToProfile,
            modifier = Modifier.padding(vertical = 8.dp)
        ) {
            Text("跳转到个人中心")
        }
        
        Button(
            onClick = onShowDialog,
            modifier = Modifier.padding(vertical = 8.dp)
        ) {
            Text("显示 Overlay 弹窗")
        }
    }
}