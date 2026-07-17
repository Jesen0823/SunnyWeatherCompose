#ifndef GLLIB_PARTICLELAYER_H
#define GLLIB_PARTICLELAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>

/**
 * 颗粒层
 * 
 * 负责渲染雾霾、雾、沙尘等颗粒效果，支持参数化配置：
 * - type: 颗粒类型（0=雾霾，1=雾，2=浮尘，3=沙尘）
 * - density: 颗粒密度（0.0~1.0）
 * - color: 颗粒颜色
 * - visibility: 能见度（0.0~1.0）
 */
class ParticleLayer : public GLLayerBase {
public:
    ParticleLayer();
    virtual ~ParticleLayer();
    
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
    
    // Particle 参数 uniform 位置
    GLuint m_ParticleTypeLoc;           // 颗粒类型
    GLuint m_ParticleDensityLoc;        // 颗粒密度
    GLuint m_ParticleColorLoc;          // 颗粒颜色
    GLuint m_ParticleVisibilityLoc;     // 能见度
    
    glm::mat4 m_MVPMatrix;              // MVP 矩阵
    
    // 参数值
    int m_ParticleType;                 // 颗粒类型（0=雾霾，1=雾，2=浮尘，3=沙尘）
    float m_ParticleDensity;            // 颗粒密度（0.0~1.0）
    glm::vec3 m_ParticleColor;          // 颗粒颜色
    float m_ParticleVisibility;         // 能见度（0.0~1.0）
    
    int m_ScreenWidth;                  // 屏幕宽度
    int m_ScreenHeight;                 // 屏幕高度
    
    float m_Time;                       // 动画时间
    int m_FrameIndex;                   // 帧索引（替代静态变量，线程安全）
};

#endif // GLLIB_PARTICLELAYER_H