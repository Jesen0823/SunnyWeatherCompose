`androidx.compose.material3.adaptive adaptive-navigation3` **vs** `material3-adaptive-navigation-suite`
          
## 核心区别

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

## 使用建议

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

## 总结

| 库 | 层级 | 复杂度 | 适用人群 |
|----|------|--------|----------|
| `adaptive-navigation3` | 底层 | 高 | 高级开发者，需要深度定制 |
| `material3-adaptive-navigation-suite` | 高层 | 低 | 大多数开发者，快速构建 |

**最终建议**：如果你不确定选择哪个，**优先使用 `material3-adaptive-navigation-suite`**，它提供了更好的开箱即用体验。只有在需要高度自定义时才考虑 `adaptive-navigation3`。