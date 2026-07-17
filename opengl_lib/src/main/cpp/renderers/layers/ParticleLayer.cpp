#include "ParticleLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "gtc/matrix_transform.hpp"

ParticleLayer::ParticleLayer() 
    : GLLayerBase(LAYER_TYPE_PARTICLE),
      m_ParticleType(0),
      m_ParticleDensity(0.5f),
      m_ParticleColor(0.6f, 0.55f, 0.45f),
      m_ParticleVisibility(0.5f),
      m_ScreenWidth(0),
      m_ScreenHeight(0),
      m_Time(0.0f),
      m_FrameIndex(0),
      m_VaoId(GL_NONE),
      m_MVPMatLoc(GL_NONE),
      m_TimeLoc(GL_NONE),
      m_ScreenSizeLoc(GL_NONE),
      m_ParticleTypeLoc(GL_NONE),
      m_ParticleDensityLoc(GL_NONE),
      m_ParticleColorLoc(GL_NONE),
      m_ParticleVisibilityLoc(GL_NONE) {
    m_VboIds[0] = m_VboIds[1] = m_VboIds[2] = GL_NONE;
}

ParticleLayer::~ParticleLayer() {}

bool ParticleLayer::Init() {
    LOGCATI("ParticleLayer::Init: called, m_ProgramObj=%d", m_ProgramObj);
    if (m_ProgramObj) {
        LOGCATI("ParticleLayer::Init: already initialized, skipping");
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

    // 片段着色器：雾霾、雾、浮尘、沙尘颗粒效果
    char fShaderStr[] =
            "#version 300 es                                                          \n"
            "precision highp float;                                                   \n"
            "layout(location = 0) out vec4 outColor;                                  \n"
            "uniform float u_time;                                                    \n"
            "uniform vec2 u_screenSize;                                               \n"
            "uniform int u_particleType;                                              \n"
            "uniform float u_particleDensity;                                         \n"
            "uniform vec3 u_particleColor;                                            \n"
            "uniform float u_particleVisibility;                                      \n"
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
            "float fbm(vec2 n) {                                                       \n"
            "    float total = 0.0, amplitude = 0.5;                                  \n"
            "    for (int i = 0; i < 5; i++) {                                        \n"
            "        total += noise(n) * amplitude;                                   \n"
            "        n *= 2.0;                                                        \n"
            "        amplitude *= 0.5;                                                \n"
            "    }                                                                    \n"
            "    return total;                                                        \n"
            "}                                                                        \n"
            "                                                                         \n"
            "void main() {                                                             \n"
            "    vec2 p = gl_FragCoord.xy / u_screenSize.xy;                          \n"
            "    vec2 uv = p * vec2(u_screenSize.x / u_screenSize.y, 1.0);            \n"
            "    vec3 result = vec3(0.0);                                             \n"
            "    float alpha = 0.0;                                                   \n"
            "                                                                         \n"
            "    float speed = 0.02;                                                   \n"
            "    if (u_particleType == 3) {                                           \n"
            "        speed = 0.05;                                                    \n"
            "    }                                                                    \n"
            "                                                                         \n"
            "    // 基础颗粒效果                                                       \n"
            "    float particleNoise = fbm(uv * 5.0 + vec2(u_time * speed, 0.0));     \n"
            "    float particle = smoothstep(0.3, 0.7, particleNoise);                \n"
            "    particle *= u_particleDensity;                                        \n"
            "                                                                         \n"
            "    // 根据类型调整效果                                                   \n"
            "    if (u_particleType == 0) {                                           \n"
            "        // 雾霾效果：均匀分布的颗粒                                       \n"
            "        alpha = particle * (1.0 - u_particleVisibility) * 0.8;           \n"
            "    } else if (u_particleType == 1) {                                    \n"
            "        // 雾效果：底部更浓，顶部渐淡                                     \n"
            "        float fogDensity = 1.0 - smoothstep(0.0, 1.0, p.y);              \n"
            "        alpha = particle * fogDensity * (1.0 - u_particleVisibility);    \n"
            "    } else if (u_particleType == 2) {                                    \n"
            "        // 浮尘效果：中等颗粒感                                           \n"
            "        alpha = particle * (1.0 - u_particleVisibility) * 0.6;           \n"
            "    } else {                                                              \n"
            "        // 沙尘效果：大颗粒，横向飘动                                     \n"
            "        float sandNoise = fbm(uv * 3.0 + vec2(u_time * speed * 2.0, 0.0));\n"
            "        float sand = smoothstep(0.2, 0.8, sandNoise);                    \n"
            "        alpha = sand * u_particleDensity * (1.0 - u_particleVisibility) * 0.7;\n"
            "    }                                                                    \n"
            "                                                                         \n"
            "    // 边缘模糊效果                                                       \n"
            "    float edgeBlur = smoothstep(0.0, 0.2, p.x) * smoothstep(0.8, 1.0, p.x);\n"
            "    edgeBlur *= smoothstep(0.0, 0.2, p.y) * smoothstep(0.8, 1.0, p.y);   \n"
            "    alpha *= edgeBlur;                                                   \n"
            "                                                                         \n"
            "    result = u_particleColor * alpha;                                    \n"
            "    alpha = clamp(alpha, 0.0, 1.0);                                      \n"
            "    outColor = vec4(result, alpha);                                       \n"
            "}";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("ParticleLayer::Init create program fail");
        return false;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_ParticleTypeLoc = glGetUniformLocation(m_ProgramObj, "u_particleType");
    m_ParticleDensityLoc = glGetUniformLocation(m_ProgramObj, "u_particleDensity");
    m_ParticleColorLoc = glGetUniformLocation(m_ProgramObj, "u_particleColor");
    m_ParticleVisibilityLoc = glGetUniformLocation(m_ProgramObj, "u_particleVisibility");

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

void ParticleLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj) {
        LOGCATE("ParticleLayer::Draw: m_ProgramObj is null, skipping");
        return;
    }
    if (!m_Enabled) {
        LOGCATE("ParticleLayer::Draw: layer disabled, skipping");
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
    glUniform1i(m_ParticleTypeLoc, m_ParticleType);
    glUniform1f(m_ParticleDensityLoc, m_ParticleDensity);
    glUniform3f(m_ParticleColorLoc, m_ParticleColor.x, m_ParticleColor.y, m_ParticleColor.z);
    glUniform1f(m_ParticleVisibilityLoc, m_ParticleVisibility);

    // 启用混合模式，实现半透明颗粒效果
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);

    glDisable(GL_BLEND);

    glBindVertexArray(GL_NONE);
}

