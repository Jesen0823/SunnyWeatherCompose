/**
 * 搜索栏组件
 * 
 * 主要职责：
 * - 提供城市搜索输入框
 * - 支持键盘搜索操作
 * - 实时响应搜索关键词变化
 * 
 * 技术要点：
 * - 基于 Material3 OutlinedTextField 实现
 * - 使用 Icons.Default.Search 作为前置图标
 * - 配置 imeAction = ImeAction.Search 支持搜索按键
 * - 通过 KeyboardActions 处理搜索按键点击，自动隐藏键盘
 */
package org.jesen.dev.sunnyweather.pose.presentation.ui.components.search

import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Search
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.unit.dp
import org.jesen.dev.sunnyweather.pose.R

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SearchBar(
    query: String,
    onQueryChange: (String) -> Unit
) {
    val keyboardController = LocalSoftwareKeyboardController.current

    OutlinedTextField(
        value = query,
        onValueChange = onQueryChange,
        leadingIcon = {
            Icon(
                imageVector = Icons.Default.Search,
                contentDescription = "搜索",
                modifier = Modifier.size(24.dp)
            )
        },
        placeholder = {
            Text(text = stringResource(id = R.string.search_hint))
        },
        keyboardOptions = KeyboardOptions(imeAction = ImeAction.Search),
        keyboardActions = KeyboardActions(
            onSearch = { keyboardController?.hide() }
        ),
        modifier = Modifier
            .fillMaxWidth()
            .padding(16.dp)
    )
}
