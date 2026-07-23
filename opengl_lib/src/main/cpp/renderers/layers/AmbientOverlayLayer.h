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
 * - 浮尘模式的土黄色遮罩效果
 *   - 远处强近处弱的垂直渐变遮罩
 *   - 土黄色调模拟浮尘弥漫效果
 * - 沙尘模式的橙红色遮罩效果
 *   - 高浓度橙红色遮罩
 *   - 模拟沙尘暴的昏暗氛围
 * 
 * 实现原理：
 * 作为全屏覆盖层，根据模式渲染不同效果：
 * - 闪电模式（overlayMode=0）：使用加法混合模式（GL_SRC_ALPHA, GL_ONE）
 * - 雾天模式（overlayMode=1）：使用普通混合模式（GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA）
 * - 浮尘模式（overlayMode=2）：使用普通混合模式，土黄色遮罩
 * - 沙尘模式（overlayMode=3）：使用普通混合模式，橙红色遮罩
 * 
 * 参数配置：
 * - flashIntensity: 闪电亮度（0.0~1.0），由 LightningLayer 实时传入
 * - isNight: 是否夜间模式（影响亮度变化幅度和颜色）
 * - overlayMode: 覆盖层模式（0=闪电，1=雾天，2=浮尘，3=沙尘）
 * - fogIntensity: 遮罩强度（0.0~1.0）
 * - overlayColor: 自定义遮罩颜色（用于浮尘和沙尘模式）
 * 
 * 使用场景：
 * - STORM_RAIN（暴雨）：与 LightningLayer 配合使用，实现闪电时的环境光效果
 * - FOG（大雾）：实现乳白色半透明遮罩效果
 * - DUST（浮尘）：实现土黄色遮罩效果
 * - SAND（沙尘暴）：实现橙红色遮罩效果
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
    GLuint m_OverlayModeLoc;
    GLuint m_FogIntensityLoc;
    GLuint m_OverlayColorLoc;
    
    glm::mat4 m_MVPMatrix;
    
    float m_FlashIntensity;
    bool m_IsNight;
    int m_OverlayMode;
    float m_FogIntensity;
    glm::vec3 m_OverlayColor;
    
    int m_ScreenWidth;
    int m_ScreenHeight;
};

#endif // GLLIB_AMBIENTOVERLAYLAYER_H
