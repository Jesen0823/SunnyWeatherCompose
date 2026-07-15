#ifndef GLLIB_BEZIERCURVESAMPLE_H
#define GLLIB_BEZIERCURVESAMPLE_H

#include <detail/type_mat4x4.hpp>
#include <detail/type_mat.hpp>
#include "base/GLSampleBase.h"
#include "CoordSystemSample.h"

class BezierCurveSample : public GLSampleBase {

public:
    BezierCurveSample();

    virtual ~BezierCurveSample();

    virtual void Init();

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

    CoordSystemSample *m_pCoordSystemSample;
    int m_FrameIndex;
};

#endif