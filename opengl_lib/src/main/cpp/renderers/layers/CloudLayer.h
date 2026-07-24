#ifndef GLLIB_CLOUDLAYER_H
#define GLLIB_CLOUDLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>

/**
 * 云层
 * 
 * 负责渲染程序化云朵效果，支持参数化配置：
 * - coverage: 云层覆盖率（0.0~1.0）
 * - darkness: 云层暗度（0.0~1.0，0=白色，1=深灰）
 * - lightness: 云层亮部系数（0.0~1.0）
 * - speed: 云层移动速度（0.0~1.0）
 * - scale: 云层缩放（0.5~2.0）
 * - alpha: 云层透明度系数
 * - cloudMode: 云层模式（0=正常，1=暴雨乌云，2=雪天云层）
 * - moonPosition: 月亮位置（夜间模式下影响云层光照）
 * 
 * 实现原理：
 * 使用程序化噪声算法（fbm + 多层噪声叠加）生成云朵形状，
 * 通过 shader 动态调整噪声迭代次数：正常模式（晴天/多云/阴天）使用完整迭代次数，
 * 雨/雪模式使用较少迭代次数以优化性能。支持夜间模式下月亮对云层的光照影响。
 * 
 * 使用加法混合模式（GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA）实现云层半透明效果。
 */
class CloudLayer : public GLLayerBase {
public:
    CloudLayer();
    virtual ~CloudLayer();
    
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
     * 设置向量参数（2D）
     * @param paramType 参数类型
     * @param x X 分量
     * @param y Y 分量
     */
    virtual void SetParamVec2(LayerParamType paramType, float x, float y) override;
    
    /**
     * 设置向量参数（3D）
     * @param paramType 参数类型
     * @param x X 分量
     * @param y Y 分量
     * @param z Z 分量
     */
    virtual void SetParamVec3(LayerParamType paramType, float x, float y, float z) override;
    
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
    
    // Cloud 参数 uniform 位置
    GLuint m_CloudCoverageLoc;          // 云层覆盖率
    GLuint m_CloudDarknessLoc;          // 云层暗度
    GLuint m_CloudLightnessLoc;         // 云层亮部系数
    GLuint m_CloudSpeedLoc;             // 云层移动速度
    GLuint m_CloudScaleLoc;             // 云层缩放
    GLuint m_CloudAlphaLoc;             // 云层透明度系数
    GLuint m_IsNightLoc;                // 夜晚模式标识
    GLuint m_CloudModeLoc;              // 云层模式标识
    GLuint m_MoonPosLoc;                // 月亮位置
    
    glm::mat4 m_MVPMatrix;              // MVP 矩阵
    
    // 参数值
    float m_CloudCoverage;              // 云层覆盖率（0.0~1.0）
    bool m_IsNight;                     // 是否夜晚模式
    int m_CloudMode;                    // 云层模式（0=正常，1=暴雨乌云，2=雪天云层）
    glm::vec2 m_MoonPos;                // 月亮位置（归一化坐标）
    float m_CloudDarkness;              // 云层暗度（0.0~1.0）
    float m_CloudLightness;             // 云层亮部系数（0.0~1.0）
    float m_CloudSpeed;                 // 云层移动速度（0.0~1.0）
    float m_CloudScale;                 // 云层缩放（0.5~2.0）
    float m_CloudAlpha;                 // 云层透明度系数
    
    int m_ScreenWidth;                  // 屏幕宽度
    int m_ScreenHeight;                 // 屏幕高度
    
    float m_Time;                       // 动画时间
    int m_FrameIndex;                   // 帧索引（替代静态变量，线程安全）
};

#endif // GLLIB_CLOUDLAYER_H