# 彩云天气 API v2.6 接口文档

## 概述

彩云天气 API v2.6 提供了多种天气数据接口，涵盖实时天气、分钟级降水、小时级预报、天级预报和预警数据。所有接口均支持 App Key & App Secret 认证方式。

**Base URL**: `https://api.caiyunapp.com/v2.6/{app_key}/{longitude},{latitude}/{endpoint}`

## 接口列表

### 1. 实况数据 (realtime)

**接口路径**: `/v2.6/{app_key}/{lng},{lat}/realtime`

**更新频率**: 1 分钟

**空间分辨率**: 1 km × 1 km

**功能描述**: 获取当前时刻的实时天气数据，包括温度、湿度、风速、风向、天气现象、空气质量等。

**返回字段**:

| JSONPath               | 字段说明               | 使用场景       |
| ---------------------- | ---------------------- | -------------- |
| `temperature`          | 地表 2 米气温          | 显示当前温度   |
| `apparent_temperature` | 体感温度               | 显示体感温度   |
| `humidity`             | 地表 2 米相对湿度(0-1) | 显示湿度百分比 |
| `wind.speed`           | 地表 10 米风速         | 显示风速       |
| `wind.direction`       | 地表 10 米风向         | 显示风向       |
| `skycon`               | 天气现象代码           | 显示天气图标   |

**skycon 天气现象代码枚举：**

| skycon 值             | 显示文本 | 说明                    |
| --------------------- | -------- | ----------------------- |
| `CLEAR_DAY`           | 晴       | 白天，云量 < 0.2        |
| `CLEAR_NIGHT`         | 晴       | 夜间，云量 < 0.2        |
| `PARTLY_CLOUDY_DAY`   | 多云     | 白天，0.8 >= 云量 > 0.2 |
| `PARTLY_CLOUDY_NIGHT` | 多云     | 夜间，0.8 >= 云量 > 0.2 |
| `CLOUDY`              | 阴       | 云量 > 0.8              |
| `LIGHT_HAZE`          | 轻度雾霾 | PM2.5 100~150           |
| `MODERATE_HAZE`       | 中度雾霾 | PM2.5 150~200           |
| `HEAVY_HAZE`          | 重度雾霾 | PM2.5 > 200             |
| `LIGHT_RAIN`          | 小雨     | 小雨天气                |
| `MODERATE_RAIN`       | 中雨     | 中雨天气                |
| `HEAVY_RAIN`          | 大雨     | 大雨天气                |
| `STORM_RAIN`          | 暴雨     | 暴雨天气                |
| `FOG`                 | 雾       | 雾天                    |
| `LIGHT_SNOW`          | 小雪     | 小雪天气                |
| `MODERATE_SNOW`       | 中雪     | 中雪天气                |
| `HEAVY_SNOW`          | 大雪     | 大雪天气                |
| `STORM_SNOW`          | 暴雪     | 暴雪天气                |
| `DUST`                | 浮尘     | 浮尘天气                |
| `SAND`                | 沙尘     | 沙尘天气                |
| `WIND`                | 大风     | 大风天气                |

**天气现象优先级（从高到低）：**
降雪 > 降雨 > 雾 > 沙尘 > 浮尘 > 雾霾 > 大风 > 阴 > 多云 > 晴

**sky.info（天气显示文本）字段取值：**

`sky.info` 对应代码中 `Sky.getSky(weather.realtime.skycon).info` 的返回值，共有以下 14 种可能取值：

| 显示文本   | 对应的 skycon 值                           | 说明          |
| ---------- | ------------------------------------------ | ------------- |
| `晴`       | `CLEAR_DAY`, `CLEAR_NIGHT`                 | 白天/夜间晴天 |
| `多云`     | `PARTLY_CLOUDY_DAY`, `PARTLY_CLOUDY_NIGHT` | 白天/夜间多云 |
| `阴`       | `CLOUDY`                                   | 阴天          |
| `轻度雾霾` | `LIGHT_HAZE`                               | PM2.5 100~150 |
| `中度雾霾` | `MODERATE_HAZE`                            | PM2.5 150~200 |
| `重度雾霾` | `HEAVY_HAZE`                               | PM2.5 > 200   |
| `小雨`     | `LIGHT_RAIN`                               | 小雨天气      |
| `中雨`     | `MODERATE_RAIN`                            | 中雨天气      |
| `大雨`     | `HEAVY_RAIN`                               | 大雨天气      |
| `暴雨`     | `STORM_RAIN`                               | 暴雨天气      |
| `雾`       | `FOG`                                      | 雾天          |
| `小雪`     | `LIGHT_SNOW`                               | 小雪天气      |
| `中雪`     | `MODERATE_SNOW`                            | 中雪天气      |
| `大雪`     | `HEAVY_SNOW`                               | 大雪天气      |
| `暴雪`     | `STORM_SNOW`                               | 暴雪天气      |
| `浮尘`     | `DUST`                                     | 浮尘天气      |
| `沙尘`     | `SAND`                                     | 沙尘天气      |
| `大风`     | `WIND`                                     | 大风天气      |

