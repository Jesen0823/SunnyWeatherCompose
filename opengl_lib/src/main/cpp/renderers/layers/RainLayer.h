#ifndef GLLIB_RAINLAYER_H
#define GLLIB_RAINLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>
#include <vector>

/**
 * 雨滴结构体
 * 存储单个雨滴的物理属性和渲染信息
 */
struct Raindrop {
    glm::vec2 pos;              // 当前位置
    glm::vec2 velocity;         // 速度向量
    float size;                 // 雨滴大小
    float mass;                 // 质量（影响下落速度）
    float drag;                 // 空气阻力系数
    float offset;               // 渲染偏移量
    glm::vec2 spread;           // 飞溅扩散范围
    float density;              // 密度系数
    int parent;                 // 父雨滴索引（用于雨滴分裂）
    glm::vec2 lastTrailPos;     // 上一帧位置（用于绘制拖尾）
};

/**
 * 雨层
 * 
 * 负责渲染雨滴效果，包括：
 * - 雨滴粒子系统（基于物理模拟的下落运动）
 * - 雨滴拖尾效果
 * - 雨滴与背景的折射效果
 * - 雨滴飞溅效果（与地面碰撞）
 * 
 * 实现原理：
 * 使用 FBO 离屏渲染，先绘制背景到纹理，然后通过折射着色器渲染雨滴，
 * 产生雨滴透过玻璃般的折射视觉效果。雨滴使用粒子系统进行物理模拟，
 * 支持风的影响导致倾斜下落。
 * 
 * 参数配置：
 * - rainIntensity: 降雨强度（0.0~2.0），影响雨滴数量和密度
 * - windSpeed: 风速（-1.0~1.0），正值向右，负值向左
 * - refraction: 折射率（0.0~1.0），控制雨滴对背景的折射程度
 * 
 * 使用场景：
 * - LIGHT_RAIN（小雨）：强度 0.3，风速 0.2，折射 0.35
 * - MODERATE_RAIN（中雨）：强度 0.7，风速 0.4，折射 0.6
 * - HEAVY_RAIN（大雨）：强度 1.0，风速 0.6，折射 0.85
 * - STORM_RAIN（暴雨）：强度 2.2，风速 0.8，折射 1.15
 */
class RainLayer : public GLLayerBase {
public:
    RainLayer();
    virtual ~RainLayer();
    
    virtual bool Init() override;
    virtual void Draw(int screenW, int screenH) override;
    virtual void Destroy() override;
    virtual void SetParamInt(LayerParamType paramType, int value) override;
    virtual void SetParamFloat(LayerParamType paramType, float value) override;
    
    void SetBackgroundTexture(GLuint textureId);
    
private:
    void UpdateRaindrops(float dt);
    void SpawnRaindrop();
    void CheckCollisions();
    
    GLuint m_ProgramObj = 0;
    GLuint m_VertexShader = 0;
    GLuint m_FragmentShader = 0;
    
    GLuint m_DistortProgram = 0;
    
    // 缓存location，避免每帧查询
    GLuint m_PositionLoc = (GLuint)-1;
    GLuint m_TexCoordLoc = (GLuint)-1;
    
    GLuint m_TimeLoc = (GLuint)-1;
    GLuint m_ScreenSizeLoc = (GLuint)-1;
    GLuint m_RainIntensityLoc = (GLuint)-1;
    GLuint m_WindSpeedLoc = (GLuint)-1;
    GLuint m_RefractionLoc = (GLuint)-1;
    GLuint m_BackgroundTexLoc = (GLuint)-1;
    GLuint m_DistortTexLoc = (GLuint)-1;
    
    // Distort program 缓存location
    GLuint m_DistortModelLoc = (GLuint)-1;
    GLuint m_DistortProjLoc = (GLuint)-1;
    GLuint m_DistortPosLoc = (GLuint)-1;
    GLuint m_DistortTexAttrLoc = (GLuint)-1;
    
    GLuint m_BackgroundTexture = 0;
    
    GLuint m_FBO = 0;
    GLuint m_DistortTexture = 0;
    
    GLuint m_VBO = 0;
    GLuint m_QuadVBO = 0;
    
    float m_Time = 0.0f;
    float m_RainIntensity = 0.8f;
    float m_WindSpeed = 0.2f;
    float m_Refraction = 0.03f;
    
    int m_FrameIndex = 0;
    int m_ScreenWidth = 0;
    int m_ScreenHeight = 0;
    
    std::vector<Raindrop> m_Raindrops;
    float m_SpawnTimer = 0.0f;
};

#endif // GLLIB_RAINLAYER_H
