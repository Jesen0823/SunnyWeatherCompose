#include "GLRenderContext.h"
#include "util/LogUtil.h"

GLRenderContext *GLRenderContext::m_pInstance = nullptr;
MySyncLock GLRenderContext::m_SingletonLock;

GLRenderContext::GLRenderContext() {
    m_pRenderer = nullptr;
    m_pCompositeRenderer = nullptr;
    m_RendererType = RENDERER_TYPE_KEY_BEZIER_CURVE;
    m_ScreenWidth = 0;
    m_ScreenHeight = 0;
}

GLRenderContext::~GLRenderContext() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
    UnLoadRenderer();
}

GLRenderContext *GLRenderContext::GetInstance() {
    if (m_pInstance == nullptr) {
        m_SingletonLock.Lock();
        if (m_pInstance == nullptr) {
            m_pInstance = new GLRenderContext();
        }
        m_SingletonLock.UnLock();
    }
    return m_pInstance;
}

void GLRenderContext::DestroyInstance() {
    if (m_pInstance != nullptr) {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

void GLRenderContext::LoadRenderer(int type) {
    LOGCATE("GLRenderContext::LoadRenderer type = %d", type);
    UnLoadRenderer();

    m_RendererType = type;

    switch (type) {
        case RENDERER_TYPE_KEY_BEATING_HEART:
            m_pRenderer = new BeatingHeartRenderer();
            break;
        case RENDERER_TYPE_KEY_BEZIER_CURVE:
            m_pRenderer = new BezierCurveRenderer();
            break;
        case RENDERER_TYPE_KEY_CLOUD:
            m_pRenderer = new CloudRenderer();
            break;
        default:
            LOGCATE("GLRenderContext::LoadRenderer unknown type = %d", type);
            break;
    }

    if (m_pRenderer) {
        if (m_RenderImage.ppPlane[0]) {
            m_pRenderer->LoadImage(&m_RenderImage);
        }
        m_pRenderer->Init();
    }
}

void GLRenderContext::UnLoadRenderer() {
    if (m_pRenderer) {
        m_pRenderer->Destroy();
        delete m_pRenderer;
        m_pRenderer = nullptr;
    }
    if (m_pCompositeRenderer) {
        m_pCompositeRenderer->Destroy();
        delete m_pCompositeRenderer;
        m_pCompositeRenderer = nullptr;
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

    if (m_pRenderer) {
        m_pRenderer->LoadImage(&m_RenderImage);
    }
}

void GLRenderContext::SetParamsInt(int paramType, int value0, int value1) {
    LOGCATE("GLRenderContext::SetParamsInt paramType = %d, value0 = %d, value1 = %d", paramType, value0, value1);

    m_Lock.Lock();
    
    if (paramType == RENDERER_TYPE) {
        if (m_pCompositeRenderer) {
            LOGCATE("GLRenderContext::SetParamsInt: skipping LoadRenderer, CompositeRenderer active");
        } else {
            LoadRenderer(value0);
        }
    } else {
        if (m_pRenderer) {
            m_pRenderer->SetParamsInt(paramType, value0, value1);
        }
    }
    
    m_Lock.UnLock();
}

void GLRenderContext::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    LOGCATE("GLRenderContext::UpdateTransformMatrix rotateX = %f, rotateY = %f, scaleX = %f, scaleY = %f",
            rotateX, rotateY, scaleX, scaleY);
    if (m_pRenderer) {
        m_pRenderer->UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    }
}

void GLRenderContext::OnSurfaceCreated() {
    LOGCATE("GLRenderContext::OnSurfaceCreated");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_Lock.Lock();
    LOGCATI("GLRenderContext::OnSurfaceCreated: lock acquired");
    LOGCATI("GLRenderContext::OnSurfaceCreated: m_CurrentSkycon=[%s], m_pCompositeRenderer=[%p], m_pRenderer=[%p]", 
            m_CurrentSkycon.c_str(), m_pCompositeRenderer, m_pRenderer);
    
    if (!m_CurrentSkycon.empty()) {
        LOGCATI("GLRenderContext::OnSurfaceCreated: skycon set, creating CompositeRenderer");
        
        if (m_pCompositeRenderer) {
            LOGCATI("GLRenderContext::OnSurfaceCreated: destroying existing CompositeRenderer=%p", m_pCompositeRenderer);
            m_pCompositeRenderer->Destroy();
            delete m_pCompositeRenderer;
            m_pCompositeRenderer = nullptr;
            LOGCATI("GLRenderContext::OnSurfaceCreated: existing CompositeRenderer destroyed");
        }
        
        m_pCompositeRenderer = new CompositeRenderer();
        LOGCATI("GLRenderContext::OnSurfaceCreated: new CompositeRenderer created=%p", m_pCompositeRenderer);
        
        LOGCATI("GLRenderContext::OnSurfaceCreated: calling ConfigureLayers(skycon=%s)", m_CurrentSkycon.c_str());
        m_pCompositeRenderer->ConfigureLayers(m_CurrentSkycon.c_str());
        LOGCATI("GLRenderContext::OnSurfaceCreated: ConfigureLayers completed");
        
        LOGCATI("GLRenderContext::OnSurfaceCreated: calling CompositeRenderer::Init()");
        m_pCompositeRenderer->Init();
        LOGCATI("GLRenderContext::OnSurfaceCreated: CompositeRenderer::Init() completed");
        
        LOGCATI("GLRenderContext::OnSurfaceCreated: using CompositeRenderer for skycon=%s, layerCount=%d", 
                m_CurrentSkycon.c_str(), m_pCompositeRenderer->GetLayerCount());
    } else if (m_pRenderer == nullptr) {
        LOGCATI("GLRenderContext::OnSurfaceCreated: no skycon, loading traditional renderer type=%d", m_RendererType);
        LoadRenderer(m_RendererType);
        LOGCATI("GLRenderContext::OnSurfaceCreated: traditional renderer loaded");
    } else if (m_pCompositeRenderer) {
        LOGCATI("GLRenderContext::OnSurfaceCreated: CompositeRenderer exists but no skycon, switching to traditional");
        m_pCompositeRenderer->Destroy();
        delete m_pCompositeRenderer;
        m_pCompositeRenderer = nullptr;
        LoadRenderer(m_RendererType);
        LOGCATI("GLRenderContext::OnSurfaceCreated: switched to traditional renderer");
    }
    
    m_Lock.UnLock();
    LOGCATI("GLRenderContext::OnSurfaceCreated: lock released, done");
}

void GLRenderContext::OnSurfaceChanged(int width, int height) {
    LOGCATE("GLRenderContext::OnSurfaceChanged width = %d, height = %d", width, height);
    
    m_Lock.Lock();
    LOGCATI("GLRenderContext::OnSurfaceChanged: lock acquired, m_pCompositeRenderer=%p, m_pRenderer=%p", 
            m_pCompositeRenderer, m_pRenderer);
    
    m_ScreenWidth = width;
    m_ScreenHeight = height;

    glViewport(0, 0, width, height);

    if (m_pCompositeRenderer) {
        LOGCATI("GLRenderContext::OnSurfaceChanged: calling CompositeRenderer::Init()");
        m_pCompositeRenderer->Init();
        LOGCATI("GLRenderContext::OnSurfaceChanged: CompositeRenderer::Init() completed");
    } else if (m_pRenderer) {
        LOGCATI("GLRenderContext::OnSurfaceChanged: calling traditional renderer::Init()");
        m_pRenderer->Init();
        LOGCATI("GLRenderContext::OnSurfaceChanged: traditional renderer::Init() completed");
    } else {
        LOGCATI("GLRenderContext::OnSurfaceChanged: no renderer available to init");
    }
    
    m_Lock.UnLock();
    LOGCATI("GLRenderContext::OnSurfaceChanged: lock released, done");
}

void GLRenderContext::OnDrawFrame() {
    static int frameCount = 0;
    if (frameCount % 60 == 0) {
        LOGCATI("GLRenderContext::OnDrawFrame frame=%d, composite=%p, traditional=%p", 
                frameCount, m_pCompositeRenderer, m_pRenderer);
    }
    frameCount++;
    
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    m_Lock.Lock();
    if (m_pCompositeRenderer) {
        m_pCompositeRenderer->Draw(m_ScreenWidth, m_ScreenHeight);
    } else if (m_pRenderer) {
        m_pRenderer->Draw(m_ScreenWidth, m_ScreenHeight);
    }
    m_Lock.UnLock();
}

void GLRenderContext::SetSkycon(const char *skycon) {
    LOGCATE("GLRenderContext::SetSkycon skycon = %s", skycon ? skycon : "null");
    
    if (!skycon || std::string(skycon).empty()) {
        LOGCATE("GLRenderContext::SetSkycon: skycon is empty, returning");
        return;
    }

    std::string skyconStr(skycon);
    
    m_Lock.Lock();
    LOGCATI("GLRenderContext::SetSkycon: lock acquired");
    LOGCATI("GLRenderContext::SetSkycon: m_CurrentSkycon=[%s], m_pCompositeRenderer=[%p], m_ScreenSize=%dx%d", 
            m_CurrentSkycon.c_str(), m_pCompositeRenderer, m_ScreenWidth, m_ScreenHeight);
    
    if (m_CurrentSkycon == skyconStr && m_pCompositeRenderer) {
        LOGCATI("GLRenderContext::SetSkycon: skycon unchanged and renderer exists, skipping");
        m_Lock.UnLock();
        return;
    }

    LOGCATI("GLRenderContext::SetSkycon: skycon changed or no renderer, unloading old renderer");
    UnLoadRenderer();
    LOGCATI("GLRenderContext::SetSkycon: old renderer unloaded");

    m_CurrentSkycon = skyconStr;
    LOGCATI("GLRenderContext::SetSkycon: m_CurrentSkycon updated to [%s]", m_CurrentSkycon.c_str());
    
    m_pCompositeRenderer = new CompositeRenderer();
    LOGCATI("GLRenderContext::SetSkycon: new CompositeRenderer created=%p", m_pCompositeRenderer);
    
    LOGCATI("GLRenderContext::SetSkycon: calling ConfigureLayers(skycon=%s)", skycon);
    m_pCompositeRenderer->ConfigureLayers(skycon);
    LOGCATI("GLRenderContext::SetSkycon: ConfigureLayers completed, layerCount=%d", m_pCompositeRenderer->GetLayerCount());
    
    if (m_ScreenWidth > 0 && m_ScreenHeight > 0) {
        LOGCATI("GLRenderContext::SetSkycon: screen size valid, calling CompositeRenderer::Init()");
        m_pCompositeRenderer->Init();
        LOGCATI("GLRenderContext::SetSkycon: CompositeRenderer::Init() completed");
    } else {
        LOGCATI("GLRenderContext::SetSkycon: screen size not set yet, deferring Init()");
    }
    
    LOGCATI("GLRenderContext::SetSkycon: skycon=%s, compositeRenderer=%p, screenSize=%dx%d", 
            skycon, m_pCompositeRenderer, m_ScreenWidth, m_ScreenHeight);
    
    m_Lock.UnLock();
    LOGCATI("GLRenderContext::SetSkycon: lock released, done");
}