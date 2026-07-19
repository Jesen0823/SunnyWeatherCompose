#ifndef GLLIB_SNOWLAYER_H
#define GLLIB_SNOWLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>
#include <vector>

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
