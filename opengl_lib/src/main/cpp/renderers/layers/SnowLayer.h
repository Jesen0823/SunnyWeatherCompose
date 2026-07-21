#ifndef GLLIB_SNOWLAYER_H
#define GLLIB_SNOWLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>
#include <vector>

/**
 * 雪层
 * 
 * 负责渲染雪花飘落效果，根据雪的级别适配不同效果：
 * - 小雪/中雪：颗粒感，安静落下，无明显风力偏移
 * - 大雪/暴雪：雪花形状分明，有风力偏移，速度更快
 * 
 * 实现原理：
 * 使用点精灵（gl_PointSize）技术，配合程序化生成的雪花形状着色器，
 * 通过顶点着色器进行位置变换和风力模拟，实现高效的大量雪花渲染。
 * 
 * 参数配置：
 * - snowIntensity: 降雪强度（0.0~1.0），影响雪花数量和密度
 * - snowSpeed: 下落速度（0.1~1.0），控制雪花飘落快慢
 * - snowSize: 雪花大小（0.5~2.0），缩放所有雪花尺寸
 * - windForce: 风力（-1.0~1.0），控制水平偏移强度
 * - particleCount: 粒子数量（1000~10000）
 * 
 * 使用场景：
 * - LIGHT_SNOW（小雪）：强度0.3，速度0.3，大小0.8，风力0.0，粒子2000
 * - MODERATE_SNOW（中雪）：强度0.5，速度0.4，大小1.0，风力0.0，粒子4000
 * - HEAVY_SNOW（大雪）：强度0.7，速度0.6，大小1.2，风力0.3，粒子6000
 * - STORM_SNOW（暴雪）：强度1.0，速度0.8，大小1.5，风力0.6，粒子8000
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
    
    void SetWindForce(float force);
    void SetParticleCount(int count);
    
private:
    void GenerateSnowflakes();
    
    GLuint m_ProgramObj = 0;
    GLuint m_VertexShader = 0;
    GLuint m_FragmentShader = 0;
    
    GLuint m_PositionLoc = -1;
    GLuint m_SpeedLoc = -1;
    GLuint m_SizeLoc = -1;
    GLuint m_SwingOffsetLoc = -1;
    GLuint m_SwingSpeedLoc = -1;
    GLuint m_WindOffsetLoc = -1;
    
    GLuint m_TimeLoc = -1;
    GLuint m_ScreenSizeLoc = -1;
    GLuint m_SnowIntensityLoc = -1;
    GLuint m_SnowSpeedLoc = -1;
    GLuint m_SnowSizeLoc = -1;
    GLuint m_WindForceLoc = -1;
    GLuint m_SnowShapeLoc = -1;
    GLuint m_GustWindXLoc = -1;
    GLuint m_GustWindYLoc = -1;
    GLuint m_GustStrengthLoc = -1;
    GLuint m_GustGroupSeedLoc = -1;
    GLuint m_GustGroupRatioLoc = -1;
    GLuint m_GustLocalTimeLoc = -1;
    GLuint m_GustBlendFactorLoc = -1;
    GLuint m_GustDurationLoc = -1;
    
    GLuint m_PositionVBO = 0;
    GLuint m_SpeedVBO = 0;
    GLuint m_SizeVBO = 0;
    GLuint m_SwingOffsetVBO = 0;
    GLuint m_SwingSpeedVBO = 0;
    GLuint m_WindOffsetVBO = 0;
    GLuint m_VaoId = 0;
    
    int m_ParticleCount = 4000;
    float m_Time = 0.0f;
    bool m_IsInitialized = false;
    
    float m_SnowIntensity = 0.5f;
    float m_SnowSpeed = 0.5f;
    float m_SnowSize = 1.0f;
    float m_WindForce = 0.0f;
    float m_SnowShape = 0.0f;
    
    float m_GustWindX = 0.0f;
    float m_GustWindY = 0.0f;
    float m_GustStrength = 0.0f;
    float m_GustTimer = 0.0f;
    float m_GustDuration = 0.0f;
    float m_GustGroupSeed = 0.0f;
    float m_GustGroupRatio = 0.5f;
    float m_PrevGustWindX = 0.0f;
    float m_PrevGustWindY = 0.0f;
    float m_GustBlendFactor = 1.0f;
    
    int m_ScreenWidth = 0;
    int m_ScreenHeight = 0;
    
    std::vector<glm::vec2> m_ParticlePositions;
    std::vector<glm::vec2> m_ParticleSpeeds;
    std::vector<float> m_ParticleSizes;
    std::vector<float> m_SwingOffsets;
    std::vector<float> m_SwingSpeeds;
    std::vector<float> m_WindOffsets;
};

#endif // GLLIB_SNOWLAYER_H
