package org.jesen.dev.sunnyweather.relationdemo.navigation_demo.ui.screens

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.relationdemo.navigation_demo.route.ProductDetail

@Composable
fun ProductListScreen(
    products: List<org.jesen.dev.sunnyweather.relationdemo.navigation_demo.route.ProductDetail>,
    onProductClick: (org.jesen.dev.sunnyweather.relationdemo.navigation_demo.route.ProductDetail) -> Unit,
    modifier: Modifier = Modifier
) {
    LazyColumn(modifier = modifier.fillMaxSize()) {
        items(products) { product ->
            Text(
                text = product.name,
                modifier = Modifier
                    .fillMaxSize()
                    .clickable { onProductClick(product) }
                    .padding(16.dp)
            )
        }
    }
}