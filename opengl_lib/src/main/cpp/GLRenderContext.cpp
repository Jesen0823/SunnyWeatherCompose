#include "GLRenderContext.h"
#include "util/LogUtil.h"

GLRenderContext *GLRenderContext::m_pInstance = nullptr;

GLRenderContext::GLRenderContext() {
    m_pSample = nullptr;
    m_SampleType = SAMPLE_TYPE_KEY_TRIANGLE;
    m_ScreenWidth = 0;
    m_ScreenHeight = 0;
}

GLRenderContext::~GLRenderContext() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
    UnLoadSample();
}

GLRenderContext *GLRenderContext::GetInstance() {
    if (m_pInstance == nullptr) {
        m_pInstance = new GLRenderContext();
    }
    return m_pInstance;
}

void GLRenderContext::DestroyInstance() {
    if (m_pInstance != nullptr) {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

void GLRenderContext::LoadSample(int type) {
    LOGCATE("GLRenderContext::LoadSample type = %d", type);
    UnLoadSample();

    m_SampleType = type;

    switch (type) {
        case SAMPLE_TYPE_KEY_BEATING_HEART:
            m_pSample = new BeatingHeartSample();
            break;
        case SAMPLE_TYPE_KEY_BEZIER_CURVE:
            m_pSample = new BezierCurveSample();
            break;
        case SAMPLE_TYPE_KEY_CLOUD:
            m_pSample = new CloudSample();
            break;
        default:
            LOGCATE("GLRenderContext::LoadSample unknown type = %d", type);
            break;
    }

    if (m_pSample) {
        if (m_RenderImage.ppPlane[0]) {
            m_pSample->LoadImage(&m_RenderImage);
        }
        m_pSample->Init();
    }
}

void GLRenderContext::UnLoadSample() {
    if (m_pSample) {
        m_pSample->Destroy();
        delete m_pSample;
        m_pSample = nullptr;
    }
}

void GLRenderContext::SetImageData(int format, int width, int height, uint8_t *pData) {
    LOGCATE("GLRenderContext::SetImageData format = %d, width = %d, height = %d", format, width, height);

    NativeImageUtil::FreeNativeImage(&m_RenderImage);

    m_RenderImage.format = format;
    m_RenderImage.width = width;
    m_RenderImage.height = height;
    NativeImageUtil::AllocNativeImage(&m_RenderImage);
    memcpy(m_RenderImage.ppPlane[0], pData, width * height * 4);

    if (m_pSample) {
        m_pSample->LoadImage(&m_RenderImage);
    }
}

void GLRenderContext::SetParamsInt(int paramType, int value0, int value1) {
    LOGCATE("GLRenderContext::SetParamsInt paramType = %d, value0 = %d, value1 = %d", paramType, value0, value1);

    switch (paramType) {
        case SAMPLE_TYPE:
            LoadSample(value0);
            break;
        default:
            if (m_pSample) {
                m_pSample->SetParamsInt(paramType, value0, value1);
            }
            break;
    }
}

void GLRenderContext::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    LOGCATE("GLRenderContext::UpdateTransformMatrix rotateX = %f, rotateY = %f, scaleX = %f, scaleY = %f",
            rotateX, rotateY, scaleX, scaleY);
    if (m_pSample) {
        m_pSample->UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    }
}

void GLRenderContext::OnSurfaceCreated() {
    LOGCATE("GLRenderContext::OnSurfaceCreated");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    if (m_pSample == nullptr) {
        LoadSample(m_SampleType);
    }
}

void GLRenderContext::OnSurfaceChanged(int width, int height) {
    LOGCATE("GLRenderContext::OnSurfaceChanged width = %d, height = %d", width, height);
    m_ScreenWidth = width;
    m_ScreenHeight = height;

    glViewport(0, 0, width, height);

    if (m_pSample) {
        m_pSample->Init();
    }
}

void GLRenderContext::OnDrawFrame() {
    LOGCATE("GLRenderContext::OnDrawFrame");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if (m_pSample) {
        m_pSample->Draw(m_ScreenWidth, m_ScreenHeight);
    }
}