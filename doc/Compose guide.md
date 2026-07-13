# Jetpack Compose 性能优化指南

## 一、核心概念：重组机制

### 1.1 重组工作流程
- 状态修改 → 快照系统记录变更 → 通知观察者 → 执行重组 → 应用 UI 变更
- 当状态在 `@Composable` 函数内部或点击事件中更新时，会分别使用 `MutableSnapshot` 和 `GlobalSnapshot`

### 1.2 重组范围确定
- Compose 会智能确定重组范围，只重新执行受状态变化影响的组件
- 如果状态对象设计不合理，可能导致整个 UI 树的重新渲染

## 二、关键优化策略

### 2.1 类型稳定性 (Stability)

#### 稳定类型的定义
一个稳定的类型必须遵守以下契约：
1. 对于相同的两个实例，`equals()` 的结果必须始终一致
2. 如果类型的某个公共属性发生了变化，Compose 必须能够收到通知（例如使用 `MutableState`）
3. 其所有公共属性的类型也都必须是稳定的

#### 不稳定的陷阱
- **陷阱 A：普通的 Data Class** - 即使属性未变，只要传入新实例，Compose 可能被迫重组
- **陷阱 B：标准集合 (List, Map, Set)** - Compose 默认将标准 Kotlin 集合视为不稳定

#### 解决方案
- 使用 `@Stable` 或 `@Immutable` 注解标记数据类
- 使用 `kotlinx.collections.immutable` 库的不可变集合

### 2.2 使用 remember 存储计算结果
对于耗时计算或频繁使用的对象，使用 `remember` 可以避免在每次重组时重复创建：

```kotlin
val buttonText = remember(count) { "Clicked $count times" }
Button(onClick = { count++ }) {
    Text(buttonText)
}
```

### 2.3 合理使用 key 参数
在 `LazyColumn` 等列表组件中，为每个项指定稳定的 `key`：

```kotlin
LazyColumn {
    items(users, key = { it.id }) { user ->
        UserItem(user)
    }
}
```

### 2.4 使用 derivedStateOf 优化派生状态
对于依赖其他状态计算得出的状态，使用 `derivedStateOf` 可以避免源状态变化时的不必要重组：

```kotlin
val searchResults by remember {
    derivedStateOf {
        items.filter { it.contains(query.value) }
    }
}
```

### 2.5 拆分细粒度 Composable 函数
将依赖不同状态的 UI 拆分为独立的 Composable，缩小重组范围：

```kotlin
// 优化前：一个函数依赖多个状态
@Composable
fun UserInfo(user: User, count: Int) {
    Column {
        Text("姓名：${user.name}") // 依赖user
        Text("计数：$count") // 依赖count
    }
}

// 优化后：拆分为独立函数
@Composable
fun UserInfo(user: User, count: Int) {
    Column {
        UserNameText(user)
        CountText(count)
    }
}
```

### 2.6 缓存临时对象/lambda
Composable 函数内创建的 lambda、匿名对象、集合等，每次重组会生成新实例：

```kotlin
// 优化前：每次重组生成新lambda
Button(onClick = { viewModel.fetchData() }) { Text("刷新") }

// 优化后：用remember缓存lambda
val fetchData = remember { { viewModel.fetchData() } }
Button(onClick = fetchData) { Text("刷新") }
```

### 2.7 推迟状态读取 (Lambda Modifier)
使用 Lambda 版本的 Modifier，将状态读取推迟到绘制阶段：

```kotlin
// 常规做法：状态读取在重组阶段
Modifier.offset(x = offset.dp)

// 优化做法：状态读取推迟到绘制阶段
Modifier.graphicsLayer { translationX = offset }
```

## 三、性能问题诊断工具

### 3.1 Layout Inspector
- 开启 "Show Recomposition Counts"
- 观察指标：Recomposition count、Skipped count

### 3.2 RecomposeHighlighter
通过添加 `RecomposeHighlighter` 修饰符，直观看到哪些组件正在重组：

```kotlin
Text(
    text = "Hello",
    modifier = Modifier.then(RecomposeHighlighter)
)
```

## 四、性能优化 Checklist

1. [ ] 所有数据类标记 `@Stable`/`@Immutable`
2. [ ] 不在 Composable 中执行耗时操作
3. [ ] 使用 `remember` 缓存昂贵计算结果
4. [ ] 使用 `derivedStateOf` 优化派生状态
5. [ ] 在 `LazyColumn`/`LazyRow` 中提供唯一的 `key`
6. [ ] 拆分细粒度 Composable 函数
7. [ ] 缓存 lambda 和临时对象
8. [ ] 避免使用不稳定的集合类型作为参数
9. [ ] Release 模式下评估性能
10. [ ] 使用 R8/Proguard 进行混淆优化

## 五、关键原则总结

1. **最小化重组范围**：让 Compose 只重组真正需要变化的部分
2. **确保数据稳定性**：使用 `@Stable`/`@Immutable` 标记数据类
3. **优化状态管理**：合理使用 `remember`、`key` 和 `derivedStateOf`
4. **保持组件小型化**：拆分细粒度组件，限制重组范围
5. **避免昂贵操作**：将耗时计算移到 ViewModel 或使用 `remember`
