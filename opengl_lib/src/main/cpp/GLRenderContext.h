#ifndef GLLIB_GLRENDERCONTEXT_H
#define GLLIB_GLRENDERCONTEXT_H

#include <stdint.h>
#include <string>
#include "renderers/base/GLRendererBase.h"
#include "renderers/BeatingHeartRenderer.h"
#include "renderers/BezierCurveRenderer.h"
#include "renderers/CloudRenderer.h"
#include "renderers/CompositeRenderer.h"
#include "util/ImageDef.h"
#include "util/LockUtil.h"

/**
 * GL渲染上下文
 * 
 * 管理OpenGL渲染器的生命周期和状态切换，支持传统renderer和天气动效系统。
 * 
 * 支持的渲染器类型：
 * - RENDERER_TYPE_KEY_BEATING_HEART: 心跳动画
 * - RENDERER_TYPE_KEY_CLOUD: 云朵动画（传统）
 * - RENDERER_TYPE_KEY_BEZIER_CURVE: 贝塞尔曲线
 * - RENDERER_TYPE_KEY_COORD_SYSTEM: 坐标系
 * - RENDERER_TYPE_KEY_WEATHER: 天气动效（基于CompositeRenderer）
 */
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

    /**
     * 根据天气类型字符串配置渲染器
     * @param skycon 天气类型字符串（如 "CLEAR_DAY", "LIGHT_RAIN" 等）
     */
    void SetSkycon(const char *skycon);

private:
    GLRenderContext();

    virtual ~GLRenderContext();

    void LoadRenderer(int type);

    void UnLoadRenderer();

    static GLRenderContext *m_pInstance;
    static MySyncLock m_SingletonLock;
    GLRendererBase *m_pRenderer;
    CompositeRenderer *m_pCompositeRenderer;
    NativeImage m_RenderImage;
    int m_RendererType;
    int m_ScreenWidth;
    int m_ScreenHeight;
    std::string m_CurrentSkycon;
    MySyncLock m_Lock;
};

#endif
