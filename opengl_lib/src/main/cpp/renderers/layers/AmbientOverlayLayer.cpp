#include "AmbientOverlayLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "gtc/matrix_transform.hpp"

AmbientOverlayLayer::AmbientOverlayLayer() 
    : GLLayerBase(LAYER_TYPE_AMBIENT_OVERLAY),
      m_FlashIntensity(0.0f),
      m_IsNight(false),
      m_ScreenWidth(0),
      m_ScreenHeight(0),
      m_VaoId(GL_NONE),
      m_MVPMatLoc(GL_NONE),
      m_FlashIntensityLoc(GL_NONE),
      m_IsNightLoc(GL_NONE) {
    m_VboIds[0] = m_VboIds[1] = m_VboIds[2] = GL_NONE;
}

AmbientOverlayLayer::~AmbientOverlayLayer() {}

bool AmbientOverlayLayer::Init() {
    if (m_ProgramObj) {
        return true;
    }

    char vShaderStr[] =
            "#version 300 es                                       \n"
            "layout(location = 0) in vec4 a_position;              \n"
            "uniform mat4 u_MVPMatrix;                             \n"
            "void main(){                                          \n"
            "    gl_Position = u_MVPMatrix * a_position;           \n"
            "}";

    char fShaderStr[] =
            "#version 300 es                                                          \n"
            "precision highp float;                                                   \n"
            "layout(location = 0) out vec4 outColor;                                  \n"
            "uniform float u_flashIntensity;                                          \n"
            "uniform bool u_isNight;                                                  \n"
            "                                                                         \n"
            "void main() {                                                             \n"
            "    vec3 flashColor = u_isNight ? vec3(0.25, 0.35, 0.55) : vec3(0.4, 0.5, 0.65);\n"
            "    float intensity = u_flashIntensity * 0.18;                           \n"
            "    vec3 result = flashColor * intensity;                                 \n"
            "    result = clamp(result, 0.0, 1.0);                                     \n"
            "    outColor = vec4(result, intensity);                                   \n"
            "}";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("AmbientOverlayLayer::Init create program fail");
        return false;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
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

void AmbientOverlayLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj || !m_Enabled || m_FlashIntensity < 0.001f) {
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

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
        default:
            break;
    }
}

void AmbientOverlayLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_LIGHTNING_INTERVAL:
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
