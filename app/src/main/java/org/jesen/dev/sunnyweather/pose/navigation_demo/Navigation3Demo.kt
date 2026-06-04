package org.jesen.dev.sunnyweather.pose.navigation_demo

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.material3.Text
import androidx.compose.material3.adaptive.ExperimentalMaterial3AdaptiveApi
import androidx.compose.material3.adaptive.navigation3.ListDetailSceneStrategy
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.sp
import androidx.navigation3.runtime.NavEntry
import androidx.navigation3.runtime.rememberNavBackStack
import androidx.navigation3.ui.NavDisplay
import org.jesen.dev.sunnyweather.pose.navigation_demo.route.ProductDetail
import org.jesen.dev.sunnyweather.pose.navigation_demo.route.ProductList
import org.jesen.dev.sunnyweather.pose.navigation_demo.ui.screens.ProductDetailScreen
import org.jesen.dev.sunnyweather.pose.navigation_demo.ui.screens.ProductListScreen

@OptIn(ExperimentalMaterial3AdaptiveApi::class)
@Composable
fun Navigation3Demo(modifier: Modifier = Modifier) {
    val backStack = rememberNavBackStack(ProductList)

    NavDisplay(
        backStack = backStack,
        onBack = { backStack.removeLastOrNull() },
        entryProvider = { key ->
            when (key) {
                is ProductList -> NavEntry(
                    key,
                    metadata = ListDetailSceneStrategy.listPane(
                        detailPlaceholder = {
                            Column(
                                modifier = Modifier
                                    .fillMaxSize(),
                                verticalArrangement = Arrangement.Center,
                            ) {
                                Text("Choose a product from the list", fontSize = 26.sp)
                            }
                        }
                    )
                ) {
                    val products = mutableListOf<ProductDetail>()
                    repeat(50) {
                        products.add(ProductDetail("Product $it", "Description of Product $it"))
                    }

                    ProductListScreen(
                        products = products,
                        onProductClick = { product ->
                            backStack.add(ProductDetail(product.name, product.description))
                        },
                        modifier
                    )
                }

                is ProductDetail -> NavEntry(key,metadata = ListDetailSceneStrategy.detailPane()) {
                    ProductDetailScreen(
                        product = ProductDetail(key.name, key.description)
                    )
                }

                else -> error("Unknown key: $key")
            }
        }
    )
}