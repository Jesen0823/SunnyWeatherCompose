#ifndef GLLIB_LIGHTNINGLAYER_H
#define GLLIB_LIGHTNINGLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>

/**
 * 闪电层
 * 
 * 负责渲染闪电效果，包括：
 * - 闪电主干（5个控制点组成的弯曲路径）
 * - 多级分支（从主干不同位置产生的分支和二级分支）
 * - 多层发光效果（核心、内层、外层、环境光晕）
 * - 动态闪烁（基于时间的快速闪烁效果）
 * - 自然参数随机化（位置、宽度、亮度、高度每次不同）
 * 
 * 实现原理：
 * 使用程序化闪电生成算法，基于线段距离场（sdSegment）计算闪电形状，
 * 通过 smoothstep 函数创建多层发光效果。闪电闪烁由 C++ 端计算并通过
 * u_flashIntensity uniform 传递给着色器，确保与环境光同步。
 * 
 * 参数配置：
 * - lightningEnabled: 是否启用闪电（0=禁用，1=启用）
 * - lightningInterval: 闪电间隔时间（秒），控制闪电触发频率
 * - isNight: 是否夜间模式（影响闪电颜色）
 * 
 * 使用场景：
 * - STORM_RAIN（暴雨）：启用闪电，间隔3秒，夜间模式根据实际时间
 * - 其他天气：禁用闪电
 * 
 * 输出接口：
 * - GetCurrentFlashIntensity(): 获取当前闪电亮度（0.0~1.0），供 AmbientOverlayLayer 使用
 * - GetFlashIntensity(): 同上，别名方法
 */
class LightningLayer : public GLLayerBase {
public:
    LightningLayer();
    virtual ~LightningLayer();
    
    virtual bool Init() override;
    virtual void Draw(int screenW, int screenH) override;
    virtual void Destroy() override;
    
    virtual void SetParamInt(LayerParamType paramType, int value) override;
    virtual void SetParamFloat(LayerParamType paramType, float value) override;
    
    float GetCurrentFlashIntensity() const { return m_CurrentFlashIntensity; }
    float GetFlashIntensity() const { return m_CurrentFlashIntensity; }
    
private:
    void UpdateMVPMatrix();
    
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    GLuint m_MVPMatLoc;
    GLuint m_TimeLoc;
    GLuint m_ScreenSizeLoc;
    GLuint m_FlashIntensityLoc;
    GLuint m_IsNightLoc;
    
    glm::mat4 m_MVPMatrix;
    
    bool m_LightningEnabled;
    float m_LightningInterval;
    bool m_IsNight;
    
    int m_ScreenWidth;
    int m_ScreenHeight;
    
    float m_Time;
    int m_FrameIndex;
    float m_CurrentFlashIntensity;
};

#endif // GLLIB_LIGHTNINGLAYER_H
