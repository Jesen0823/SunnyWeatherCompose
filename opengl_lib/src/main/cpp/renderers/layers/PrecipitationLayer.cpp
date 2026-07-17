#include "PrecipitationLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "gtc/matrix_transform.hpp"

PrecipitationLayer::PrecipitationLayer() 
    : GLLayerBase(LAYER_TYPE_PRECIPITATION),
      m_PrecipitationType(0),
      m_PrecipitationIntensity(0.5f),
      m_PrecipitationSpeed(0.5f),
      m_PrecipitationSize(1.0f),
      m_ScreenWidth(0),
      m_ScreenHeight(0),
      m_Time(0.0f),
      m_FrameIndex(0),
      m_VaoId(GL_NONE),
      m_MVPMatLoc(GL_NONE),
      m_TimeLoc(GL_NONE),
      m_ScreenSizeLoc(GL_NONE),
      m_PrecipitationTypeLoc(GL_NONE),
      m_PrecipitationIntensityLoc(GL_NONE),
      m_PrecipitationSpeedLoc(GL_NONE),
      m_PrecipitationSizeLoc(GL_NONE) {
    m_VboIds[0] = m_VboIds[1] = m_VboIds[2] = GL_NONE;
}

PrecipitationLayer::~PrecipitationLayer() {}

