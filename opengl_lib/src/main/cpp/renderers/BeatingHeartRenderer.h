#ifndef GLLIB_BEATINGHEARTRENDERER_H
#define GLLIB_BEATINGHEARTRENDERER_H

#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include "base/GLRendererBase.h"

/**
 * 心跳动画渲染器（旧版）
 * 
 * 负责渲染心跳动画效果，使用纹理图片作为心跳素材，
 * 通过时间参数控制心跳的缩放动画。
 * 
 * 注意：此类为旧版渲染器，不参与天气动效系统。
 */
class BeatingHeartRenderer : public GLRendererBase {

public:
    BeatingHeartRenderer();

    virtual ~BeatingHeartRenderer();

    virtual bool Init();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Draw(int screenW, int screenH);

    virtual void Destroy();

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_TimeLoc;
    GLint m_SizeLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};

#endif
