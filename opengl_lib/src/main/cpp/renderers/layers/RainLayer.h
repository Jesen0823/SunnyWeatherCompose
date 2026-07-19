#ifndef GLLIB_RAINLAYER_H
#define GLLIB_RAINLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>
#include <vector>

struct Raindrop {
    glm::vec2 pos;
    glm::vec2 velocity;
    float size;
    float mass;
    float drag;
    float offset;
    glm::vec2 spread;
    float density;
    int parent;
    glm::vec2 lastTrailPos;
};

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
