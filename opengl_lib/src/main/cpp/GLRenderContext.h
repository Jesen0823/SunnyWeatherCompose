#ifndef GLLIB_GLRENDERCONTEXT_H
#define GLLIB_GLRENDERCONTEXT_H

#include <stdint.h>
#include "sample/base/GLSampleBase.h"
#include "sample/BeatingHeartSample.h"
#include "sample/BezierCurveSample.h"
#include "sample/CloudSample.h"
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

    void LoadSample(int type);

    void UnLoadSample();

    static GLRenderContext *m_pInstance;
    GLSampleBase *m_pSample;
    NativeImage m_RenderImage;
    int m_SampleType;
    int m_ScreenWidth;
    int m_ScreenHeight;
};

#endif