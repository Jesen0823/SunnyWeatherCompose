#ifndef GLLIB_COORDSYSTEMRENDERER_H
#define GLLIB_COORDSYSTEMRENDERER_H

#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <GLES3/gl3.h>
#include "base/GLRendererBase.h"
#include "ImageDef.h"

/**
 * 坐标系统渲染器（旧版）
 * 
 * 负责渲染3D坐标系（X/Y/Z轴），用于辅助调试和可视化。
 * 
 * 注意：此类为旧版渲染器，不参与天气动效系统。
 */
class CoordSystemRenderer : public GLRendererBase {
public:
    CoordSystemRenderer();

    virtual ~CoordSystemRenderer();

    virtual bool Init();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Draw(int screenW, int screenH);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    virtual void Destroy();

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VAOId;
    GLuint m_VBOIds[3];
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};

#endif
