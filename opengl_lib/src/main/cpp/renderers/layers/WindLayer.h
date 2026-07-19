#ifndef GLLIB_WINDLAYER_H
#define GLLIB_WINDLAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>

class WindLayer : public GLLayerBase {
public:
    WindLayer();
    virtual ~WindLayer();
    
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
    GLuint m_WindLinesEnabledLoc;
    GLuint m_WindStrengthLoc;
    
    glm::mat4 m_MVPMatrix;
    
    bool m_WindLinesEnabled;
    float m_WindStrength;
    
    int m_ScreenWidth;
    int m_ScreenHeight;
    
    float m_Time;
    int m_FrameIndex;
};

#endif // GLLIB_WINDLAYER_H