bool PrecipitationLayer::Init() {
    LOGCATI("PrecipitationLayer::Init: called, m_ProgramObj=%d", m_ProgramObj);
    if (m_ProgramObj) {
        LOGCATI("PrecipitationLayer::Init: already initialized, skipping");
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

    // 片段着色器：雨滴和雪花粒子效果
    char fShaderStr[] =
            "#version 300 es                                                          \n"
            "precision highp float;                                                   \n"
            "layout(location = 0) out vec4 outColor;                                  \n"
            "uniform float u_time;                                                    \n"
            "uniform vec2 u_screenSize;                                               \n"
            "uniform int u_precipitationType;                                         \n"
            "uniform float u_precipitationIntensity;                                  \n"
            "uniform float u_precipitationSpeed;                                      \n"
            "uniform float u_precipitationSize;                                       \n"
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
            "void main() {                                                             \n"
            "    vec2 p = gl_FragCoord.xy / u_screenSize.xy;                          \n"
            "    vec2 uv = p * vec2(u_screenSize.x / u_screenSize.y, 1.0);            \n"
            "    vec3 result = vec3(0.0);                                             \n"
            "    float alpha = 0.0;                                                   \n"
            "                                                                         \n"
            "    if (u_precipitationType == 0) {                                      \n"
            "        // 雨滴效果                                                       \n"
            "        float dropCount = 100.0 + u_precipitationIntensity * 200.0;      \n"
            "        float dropSpeed = 0.5 + u_precipitationSpeed * 1.5;              \n"
            "        float dropSize = 0.01 + u_precipitationSize * 0.02;              \n"
            "        float dropLength = dropSize * 3.0;                                \n"
            "                                                                         \n"
            "        for (float i = 0.0; i < dropCount; i += 1.0) {                   \n"
            "            vec2 seed = vec2(i * 7.123, i * 13.789);                     \n"
            "            float x = fract(noise(seed));                                 \n"
            "            float y = fract(noise(seed + vec2(0.1, 0.0)));               \n"
            "            vec2 dropPos = vec2(x, y - u_time * dropSpeed * 0.1);        \n"
            "            dropPos.y = fract(dropPos.y);                                \n"
            "                                                                         \n"
            "            float dist = abs(p.y - dropPos.y) / dropLength;              \n"
            "            dist += abs(p.x - dropPos.x) / dropSize;                     \n"
            "            float drop = smoothstep(0.0, 1.0, 1.0 - dist);               \n"
            "            drop *= drop;                                                \n"
            "                                                                         \n"
            "            result += vec3(0.7, 0.8, 0.9) * drop;                        \n"
            "            alpha += drop;                                                \n"
            "        }                                                                \n"
            "    } else {                                                              \n"
            "        // 雪花效果                                                       \n"
            "        float snowCount = 30.0 + u_precipitationIntensity * 100.0;       \n"
            "        float snowSpeed = 0.1 + u_precipitationSpeed * 0.3;              \n"
            "        float snowSize = 0.02 + u_precipitationSize * 0.04;              \n"
            "                                                                         \n"
            "        for (float i = 0.0; i < snowCount; i += 1.0) {                   \n"
            "            vec2 seed = vec2(i * 9.567, i * 17.345);                    \n"
            "            float x = fract(noise(seed));                                 \n"
            "            float y = fract(noise(seed + vec2(0.1, 0.0)));               \n"
            "            vec2 snowPos = vec2(x, y - u_time * snowSpeed * 0.1);        \n"
            "            snowPos.y = fract(snowPos.y);                                \n"
            "            snowPos.x += sin(u_time * 0.5 + i) * 0.02;                   \n"
            "                                                                         \n"
            "            float dist = length(p - snowPos) / snowSize;                  \n"
            "            float snow = smoothstep(0.0, 1.0, 1.0 - dist);               \n"
            "            snow *= snow;                                                \n"
            "                                                                         \n"
            "            float twinkle = sin(u_time * 2.0 + i) * 0.3 + 0.7;          \n"
            "            result += vec3(1.0) * snow * twinkle;                        \n"
            "            alpha += snow;                                                \n"
            "        }                                                                \n"
            "    }                                                                    \n"
            "                                                                         \n"
            "    alpha = clamp(alpha * 0.5, 0.0, 1.0);                                \n"
            "    outColor = vec4(result, alpha);                                       \n"
            "}";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("PrecipitationLayer::Init create program fail");
        return false;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_PrecipitationTypeLoc = glGetUniformLocation(m_ProgramObj, "u_precipitationType");
    m_PrecipitationIntensityLoc = glGetUniformLocation(m_ProgramObj, "u_precipitationIntensity");
    m_PrecipitationSpeedLoc = glGetUniformLocation(m_ProgramObj, "u_precipitationSpeed");
    m_PrecipitationSizeLoc = glGetUniformLocation(m_ProgramObj, "u_precipitationSize");

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

void PrecipitationLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj) {
        LOGCATE("PrecipitationLayer::Draw: m_ProgramObj is null, skipping");
        return;
    }
    if (!m_Enabled) {
        LOGCATE("PrecipitationLayer::Draw: layer disabled, skipping");
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
    glUniform1i(m_PrecipitationTypeLoc, m_PrecipitationType);
    glUniform1f(m_PrecipitationIntensityLoc, m_PrecipitationIntensity);
    glUniform1f(m_PrecipitationSpeedLoc, m_PrecipitationSpeed);
    glUniform1f(m_PrecipitationSizeLoc, m_PrecipitationSize);

    // 启用混合模式，实现半透明雨滴/雪花效果
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);

    glDisable(GL_BLEND);

    glBindVertexArray(GL_NONE);
}

void PrecipitationLayer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        m_ProgramObj = GL_NONE;
    }
}

void PrecipitationLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        case PARAM_PRECIPITATION_TYPE:
            m_PrecipitationType = value;
            break;
        default:
            break;
    }
}

void PrecipitationLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_PRECIPITATION_INTENSITY:
            m_PrecipitationIntensity = value;
            break;
        case PARAM_PRECIPITATION_SPEED:
            m_PrecipitationSpeed = value;
            break;
        case PARAM_PRECIPITATION_SIZE:
            m_PrecipitationSize = value;
            break;
        default:
            break;
    }
}

void PrecipitationLayer::UpdateMVPMatrix() {
    float ratio = (float) m_ScreenWidth / m_ScreenHeight;

    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 Model = glm::mat4(1.0f);

    m_MVPMatrix = Projection * View * Model;
}