void ParticleLayer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        m_ProgramObj = GL_NONE;
    }
}

void ParticleLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        case PARAM_PARTICLE_TYPE:
            m_ParticleType = value;
            break;
        default:
            break;
    }
}

void ParticleLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_PARTICLE_DENSITY:
            m_ParticleDensity = value;
            break;
        case PARAM_PARTICLE_COLOR_R:
            m_ParticleColor.x = value;
            break;
        case PARAM_PARTICLE_COLOR_G:
            m_ParticleColor.y = value;
            break;
        case PARAM_PARTICLE_COLOR_B:
            m_ParticleColor.z = value;
            break;
        case PARAM_PARTICLE_VISIBILITY:
            m_ParticleVisibility = value;
            break;
        default:
            break;
    }
}

void ParticleLayer::SetParamVec3(LayerParamType paramType, float x, float y, float z) {
    switch (paramType) {
        case PARAM_SKY_COLOR_TOP:
        case PARAM_SKY_COLOR_BOTTOM:
            m_ParticleColor = glm::vec3(x, y, z);
            break;
        default:
            break;
    }
}

void ParticleLayer::UpdateMVPMatrix() {
    float ratio = (float) m_ScreenWidth / m_ScreenHeight;

    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 Model = glm::mat4(1.0f);

    m_MVPMatrix = Projection * View * Model;
}