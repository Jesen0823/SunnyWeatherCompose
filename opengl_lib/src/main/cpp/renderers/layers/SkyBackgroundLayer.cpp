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
      m_StarDensity(1.2f),
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
      m_MoonIntensityLoc(GL_NONE),
      m_StarDensityLoc(GL_NONE) {
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
            "uniform float u_starDensity;                                              \n"
            "                                                                         \n"
            "vec2 hash(vec2 p) {                                                      \n"
            "    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));    \n"
            "    return -1.0 + 2.0 * fract(sin(p) * 43758.5453123);                   \n"
            "}                                                                        \n"
            "                                                                         \n"
            "float noise(vec2 p) {                                                     \n"
            "    const float K1 = 0.366025404;                                         \n"
            "    const float K2 = 0.211324865;                                         \n"
            "    vec2 i = floor(p + (p.x + p.y) * K1);                                 \n"
            "    vec2 a = p - i + (i.x + i.y) * K2;                                   \n"
            "    vec2 o = (a.x > a.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);              \n"
            "    vec2 b = a - o + K2;                                                 \n"
            "    vec2 c = a - 1.0 + 2.0 * K2;                                         \n"
            "    vec3 h = max(0.5 - vec3(dot(a, a), dot(b, b), dot(c, c)), 0.0);      \n"
            "    vec3 n = h * h * h * h * vec3(dot(a, hash(i)),                        \n"
            "                                  dot(b, hash(i + o)),                     \n"
            "                                  dot(c, hash(i + 1.0)));                  \n"
            "    return dot(n, vec3(70.0));                                            \n"
            "}                                                                        \n"
            "                                                                         \n"
            "void main() {                                                             \n"
            "    vec2 p = gl_FragCoord.xy / u_screenSize.xy;                          \n"
            "    vec3 skyColor = mix(u_skyColorBottom, u_skyColorTop, p.y);            \n"
            "    vec3 result = skyColor;                                               \n"
            "    if (u_timeOfDay == 0) {                                               \n"
            "        vec2 sunPos = vec2(0.75, 0.85);                                   \n"
            "        float dist = distance(p, sunPos);                                  \n"
            "        float sunCore = 1.0 - smoothstep(0.0, 0.08, dist);                \n"
            "        float sunInnerGlow = 1.0 - smoothstep(0.0, 0.20, dist);           \n"
            "        float sunOuterGlow = 1.0 - smoothstep(0.0, 0.45, dist);           \n"
            "        float sunRays = 1.0 - smoothstep(0.0, 0.65, dist);               \n"
            "        vec3 sunColor = vec3(1.0, 0.92, 0.75) * u_sunIntensity * 0.8;    \n"
            "        vec3 sunCoreColor = vec3(1.0, 1.0, 0.98) * 0.9;                  \n"
            "        vec3 sunGlowColor = vec3(1.0, 0.82, 0.5) * 0.6;                  \n"
            "        vec3 sunRayColor = vec3(1.0, 0.9, 0.6) * 0.15;                   \n"
            "        result = mix(result, sunCoreColor, sunCore * 0.9);                \n"
            "        result = result + sunGlowColor * sunInnerGlow * 0.3;              \n"
            "        result = result + sunGlowColor * sunOuterGlow * 0.15;             \n"
            "        float rayAngle = atan(p.y - sunPos.y, p.x - sunPos.x);            \n"
            "        float rayPattern = sin(rayAngle * 10.0) * 0.4 + 0.6;              \n"
            "        rayPattern *= sin(rayAngle * 5.0) * 0.2 + 0.8;                    \n"
            "        result = result + sunRayColor * sunRays * rayPattern * 0.25;      \n"
            "    } else {                                                              \n"
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
    m_StarDensityLoc = glGetUniformLocation(m_ProgramObj, "u_starDensity");

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
    glUniform1f(m_StarDensityLoc, m_StarDensity);

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
        case PARAM_STAR_DENSITY:
            m_StarDensity = value;
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