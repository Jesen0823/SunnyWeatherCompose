#ifndef GLLIB_CLOUDRENDERER_H
#define GLLIB_CLOUDRENDERER_H

#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include "base/GLRendererBase.h"

/**
 * 云层渲染器（旧版）
 * 
 * 负责渲染云层效果，使用纹理图片作为云层素材。
 * 支持通过 MVP 矩阵变换实现云层的旋转和缩放动画。
 * 
 * 注意：此类为旧版云层渲染器，新项目推荐使用 CloudLayer 类，
 * 后者使用程序化生成云层，支持更多参数配置和更好的视觉效果。
 */
class CloudRenderer : public GLRendererBase {

public:
    CloudRenderer();

    virtual ~CloudRenderer();

    virtual void LoadImage(NativeImage *pImage);

    virtual bool Init();

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
