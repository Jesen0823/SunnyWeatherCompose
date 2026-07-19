#include "WindLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "gtc/matrix_transform.hpp"

WindLayer::WindLayer() 
    : GLLayerBase(LAYER_TYPE_WIND),
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
      m_WindLinesEnabledLoc(GL_NONE),
      m_WindStrengthLoc(GL_NONE) {
    m_VboIds[0] = m_VboIds[1] = m_VboIds[2] = GL_NONE;
}

WindLayer::~WindLayer() {}

bool WindLayer::Init() {
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
            "uniform float u_time;                                                    \n"
            "uniform vec2 u_screenSize;                                               \n"
            "uniform bool u_windLinesEnabled;                                         \n"
            "uniform float u_windStrength;                                            \n"
            "                                                                         \n"
            "float hash(float x) {                                                     \n"
            "    return fract(sin(x) * 43758.5453123);                                  \n"
            "}                                                                        \n"
            "                                                                         \n"
            "float noise(vec2 p) {                                                     \n"
            "    vec2 i = floor(p);                                                    \n"
            "    vec2 f = fract(p);                                                    \n"
            "    float u = f.x * f.x * (3.0 - 2.0 * f.x);                              \n"
            "    float v = f.y * f.y * (3.0 - 2.0 * f.y);                              \n"
            "    float a = hash(i.x + hash(i.y));                                       \n"
            "    float b = hash(i.x + 1.0 + hash(i.y));                                \n"
            "    float c = hash(i.x + hash(i.y + 1.0));                                \n"
            "    float d = hash(i.x + 1.0 + hash(i.y + 1.0));                          \n"
            "    return mix(mix(a, b, u), mix(c, d, u), v);                            \n"
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
            "    \n"
            "    if (u_windLinesEnabled) {                                            \n"
            "        float wind = windLine(p, u_time, u_windStrength);                \n"
            "        result += vec3(0.8, 0.85, 0.9) * wind;                           \n"
            "        alpha += wind;                                                    \n"
            "    }                                                                    \n"
            "    \n"
            "    alpha = clamp(alpha, 0.0, 1.0);                                      \n"
            "    outColor = vec4(result, alpha);                                       \n"
            "}";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("WindLayer::Init create program fail");
        return false;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_WindLinesEnabledLoc = glGetUniformLocation(m_ProgramObj, "u_windLinesEnabled");
    m_WindStrengthLoc = glGetUniformLocation(m_ProgramObj, "u_windStrength");

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

void WindLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj || !m_Enabled) {
        return;
    }

    m_ScreenWidth = screenW;
    m_ScreenHeight = screenH;

    UpdateMVPMatrix();

    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    m_FrameIndex++;
    m_Time = m_FrameIndex * 0.04f;

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniform1f(m_TimeLoc, m_Time);
    glUniform2f(m_ScreenSizeLoc, screenW, screenH);
    glUniform1i(m_WindLinesEnabledLoc, m_WindLinesEnabled ? 1 : 0);
    glUniform1f(m_WindStrengthLoc, m_WindStrength);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);

    glDisable(GL_BLEND);

    glBindVertexArray(GL_NONE);
}

void WindLayer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        m_ProgramObj = GL_NONE;
    }
}

void WindLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        case PARAM_WIND_LINES_ENABLED:
            m_WindLinesEnabled = (value != 0);
            break;
        default:
            break;
    }
}

void WindLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_WIND_STRENGTH:
            m_WindStrength = value;
            break;
        default:
            break;
    }
}

void WindLayer::UpdateMVPMatrix() {
    float ratio = (float) m_ScreenWidth / m_ScreenHeight;
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 Model = glm::mat4(1.0f);
    m_MVPMatrix = Projection * View * Model;
}
