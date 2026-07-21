#ifndef GLLIB_SHADERLOADER_H
#define GLLIB_SHADERLOADER_H

#include <string>
#include <android/asset_manager.h>

class ShaderLoader {
private:
    static AAssetManager *m_pAssetManager;

public:
    static void SetAssetManager(AAssetManager *pAssetManager);

    static std::string LoadShaderFromAssets(const char *filename);

    static bool IsAssetManagerInitialized();
};

#endif // GLLIB_SHADERLOADER_H
