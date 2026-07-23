#ifndef GLLIB_AMBIENTOVERLAYLAYER_H
#define GLLIB_AMBIENTOVERLAYLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>

/**
 * 环境光覆盖层
 * 
 * 负责渲染全屏覆盖效果，包括：
 * - 闪电时的环境亮度变化效果
 *   - 全屏亮度提升（闪电出现时）
 *   - 渐变过渡（亮度平滑变化）
 * - 雾天模式的乳白色半透明遮罩效果
 *   - 持续的雾天遮罩层
 *   - 夜间/白天模式适配（不同的乳白色调）
 * 
 * 实现原理：
 * 作为全屏覆盖层，根据模式渲染不同效果：
 * - 闪电模式：使用加法混合模式（GL_SRC_ALPHA, GL_ONE）
 * - 雾天模式：使用普通混合模式（GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA）
 * 
 * 参数配置：
 * - flashIntensity: 闪电亮度（0.0~1.0），由 LightningLayer 实时传入
 * - isNight: 是否夜间模式（影响亮度变化幅度和颜色）
 * - isFogMode: 是否雾天模式
 * - fogIntensity: 雾天遮罩强度（0.0~1.0）
 * 
 * 使用场景：
 * - STORM_RAIN（暴雨）：与 LightningLayer 配合使用，实现闪电时的环境光效果
 * - FOG（大雾）：实现乳白色半透明遮罩效果
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
    GLuint m_IsFogModeLoc;
    GLuint m_FogIntensityLoc;
    
    glm::mat4 m_MVPMatrix;
    
    float m_FlashIntensity;
    bool m_IsNight;
    bool m_IsFogMode;
    float m_FogIntensity;
    
    int m_ScreenWidth;
    int m_ScreenHeight;
};

#endif // GLLIB_AMBIENTOVERLAYLAYER_H
