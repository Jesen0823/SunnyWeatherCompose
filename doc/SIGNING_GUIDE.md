# Android 签名配置与管理指南

## 一、核心概念

### 1.1 Keystore（密钥库）

Keystore 是一个加密文件，包含一个或多个密钥对（Key Pair）。它是签名文件的容器，不是项目专属的。

**特点**：

- 一个 keystore 文件可以包含多个密钥对（通过 alias 区分）
- keystore 有统一的密码（storePassword）
- 每个密钥对（alias）有独立的密码（keyPassword）
- keystore 格式：`.jks`（Java Keystore）或 `.keystore`（旧格式）

### 1.2 Alias（别名）

Alias 是 keystore 中密钥对的唯一标识。每个 alias 对应一个独立的密钥对，可以用于签名不同的应用。

**特点**：

- 同一 keystore 中 alias 名称必须唯一
- 每个 alias 可以有独立的密码
- alias 命名应与 applicationId 对应

### 1.3 签名的作用

- **应用身份认证**：确保应用来源可信
- **应用完整性保护**：防止应用被篡改
- **应用升级验证**：只有使用相同签名的 APK 才能覆盖安装
- **Google Play 上架要求**：必须使用签名的 APK
- **第三方 API 验证**：如高德地图 API 需要验证应用签名 SHA1

### 1.4 Debug 签名 vs Release 签名

| 类型        | 用途           | 默认签名文件                            | 安全性                     |
| ----------- | -------------- | --------------------------------------- | -------------------------- |
| **Debug**   | 开发调试、测试 | `~/.android/debug.keystore`（自动生成） | 低（密码固定为 `android`） |
| **Release** | 正式发布       | 开发者手动创建                          | 高（自定义密码）           |

## 二、签名文件管理策略

### 2.1 三种管理策略对比

| 策略                         | 描述                                   | 优点                             | 缺点                               | 适用场景                             |
| ---------------------------- | -------------------------------------- | -------------------------------- | ---------------------------------- | ------------------------------------ |
| **单一 keystore + 多 alias** | 一个 keystore 文件，每个应用一个 alias | 管理简单，只需维护一个文件和密码 | 一旦 keystore 泄露，所有应用受影响 | 个人开发者、小团队、同一组织下的应用 |
| **按产品线分 keystore**      | 按业务线创建多个 keystore              | 风险隔离，便于权限管理           | 管理复杂度中等                     | 大型企业、多业务线                   |
| **每应用独立 keystore**      | 每个应用创建独立的 keystore 文件       | 完全隔离，适合应用转让           | 管理复杂，文件众多                 | 应用可能被出售、外包项目             |

### 2.2 推荐策略（个人/小团队）

**单一 keystore + 多 alias**

```
F:\Android\keystores\
├── jesen-release.jks          # 开发者级 Release keystore
│   ├── alias: sunnyweather    # SunnyWeatherCompose 应用
│   ├── alias: otherapp        # 其他应用
│   └── alias: myapp           # 其他应用
└── jesen-debug.jks            # 开发者级 Debug keystore（所有项目共用）
    └── alias: androiddebugkey # 默认调试密钥
```

**命名规范**：

- **Release keystore 文件名**：`{开发者/组织名}-release.jks`（如 `jesen-release.jks`）
- **Debug keystore 文件名**：`{开发者/组织名}-debug.jks`（如 `jesen-debug.jks`）
- **Release alias 命名**：`{applicationId 的最后一段}`（如 `org.jesen.dev.sunnyweather.pose` → `sunnyweather`）
- **Debug alias 命名**：固定为 `androiddebugkey`（与默认调试签名一致）

### 2.3 为什么需要自定义 Debug Keystore

Android Studio 会自动在 `~/.android/debug.keystore` 生成调试签名文件，但存在以下问题：

| 问题                   | 影响                                                   |
| ---------------------- | ------------------------------------------------------ |
| **SHA1 不固定**        | 换机器或删除文件后 SHA1 变化，需要重新注册到高德等 API |
| **团队成员 SHA1 不同** | 每个成员需要单独注册 SHA1 到 API                       |
| **无法互相安装调试包** | 使用不同签名的调试包无法覆盖安装                       |

**使用自定义 Debug Keystore 的好处**：

1. **SHA1 固定**：注册到高德等 API 后无需重新注册
2. **团队协作便利**：团队成员可以互相安装调试包
3. **跨机器一致**：换机器或删除 `~/.android/debug.keystore` 不影响开发
4. **便于 CI/CD**：CI/CD 环境可以使用同一调试签名

### 2.4 何时需要独立 keystore

以下情况建议为应用创建独立的 keystore：