**代码使用示例：**

```kotlin
// 获取天气枚举对象
val sky = Sky.getSky(weather.realtime.skycon)  // 如 weather.realtime.skycon = "CLEAR_DAY"

// sky.info 返回值："晴"（字符串）
text = sky.info  // CurrentWeatherCard.kt#L126

// sky.icon 返回值：R.drawable.ic_clear_day（图标资源ID）
painter = painterResource(id = sky.icon)  // CurrentWeatherCard.kt#L116
```

| `cloudrate` | 总云量(0.0-1.0) | 显示云量 |
| `visibility` | 地表水平能见度 | 显示能见度 |
| `pressure` | 地面气压 | 显示气压 |
| `dswrf` | 向下短波辐射通量 | 计算紫外线强度 |
| `precipitation.local.intensity` | 本地降水强度 | 显示是否正在下雨 |
| `precipitation.nearest.distance` | 最近降水带距离 | 显示附近降水情况 |
| `air_quality.pm25` | PM2.5 浓度 | 显示空气质量 |
| `air_quality.aqi.chn` | 国标 AQI | 显示空气质量指数 |
| `life_index.ultraviolet.desc` | 紫外线指数描述 | 显示紫外线指数 |
| `life_index.comfort.desc` | 舒适度指数描述 | 显示舒适度 |

**使用场景**:

- 显示当前天气状况
- 实时更新天气信息
- 桌面小部件显示当前天气

---

### 2. 分钟级降水 (minutely)

**接口路径**: `/v2.6/{app_key}/{lng},{lat}/minutely`

**更新频率**: 1 分钟

**空间分辨率**: 1 km × 1 km

**访问限制**: 增值服务，仅企业套餐可用

**功能描述**: 获取未来2小时的分钟级降水预测，基于雷达数据。

**返回字段**:

| JSONPath           | 字段说明                  | 使用场景      |
| ------------------ | ------------------------- | ------------- |
| `precipitation`    | 未来1小时每分钟降水强度   | 短时降水预警  |
| `precipitation_2h` | 未来2小时每分钟降水强度   | 2小时降水预测 |
| `probability`      | 未来2小时每半小时降水概率 | 降水概率显示  |
| `description`      | 近2小时降水描述           | 口语化提示    |

**使用场景**:

- 短时降水预警（"未来10分钟有雨"）
- 精准降水时间预测
- 用户出门前判断是否需要带伞

---

### 3. 小时级预报 (hourly)

**接口路径**: `/v2.6/{app_key}/{lng},{lat}/hourly`

**更新频率**: 近时效5-15分钟，其余1小时级

**空间分辨率**: 9-13 km（前2小时降水可细化至1km）

**时间范围**: 未来15天逐小时

**功能描述**: 获取逐小时的天气预报，包括温度、降水、风速等。

**请求参数**:

| 参数          | 类型 | 默认值 | 说明                     |
| ------------- | ---- | ------ | ------------------------ |
| `hourlysteps` | int  | 24     | 返回小时数，范围[1, 360] |

**返回字段**:

| JSONPath                     | 字段说明           | 使用场景       |
| ---------------------------- | ------------------ | -------------- |
| `temperature.value`          | 地表2米气温        | 逐小时温度曲线 |
| `apparent_temperature.value` | 体感温度           | 逐小时体感温度 |
| `precipitation.value`        | 降水量             | 逐小时降水预测 |
| `precipitation.probability`  | 降水概率(0-100%)   | 逐小时降水概率 |
| `wind.speed`                 | 风速               | 逐小时风速     |
| `wind.direction`             | 风向               | 逐小时风向     |
| `humidity.value`             | 相对湿度           | 逐小时湿度     |
| `skycon.value`               | 天气现象           | 逐小时天气图标 |
| `cloudrate.value`            | 云量               | 逐小时云量     |
| `pressure.value`             | 气压               | 逐小时气压     |
| `visibility.value`           | 能见度             | 逐小时能见度   |
| `air_quality.aqi.value.chn`  | 国标AQI            | 逐小时空气质量 |
| `description`                | 未来24小时天气描述 | 口语化总结     |
| `forecast_keypoint`          | 预报关键点         | 重点提示       |

**使用场景**:

- 查看未来24小时天气变化
- 制作温度趋势图
- 判断何时下雨、气温变化

---

### 4. 天级别预报 (daily)

**接口路径**: `/v2.6/{app_key}/{lng},{lat}/daily`

**更新频率**: 2 小时

**空间分辨率**: 12 km × 12 km

**时间范围**: 未来15天逐日

