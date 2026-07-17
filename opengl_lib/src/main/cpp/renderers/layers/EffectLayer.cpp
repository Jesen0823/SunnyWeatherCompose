#include "EffectLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "gtc/matrix_transform.hpp"

EffectLayer::EffectLayer() 
    : GLLayerBase(LAYER_TYPE_EFFECT),
      m_LightningEnabled(false),
      m_LightningInterval(5.0f),
      m_WindLinesEnabled(false),
      m_WindStrength(0.5f),
      m_ScreenWidth(0),
      m_ScreenHeight(0),
      m_Time(0.0f),
      m_FrameIndex(0),
      m_VaoId(GL_NONE),
      m_MVPMatLoc(GL_NONE),
      m_TimeLoc(GL_NONE),
      m_ScreenSizeLoc(GL_NONE),
      m_LightningEnabledLoc(GL_NONE),
      m_LightningIntervalLoc(GL_NONE),
      m_WindLinesEnabledLoc(GL_NONE),
      m_WindStrengthLoc(GL_NONE) {
    m_VboIds[0] = m_VboIds[1] = m_VboIds[2] = GL_NONE;
}

EffectLayer::~EffectLayer() {}

bool EffectLayer::Init() {
    LOGCATI("EffectLayer::Init: called, m_ProgramObj=%d", m_ProgramObj);
    if (m_ProgramObj) {
        LOGCATI("EffectLayer::Init: already initialized, skipping");
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

    // 片段着色器：闪电和风力线条特效
    char fShaderStr[] =
            "#version 300 es                                                          \n"
            "precision highp float;                                                   \n"
            "layout(location = 0) out vec4 outColor;                                  \n"
            "uniform float u_time;                                                    \n"
            "uniform vec2 u_screenSize;                                               \n"
            "uniform bool u_lightningEnabled;                                         \n"
            "uniform float u_lightningInterval;                                       \n"
            "uniform bool u_windLinesEnabled;                                         \n"
            "uniform float u_windStrength;                                            \n"
            "                                                                         \n"
            "float hash(vec2 p) {                                                      \n"
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
            "    vec3 n = h * h * h * h * vec3(dot(a, hash(i)), dot(b, hash(i + o)), dot(c, hash(i + 1.0)));\n"
            "    return dot(n, vec3(70.0));                                            \n"
            "}                                                                        \n"
            "                                                                         \n"
            "float lightningFlash(float time, float interval) {                         \n"
            "    float flash = 0.0;                                                    \n"
            "    float noiseVal = noise(vec2(time * 100.0, 0.0));                      \n"
            "    float trigger = smoothstep(0.95, 1.0, noiseVal);                      \n"
            "    if (trigger > 0.5) {                                                 \n"
            "        float flashTime = mod(time, interval);                             \n"
            "        flash = smoothstep(0.0, 0.02, flashTime) * smoothstep(0.05, 0.02, flashTime);\n"
            "    }                                                                    \n"
            "    return flash;                                                         \n"
            "}                                                                        \n"
            "                                                                         \n"
            "float windLine(vec2 p, float time, float strength) {                      \n"
            "    float lineCount = 5.0 + strength * 10.0;                             \n"
            "    float lineSpeed = 1.0 + strength * 2.0;                              \n"
            "    float lineWidth = 0.002 + strength * 0.005;                           \n"
            "    float lineLength = 0.1 + strength * 0.2;                             \n"
            "    float result = 0.0;                                                   \n"
            "                                                                         \n"
            "    for (float i = 0.0; i < lineCount; i += 1.0) {                       \n"
            "        float y = 0.2 + i * 0.15;                                         \n"
            "        float x = mod(time * lineSpeed + i * 0.5, 1.5) - 0.25;            \n"
            "        vec2 linePos = vec2(x, y);                                        \n"
            "                                                                         \n"
            "        float distX = abs(p.x - linePos.x) / lineLength;                  \n"
            "        float distY = abs(p.y - linePos.y) / lineWidth;                   \n"
            "        float dist = max(distX, distY);                                   \n"
            "        float line = smoothstep(0.0, 1.0, 1.0 - dist);                   \n"
            "        line *= line;                                                     \n"
            "                                                                         \n"
            "        float wave = sin(time * 5.0 + i) * 0.05;                         \n"
            "        line *= smoothstep(0.0, 1.0, p.y + wave - 0.1);                  \n"
            "        line *= smoothstep(1.0, 0.0, p.y + wave - 0.9);                  \n"
            "                                                                         \n"
            "        result += line;                                                   \n"
            "    }                                                                    \n"
            "                                                                         \n"
            "    return result;                                                        \n"
            "}                                                                        \n"
            "                                                                         \n"
            "void main() {                                                             \n"
            "    vec2 p = gl_FragCoord.xy / u_screenSize.xy;                          \n"
            "    vec3 result = vec3(0.0);                                             \n"
            "    float alpha = 0.0;                                                   \n"
            "                                                                         \n"
            "    // 闪电效果                                                           \n"
            "    if (u_lightningEnabled) {                                            \n"
            "        float flash = lightningFlash(u_time, u_lightningInterval);        \n"
            "        result += vec3(1.0) * flash;                                      \n"
            "        alpha += flash;                                                   \n"
            "    }                                                                    \n"
            "                                                                         \n"
            "    // 风力线条效果                                                       \n"
            "    if (u_windLinesEnabled) {                                            \n"
            "        float wind = windLine(p, u_time, u_windStrength);                \n"
            "        result += vec3(0.8, 0.85, 0.9) * wind;                           \n"
            "        alpha += wind;                                                    \n"
            "    }                                                                    \n"
            "                                                                         \n"
            "    alpha = clamp(alpha, 0.0, 1.0);                                      \n"
            "    outColor = vec4(result, alpha);                                       \n"
            "}";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("EffectLayer::Init create program fail");
        return false;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_LightningEnabledLoc = glGetUniformLocation(m_ProgramObj, "u_lightningEnabled");
    m_LightningIntervalLoc = glGetUniformLocation(m_ProgramObj, "u_lightningInterval");
    m_WindLinesEnabledLoc = glGetUniformLocation(m_ProgramObj, "u_windLinesEnabled");
    m_WindStrengthLoc = glGetUniformLocation(m_ProgramObj, "u_windStrength");

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
    return true;
}

void EffectLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj) {
        LOGCATE("EffectLayer::Draw: m_ProgramObj is null, skipping");
        return;
    }
    if (!m_Enabled) {
        LOGCATE("EffectLayer::Draw: layer disabled, skipping");
        return;
    }

    m_ScreenWidth = screenW;
    m_ScreenHeight = screenH;

    UpdateMVPMatrix();

    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    // 更新时间
    m_FrameIndex++;
    m_Time = m_FrameIndex * 0.04f;

    // 设置 uniform 参数
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniform1f(m_TimeLoc, m_Time);
    glUniform2f(m_ScreenSizeLoc, screenW, screenH);
    glUniform1i(m_LightningEnabledLoc, m_LightningEnabled ? 1 : 0);
    glUniform1f(m_LightningIntervalLoc, m_LightningInterval);
    glUniform1i(m_WindLinesEnabledLoc, m_WindLinesEnabled ? 1 : 0);
    glUniform1f(m_WindStrengthLoc, m_WindStrength);

    // 启用混合模式，实现特效叠加效果
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);

    glDisable(GL_BLEND);

    glBindVertexArray(GL_NONE);
}

void EffectLayer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        m_ProgramObj = GL_NONE;
    }
}

void EffectLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        case PARAM_EFFECT_LIGHTNING_ENABLED:
            m_LightningEnabled = (value != 0);
            break;
        case PARAM_EFFECT_WIND_LINES_ENABLED:
            m_WindLinesEnabled = (value != 0);
            break;
        default:
            break;
    }
}

void EffectLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_EFFECT_LIGHTNING_INTERVAL:
            m_LightningInterval = value;
            break;
        case PARAM_EFFECT_WIND_STRENGTH:
            m_WindStrength = value;
            break;
        default:
            break;
    }
}

void EffectLayer::UpdateMVPMatrix() {
    float ratio = (float) m_ScreenWidth / m_ScreenHeight;

    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 Model = glm::mat4(1.0f);

    m_MVPMatrix = Projection * View * Model;
}