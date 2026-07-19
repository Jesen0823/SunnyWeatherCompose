#ifndef GLLIB_SNOWLAYER_H
#define GLLIB_SNOWLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>
#include <vector>

/**
 * 雪层
 * 
 * 负责渲染雪花飘落效果，包括：
 * - 雪花粒子系统（随机分布、大小和形状）
 * - 雪花飘落动画（带有左右摆动）
 * - 雪花大小变化（近大远小）
 * - 雪花透明度变化（远近不同）
 * 
 * 实现原理：
 * 使用实例化渲染技术，为每个雪花生成独立的位置、速度、大小和摆动参数，
 * 通过顶点着色器进行变换，实现高效的大量雪花渲染。雪花运动模拟了真实的
 * 飘落轨迹，包括重力下落和风力摆动。
 * 
 * 参数配置：
 * - snowIntensity: 降雪强度（0.0~1.0），影响雪花数量
 * - snowSpeed: 下落速度（0.1~1.0），控制雪花飘落快慢
 * - snowSize: 雪花大小（0.5~2.0），缩放所有雪花尺寸
 * 
 * 使用场景：
 * - LIGHT_SNOW（小雪）：强度 0.2，速度 0.3，大小 0.8
 * - MODERATE_SNOW（中雪）：强度 0.45，速度 0.4，大小 1.0
 * - HEAVY_SNOW（大雪）：强度 0.7，速度 0.5，大小 1.2
 * - STORM_SNOW（暴雪）：强度 1.0，速度 0.7，大小 1.5
 */
class SnowLayer : public GLLayerBase {
public:
    SnowLayer();
    virtual ~SnowLayer();
    
    virtual bool Init() override;
    virtual void Draw(int screenW, int screenH) override;
    virtual void Destroy() override;
    virtual void SetParamInt(LayerParamType paramType, int value) override;
    virtual void SetParamFloat(LayerParamType paramType, float value) override;
    
private:
    void GenerateSnowflakes();
    
    GLuint m_ProgramObj = 0;
    GLuint m_VertexShader = 0;
    GLuint m_FragmentShader = 0;
    
    GLuint m_PosOffsetLoc = -1;
    GLuint m_PositionLoc = -1;
    GLuint m_SpeedLoc = -1;
    GLuint m_LengthLoc = -1;
    GLuint m_WidthLoc = -1;
    
    GLuint m_TimeLoc = -1;
    GLuint m_ScreenSizeLoc = -1;
    GLuint m_SnowIntensityLoc = -1;
    GLuint m_SnowSpeedLoc = -1;
    GLuint m_SnowSizeLoc = -1;
    
    GLuint m_QuadVBO = 0;
    GLuint m_PositionVBO = 0;
    GLuint m_SpeedVBO = 0;
    GLuint m_LengthVBO = 0;
    GLuint m_WidthVBO = 0;
    
    int m_ParticleCount = 5000;
    float m_Time = 0.0f;
    
    float m_SnowIntensity = 0.5f;
    float m_SnowSpeed = 0.5f;
    float m_SnowSize = 1.0f;
    
    int m_ScreenWidth = 0;
    int m_ScreenHeight = 0;
    
    std::vector<glm::vec2> m_ParticlePositions;
    std::vector<glm::vec2> m_ParticleSpeeds;
    std::vector<float> m_ParticleLengths;
    std::vector<float> m_ParticleWidths;
};

#endif // GLLIB_SNOWLAYER_H