**功能描述**: 获取每日的天气预报，包括最高/最低气温、天气现象、生活指数等。

**请求参数**:

| 参数         | 类型 | 默认值 | 说明                  |
| ------------ | ---- | ------ | --------------------- |
| `dailysteps` | int  | 7      | 返回天数，范围[1, 15] |

**返回字段**:

| JSONPath                      | 字段说明               | 使用场景         |
| ----------------------------- | ---------------------- | ---------------- |
| `temperature.max/min/avg`     | 全天最高/最低/平均气温 | 显示每日温度范围 |
| `temperature_08h_20h`         | 白天(08-20时)气温      | 白天温度预测     |
| `temperature_20h_32h`         | 夜晚(20-次日08时)气温  | 夜晚温度预测     |
| `precipitation.max/min/avg`   | 全天降水量             | 显示降水情况     |
| `precipitation.probability`   | 降水概率               | 显示降水概率     |
| `precipitation_08h_20h`       | 白天降水数据           | 白天降水预测     |
| `precipitation_20h_32h`       | 夜晚降水数据           | 夜晚降水预测     |
| `wind.max/min/avg.speed`      | 风速                   | 显示风速         |
| `wind.max/min/avg.direction`  | 风向                   | 显示风向         |
| `humidity.max/min/avg`        | 相对湿度               | 显示湿度         |
| `cloudrate.max/min/avg`       | 云量                   | 显示云量         |
| `pressure.max/min/avg`        | 气压                   | 显示气压         |
| `visibility.max/min/avg`      | 能见度                 | 显示能见度       |
| `dswrf.max/min/avg`           | 向下短波辐射通量       | 计算紫外线       |
| `skycon.value`                | 全天主要天气现象       | 显示天气图标     |
| `skycon_08h_20h.value`        | 白天天气现象           | 白天天气图标     |
| `skycon_20h_32h.value`        | 夜晚天气现象           | 夜晚天气图标     |
| `astro.sunrise.time`          | 日出时间               | 显示日出时间     |
| `astro.sunset.time`           | 日落时间               | 显示日落时间     |
| `air_quality.aqi`             | AQI数据                | 显示空气质量     |
| `air_quality.pm25`            | PM2.5浓度              | 显示PM2.5        |
| `life_index.ultraviolet.desc` | 紫外线指数描述         | 显示紫外线指数   |
| `life_index.dressing.desc`    | 穿衣指数描述           | 显示穿衣建议     |
| `life_index.comfort.desc`     | 舒适度指数描述         | 显示舒适度       |
| `life_index.coldRisk.desc`    | 感冒指数描述           | 显示感冒风险     |
| `life_index.carWashing.desc`  | 洗车指数描述           | 显示洗车建议     |

**使用场景**:

- 查看未来7-15天天气预报
- 显示每日温度范围和天气状况
- 提供生活指数建议（穿衣、洗车、紫外线等）
- 日出日落时间显示

---

### 5. 预警数据 (alert)

**接口路径**: 在任意接口后添加 `?alert=true`

**更新频率**: 同中央气象台同步

**访问限制**: 增值服务，不适用免费额度

**功能描述**: 获取天气预警信息，如台风、暴雨、大风等预警。

**返回字段**:

| JSONPath                       | 字段说明   | 使用场景     |
| ------------------------------ | ---------- | ------------ |
| `alert.content[].title`        | 预警标题   | 显示预警标题 |
| `alert.content[].description`  | 预警描述   | 显示预警详情 |
| `alert.content[].level`        | 预警级别   | 显示预警等级 |
| `alert.content[].type`         | 预警类型   | 显示预警类型 |
| `alert.content[].pubtimestamp` | 发布时间戳 | 显示发布时间 |
| `alert.content[].location`     | 预警地点   | 显示预警区域 |

**预警类型编码**:

| 编码 | 预警类型 |
| ---- | -------- |
| 01   | 台风     |
| 02   | 暴雨     |
| 03   | 暴雪     |
| 04   | 寒潮     |
| 05   | 大风     |
| 06   | 沙尘暴   |
| 07   | 高温     |
| 08   | 干旱     |
| 09   | 雷电     |
| 10   | 冰雹     |
| 11   | 霜冻     |
| 12   | 大雾     |
| 13   | 霾       |
| 14   | 道路结冰 |
| 15   | 森林火险 |
| 16   | 雷雨大风 |

**预警级别编码**:

| 编码 | 预警级别 |
| ---- | -------- |
| 00   | 白色     |
| 01   | 蓝色     |
| 02   | 黄色     |
| 03   | 橙色     |
| 04   | 红色     |

**使用场景**:

- 显示天气预警通知
- 紧急天气事件提醒
- 预警信息详情页面

---

### 6. 综合接口 (weather)

**接口路径**: `/v2.6/{app_key}/{lng},{lat}/weather`

