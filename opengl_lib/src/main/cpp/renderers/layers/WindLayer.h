#ifndef GLLIB_WINDLAYER_H
#define GLLIB_WINDLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>

/**
 * 风层
 * 
 * 负责渲染风力效果，包括：
 * - 风线效果（水平飘动的线条）
 * - 风线透明度渐变（从左到右或从右到左）
 * - 风线运动动画（随风速移动）
 * 
 * 实现原理：
 * 使用程序化风线生成算法，基于噪声函数创建流畅的风线形状，
 * 通过时间参数控制风线的移动和变形。风线使用渐变透明度，
 * 模拟真实的风视觉效果。
 * 
 * 参数配置：
 * - windLinesEnabled: 是否启用风线（0=禁用，1=启用）
 * - windStrength: 风力强度（0.1~1.0），控制风线速度和密度
 * 
 * 使用场景：
 * - WIND（大风天气）：启用风线，强度 0.7
 * - STORM_SNOW（暴雪）：启用风线，强度 0.8
 * - SAND（沙尘）：启用风线，强度 0.6
 * - 其他天气：根据需要可启用
 */
class WindLayer : public GLLayerBase {
public:
    WindLayer();
    virtual ~WindLayer();
    
    virtual bool Init() override;
    virtual void Draw(int screenW, int screenH) override;
    virtual void Destroy() override;
    
    virtual void SetParamInt(LayerParamType paramType, int value) override;
    virtual void SetParamFloat(LayerParamType paramType, float value) override;
    
private:
    void UpdateMVPMatrix();
    
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    GLuint m_MVPMatLoc;
    GLuint m_TimeLoc;
    GLuint m_ScreenSizeLoc;
    GLuint m_WindLinesEnabledLoc;
    GLuint m_WindStrengthLoc;
    
    glm::mat4 m_MVPMatrix;
    
    bool m_WindLinesEnabled;
    float m_WindStrength;
    
    int m_ScreenWidth;
    int m_ScreenHeight;
    
    float m_Time;
    int m_FrameIndex;
};

#endif // GLLIB_WINDLAYER_H
