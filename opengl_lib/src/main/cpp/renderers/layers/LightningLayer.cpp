#include "LightningLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "../../util/ShaderLoader.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

LightningLayer::LightningLayer() 
    : GLLayerBase(LAYER_TYPE_LIGHTNING),
      m_LightningEnabled(false),
      m_LightningInterval(5.0f),
      m_IsNight(false),
      m_ScreenWidth(0),
      m_ScreenHeight(0),
      m_Time(0.0f),
      m_FrameIndex(0),
      m_CurrentFlashIntensity(0.0f),
      m_VaoId(GL_NONE),
      m_MVPMatLoc(GL_NONE),
      m_TimeLoc(GL_NONE),
      m_ScreenSizeLoc(GL_NONE),
      m_FlashIntensityLoc(GL_NONE),
      m_IsNightLoc(GL_NONE) {
    m_VboIds[0] = m_VboIds[1] = m_VboIds[2] = GL_NONE;
}

LightningLayer::~LightningLayer() {}

bool LightningLayer::Init() {
    if (m_ProgramObj) {
        return true;
    }

    std::string vShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/lightning_layer_v.glsl");
    std::string fShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/lightning_layer_f.glsl");

    if (vShaderStr.empty()) {
        LOGCATE("LightningLayer::Init: failed to load vertex shader");
        return false;
    }
    if (fShaderStr.empty()) {
        LOGCATE("LightningLayer::Init: failed to load fragment shader");
        return false;
    }

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr.c_str(), fShaderStr.c_str(), m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("LightningLayer::Init create program fail");
        return false;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_FlashIntensityLoc = glGetUniformLocation(m_ProgramObj, "u_flashIntensity");
    m_IsNightLoc = glGetUniformLocation(m_ProgramObj, "u_isNight");

    GLfloat verticesCoords[] = {
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
    };

    GLfloat textureCoords[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
    };

    GLushort indices[] = {0, 1, 2, 0, 2, 3};

    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

    glBindVertexArray(GL_NONE);
    return true;
}

void LightningLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj || !m_Enabled || !m_LightningEnabled) {
        m_CurrentFlashIntensity = 0.0f;
        return;
    }

    m_ScreenWidth = screenW;
    m_ScreenHeight = screenH;

    UpdateMVPMatrix();

    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    m_FrameIndex++;
    m_Time = m_FrameIndex * 0.04f;

    float cycleTime = fmod(m_Time, m_LightningInterval);
    float flashDuration = 1.2f;
    float flashPhase = cycleTime / flashDuration;
    float flash = 0.0f;
    
    if (flashPhase < 1.0f) {
        float preFlash = glm::smoothstep(0.0f, 0.08f, flashPhase) * glm::smoothstep(0.25f, 0.15f, flashPhase) * 0.1f;
        
        float mainFlash = 0.0f;
        if (flashPhase >= 0.25f && flashPhase < 0.35f) {
            float t = (flashPhase - 0.25f) / 0.1f;
            mainFlash = glm::smoothstep(0.0f, 0.1f, t) * glm::smoothstep(1.0f, 0.7f, t);
        }
        
        float bounce1 = 0.0f;
        if (flashPhase >= 0.38f && flashPhase < 0.48f) {
            float t = (flashPhase - 0.38f) / 0.1f;
            bounce1 = glm::smoothstep(0.0f, 0.15f, t) * glm::smoothstep(1.0f, 0.55f, t) * 0.7f;
        }
        
        float bounce2 = 0.0f;
        if (flashPhase >= 0.52f && flashPhase < 0.62f) {
            float t = (flashPhase - 0.52f) / 0.1f;
            bounce2 = glm::smoothstep(0.0f, 0.2f, t) * glm::smoothstep(1.0f, 0.5f, t) * 0.5f;
        }
        
        float bounce3 = 0.0f;
        if (flashPhase >= 0.66f && flashPhase < 0.76f) {
            float t = (flashPhase - 0.66f) / 0.1f;
            bounce3 = glm::smoothstep(0.0f, 0.25f, t) * glm::smoothstep(1.0f, 0.45f, t) * 0.35f;
        }
        
        float decay = glm::smoothstep(0.8f, 1.0f, flashPhase) * 0.2f;
        
        flash = preFlash + mainFlash + bounce1 + bounce2 + bounce3 + decay;
    }
    
    m_CurrentFlashIntensity = flash;

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniform1f(m_TimeLoc, m_Time);
    glUniform2f(m_ScreenSizeLoc, screenW, screenH);
    glUniform1f(m_FlashIntensityLoc, flash);
    glUniform1i(m_IsNightLoc, m_IsNight ? 1 : 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);

    glDisable(GL_BLEND);

    glBindVertexArray(GL_NONE);
}

void LightningLayer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        m_ProgramObj = GL_NONE;
    }
}

void LightningLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        case PARAM_LIGHTNING_ENABLED:
            m_LightningEnabled = (value != 0);
            break;
        case PARAM_LIGHTNING_IS_NIGHT:
            m_IsNight = (value != 0);
            break;
        default:
            break;
    }
}

void LightningLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_LIGHTNING_INTERVAL:
            m_LightningInterval = value;
            break;
        default:
            break;
    }
}

void LightningLayer::UpdateMVPMatrix() {
    float ratio = (float) m_ScreenWidth / m_ScreenHeight;
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 Model = glm::mat4(1.0f);
    m_MVPMatrix = Projection * View * Model;
}
