#include "ShaderLoader.h"
#include "LogUtil.h"

AAssetManager *ShaderLoader::m_pAssetManager = nullptr;

void ShaderLoader::SetAssetManager(AAssetManager *pAssetManager) {
    m_pAssetManager = pAssetManager;
    if (m_pAssetManager != nullptr) {
        LOGCATI("ShaderLoader::SetAssetManager: AssetManager initialized successfully");
    }
}

bool ShaderLoader::IsAssetManagerInitialized() {
    return m_pAssetManager != nullptr;
}

std::string ShaderLoader::LoadShaderFromAssets(const char *filename) {
    if (m_pAssetManager == nullptr) {
        LOGCATE("ShaderLoader::LoadShaderFromAssets: AssetManager not initialized!");
        return "";
    }

    AAsset *asset = AAssetManager_open(m_pAssetManager, filename, AASSET_MODE_BUFFER);
    if (asset == nullptr) {
        LOGCATE("ShaderLoader::LoadShaderFromAssets: Failed to open asset: %s", filename);
        return "";
    }

    off_t length = AAsset_getLength(asset);
    if (length <= 0) {
        LOGCATE("ShaderLoader::LoadShaderFromAssets: Asset %s has zero length", filename);
        AAsset_close(asset);
        return "";
    }

    char *buffer = new char[length + 1];
    int bytesRead = AAsset_read(asset, buffer, length);
    buffer[length] = '\0';

    AAsset_close(asset);

    if (bytesRead != length) {
        LOGCATE("ShaderLoader::LoadShaderFromAssets: Failed to read all bytes from %s", filename);
        delete[] buffer;
        return "";
    }

    LOGCATI("ShaderLoader::LoadShaderFromAssets: Successfully loaded shader: %s, length: %d", filename, (int) length);
    std::string result(buffer);
    delete[] buffer;
    return result;
}
