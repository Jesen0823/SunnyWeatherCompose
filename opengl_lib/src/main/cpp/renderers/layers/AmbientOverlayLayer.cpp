#include "AmbientOverlayLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "../../util/ShaderLoader.h"
#include "gtc/matrix_transform.hpp"

AmbientOverlayLayer::AmbientOverlayLayer() 
    : GLLayerBase(LAYER_TYPE_AMBIENT_OVERLAY),
      m_FlashIntensity(0.0f),
      m_IsNight(false),
      m_IsFogMode(false),
      m_FogIntensity(0.0f),
      m_ScreenWidth(0),
      m_ScreenHeight(0),
      m_VaoId(GL_NONE),
      m_MVPMatLoc(GL_NONE),
      m_FlashIntensityLoc(GL_NONE),
      m_IsNightLoc(GL_NONE),
      m_IsFogModeLoc(GL_NONE),
      m_FogIntensityLoc(GL_NONE) {
    m_VboIds[0] = m_VboIds[1] = m_VboIds[2] = GL_NONE;
}

AmbientOverlayLayer::~AmbientOverlayLayer() {}

bool AmbientOverlayLayer::Init() {
    if (m_ProgramObj) {
        return true;
    }

    std::string vShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/ambient_overlay_layer_v.glsl");
    std::string fShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/ambient_overlay_layer_f.glsl");

    if (vShaderStr.empty()) {
        LOGCATE("AmbientOverlayLayer::Init: failed to load vertex shader");
        return false;
    }
    if (fShaderStr.empty()) {
        LOGCATE("AmbientOverlayLayer::Init: failed to load fragment shader");
        return false;
    }

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr.c_str(), fShaderStr.c_str(), m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("AmbientOverlayLayer::Init create program fail");
        return false;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_FlashIntensityLoc = glGetUniformLocation(m_ProgramObj, "u_flashIntensity");
    m_IsNightLoc = glGetUniformLocation(m_ProgramObj, "u_isNight");
    m_IsFogModeLoc = glGetUniformLocation(m_ProgramObj, "u_isFogMode");
    m_FogIntensityLoc = glGetUniformLocation(m_ProgramObj, "u_fogIntensity");

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

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

    glBindVertexArray(GL_NONE);
    return true;
}

void AmbientOverlayLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj || !m_Enabled) {
        return;
    }

    if (!m_IsFogMode && m_FlashIntensity < 0.001f) {
        return;
    }

    m_ScreenWidth = screenW;
    m_ScreenHeight = screenH;

    UpdateMVPMatrix();

    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniform1f(m_FlashIntensityLoc, m_FlashIntensity);
    glUniform1i(m_IsNightLoc, m_IsNight ? 1 : 0);
    glUniform1i(m_IsFogModeLoc, m_IsFogMode ? 1 : 0);
    glUniform1f(m_FogIntensityLoc, m_FogIntensity);

    glEnable(GL_BLEND);
    if (m_IsFogMode) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);

    glDisable(GL_BLEND);

    glBindVertexArray(GL_NONE);
}

void AmbientOverlayLayer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        m_ProgramObj = GL_NONE;
    }
}

void AmbientOverlayLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        case PARAM_LIGHTNING_IS_NIGHT:
            m_IsNight = (value != 0);
            break;
        case PARAM_FOG_MODE:
            m_IsFogMode = (value != 0);
            break;
        default:
            break;
    }
}

void AmbientOverlayLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_LIGHTNING_INTERVAL:
            break;
        case PARAM_FOG_INTENSITY:
            m_FogIntensity = value;
            break;
        default:
            break;
    }
}

void AmbientOverlayLayer::SetFlashIntensity(float intensity) {
    m_FlashIntensity = intensity;
}

void AmbientOverlayLayer::UpdateMVPMatrix() {
    float ratio = (float) m_ScreenWidth / m_ScreenHeight;
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 Model = glm::mat4(1.0f);
    m_MVPMatrix = Projection * View * Model;
}
