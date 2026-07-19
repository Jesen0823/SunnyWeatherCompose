#ifndef GLLIB_AMBIENTOVERLAYLAYER_H
#define GLLIB_AMBIENTOVERLAYLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>

class AmbientOverlayLayer : public GLLayerBase {
public:
    AmbientOverlayLayer();
    virtual ~AmbientOverlayLayer();
    
    virtual bool Init() override;
    virtual void Draw(int screenW, int screenH) override;
    virtual void Destroy() override;
    
    virtual void SetParamInt(LayerParamType paramType, int value) override;
    virtual void SetParamFloat(LayerParamType paramType, float value) override;
    
    void SetFlashIntensity(float intensity);
    
private:
    void UpdateMVPMatrix();
    
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    GLuint m_MVPMatLoc;
    GLuint m_FlashIntensityLoc;
    GLuint m_IsNightLoc;
    
    glm::mat4 m_MVPMatrix;
    
    float m_FlashIntensity;
    bool m_IsNight;
    
    int m_ScreenWidth;
    int m_ScreenHeight;
};

#endif // GLLIB_AMBIENTOVERLAYLAYER_H