1. **应用可能被出售或转让**：买家需要独立的签名文件
2. **应用由不同团队开发维护**：便于权限管理和安全隔离
3. **应用面向不同客户**：每个客户需要独立的签名身份
4. **合规要求**：某些行业要求应用使用独立的签名

### 2.5 Google Play App Signing 的影响

启用 Google Play App Signing 后，签名分为两层：

| 密钥类型         | 用途                          | 管理方 | 存储位置      |
| ---------------- | ----------------------------- | ------ | ------------- |
| **上传密钥**     | 签名上传到 Google Play 的 APK | 开发者 | 本地 keystore |
| **应用签名密钥** | 签名最终分发给用户的 APK      | Google | Google 服务器 |

**关键结论**：

- 本地 keystore 实际只是"上传密钥"
- 多应用共用上传 keystore 的风险更低（因为最终签名由 Google 管理）
- 但仍需妥善保管上传密钥（丢失后需要申请重置）

## 三、签名文件创建

### 3.1 创建 Debug Keystore

**方法1：使用 keytool 命令行（推荐）**

```bash
keytool -genkey -v -keystore F:\Android\keystores\jesen-debug.jks -keyalg RSA -keysize 2048 -validity 9125 -alias androiddebugkey -storepass android -keypass android -dname "CN=Android Debug,O=Android,C=US"
```

**参数说明**：

- `-alias androiddebugkey`：与默认调试签名保持一致
- `-storepass android`：与默认调试签名密码保持一致
- `-keypass android`：与默认调试签名密钥密码保持一致
- `-dname "CN=Android Debug,O=Android,C=US"`：与默认调试签名信息保持一致

**方法2：使用 Android Studio GUI**

1. 点击菜单：`Build` > `Generate Signed Bundle/APK`
2. 选择 `APK`，点击 `Next`
3. 在 `Key store path` 点击 `Create new...`
4. 填写以下信息：
   - **Key store path**: `F:\Android\keystores\jesen-debug.jks`
   - **Password**: `android`（与默认保持一致）
   - **Alias**: `androiddebugkey`（与默认保持一致）
   - **Password**: `android`（与默认保持一致）
   - **Validity**: 建议设置为25年以上（9125）
   - **First and Last Name**: `Android Debug`
   - **Organization**: `Android`
   - **Country Code**: `US`
5. 点击 `OK` 创建签名文件

### 3.2 创建 Release Keystore

**方法1：使用 Android Studio GUI（推荐）**

**创建 keystore**：

1. 打开 Android Studio，点击菜单：`Build` > `Generate Signed Bundle/APK`
2. 选择 `APK`，点击 `Next`
3. 在 `Key store path` 点击 `Create new...`
4. 填写以下信息：
   - **Key store path**: `F:\Android\keystores\jesen-release.jks`
   - **Password**: 设置 keystore 密码（建议16位以上，包含大小写字母和数字）
   - **Alias**: `sunnyweather`（与 applicationId 对应）
   - **Password**: 设置 alias 密码（建议与 keystore 密码相同或不同）
   - **Validity**: 建议设置为25年以上（365 \* 25 = 9125）
   - **First and Last Name**: 填写姓名
   - **Organizational Unit**: 组织单位（可选）
   - **Organization**: 组织名称（可选）
   - **City or Locality**: 城市（可选）
   - **State or Province**: 省份（可选）
   - **Country Code (XX)**: CN
5. 点击 `OK` 创建签名文件

**为同一 keystore 添加新 alias**：

1. 点击菜单：`Build` > `Generate Signed Bundle/APK`
2. 选择 `APK`，点击 `Next`
3. 在 `Key store path` 选择已有的 keystore 文件
4. 在 `Key alias` 点击 `+` 添加新 alias
5. 填写新 alias 的信息（步骤同上）

**方法2：使用 keytool 命令行**

**创建 keystore 并添加第一个 alias**：

```bash
keytool -genkey -v -keystore F:\Android\keystores\jesen-release.jks -keyalg RSA -keysize 2048 -validity 9125 -alias sunnyweather
```

**为已有 keystore 添加新 alias**：

```bash
keytool -genkey -v -keystore F:\Android\keystores\jesen-release.jks -keyalg RSA -keysize 2048 -validity 9125 -alias otherapp
```

执行后会提示输入 keystore 密码和 alias 信息。

## 四、配置项目签名

### 4.1 local.properties 配置

1. 复制 `local.properties.template` 为 `local.properties`
2. 填写真实配置：

