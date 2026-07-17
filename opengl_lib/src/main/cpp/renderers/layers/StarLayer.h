#ifndef STAR_LAYER_H
#define STAR_LAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>
#include <vector>

class StarLayer : public GLLayerBase {
public:
    StarLayer();
    ~StarLayer() override;

    bool Init() override;
    void Draw(int screenW, int screenH) override;
    void Destroy() override;

    void SetParamFloat(LayerParamType paramType, float value) override;
    void SetParamVec3(LayerParamType paramType, float x, float y, float z) override;

private:
    void GenerateStars();

    GLuint m_ProgramObj = 0;
    GLuint m_VertexShader = 0;
    GLuint m_FragmentShader = 0;

    GLuint m_PositionLoc = -1;
    GLuint m_ColorLoc = -1;
    GLuint m_SizeLoc = -1;
    GLuint m_TimeLoc = -1;
    GLuint m_ScreenSizeLoc = -1;

    GLuint m_VBO = 0;
    GLuint m_ColorVBO = 0;
    GLuint m_SizeVBO = 0;
    GLuint m_TwinkleVBO = 0;

    GLuint m_TwinkleOffsetLoc = -1;

    glm::mat4 m_MVPMatrix;

    int m_StarCount = 300;
    float m_Time = 0.0f;
    float m_TwinkleSpeed = 1.0f;

    std::vector<glm::vec2> m_StarPositions;
    std::vector<glm::vec3> m_StarColors;
    std::vector<float> m_StarSizes;
    std::vector<float> m_StarTwinkleOffsets;
};

#endif // STAR_LAYER_H