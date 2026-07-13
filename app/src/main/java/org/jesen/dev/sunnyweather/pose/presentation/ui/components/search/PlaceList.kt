/**
 * 城市列表组件
 * 
 * 主要职责：
 * - 显示搜索结果中的城市列表
 * - 支持点击选择城市
 * 
 * 技术要点：
 * - 使用 LazyColumn 实现滚动列表
 * - 通过 items 函数遍历城市列表
 * - 使用 Surface 包装 ListItem，提供点击反馈
 * - 显示城市名称和地址信息
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components.search

import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.ListItem
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.domain.model.Place

@Composable
fun PlaceList(
    places: List<Place>,
    onPlaceSelected: (Place) -> Unit
) {
    LazyColumn(modifier = Modifier.fillMaxSize()) {
        items(places, key = { place -> place.name }) { place ->
            Surface(
                modifier = Modifier
                    .fillMaxSize()
                    .padding(horizontal = 16.dp, vertical = 4.dp),
                onClick = { onPlaceSelected(place) }
            ) {
                ListItem(
                    headlineContent = {
                        Text(
                            text = place.name,
                            style = MaterialTheme.typography.bodyLarge
                        )
                    },
                    supportingContent = {
                        Text(
                            text = place.address,
                            style = MaterialTheme.typography.bodyMedium
                        )
                    }
                )
            }
        }
    }
}