```properties
# ==========================================
# Release 签名文件配置（所有项目共用）
# ==========================================
keystore.file=F:/Android/keystores/jesen-release.jks
keystore.password=你的keystore密码

# ==========================================
# Debug 签名文件配置（所有项目共用）
# ==========================================
keystore.debug.file=F:/Android/keystores/jesen-debug.jks
keystore.debug.password=android
keystore.debug.alias=androiddebugkey
keystore.debug.keyPassword=android

# ==========================================
# SunnyWeatherCompose 项目配置
# ==========================================
# applicationId: org.jesen.dev.sunnyweather.pose
keystore.sunnyweather.alias=sunnyweather
keystore.sunnyweather.keyPassword=你的alias密码

# ==========================================
# 其他项目配置示例
# ==========================================
# applicationId: org.jesen.dev.otherapp
# keystore.otherapp.alias=otherapp
# keystore.otherapp.keyPassword=你的alias密码
```

### 4.2 build.gradle.kts 配置

```kotlin
signingConfigs {
    val keystoreProperties = Properties().apply {
        val localPropertiesFile = rootProject.file("local.properties")
        if (localPropertiesFile.exists()) {
            load(localPropertiesFile.inputStream())
        }
    }

    create("release") {
        val keystoreFile = keystoreProperties.getProperty("keystore.file")
        val projectAlias = "sunnyweather"
        if (keystoreFile?.isNotEmpty() == true) {
            storeFile = file(keystoreFile)
            storePassword = keystoreProperties.getProperty("keystore.password") ?: ""
            keyAlias = keystoreProperties.getProperty("keystore.$projectAlias.alias") ?: ""
            keyPassword = keystoreProperties.getProperty("keystore.$projectAlias.keyPassword") ?: ""
        }
    }

    create("debug") {
        val debugKeystoreFile = keystoreProperties.getProperty("keystore.debug.file")
        if (debugKeystoreFile?.isNotEmpty() == true) {
            storeFile = file(debugKeystoreFile)
            storePassword = keystoreProperties.getProperty("keystore.debug.password") ?: ""
            keyAlias = keystoreProperties.getProperty("keystore.debug.alias") ?: ""
            keyPassword = keystoreProperties.getProperty("keystore.debug.keyPassword") ?: ""
        }
    }
}

buildTypes {
    release {
        signingConfig = signingConfigs.getByName("release")
        // ...
    }

    debug {
        signingConfig = signingConfigs.getByName("debug")
    }
}
```

**注意**：每个项目只需修改 `projectAlias` 变量即可使用同一 keystore 的不同 alias。

## 五、提取 SHA1 指纹

### 5.1 Debug 签名 SHA1（开发调试用）

```bash
keytool -list -v -keystore F:\Android\keystores\jesen-debug.jks -alias androiddebugkey -storepass android -keypass android
```

### 5.2 Release 签名 SHA1（生产环境用）

```bash
keytool -list -v -keystore F:\Android\keystores\jesen-release.jks -alias sunnyweather
```

### 5.3 提取结果示例

```
证书指纹:
         MD5:  12:34:56:78:90:AB:CD:EF:12:34:56:78:90:AB:CD:EF
         SHA1: 12:34:56:78:90:AB:CD:EF:12:34:56:78:90:AB:CD:EF:12:34:56:78
         SHA256: 12:34:56:78:90:AB:CD:EF:12:34:56:78:90:AB:CD:EF:12:34:56:78:90:AB:CD:EF:12:34:56:78:90:AB:CD:EF:12:34:56:78:90:AB:CD:EF
```

### 5.4 高德地图 API 注册

将以下 SHA1 添加到高德地图 API 密钥配置：

| 场景              | SHA1 来源                                      |
| ----------------- | ---------------------------------------------- |
| 本地开发调试      | Debug keystore（`jesen-debug.jks`）的 SHA1     |
| 本地 Release 构建 | Release keystore（`jesen-release.jks`）的 SHA1 |
| Google Play 发布  | Google Play Console 中应用签名密钥的 SHA1      |

## 六、企业开发签名管理实践

### 6.1 权限管理

| 角色           | 权限                                             |
| -------------- | ------------------------------------------------ |
| **密钥管理员** | 持有 keystore 文件和密码，负责创建新 alias、备份 |
| **开发人员**   | 仅知道项目对应的 alias 密码，通过 CI/CD 获取     |
| **运维人员**   | 通过自动化流程签名，不直接接触密钥               |

### 6.2 安全存储

1. **离线备份**：存储到加密的外部硬盘或 USB 密钥
2. **云存储**：使用加密云盘（如 Google Drive、OneDrive 加密文件夹）
3. **纸质备份**：打印密码和指纹信息，存放在安全位置

### 6.3 定期审计

