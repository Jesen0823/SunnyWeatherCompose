#include "SkyBackgroundLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "../../util/ShaderLoader.h"
#include "gtc/matrix_transform.hpp"

SkyBackgroundLayer::SkyBackgroundLayer() 
    : GLLayerBase(LAYER_TYPE_SKY_BACKGROUND),
      m_TimeOfDay(0),
      m_SkyColorTop(0.4f, 0.7f, 1.0f),
      m_SkyColorBottom(0.2f, 0.4f, 0.6f),
      m_SunIntensity(1.0f),
      m_MoonIntensity(0.8f),
      m_SunVisible(true),
      m_SkyMode(0),
      m_ScreenWidth(0),
      m_ScreenHeight(0),
      m_Time(0.0f),
      m_FrameIndex(0),
      m_VaoId(GL_NONE),
      m_MVPMatLoc(GL_NONE),
      m_TimeLoc(GL_NONE),
      m_ScreenSizeLoc(GL_NONE),
      m_TimeOfDayLoc(GL_NONE),
      m_SkyColorTopLoc(GL_NONE),
      m_SkyColorBottomLoc(GL_NONE),
      m_SunIntensityLoc(GL_NONE),
      m_MoonIntensityLoc(GL_NONE) {
    m_VboIds[0] = m_VboIds[1] = m_VboIds[2] = GL_NONE;
}

SkyBackgroundLayer::~SkyBackgroundLayer() {}

bool SkyBackgroundLayer::Init() {
    LOGCATI("SkyBackgroundLayer::Init: called, m_ProgramObj=%d", m_ProgramObj);
    if (m_ProgramObj) {
        LOGCATI("SkyBackgroundLayer::Init: already initialized, skipping");
        return true;
    }

    std::string vShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/sky_background_layer_v.glsl");
    std::string fShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/sky_background_layer_f.glsl");

    if (vShaderStr.empty()) {
        LOGCATE("SkyBackgroundLayer::Init: failed to load vertex shader");
        return false;
    }
    if (fShaderStr.empty()) {
        LOGCATE("SkyBackgroundLayer::Init: failed to load fragment shader");
        return false;
    }

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr.c_str(), fShaderStr.c_str(), m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("SkyBackgroundLayer::Init create program fail");
        return false;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_TimeOfDayLoc = glGetUniformLocation(m_ProgramObj, "u_timeOfDay");
    m_SkyColorTopLoc = glGetUniformLocation(m_ProgramObj, "u_skyColorTop");
    m_SkyColorBottomLoc = glGetUniformLocation(m_ProgramObj, "u_skyColorBottom");
    m_SunIntensityLoc = glGetUniformLocation(m_ProgramObj, "u_sunIntensity");
    m_MoonIntensityLoc = glGetUniformLocation(m_ProgramObj, "u_moonIntensity");
    m_SunVisibleLoc = glGetUniformLocation(m_ProgramObj, "u_sunVisible");
    m_SkyModeLoc = glGetUniformLocation(m_ProgramObj, "u_skyMode");

    // 顶点坐标
    GLfloat verticesCoords[] = {
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
    };

    // 纹理坐标（虽然这里不需要纹理，但为了保持一致性）
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
    LOGCATI("SkyBackgroundLayer::Init success, m_ProgramObj = %d", m_ProgramObj);
    return true;
}

void SkyBackgroundLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj) {
        LOGCATE("SkyBackgroundLayer::Draw: m_ProgramObj is null, skipping");
        return;
    }
    if (!m_Enabled) {
        LOGCATE("SkyBackgroundLayer::Draw: layer disabled, skipping");
        return;
    }

    m_ScreenWidth = screenW;
    m_ScreenHeight = screenH;

    UpdateMVPMatrix();

    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    // 更新时间
    m_FrameIndex++;
    m_Time = m_FrameIndex * 0.02f;

    // 设置 uniform 参数
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniform1f(m_TimeLoc, m_Time);
    glUniform2f(m_ScreenSizeLoc, screenW, screenH);
    glUniform1i(m_TimeOfDayLoc, m_TimeOfDay);
    glUniform3f(m_SkyColorTopLoc, m_SkyColorTop.x, m_SkyColorTop.y, m_SkyColorTop.z);
    glUniform3f(m_SkyColorBottomLoc, m_SkyColorBottom.x, m_SkyColorBottom.y, m_SkyColorBottom.z);
    glUniform1f(m_SunIntensityLoc, m_SunIntensity);
    glUniform1f(m_MoonIntensityLoc, m_MoonIntensity);
    glUniform1i(m_SunVisibleLoc, m_SunVisible ? 1 : 0);
    glUniform1i(m_SkyModeLoc, m_SkyMode);

    // 关闭深度测试，确保背景层完全覆盖
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    glBindVertexArray(GL_NONE);
}

void SkyBackgroundLayer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        m_ProgramObj = GL_NONE;
    }
}

void SkyBackgroundLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        case PARAM_TIME_OF_DAY:
            m_TimeOfDay = value;
            break;
        case PARAM_SUN_VISIBLE:
            m_SunVisible = (value != 0);
            break;
        case PARAM_SKY_MODE:
            m_SkyMode = value;
            break;
        default:
            break;
    }
}

void SkyBackgroundLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_SUN_INTENSITY:
            m_SunIntensity = value;
            break;
        case PARAM_MOON_INTENSITY:
            m_MoonIntensity = value;
            break;
        default:
            break;
    }
}

void SkyBackgroundLayer::SetParamVec3(LayerParamType paramType, float x, float y, float z) {
    switch (paramType) {
        case PARAM_SKY_COLOR_TOP:
            m_SkyColorTop = glm::vec3(x, y, z);
            break;
        case PARAM_SKY_COLOR_BOTTOM:
            m_SkyColorBottom = glm::vec3(x, y, z);
            break;
        default:
            break;
    }
}

void SkyBackgroundLayer::UpdateMVPMatrix() {
    float ratio = (float) m_ScreenWidth / m_ScreenHeight;

    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 Model = glm::mat4(1.0f);

    m_MVPMatrix = Projection * View * Model;
}