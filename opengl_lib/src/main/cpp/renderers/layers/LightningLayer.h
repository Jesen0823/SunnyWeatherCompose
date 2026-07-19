#ifndef GLLIB_LIGHTNINGLAYER_H
#define GLLIB_LIGHTNINGLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>

class LightningLayer : public GLLayerBase {
public:
    LightningLayer();
    virtual ~LightningLayer();
    
    virtual bool Init() override;
    virtual void Draw(int screenW, int screenH) override;
    virtual void Destroy() override;
    
    virtual void SetParamInt(LayerParamType paramType, int value) override;
    virtual void SetParamFloat(LayerParamType paramType, float value) override;
    
private:
    void UpdateMVPMatrix();
    
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    GLuint m_MVPMatLoc;
    GLuint m_TimeLoc;
    GLuint m_ScreenSizeLoc;
    GLuint m_LightningEnabledLoc;
    GLuint m_LightningIntervalLoc;
    GLuint m_IsNightLoc;
    
    glm::mat4 m_MVPMatrix;
    
    bool m_LightningEnabled;
    float m_LightningInterval;
    bool m_IsNight;
    
    int m_ScreenWidth;
    int m_ScreenHeight;
    
    float m_Time;
    int m_FrameIndex;
};

#endif // GLLIB_LIGHTNINGLAYER_H