- 定期检查 keystore 文件完整性
- 定期更换密码（建议每6个月）
- 记录所有签名操作日志

### 6.4 CI/CD 集成

在 CI/CD 流程中使用环境变量注入签名信息：

```yaml
# GitHub Actions 示例
env:
  KEYSTORE_FILE: ${{ secrets.KEYSTORE_FILE }}
  KEYSTORE_PASSWORD: ${{ secrets.KEYSTORE_PASSWORD }}
  DEBUG_KEYSTORE_FILE: ${{ secrets.DEBUG_KEYSTORE_FILE }}
  ALIAS: ${{ secrets.ALIAS }}
  KEY_PASSWORD: ${{ secrets.KEY_PASSWORD }}
```

## 七、文件存储结构

```
F:\Android\
├── keystores/                    # 签名文件存储目录（独立于所有项目）
│   ├── jesen-release.jks         # 主 Release 签名文件（开发者级）
│   ├── jesen-release.jks.sha1    # Release 签名 SHA1 记录文件
│   ├── jesen-debug.jks           # 主 Debug 签名文件（所有项目共用）
│   ├── jesen-debug.jks.sha1      # Debug 签名 SHA1 记录文件
│   └── backups/                  # 备份目录
│       ├── jesen-release.jks.backup.20240101
│       ├── jesen-release.jks.backup.20240701
│       ├── jesen-debug.jks.backup.20240101
│       └── jesen-debug.jks.backup.20240701
└── AndroidStudioProjects/        # 项目目录
    ├── SunnyWeatherCompose/
    │   ├── local.properties      # 密码配置（不提交）
    │   └── local.properties.template  # 密码模板（提交）
    └── OtherApp/
        ├── local.properties
        └── local.properties.template
```

## 八、常用命令

### 8.1 查看 keystore 中的所有 alias

```bash
keytool -list -keystore F:\Android\keystores\jesen-release.jks
```

### 8.2 查看指定 alias 的详细信息

```bash
keytool -list -v -keystore F:\Android\keystores\jesen-release.jks -alias sunnyweather
```

### 8.3 创建 Debug keystore

```bash
keytool -genkey -v -keystore F:\Android\keystores\jesen-debug.jks -keyalg RSA -keysize 2048 -validity 9125 -alias androiddebugkey -storepass android -keypass android -dname "CN=Android Debug,O=Android,C=US"
```

### 8.4 删除 alias

```bash
keytool -delete -keystore F:\Android\keystores\jesen-release.jks -alias oldapp
```

### 8.5 导出证书

```bash
keytool -exportcert -keystore F:\Android\keystores\jesen-release.jks -alias sunnyweather -file sunnyweather.crt
```

### 8.6 验证 APK 签名

```bash
jarsigner -verify -verbose -certs app-release.apk
```

## 九、安全注意事项

1. **绝对不要将签名文件提交到版本控制**
2. **绝对不要将密码硬编码在代码中**
3. **签名文件应存储在项目外部的安全位置**
4. **密码应定期更换**
5. **建议对签名文件进行多重备份**
6. **如果签名文件丢失，将无法更新已发布的应用**
7. **启用 Google Play App Signing 后，仍需妥善保管上传密钥**
8. **Debug keystore 的密码固定为 `android`，不要用于生产环境**

## 十、常见问题

### Q1：可以使用同一 alias 签名多个应用吗？

**不推荐**。每个应用应使用独立的 alias，便于管理和隔离。

### Q2：忘记 keystore 密码怎么办？

**无法找回**。如果启用了 Google Play App Signing，可以申请重置上传密钥。否则只能重新发布应用（使用新的包名）。

### Q3：可以更换应用的签名吗？

**Google Play 不支持**。一旦应用首次上传，签名密钥就固定了。其他应用市场可能支持，但会导致用户无法升级安装。

### Q4：Debug 和 Release 签名有什么区别？

- **Debug 签名**：用于开发调试，密码固定为 `android`，安全性低
- **Release 签名**：用于正式发布，自定义密码，安全性高
- 调试应用时使用 Debug 签名，发布时使用 Release 签名

### Q5：签名文件可以复制到其他机器使用吗？

**可以**。签名文件是独立的，可以复制到任何机器使用。但需注意安全传输。

### Q6：为什么需要自定义 Debug keystore？

- **SHA1 固定**：注册到高德等 API 后无需重新注册
- **团队协作便利**：团队成员可以互相安装调试包
- **跨机器一致**：换机器不影响开发

### Q7：Debug keystore 的密码为什么固定为 android？

为了与 Android Studio 默认生成的调试签名保持一致，方便兼容现有开发流程和工具。但不要用于生产环境。
