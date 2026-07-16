#ifndef GLLIB_GLRENDERCONTEXT_H
#define GLLIB_GLRENDERCONTEXT_H

#include <stdint.h>
#include "renderers/base/GLRendererBase.h"
#include "renderers/BeatingHeartRenderer.h"
#include "renderers/BezierCurveRenderer.h"
#include "renderers/CloudRenderer.h"
#include "util/ImageDef.h"

class GLRenderContext {
public:
    static GLRenderContext *GetInstance();

    static void DestroyInstance();

    void SetImageData(int format, int width, int height, uint8_t *pData);

    void SetParamsInt(int paramType, int value0, int value1);

    void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void OnSurfaceCreated();

    void OnSurfaceChanged(int width, int height);

    void OnDrawFrame();

private:
    GLRenderContext();

    virtual ~GLRenderContext();

    void LoadRenderer(int type);

    void UnLoadRenderer();

    static GLRenderContext *m_pInstance;
    GLRendererBase *m_pRenderer;
    NativeImage m_RenderImage;
    int m_RendererType;
    int m_ScreenWidth;
    int m_ScreenHeight;
};

#endif
