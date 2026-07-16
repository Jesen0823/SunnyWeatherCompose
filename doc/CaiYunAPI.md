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

- ClearRenderer (晴天) - 白天/夜间晴天
- CloudyRenderer (阴天) -阴
- PartyCloudyRender ( 多云) -多云
- RainRenderer （雨滴动画）- 覆盖小雨/中雨/大雨/暴雨
- SnowRenderer （雪花动画）- 覆盖小雪/中雪/大雪/暴雪
- HazeRenderer （雾霾效果）- 覆盖轻度/中度/重度雾霾
- FogRenderer （雾效果）- 覆盖雾天
- DustRenderer （沙尘效果）- 覆盖浮尘/沙尘
- WindRenderer （大风效果）- 覆盖大风