**功能描述**: 打包获取 realtime、minutely、hourly、daily 和 alert 数据，一次请求获取所有天气信息。

**请求参数**:

| 参数          | 类型 | 默认值 | 说明             |
| ------------- | ---- | ------ | ---------------- |
| `alert`       | bool | false  | 是否包含预警数据 |
| `dailysteps`  | int  | 7      | 返回天数         |
| `hourlysteps` | int  | 24     | 返回小时数       |

**返回结构**:

```json
{
  "result": {
    "alert": { ... },      // 预警数据（如果alert=true）
    "realtime": { ... },   // 实时天气数据
    "minutely": { ... },   // 分钟级降水数据
    "hourly": { ... },     // 小时级预报数据
    "daily": { ... },      // 天级别预报数据
    "primary": 0,
    "forecast_keypoint": "..."
  }
}
```

**使用场景**:

- 一次性获取所有天气数据
- 减少网络请求次数
- 页面初始化时加载完整天气信息

---

## 认证方式

### App Key & App Secret 认证（推荐）

**请求格式**:

```
GET https://api.caiyunapp.com/v2.6/{app_key}/{lng},{lat}/{endpoint}
Headers:
  x-cy-nonce: {随机字符串}
  x-cy-timestamp: {时间戳}
  x-cy-signature: {签名}
```

**签名计算步骤**:

1. 将 query 参数按字母顺序排序
2. 构建签名字符串: `{method}:{path}:{query}:{app_key}:{nonce}:{timestamp}`
3. 使用 HMAC-SHA256 计算哈希值（密钥为 app_secret）
4. 使用 URL Safe Base64 编码得到签名

### Token 认证（不建议）

**请求格式**:

```
GET https://api.caiyunapp.com/v2.6/{token}/{lng},{lat}/{endpoint}
```

---

## 数据时间范围说明

| 接口     | 时间范围       |
| -------- | -------------- |
| realtime | 当前时刻       |
| minutely | 未来2小时      |
| hourly   | 未来15天逐小时 |
| daily    | 未来15天逐日   |
| alert    | 当前有效预警   |

---

## 空间分辨率说明

| 接口     | 空间分辨率                        |
| -------- | --------------------------------- |
| realtime | 1 km × 1 km                       |
| minutely | 1 km × 1 km                       |
| hourly   | 9-13 km（前2小时降水可细化至1km） |
| daily    | 12 km × 12 km                     |
| alert    | 按行政区划                        |

---

## 使用建议

### 场景一：天气首页

**推荐接口**: `weather` 综合接口

- 一次性获取实时天气 + 7日预报 + 24小时预报
- 减少网络请求次数，提升页面加载速度

### 场景二：实时天气更新

**推荐接口**: `realtime` 实况接口

- 更新频率1分钟，适合实时刷新
- 数据量小，响应快

### 场景三：降水预警

**推荐接口**: `minutely` 分钟级降水接口（需企业套餐）

- 未来2小时分钟级降水预测
- 精准判断何时下雨

### 场景四：小时级趋势

**推荐接口**: `hourly` 小时级预报接口

- 查看未来24小时天气变化
- 制作温度趋势图

### 场景五：多日预报

**推荐接口**: `daily` 天级别预报接口

- 查看未来15天天气预报
- 获取生活指数建议

### 场景六：预警通知

**推荐接口**: 任意接口 + `?alert=true`

- 获取天气预警信息
- 紧急事件提醒

---

## 错误码说明

| 错误码 | 说明       | 处理建议                       |
| ------ | ---------- | ------------------------------ |
| 401    | 认证失败   | 检查 app_key、app_secret、签名 |
| 403    | 权限不足   | 检查是否有权限访问该接口       |
| 404    | 接口不存在 | 检查接口路径是否正确           |
| 429    | 频率限制   | 减少请求频率，实现重试机制     |
| 500    | 服务器错误 | 稍后重试                       |

## 对应的OpenGL动效规划

