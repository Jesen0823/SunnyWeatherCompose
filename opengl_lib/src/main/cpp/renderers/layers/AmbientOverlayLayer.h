#ifndef GLLIB_AMBIENTOVERLAYLAYER_H
#define GLLIB_AMBIENTOVERLAYLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>

/**
 * 环境光覆盖层
 * 
 * 负责渲染闪电时的环境亮度变化效果，包括：
 * - 全屏亮度提升（闪电出现时）
 * - 渐变过渡（亮度平滑变化）
 * - 夜间/白天模式适配（不同的亮度变化曲线）
 * 
 * 实现原理：
 * 作为全屏覆盖层，根据 LightningLayer 传入的 flashIntensity 值，
 * 计算环境光亮度并叠加到整个画面上。使用加法混合模式（GL_SRC_ALPHA, GL_ONE），
 * 确保亮度提升不会遮挡背景元素。
 * 
 * 参数配置：
 * - flashIntensity: 闪电亮度（0.0~1.0），由 LightningLayer 实时传入
 * - isNight: 是否夜间模式（影响亮度变化幅度和颜色）
 * 
 * 使用场景：
 * - STORM_RAIN（暴雨）：与 LightningLayer 配合使用，实现闪电时的环境光效果
 * 
 * 输入接口：
 * - SetFlashIntensity(float intensity): 设置当前闪电亮度，由 CompositeRenderer 调用
 */
class AmbientOverlayLayer : public GLLayerBase {
public:
    AmbientOverlayLayer();
    virtual ~AmbientOverlayLayer();
    
    virtual bool Init() override;
    virtual void Draw(int screenW, int screenH) override;
    virtual void Destroy() override;
    
    virtual void SetParamInt(LayerParamType paramType, int value) override;
    virtual void SetParamFloat(LayerParamType paramType, float value) override;
    
    void SetFlashIntensity(float intensity);
    
private:
    void UpdateMVPMatrix();
    
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    GLuint m_MVPMatLoc;
    GLuint m_FlashIntensityLoc;
    GLuint m_IsNightLoc;
    
    glm::mat4 m_MVPMatrix;
    
    float m_FlashIntensity;
    bool m_IsNight;
    
    int m_ScreenWidth;
    int m_ScreenHeight;
};

#endif // GLLIB_AMBIENTOVERLAYLAYER_H
