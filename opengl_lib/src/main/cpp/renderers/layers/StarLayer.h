#ifndef STAR_LAYER_H
#define STAR_LAYER_H

#include "../base/GLLayerBase.h"
#include <glm.hpp>
#include <vector>

/**
 * 星空层
 * 
 * 负责渲染夜间星星效果，包括：
 * - 聚类分布的星星位置（60%概率属于星团，40%随机分布）
 * - 星星闪烁动画（不同频率和相位的正弦波叠加）
 * - 星星颜色变化（白色、淡蓝色、淡黄色）
 * - 星星大小变化（三等分级：亮星、中等星、暗星）
 * 
 * 实现原理：
 * 使用 VAO + 单个合并 VBO 存储星星数据（StarVertex 结构体包含位置、颜色、大小、闪烁偏移），
 * 通过顶点着色器传递属性，片段着色器绘制带发光效果的星星。初始化后释放 CPU 端数据节省内存。
 * 使用加法混合模式（GL_SRC_ALPHA, GL_ONE）实现星星发光叠加效果。
 * 
 * 参数配置：
 * - starCount: 星星数量（10~500），控制屏幕上星星总数
 * - twinkleSpeed: 闪烁速度（0.5~2.0），控制星星闪烁快慢
 * 
 * 使用场景：
 * - CLEAR_NIGHT（晴朗夜间）：300颗星星，正常闪烁
 * - PARTLY_CLOUDY_NIGHT（多云夜间）：50颗星星，部分被云层遮挡
 * - CLOUDY（阴天）：星星不可见
 * - RAIN/SNOW（雨雪天气）：星星不可见
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
    GLuint m_VaoId = 0;

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