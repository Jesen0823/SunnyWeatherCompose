#ifndef GLLIB_EFFECTLAYER_H
#define GLLIB_EFFECTLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>

/**
 * 特效层
 * 
 * 负责渲染闪电和风力线条等特效，支持参数化配置：
 * - lightningEnabled: 闪电启用（0=关闭，1=开启）
 * - lightningInterval: 闪电间隔（秒）
 * - windLinesEnabled: 风力线条启用（0=关闭，1=开启）
 * - windStrength: 风力强度（0.0~1.0）
 */
class EffectLayer : public GLLayerBase {
public:
    EffectLayer();
    virtual ~EffectLayer();
    
    /**
     * 初始化层
     */
    virtual bool Init() override;
    
    /**
     * 绘制层
     * @param screenW 屏幕宽度
     * @param screenH 屏幕高度
     */
    virtual void Draw(int screenW, int screenH) override;
    
    /**
     * 销毁层资源
     */
    virtual void Destroy() override;
    
    /**
     * 设置整数参数
     * @param paramType 参数类型
     * @param value 参数值
     */
    virtual void SetParamInt(LayerParamType paramType, int value) override;
    
    /**
     * 设置浮点参数
     * @param paramType 参数类型
     * @param value 参数值
     */
    virtual void SetParamFloat(LayerParamType paramType, float value) override;
    
private:
    /**
     * 更新 MVP 矩阵
     */
    void UpdateMVPMatrix();
    
    GLuint m_VaoId;                     // VAO ID
    GLuint m_VboIds[3];                 // VBO IDs
    GLuint m_MVPMatLoc;                 // MVP 矩阵 uniform 位置
    GLuint m_TimeLoc;                   // 时间 uniform 位置
    GLuint m_ScreenSizeLoc;             // 屏幕尺寸 uniform 位置
    
    // Effect 参数 uniform 位置
    GLuint m_LightningEnabledLoc;       // 闪电启用
    GLuint m_LightningIntervalLoc;      // 闪电间隔
    GLuint m_WindLinesEnabledLoc;       // 风力线条启用
    GLuint m_WindStrengthLoc;           // 风力强度
    
    glm::mat4 m_MVPMatrix;              // MVP 矩阵
    
    // 参数值
    bool m_LightningEnabled;            // 闪电启用
    float m_LightningInterval;          // 闪电间隔（秒）
    bool m_WindLinesEnabled;            // 风力线条启用
    float m_WindStrength;               // 风力强度（0.0~1.0）
    
    int m_ScreenWidth;                  // 屏幕宽度
    int m_ScreenHeight;                 // 屏幕高度
    
    float m_Time;                       // 动画时间
    int m_FrameIndex;                   // 帧索引（替代静态变量，线程安全）
};

#endif // GLLIB_EFFECTLAYER_H