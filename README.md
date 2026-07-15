# 阳光天气



## 📊 动画背景产品决策分析

### 方案对比

| 维度 | **方案A：作为Loading组件** | **方案B：作为常态背景** |
|------|---------------------------|-----------------------|
| **用户停留时间** | 短（仅加载时显示，通常1-3秒） | 长（用户查看天气时持续展示） |
| **视觉价值** | 有限（很快消失） | 高（贯穿整个使用过程） |
| **沉浸感** | 无（过渡状态） | 强（营造天气氛围） |
| **iOS参考** | 无官方先例 | iOS 7+核心设计理念 |
| **扩展性** | 受限（仅loading场景） | 强（可根据天气类型切换动画） |

### 🎯 推荐方案：**方案B - 常态背景**

**设计理由：**

1. **iOS 官方设计范式**
   - iOS 7 起，天气 App 的核心设计理念就是**动态背景模拟当前天气状况**：晴天云朵漂浮、雨天雨滴下落、雷暴闪电闪烁
   - iOS 15 进一步强化：背景随时间变化（白天明亮/夜晚黑暗），随天气模式切换动画

2. **用户体验最大化**
   - 用户查看天气时，**80%以上时间**处于"成功展示"状态
   - 将最美的动画放在用户停留最久的地方，才能发挥最大价值

3. **情感化设计**
   - 天气不只是数据，更是一种**氛围体验**
   - 背景动画能让用户"感受"天气，而不只是"读取"天气

4. **功能扩展性**
   - 后续可根据天气类型动态切换：晴天→云朵、雨天→雨滴、雪天→雪花
   - 形成**"天气状态→视觉反馈"**的闭环体验

### 🎨 设计建议

```
WeatherSuccessContent 背景层设计：

┌─────────────────────────────────┐
│  Layer 1: OpenGL 动画背景        │  ← 云朵/雨滴/雪花动画，半透明
│  (根据天气类型动态切换)          │
├─────────────────────────────────┤
│  Layer 2: 渐变遮罩              │  ← 确保文字可读性
│  (从顶部到底部渐变)             │
├─────────────────────────────────┤
│  Layer 3: 天气卡片内容          │  ← 温度、预报等信息
│  (卡片式布局，带半透明背景)     │
└─────────────────────────────────┘
```

### ⚡ Loading 状态设计建议

如果后续需要优化 Loading 状态，可以采用更轻量化的方案：
- 简洁的进度指示器 + 天气图标动画（如小云朵旋转）
- 避免过度设计，Loading 状态应快速过渡，不应抢夺注意力

### 📝 结论

**推荐将 OpenGL 动画作为天气成功展示的常态背景**，这符合 iOS 官方天气 App 的设计理念，能最大化视觉价值和用户体验，同时具备良好的功能扩展性。




## navigation3区别

`androidx.compose.material3.adaptive adaptive-navigation3` **vs** `material3-adaptive-navigation-suite`

### 1. **`androidx.compose.material3.adaptive:adaptive-navigation3`**

| 特性 | 说明 |
|------|------|
| **用途** | Navigation 3 的 Material3 自适应导航集成层 |
| **定位** | 低层级 API，提供 Navigation 3 与 Material3 自适应系统的桥接 |
| **核心功能** | 支持 `ListDetailPaneScaffold` 与 Navigation 3 的集成 |
| **适用场景** | 需要自定义自适应导航行为的高级场景 |

### 2. **`androidx.compose.material3:material3-adaptive-navigation-suite`**

| 特性 | 说明 |
|------|------|
| **用途** | 提供开箱即用的自适应导航组件套件 |
| **定位** | 高层级 API，封装了完整的自适应导航逻辑 |
| **核心功能** | 包含 `NavigationSuiteScaffold`，自动根据窗口大小切换导航栏/导航轨 |
| **适用场景** | 快速构建自适应应用，无需深入了解底层实现 |

## 功能对比

```
┌─────────────────────────────────────────────────────────────────────┐
│                      Navigation 3 依赖体系                         │
├─────────────────────────────────────────────────────────────────────┤
│  基础导航层                                                         │
│  ├── navigation3-runtime     ← 核心导航运行时                       │
│  └── navigation3-ui          ← Compose UI 集成                     │
├─────────────────────────────────────────────────────────────────────┤
│  自适应导航层 (选其一)                                               │
│  ├── adaptive-navigation3                     ← 低层级，高度可定制    │
│  │   (androidx.compose.material3.adaptive)                         │
│  └── material3-adaptive-navigation-suite     ← 高层级，开箱即用      │
│      (androidx.compose.material3)                                  │
└─────────────────────────────────────────────────────────────────────┘
```

【建议】

### ✅ 如果你使用 **Compose Navigation 3**：

**推荐方案**：根据你的需求选择

| 场景 | 推荐库 | 原因 |
|------|--------|------|
| 快速开发、开箱即用 | `material3-adaptive-navigation-suite` | 提供 `NavigationSuiteScaffold` 自动适配 |
| 高度自定义导航行为 | `adaptive-navigation3` | 提供底层 API 灵活性更高 |
| 仅需基础导航 | `navigation3-runtime` + `navigation3-ui` | 不需要自适应功能时 |

### 📦 推荐依赖配置

```toml
# 基础 Navigation 3
androidx-navigation3-runtime = { module = "androidx.navigation3:navigation3-runtime", version.ref = "nav3Core" }
androidx-navigation3-ui = { module = "androidx.navigation3:navigation3-ui", version.ref = "nav3Core" }

# 自适应导航（二选一）
# 选项A: 高层级套件（推荐）
androidx-material3-adaptive-navigation-suite = { group = "androidx.compose.material3", name = "material3-adaptive-navigation-suite", version.ref = "material3AdaptiveNavSuite" }

# 选项B: 低层级集成
androidx-material3-adaptive-navigation3 = { group = "androidx.compose.material3.adaptive", name = "adaptive-navigation3", version.ref = "material3AdaptiveNav3" }
```

### 🚀 最佳实践

根据 Google Codelab 的官方推荐：

```kotlin
// 使用 navigation3-ui 进行基础导航
val navigator = rememberNavigator<MyScreen>()

// 使用 material3-adaptive-navigation-suite 的 NavigationSuiteScaffold
// 自动根据窗口大小切换 BottomNavigation / NavigationRail
NavigationSuiteScaffold(
    navigationSuiteItems = { /* 导航项 */ }
) {
    NavDisplay(navigator) { screen ->
        when (screen) { ... }
    }
}
```

### 总结

| 库 | 层级 | 复杂度 | 适用人群 |
|----|------|--------|----------|
| `adaptive-navigation3` | 底层 | 高 | 高级开发者，需要深度定制 |
| `material3-adaptive-navigation-suite` | 高层 | 低 | 大多数开发者，快速构建 |

**最终建议**：如果你不确定选择哪个，**优先使用 `material3-adaptive-navigation-suite`**，它提供了更好的开箱即用体验。只有在需要高度自定义时才考虑 `adaptive-navigation3`。