| 显示文本   | 对应的 skycon 值                           | 说明          | UI动效特征/细节                                                                              |
| ---------- | ------------------------------------------ | ------------- | -------------------------------------------------------------------------------------------- |
| `晴`       | `CLEAR_DAY`, `CLEAR_NIGHT`                 | 白天/夜间晴天 | 白天：明亮蓝色渐变天空，少量薄云缓慢飘动，太阳光晕效果；夜间：深蓝色星空，星星闪烁，月亮光晕 |
| `多云`     | `PARTLY_CLOUDY_DAY`, `PARTLY_CLOUDY_NIGHT` | 白天/夜间多云 | 中等密度云朵，蓝天背景占比约60%，云朵移动速度中等，部分遮挡太阳/月亮                         |
| `阴`       | `CLOUDY`                                   | 阴天          | 厚重灰色云层覆盖，天空呈深灰色，几乎无蓝色区域，云层缓慢移动，整体色调暗淡                   |
| `轻度雾霾` | `LIGHT_HAZE`                               | PM2.5 100~150 | 灰黄色调背景，轻微颗粒感，能见度降低，天空朦胧，云朵边缘模糊                                 |
| `中度雾霾` | `MODERATE_HAZE`                            | PM2.5 150~200 | 深灰黄色调，明显颗粒效果，能见度显著降低，天空浑浊，物体轮廓模糊                             |
| `重度雾霾` | `HEAVY_HAZE`                               | PM2.5 > 200   | 棕灰色调，浓厚颗粒感，能见度极低，天空完全浑浊，呈现"暗箱"效果                               |
| `小雨`     | `LIGHT_RAIN`                               | 小雨天气      | 浅蓝色灰色天空，稀疏雨滴缓慢下落，雨滴较小，背景云层较薄                                     |
| `中雨`     | `MODERATE_RAIN`                            | 中雨天气      | 深灰蓝色天空，中等密度雨滴，下落速度较快，雨滴较大，背景云层较厚                             |
| `大雨`     | `HEAVY_RAIN`                               | 大雨天气      | 深灰色天空，密集雨滴快速下落，雨丝明显，背景云层厚重，可能有闪电闪烁                         |
| `暴雨`     | `STORM_RAIN`                               | 暴雨天气      | 近乎黑色的天空，极密集雨滴，强烈闪电效果，雷声震动，云层快速翻滚                             |
| `雾`       | `FOG`                                      | 雾天          | 白色渐变背景，能见度极低，柔和模糊效果，远处物体渐隐，整体呈现朦胧感                         |
| `小雪`     | `LIGHT_SNOW`                               | 小雪天气      | 浅蓝色天空，稀疏雪花缓慢飘落，雪花较小，地面有轻微积雪效果                                   |
| `中雪`     | `MODERATE_SNOW`                            | 中雪天气      | 灰白色天空，中等密度雪花，飘落速度中等，雪花较大，地面有明显积雪                             |
| `大雪`     | `HEAVY_SNOW`                               | 大雪天气      | 白色天空，密集雪花快速飘落，雪花大而清晰，地面积雪厚重，视线受阻                             |
| `暴雪`     | `STORM_SNOW`                               | 暴雪天气      | 白茫茫天空，极密集雪花，暴风雪效果，能见度极低，雪花呈片状飞舞                               |
| `浮尘`     | `DUST`                                     | 浮尘天气      | 土黄色调，悬浮颗粒效果，能见度降低，天空呈灰黄色，光线散射                                   |
| `沙尘`     | `SAND`                                     | 沙尘天气      | 深土黄色调，强颗粒感，沙尘飞舞效果，能见度显著降低，天空呈橙黄色                             |
| `大风`     | `WIND`                                     | 大风天气      | 云层快速移动，风力线条效果，云朵被拉伸变形，整体呈现动态流动感                               |

## OpenGL动效架构设计

### 设计原则

为提高复用性和灵活性，采用**分层组合架构**（Layer-Composite Pattern）：

- 将天气动效拆解为可复用的渲染层（Layer）
- 每个 skycon 映射为多层组合 + 参数配置
- 层之间按顺序叠加渲染，支持透明度混合
- 通过参数化控制同层在不同天气下的表现

### 基础类结构

```
GLRendererBase (现有基类)
    │
    ├── GLLayerBase (新增层基类)
    │       ├── SkyBackgroundLayer    (天空背景层：渐变、太阳、月亮、星星)
    │       ├── CloudLayer            (云层：程序化云朵)
    │       ├── PrecipitationLayer    (降水层：雨滴/雪花粒子)
    │       ├── ParticleLayer         (颗粒层：雾霾/沙尘/雾)
    │       └── EffectLayer           (特效层：闪电、风力线条)
    │
    └── CompositeRenderer (新增组合渲染器)
            └── LayerStack (层堆栈，管理渲染顺序)
```

#### 类接口定义

| 类名                 | 职责                         | 核心方法                                  |
| -------------------- | ---------------------------- | ----------------------------------------- |
| `GLRendererBase`     | 现有渲染器基类               | Init(), Draw(), Destroy(), SetParamsInt() |
| `GLLayerBase`        | 层基类（继承GLRendererBase） | Init(), Draw(), Destroy(), SetParams()    |
| `SkyBackgroundLayer` | 天空背景层                   | setTimeOfDay(), setSkyColor()             |
| `CloudLayer`         | 云层                         | setCoverage(), setDarkness(), setSpeed()  |
| `PrecipitationLayer` | 降水层                       | setType(rain/snow), setIntensity()        |
| `ParticleLayer`      | 颗粒层                       | setType(haze/fog/dust/sand), setDensity() |
| `EffectLayer`        | 特效层                       | setLightning(), setWindLines()            |
| `CompositeRenderer`  | 组合渲染器                   | addLayer(), removeLayer(), Draw()         |

