#include "SkyBackgroundLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
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

    // 顶点着色器
    char vShaderStr[] =
            "#version 300 es                                       \n"
            "layout(location = 0) in vec4 a_position;              \n"
            "uniform mat4 u_MVPMatrix;                             \n"
            "void main(){                                          \n"
            "    gl_Position = u_MVPMatrix * a_position;           \n"
            "}";

    // ====== 完整版本（已修复）======
    // 修复内容：将 hash 函数返回类型从 float 改为 vec2
    // 原问题：sin(p) 返回 vec2，导致整个表达式返回 vec2，但函数声明返回 float
    // 影响：dot(a, hash(i)) 中 dot(vec2, float) 类型不匹配，编译失败
    // ========================================
    char fShaderStr[] =
            "#version 300 es                                                          \n"
            "precision highp float;                                                   \n"
            "layout(location = 0) out vec4 outColor;                                  \n"
            "uniform float u_time;                                                    \n"
            "uniform vec2 u_screenSize;                                               \n"
            "uniform int u_timeOfDay;                                                 \n"
            "uniform vec3 u_skyColorTop;                                              \n"
            "uniform vec3 u_skyColorBottom;                                           \n"
            "uniform float u_sunIntensity;                                             \n"
            "uniform float u_moonIntensity;                                            \n"
            "uniform bool u_sunVisible;                                                \n"
            "uniform int u_skyMode;                                                    \n"
            "                                                                         \n"
            "void main() {                                                             \n"
            "    vec2 p = gl_FragCoord.xy / u_screenSize.xy;                          \n"
            "    vec3 skyColor = mix(u_skyColorBottom, u_skyColorTop, p.y);            \n"
            "    vec3 result = skyColor;                                               \n"
            "    if (u_skyMode == 1) {                                                 \n"
            "        result = skyColor * 0.85;                                         \n"
            "    } else if (u_skyMode == 2) {                                          \n"
            "        result = skyColor * 0.95 + vec3(0.02, 0.02, 0.05);                \n"
            "    } else if (u_skyMode == 3) {                                          \n"
            "        result = skyColor + vec3(0.05, 0.03, 0.0);                        \n"
            "    }                                                                     \n"
            "    if (u_timeOfDay == 0 && u_sunVisible) {                               \n"
            "        vec2 sunPos = vec2(0.70, 0.80);                                   \n"
            "        float dist = distance(p, sunPos);                                  \n"
            "        float angle = atan(p.y - sunPos.y, p.x - sunPos.x);                \n"
            "                                                                         \n"
            "        float sunCore = exp(-dist * 80.0);                                \n"
            "        float sunInnerGlow = exp(-dist * 12.0);                           \n"
            "        float sunOuterGlow = exp(-dist * 4.0);                            \n"
            "                                                                         \n"
            "        vec3 sunCoreColor = vec3(1.0, 1.0, 0.98);                        \n"
            "        vec3 sunInnerColor = vec3(1.0, 0.98, 0.92);                      \n"
            "        vec3 sunOuterColor = vec3(1.0, 0.92, 0.75);                      \n"
            "                                                                         \n"
            "        float coreBlend = smoothstep(0.0, 0.025, dist);                  \n"
            "        result = mix(sunCoreColor, result, coreBlend);                    \n"
            "                                                                         \n"
            "        result = result + sunInnerColor * sunInnerGlow * 0.5;            \n"
            "        result = result + sunOuterColor * sunOuterGlow * 0.35;           \n"
            "                                                                         \n"
            "        float rayPattern = sin(angle * 6.0 + u_time * 0.2) * 0.3 + 0.7;  \n"
            "        rayPattern *= sin(angle * 3.0) * 0.2 + 0.8;                      \n"
            "        float sunRays = exp(-dist * 2.5);                                \n"
            "        vec3 sunRayColor = vec3(1.0, 0.95, 0.85);                       \n"
            "        result = result + sunRayColor * sunRays * rayPattern * 0.12;     \n"
            "    } else if (u_timeOfDay == 1) {                                        \n"
            "        vec2 moonPos = vec2(0.25, 0.8);                                   \n"
            "        float moonDist = distance(p, moonPos);                            \n"
            "        float moonGlow = 1.0 - smoothstep(0.0, 0.50, moonDist);           \n"
            "        float moonCore = 1.0 - smoothstep(0.0, 0.06, moonDist);           \n"
            "        vec3 moonColor = vec3(0.95, 0.96, 1.0) * u_moonIntensity;         \n"
            "        result = result + moonColor * moonGlow * 0.22;                    \n"
            "        result = result + vec3(1.0, 1.0, 1.0) * moonCore * 0.6;           \n"
            "    }                                                                    \n"
            "    outColor = vec4(result, 1.0);                                         \n"
            "}";

    /* ====== 简化版（调试用）======
    // 功能：仅绘制蓝天渐变 + 太阳效果，无星星和月亮
    // 使用场景：验证着色器基础架构是否正常工作
    // ========================================
    char fShaderStr[] =
            "#version 300 es                                                          \n"
            "precision highp float;                                                   \n"
            "layout(location = 0) out vec4 outColor;                                  \n"
            "uniform float u_time;                                                    \n"
            "uniform vec2 u_screenSize;                                               \n"
            "uniform int u_timeOfDay;                                                 \n"
            "uniform vec3 u_skyColorTop;                                              \n"
            "uniform vec3 u_skyColorBottom;                                           \n"
            "uniform float u_sunIntensity;                                             \n"
            "uniform float u_moonIntensity;                                            \n"
            "uniform float u_starDensity;                                              \n"
            "                                                                         \n"
            "void main() {                                                             \n"
            "    vec2 p = gl_FragCoord.xy / u_screenSize.xy;                          \n"
            "    vec3 skyColor = mix(u_skyColorBottom, u_skyColorTop, p.y);            \n"
            "    vec3 result = skyColor;                                               \n"
            "    if (u_timeOfDay == 0) {                                               \n"
            "        vec2 sunPos = vec2(0.75, 0.85);                                   \n"
            "        float dist = distance(p, sunPos);                                  \n"
            "        float sunGlow = 1.0 - smoothstep(0.0, 0.5, dist);                 \n"
            "        float sunCore = 1.0 - smoothstep(0.0, 0.12, dist);                \n"
            "        vec3 sunColor = vec3(1.0, 0.9, 0.6) * u_sunIntensity;            \n"
            "        result = result + sunColor * sunGlow * 0.4;                        \n"
            "        result = result + vec3(1.0, 1.0, 0.9) * sunCore * 0.6;            \n"
            "    }                                                                    \n"
            "    outColor = vec4(result, 1.0);                                         \n"
            "}";
    */

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
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