#ifndef GLLIB_BEZIERCURVERENDERER_H
#define GLLIB_BEZIERCURVERENDERER_H

#include <detail/type_mat4x4.hpp>
#include <detail/type_mat.hpp>
#include "base/GLRendererBase.h"
#include "CoordSystemRenderer.h"

/**
 * 贝塞尔曲线渲染器（旧版）
 * 
 * 负责渲染贝塞尔曲线，支持二阶贝塞尔曲线的绘制和动画。
 * 内置坐标系统渲染器，用于显示坐标系参考。
 * 
 * 注意：此类为旧版渲染器，不参与天气动效系统。
 */
class BezierCurveRenderer : public GLRendererBase {

public:
    BezierCurveRenderer();

    virtual ~BezierCurveRenderer();

    virtual bool Init();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Draw(int screenW, int screenH);

    virtual void Destroy();

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    void DrawArray();

    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboId;
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

    CoordSystemRenderer *m_pCoordSystemRenderer;
    int m_FrameIndex;
};

#endif