### Layer参数清单

#### SkyBackgroundLayer 参数

| 参数名           | 类型  | 取值范围  | 语义说明         |
| ---------------- | ----- | --------- | ---------------- |
| `timeOfDay`      | enum  | DAY/NIGHT | 白天/夜间模式    |
| `skyTopColor`    | vec3  | RGB       | 天空顶部颜色     |
| `skyBottomColor` | vec3  | RGB       | 天空底部颜色     |
| `sunIntensity`   | float | 0.0~1.0   | 太阳亮度（白天） |
| `moonIntensity`  | float | 0.0~1.0   | 月亮亮度（夜间） |
| `starDensity`    | float | 0.0~1.0   | 星星密度（夜间） |

#### CloudLayer 参数

| 参数名     | 类型  | 取值范围 | 语义说明                     |
| ---------- | ----- | -------- | ---------------------------- |
| `coverage` | float | 0.0~1.0  | 云层覆盖率（0=晴朗，1=全阴） |
| `darkness` | float | 0.0~1.0  | 云层暗度（0=白色，1=深灰）   |
| `speed`    | float | 0.0~1.0  | 移动速度（0=静止，1=快速）   |
| `scale`    | float | 0.5~2.0  | 云层缩放                     |

#### PrecipitationLayer 参数

| 参数名      | 类型  | 取值范围  | 语义说明                       |
| ----------- | ----- | --------- | ------------------------------ |
| `type`      | enum  | RAIN/SNOW | 降水类型                       |
| `intensity` | float | 0.0~1.0   | 降水强度（小雨=0.2，暴雨=1.0） |
| `speed`     | float | 0.0~1.0   | 下落速度                       |
| `size`      | float | 0.5~2.0   | 雨滴/雪花大小                  |

#### ParticleLayer 参数

| 参数名       | 类型  | 取值范围           | 语义说明                 |
| ------------ | ----- | ------------------ | ------------------------ |
| `type`       | enum  | HAZE/FOG/DUST/SAND | 颗粒类型                 |
| `density`    | float | 0.0~1.0            | 颗粒密度                 |
| `color`      | vec3  | RGB                | 颗粒颜色                 |
| `visibility` | float | 0.0~1.0            | 能见度（0=极低，1=正常） |

#### EffectLayer 参数

| 参数名              | 类型  | 取值范围   | 语义说明         |
| ------------------- | ----- | ---------- | ---------------- |
| `lightningEnabled`  | bool  | true/false | 是否启用闪电     |
| `lightningInterval` | float | 2.0~10.0   | 闪电间隔（秒）   |
| `windLinesEnabled`  | bool  | true/false | 是否启用风力线条 |
| `windStrength`      | float | 0.0~1.0    | 风力强度         |

### skycon → 层组合映射表

每个 skycon 映射为多层组合，通过参数控制表现。渲染顺序：背景层 → 云层 → 降水层 → 颗粒层 → 特效层。

