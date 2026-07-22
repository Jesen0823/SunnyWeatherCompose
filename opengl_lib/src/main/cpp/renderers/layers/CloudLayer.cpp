#include "CloudLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "../../util/ShaderLoader.h"
#include "gtc/matrix_transform.hpp"

CloudLayer::CloudLayer() 
    : GLLayerBase(LAYER_TYPE_CLOUD),
      m_CloudCoverage(0.85f),
      m_CloudDarkness(0.80f),
      m_CloudLightness(1.0f),
      m_CloudSpeed(0.01f),
      m_CloudScale(0.7f),
      m_CloudAlpha(20.0f),
      m_IsNight(false),
      m_CloudMode(0),
      m_MoonPos(0.25f, 0.8f),
      m_ScreenWidth(0),
      m_ScreenHeight(0),
      m_Time(0.0f),
      m_FrameIndex(0),
      m_VaoId(GL_NONE),
      m_MVPMatLoc(GL_NONE),
      m_TimeLoc(GL_NONE),
      m_ScreenSizeLoc(GL_NONE),
      m_CloudCoverageLoc(GL_NONE),
      m_CloudDarknessLoc(GL_NONE),
      m_CloudLightnessLoc(GL_NONE),
      m_CloudSpeedLoc(GL_NONE),
      m_CloudScaleLoc(GL_NONE),
      m_CloudAlphaLoc(GL_NONE),
      m_IsNightLoc(GL_NONE),
      m_CloudModeLoc(GL_NONE) {
    m_VboIds[0] = m_VboIds[1] = m_VboIds[2] = GL_NONE;
}

CloudLayer::~CloudLayer() {}

bool CloudLayer::Init() {
    LOGCATI("CloudLayer::Init: called, m_ProgramObj=%d", m_ProgramObj);
    if (m_ProgramObj) {
        LOGCATI("CloudLayer::Init: already initialized, skipping");
        return true;
    }

    std::string vShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/cloud_layer_v.glsl");
    std::string fShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/cloud_layer_f.glsl");

    if (vShaderStr.empty()) {
        LOGCATE("CloudLayer::Init: failed to load vertex shader");
        return false;
    }
    if (fShaderStr.empty()) {
        LOGCATE("CloudLayer::Init: failed to load fragment shader");
        return false;
    }

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr.c_str(), fShaderStr.c_str(), m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("CloudLayer::Init create program fail");
        return false;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_CloudCoverageLoc = glGetUniformLocation(m_ProgramObj, "u_cloudCoverage");
    m_CloudDarknessLoc = glGetUniformLocation(m_ProgramObj, "u_cloudDarkness");
    m_CloudLightnessLoc = glGetUniformLocation(m_ProgramObj, "u_cloudLightness");
    m_CloudSpeedLoc = glGetUniformLocation(m_ProgramObj, "u_cloudSpeed");
    m_CloudScaleLoc = glGetUniformLocation(m_ProgramObj, "u_cloudScale");
    m_CloudAlphaLoc = glGetUniformLocation(m_ProgramObj, "u_cloudAlpha");
    m_IsNightLoc = glGetUniformLocation(m_ProgramObj, "u_isNight");
    m_CloudModeLoc = glGetUniformLocation(m_ProgramObj, "u_cloudMode");
    m_MoonPosLoc = glGetUniformLocation(m_ProgramObj, "u_moonPos");

    // 顶点坐标
    GLfloat verticesCoords[] = {
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
    };

    // 纹理坐标
    GLfloat textureCoords[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
    };

    // 索引
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
    LOGCATI("CloudLayer::Init success, m_ProgramObj = %d", m_ProgramObj);
    return true;
}

void CloudLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj || !m_Enabled) {
        return;
    }

    if (m_ScreenWidth != screenW || m_ScreenHeight != screenH) {
        m_ScreenWidth = screenW;
        m_ScreenHeight = screenH;
        UpdateMVPMatrix();
    }

    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    m_FrameIndex++;
    m_Time = m_FrameIndex * 0.04f;

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniform1f(m_TimeLoc, m_Time);
    glUniform2f(m_ScreenSizeLoc, screenW, screenH);
    glUniform1f(m_CloudCoverageLoc, m_CloudCoverage);
    glUniform1f(m_CloudDarknessLoc, m_CloudDarkness);
    glUniform1f(m_CloudLightnessLoc, m_CloudLightness);
    glUniform1f(m_CloudSpeedLoc, m_CloudSpeed);
    glUniform1f(m_CloudScaleLoc, m_CloudScale);
    glUniform1f(m_CloudAlphaLoc, m_CloudAlpha);
    glUniform1i(m_IsNightLoc, m_IsNight ? 1 : 0);
    glUniform1i(m_CloudModeLoc, m_CloudMode);
    glUniform2f(m_MoonPosLoc, m_MoonPos.x, m_MoonPos.y);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);

    glDisable(GL_BLEND);

    glBindVertexArray(GL_NONE);
}

void CloudLayer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        m_ProgramObj = GL_NONE;
    }
}

void CloudLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        case PARAM_TIME_OF_DAY:
            m_IsNight = (value == 1);
            break;
        case PARAM_CLOUD_MODE:
            m_CloudMode = value;
            break;
        default:
            break;
    }
}

void CloudLayer::SetParamVec2(LayerParamType paramType, float x, float y) {
    switch (paramType) {
        case PARAM_MOON_POSITION:
            m_MoonPos = glm::vec2(x, y);
            break;
        default:
            break;
    }
}

void CloudLayer::SetParamVec3(LayerParamType paramType, float x, float y, float z) {
    switch (paramType) {
        case PARAM_MOON_POSITION:
            m_MoonPos = glm::vec2(x, y);
            break;
        default:
            break;
    }
}

void CloudLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_CLOUD_COVERAGE:
            m_CloudCoverage = value;
            break;
        case PARAM_CLOUD_DARKNESS:
            m_CloudDarkness = value;
            break;
        case PARAM_CLOUD_LIGHTNESS:
            m_CloudLightness = value;
            break;
        case PARAM_CLOUD_SPEED:
            m_CloudSpeed = value;
            break;
        case PARAM_CLOUD_SCALE:
            m_CloudScale = value;
            break;
        case PARAM_CLOUD_ALPHA:
            m_CloudAlpha = value;
            break;
        default:
            break;
    }
}

void CloudLayer::UpdateMVPMatrix() {
    float ratio = (float) m_ScreenWidth / m_ScreenHeight;

    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 Model = glm::mat4(1.0f);

    m_MVPMatrix = Projection * View * Model;
}