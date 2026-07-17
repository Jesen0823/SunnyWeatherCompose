#ifndef GLLIB_PRECIPITATIONLAYER_H
#define GLLIB_PRECIPITATIONLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>

/**
 * 降水层
 * 
 * 负责渲染雨滴和雪花粒子效果，支持参数化配置：
 * - type: 降水类型（0=雨，1=雪）
 * - intensity: 降水强度（0.0~1.0）
 * - speed: 下落速度（0.0~1.0）
 * - size: 雨滴/雪花大小（0.5~2.0）
 */
class PrecipitationLayer : public GLLayerBase {
public:
    PrecipitationLayer();
    virtual ~PrecipitationLayer();
    
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
    
    // Precipitation 参数 uniform 位置
    GLuint m_PrecipitationTypeLoc;      // 降水类型
    GLuint m_PrecipitationIntensityLoc; // 降水强度
    GLuint m_PrecipitationSpeedLoc;     // 下落速度
    GLuint m_PrecipitationSizeLoc;      // 大小
    
    glm::mat4 m_MVPMatrix;              // MVP 矩阵
    
    // 参数值
    int m_PrecipitationType;            // 降水类型（0=雨，1=雪）
    float m_PrecipitationIntensity;     // 降水强度（0.0~1.0）
    float m_PrecipitationSpeed;         // 下落速度（0.0~1.0）
    float m_PrecipitationSize;          // 大小（0.5~2.0）
    
    int m_ScreenWidth;                  // 屏幕宽度
    int m_ScreenHeight;                 // 屏幕高度
    
    float m_Time;                       // 动画时间
    int m_FrameIndex;                   // 帧索引（替代静态变量，线程安全）
};

#endif // GLLIB_PRECIPITATIONLAYER_H