| skycon                | 层组合配置                                                                                                                                                                                                                                                                               |
| --------------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `CLEAR_DAY`           | SkyBackgroundLayer(timeOfDay=DAY) + CloudLayer(coverage=0.15, darkness=0.3, speed=0.2)                                                                                                                                                                                                   |
| `CLEAR_NIGHT`         | SkyBackgroundLayer(timeOfDay=NIGHT) + CloudLayer(coverage=0.05, darkness=0.2, speed=0.1)                                                                                                                                                                                                 |
| `PARTLY_CLOUDY_DAY`   | SkyBackgroundLayer(timeOfDay=DAY) + CloudLayer(coverage=0.5, darkness=0.4, speed=0.3)                                                                                                                                                                                                    |
| `PARTLY_CLOUDY_NIGHT` | SkyBackgroundLayer(timeOfDay=NIGHT) + CloudLayer(coverage=0.5, darkness=0.5, speed=0.25)                                                                                                                                                                                                 |
| `CLOUDY`              | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.3,0.35,0.4) + CloudLayer(coverage=0.9, darkness=0.7, speed=0.2)                                                                                                                                                                          |
| `LIGHT_HAZE`          | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.6,0.55,0.45) + CloudLayer(coverage=0.3, darkness=0.5) + ParticleLayer(type=HAZE, density=0.3, visibility=0.7)                                                                                                                            |
| `MODERATE_HAZE`       | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.5,0.45,0.35) + CloudLayer(coverage=0.4, darkness=0.6) + ParticleLayer(type=HAZE, density=0.5, visibility=0.5)                                                                                                                            |
| `HEAVY_HAZE`          | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.4,0.35,0.25) + CloudLayer(coverage=0.5, darkness=0.7) + ParticleLayer(type=HAZE, density=0.8, visibility=0.2)                                                                                                                            |
| `LIGHT_RAIN`          | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.35,0.4,0.5) + CloudLayer(coverage=0.6, darkness=0.6, speed=0.3) + PrecipitationLayer(type=RAIN, intensity=0.25, speed=0.5)                                                                                                               |
| `MODERATE_RAIN`       | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.25,0.3,0.4) + CloudLayer(coverage=0.8, darkness=0.7, speed=0.4) + PrecipitationLayer(type=RAIN, intensity=0.5, speed=0.7)                                                                                                                |
| `HEAVY_RAIN`          | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.2,0.25,0.3) + CloudLayer(coverage=0.9, darkness=0.8, speed=0.5) + PrecipitationLayer(type=RAIN, intensity=0.75, speed=0.9) + EffectLayer(lightningEnabled=true, lightningInterval=8.0)                                                   |
| `STORM_RAIN`          | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.1,0.15,0.2) + CloudLayer(coverage=1.0, darkness=0.95, speed=0.8) + PrecipitationLayer(type=RAIN, intensity=1.0, speed=1.0) + EffectLayer(lightningEnabled=true, lightningInterval=3.0)                                                   |
| `FOG`                 | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.8,0.8,0.8) + ParticleLayer(type=FOG, density=0.7, visibility=0.15)                                                                                                                                                                       |
| `LIGHT_SNOW`          | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.6,0.65,0.75) + CloudLayer(coverage=0.5, darkness=0.5, speed=0.2) + PrecipitationLayer(type=SNOW, intensity=0.2, speed=0.3)                                                                                                               |
| `MODERATE_SNOW`       | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.5,0.55,0.65) + CloudLayer(coverage=0.7, darkness=0.6, speed=0.25) + PrecipitationLayer(type=SNOW, intensity=0.45, speed=0.4)                                                                                                             |
| `HEAVY_SNOW`          | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.4,0.45,0.55) + CloudLayer(coverage=0.9, darkness=0.75, speed=0.3) + PrecipitationLayer(type=SNOW, intensity=0.7, speed=0.5) + ParticleLayer(type=FOG, density=0.2, visibility=0.6)                                                       |
| `STORM_SNOW`          | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.3,0.35,0.45) + CloudLayer(coverage=1.0, darkness=0.9, speed=0.6) + PrecipitationLayer(type=SNOW, intensity=1.0, speed=0.7) + ParticleLayer(type=FOG, density=0.4, visibility=0.3) + EffectLayer(windLinesEnabled=true, windStrength=0.8) |
| `DUST`                | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.6,0.5,0.35) + ParticleLayer(type=DUST, density=0.4, visibility=0.5)                                                                                                                                                                      |
| `SAND`                | SkyBackgroundLayer(timeOfDay=DAY, skyTopColor=0.7,0.5,0.25) + ParticleLayer(type=SAND, density=0.7, visibility=0.3) + EffectLayer(windLinesEnabled=true, windStrength=0.6)                                                                                                               |
| `WIND`                | SkyBackgroundLayer(timeOfDay=DAY) + CloudLayer(coverage=0.4, darkness=0.4, speed=0.9) + EffectLayer(windLinesEnabled=true, windStrength=0.7)                                                                                                                                             |

### 架构演进路径

当前 `GLRenderContext` 只持有单个 `m_pRenderer` 指针，需演进为支持 `CompositeRenderer`：

**阶段1（兼容现有）**：

- `CloudRenderer` 重命名为 `WeatherRenderer`
-

**阶段2（分层重构）**：

- 实现 `GLLayerBase` 和各具体 Layer
- 实现 `CompositeRenderer` 管理层堆栈
- `GLRenderContext` 改为持有 `CompositeRenderer`

**阶段3（动态切换）**：

- 根据 skycon 动态配置层组合
- 支持层的热切换（如大风天气叠加风力效果到任意天气）

### 复用关系说明

| Layer                  | 被哪些天气复用             | 复用方式                                   |
| ---------------------- | -------------------------- | ------------------------------------------ |
| CloudLayer             | 晴、多云、阴、雨、雪、大风 | 通过 coverage/darkness/speed 参数区分      |
| SkyBackgroundLayer     | 所有天气                   | 通过 timeOfDay 和 skyColor 参数区分        |
| EffectLayer(lightning) | 大雨、暴雨、暴雪           | 通过 lightningEnabled 和 interval 参数控制 |
| EffectLayer(wind)      | 大风、沙尘、暴雪           | 通过 windStrength 参数控制                 |
| ParticleLayer(FOG)     | 大雪、暴雪                 | 低能见度时叠加薄雾效果                     |

### 日夜判定策略

彩云 API 的 skycon 值分为两类：

**显式日夜类型**（自带 DAY/NIGHT 后缀）：

- `CLEAR_DAY`, `CLEAR_NIGHT`
- `PARTLY_CLOUDY_DAY`, `PARTLY_CLOUDY_NIGHT`

