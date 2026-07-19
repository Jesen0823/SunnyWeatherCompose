#ifndef STAR_LAYER_H
#define STAR_LAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>
#include <vector>

/**
 * 星空层
 * 
 * 负责渲染夜间星星效果，包括：
 * - 随机分布的星星位置
 * - 星星闪烁动画（不同频率和相位）
 * - 星星颜色变化（白色到淡黄色）
 * - 星星大小变化（随机大小）
 * 
 * 实现原理：
 * 使用 VBO 存储星星的位置、颜色、大小和闪烁偏移量，通过顶点着色器计算
 * 闪烁动画，片段着色器绘制带发光效果的星星。星星数量可配置，支持不同
 * 天气场景下的可见性变化。
 * 
 * 参数配置：
 * - starCount: 星星数量（10~500），控制屏幕上星星总数
 * - twinkleSpeed: 闪烁速度（0.5~2.0），控制星星闪烁快慢
 * - starColor: 星星颜色（RGB），默认为白色偏黄
 * 
 * 使用场景：
 * - CLEAR_NIGHT（晴朗夜间）：300颗星星，正常闪烁
 * - PARTLY_CLOUDY_NIGHT（多云夜间）：150颗星星，部分被云层遮挡
 * - CLOUDY（阴天）：30颗星星，大部分被云层遮挡
 * - RAIN/SNOW（雨雪天气）：20颗星星，几乎不可见
 */
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