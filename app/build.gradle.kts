import org.jetbrains.kotlin.gradle.dsl.JvmTarget
import java.util.Properties

plugins {
    alias(libs.plugins.android.application)
    // alias(libs.plugins.kotlin.android) // 需要移除 org.jetbrains.kotlin.android 插件，因为 AGP 9.0+ 已经内置了 Kotlin 支持
    alias(libs.plugins.kotlin.compose)
    alias(libs.plugins.jetbrains.kotlin.serialization)
}

android {
    namespace = "org.jesen.dev.sunnyweather.pose"
    compileSdk = 37

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
        
        named("debug") {
            val debugKeystoreFile = keystoreProperties.getProperty("keystore.debug.file")
            if (debugKeystoreFile?.isNotEmpty() == true) {
                storeFile = file(debugKeystoreFile)
                storePassword = keystoreProperties.getProperty("keystore.debug.password") ?: ""
                keyAlias = keystoreProperties.getProperty("keystore.debug.alias") ?: ""
                keyPassword = keystoreProperties.getProperty("keystore.debug.keyPassword") ?: ""
            }
        }
    }

    defaultConfig {
        applicationId = "org.jesen.dev.sunnyweather.pose"
        minSdk = 24
        targetSdk = 36
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            signingConfig = signingConfigs.getByName("release")
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
        
        debug {
            signingConfig = signingConfigs.getByName("debug")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    kotlin {
        compilerOptions {
            jvmTarget.set(JvmTarget.JVM_11)
        }
    }
    buildFeatures {
        compose = true
    }
}

dependencies {
    implementation(libs.androidx.compose.runtime)
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.lifecycle.runtime.ktx)
    implementation(libs.androidx.activity.compose)
    implementation(platform(libs.androidx.compose.bom))
    implementation(libs.androidx.compose.ui)
    implementation(libs.androidx.compose.ui.graphics)
    implementation(libs.androidx.compose.ui.tooling.preview)
    implementation(libs.androidx.compose.material3)
    implementation(libs.androidx.compose.material.icons.core)
    implementation(libs.androidx.compose.material.icons.extended)

    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
    androidTestImplementation(platform(libs.androidx.compose.bom))
    androidTestImplementation(libs.androidx.compose.ui.test.junit4)
    debugImplementation(libs.androidx.compose.ui.tooling)
    debugImplementation(libs.androidx.compose.ui.test.manifest)

    // navigation3
    implementation(libs.androidx.navigation3.ui)
    implementation(libs.androidx.navigation3.runtime)
    implementation(libs.androidx.lifecycle.viewmodel.navigation3)
    implementation(libs.androidx.material3.adaptive.navigation3)
    implementation(libs.kotlinx.serialization.core)

    // ktor
    implementation(libs.ktor.client.core)
    implementation(libs.ktor.client.okhttp)
    implementation(libs.ktor.client.content.negotiation)
    implementation(libs.ktor.serialization.kotlinx.json)
    implementation(libs.ktor.client.logging)
    implementation(libs.kotlinx.serialization.json)

    // Preferences DataStore (SharedPreferences like APIs)
    implementation(libs.androidx.datastore.preferences)
    // Alternatively - without an Android dependency.
    implementation(libs.androidx.datastore.preferences.core)

    // lifecycle of compose
    implementation(libs.androidx.lifecycle.runtime.compose)
    implementation(libs.androidx.lifecycle.viewmodel.compose)

    
}