**隐式日夜类型**（无 DAY/NIGHT 后缀）：

- `CLOUDY`, `LIGHT_HAZE`, `MODERATE_HAZE`, `HEAVY_HAZE`
- `LIGHT_RAIN`, `MODERATE_RAIN`, `HEAVY_RAIN`, `STORM_RAIN`
- `FOG`, `LIGHT_SNOW`, `MODERATE_SNOW`, `HEAVY_SNOW`, `STORM_SNOW`
- `DUST`, `SAND`, `WIND`

**判定规则**：隐式日夜类型需要通过外部时间信息判定：

| 判定方式     | 数据源                              | 说明                   |
| ------------ | ----------------------------------- | ---------------------- |
| 日出日落时间 | 彩云 API 返回的 sunrise/sunset 字段 | 最准确，与实际天气同步 |
| 系统时间     | Android System.currentTimeMillis()  | 备用方案，简单直接     |
| 本地时角     | 根据经纬度计算太阳位置              | 高精度天文算法         |

**推荐实现**：优先使用彩云 API 返回的日出日落时间：

```
if (currentTime >= sunrise && currentTime < sunset) {
    timeOfDay = DAY
} else {
    timeOfDay = NIGHT
}
```

### JNI 桥接规划

#### 现有接口

当前 `GLRenderContext` 通过 `SetParamsInt()` 传递 renderer 类型：

```cpp
void SetParamsInt(int paramType, int value0, int value1);
```

对应 Kotlin 层调用：

```kotlin
GLRenderContext.setParamsInt(RENDERER_TYPE, RENDERER_TYPE_KEY_CLOUD, 0)
```

#### 演进方案

**阶段1（兼容现有）**：

- 在 Kotlin 层根据 skycon 映射到 int 类型的 renderer ID
- 通过现有 `SetParamsInt()` 传递

**阶段2（扩展接口）**：

- 新增 `SetSkycon(const char* skycon)` 方法
- Kotlin 直接传递 skycon 字符串
- C++ 内部解析并配置层组合

```cpp
// C++ 新增接口
void GLRenderContext::SetSkycon(const char* skycon);

// Kotlin 调用
GLRenderContext.setSkycon("CLEAR_DAY")
```

#### skycon 传递路径

```
Sky.kt (getSky())
    │
    └── WeatherViewModel (获取 skycon)
            │
            └── WeatherScreen (传递 skycon 到 GLView)
                    │
                    └── GLView.kt (JNI 调用)
                            │
                            └── GLRenderContext::SetSkycon()
                                    │
                                    └── CompositeRenderer::configureLayers(skycon)
```

### 现有 CloudRenderer 与 CloudLayer 参数映射

当前 `CloudRenderer.cpp` 的 fragment shader 中包含多个硬编码常量，这些将直接演变为 `CloudLayer` 的 uniform 参数：

| CloudLayer 参数  | 对应 shader 常量 | 当前值              | 语义说明                         |
| ---------------- | ---------------- | ------------------- | -------------------------------- |
| `coverage`       | `cloudcover`     | 0.2                 | 云层覆盖率（当前对应晴天）       |
| `darkness`       | `clouddark`      | 0.5                 | 云层暗度                         |
| `lightness`      | `cloudlight`     | 0.3                 | 云层亮部系数                     |
| `speed`          | `speed`          | 0.03                | 云层移动速度                     |
| `scale`          | `cloudscale`     | 1.1                 | 云层缩放比例                     |
| `alpha`          | `cloudalpha`     | 8.0                 | 云层透明度系数                   |
| `skyTint`        | `skytint`        | 0.5                 | 天空色调混合系数                 |
| `skyColorTop`    | `skycolour2`     | vec3(0.4, 0.7, 1.0) | 天空顶部颜色（当前对应晴天白天） |
| `skyColorBottom` | `skycolour1`     | vec3(0.2, 0.4, 0.6) | 天空底部颜色（当前对应晴天白天） |

**阶段1实现指引**：

1. 将上述硬编码常量改为 uniform 变量
2. 通过 `SetParams()` 方法传递参数值
3. 根据 skycon 查表设置对应参数组合

### 渲染顺序与混合模式

| 渲染顺序 | Layer              | 混合模式     | 说明                 |
| -------- | ------------------ | ------------ | -------------------- |
| 1        | SkyBackgroundLayer | 不透明覆盖   | 绘制天空背景         |
| 2        | CloudLayer         | Alpha混合    | 云朵覆盖在天空上     |
| 3        | PrecipitationLayer | Alpha混合    | 雨滴/雪花叠加        |
| 4        | ParticleLayer      | Alpha混合    | 雾霾/沙尘/雾叠加     |
| 5        | EffectLayer        | Additive混合 | 闪电、风力线条等特效 |
