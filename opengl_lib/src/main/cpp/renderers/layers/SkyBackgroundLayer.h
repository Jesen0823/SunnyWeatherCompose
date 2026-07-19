#ifndef GLLIB_SKYBACKGROUNDLAYER_H
#define GLLIB_SKYBACKGROUNDLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>

/**
 * 天空背景层
 * 
 * 负责渲染天空背景，包括：
 * - 天空渐变（日/夜模式不同颜色）
 * - 太阳光晕效果（白天模式）
 * - 月亮光晕效果（夜间模式）
 * 
 * 参数配置：
 * - timeOfDay: 日夜模式（0=白天，1=夜间）
 * - skyColorTop: 天空顶部颜色
 * - skyColorBottom: 天空底部颜色
 * - sunIntensity: 太阳亮度（白天模式）
 * - moonIntensity: 月亮亮度（夜间模式）
 */
class SkyBackgroundLayer : public GLLayerBase {
public:
    SkyBackgroundLayer();
    virtual ~SkyBackgroundLayer();
    
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
    
    /**
     * 设置向量参数（3D）
     * @param paramType 参数类型
     * @param x X 分量
     * @param y Y 分量
     * @param z Z 分量
     */
    virtual void SetParamVec3(LayerParamType paramType, float x, float y, float z) override;
    
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
    GLuint m_TimeOfDayLoc;              // 日夜模式 uniform 位置
    GLuint m_SkyColorTopLoc;            // 天空顶部颜色 uniform 位置
    GLuint m_SkyColorBottomLoc;         // 天空底部颜色 uniform 位置
    GLuint m_SunIntensityLoc;           // 太阳亮度 uniform 位置
    GLuint m_MoonIntensityLoc;          // 月亮亮度 uniform 位置
    GLuint m_SunVisibleLoc;             // 太阳可见性 uniform 位置
    GLuint m_SkyModeLoc;                // 天空模式 uniform 位置
    
    glm::mat4 m_MVPMatrix;              // MVP 矩阵
    
    int m_TimeOfDay;                    // 日夜模式（0=白天，1=夜间）
    glm::vec3 m_SkyColorTop;            // 天空顶部颜色
    glm::vec3 m_SkyColorBottom;         // 天空底部颜色
    float m_SunIntensity;               // 太阳亮度（0.0~1.0）
    float m_MoonIntensity;              // 月亮亮度（0.0~1.0）
    bool m_SunVisible;                  // 太阳可见性（true=可见，false=隐藏）
    int m_SkyMode;                      // 天空模式（0=正常，1=阴雨，2=雪天，3=沙尘）
    
    int m_ScreenWidth;                  // 屏幕宽度
    int m_ScreenHeight;                 // 屏幕高度
    
    float m_Time;                       // 动画时间
    int m_FrameIndex;                   // 帧索引（替代静态变量，线程安全）
};

#endif // GLLIB_SKYBACKGROUNDLAYER_H