#ifndef GLLIB_GLRENDERERBASE_H
#define GLLIB_GLRENDERERBASE_H

#include <stdint.h>
#include <GLES3/gl3.h>
#include "ImageDef.h"
#include "LockUtil.h"

#define MATH_PI 3.1415926535897932384626433832802

#define RENDERER_TYPE                                 200

#define RENDERER_TYPE_KEY_BEATING_HEART               RENDERER_TYPE+0
#define RENDERER_TYPE_KEY_CLOUD                       RENDERER_TYPE+1
#define RENDERER_TYPE_KEY_BEZIER_CURVE                RENDERER_TYPE+2
#define RENDERER_TYPE_KEY_COORD_SYSTEM                RENDERER_TYPE+3

#define RENDERER_TYPE_KEY_SET_TOUCH_LOC               RENDERER_TYPE+999
#define RENDERER_TYPE_SET_GRAVITY_XY                  RENDERER_TYPE+1000

/**
 * OpenGL渲染器基类
 * 
 * 所有渲染器（包括传统Renderer和Layer）的抽象基类，定义统一的生命周期接口。
 * 
 * 设计模式：Template Method模式
 * - 定义渲染器的骨架算法，子类实现具体步骤
 * - 提供统一的Init/Draw/Destroy生命周期管理
 * 
 * 核心职责：
 * - 管理着色器程序（顶点着色器、片段着色器、程序对象）
 * - 管理表面尺寸信息
 * - 提供线程安全的操作接口（通过m_Lock）
 * 
 * 使用场景：
 * - 传统Renderer：BeatingHeartRenderer、BezierCurveRenderer、CloudRenderer等
 * - 天气动效Layer：SkyBackgroundLayer、CloudLayer、SnowLayer等（通过GLLayerBase继承）
 */
class GLRendererBase {
protected:
    GLuint m_VertexShader;
    GLuint m_FragmentShader;
    GLuint m_ProgramObj;
    MySyncLock m_Lock;
    int m_SurfaceWidth;
    int m_SurfaceHeight;

public:
    GLRendererBase() {
        this->m_FragmentShader = 0;
        this->m_VertexShader = 0;
        this->m_ProgramObj = 0;
        this->m_SurfaceWidth = 0;
        this->m_SurfaceHeight = 0;
    }

    virtual ~GLRendererBase() {
    }

    virtual bool Init() = 0;

    virtual void LoadImage(NativeImage *pImage) {
    }

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    };

    virtual void LoadMultiImageWithIndex(int index, NativeImage *pImage) {
    };

    virtual void Draw(int screenW, int screenH) = 0;

    virtual void Destroy() = 0;

    virtual void LoadShortArrData(short *const pShortArr, int arrSize) {

    }

    virtual void SetTouchLocation(float x, float y) {

    }

    virtual void SetGravityXY(float x, float y) {

    }

    virtual void SetParamsInt(int paramType, int value0, int value1) {

    }
};

#endif
