#include "SnowLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"

static const char *TAG = "SnowLayer";

SnowLayer::SnowLayer() 
    : GLLayerBase(LAYER_TYPE_SNOW) {
    GenerateSnowflakes();
}

SnowLayer::~SnowLayer() {
    Destroy();
}

void SnowLayer::GenerateSnowflakes() {
    m_ParticlePositions.resize(m_ParticleCount);
    m_ParticleSpeeds.resize(m_ParticleCount);
    m_ParticleLengths.resize(m_ParticleCount);
    m_ParticleWidths.resize(m_ParticleCount);

    for (int i = 0; i < m_ParticleCount; i++) {
        m_ParticlePositions[i] = glm::vec2(
            (float)rand() / RAND_MAX,
            (float)rand() / RAND_MAX * 1.5f - 0.25f
        );
        
        float speedBase = 0.3f + ((float)rand() / RAND_MAX) * 0.4f;
        float angle = -0.05f + ((float)rand() / RAND_MAX) * 0.1f;
        m_ParticleSpeeds[i] = glm::vec2(angle, -speedBase);
        
        float depth = (float)rand() / RAND_MAX;
        m_ParticleLengths[i] = 0.15f + depth * 0.15f;
        
        m_ParticleWidths[i] = 0.005f + ((1.0f - depth) * 0.006f);
    }
}

bool SnowLayer::Init() {
    if (m_ProgramObj != 0) {
        LOGCATI("SnowLayer::Init: already initialized, m_ProgramObj=%d", m_ProgramObj);
        return true;
    }

    LOGCATI("SnowLayer::Init: called, m_ProgramObj=%d, m_ParticleCount=%d", m_ProgramObj, m_ParticleCount);

    const char *vShaderStr =
        "#version 300 es                          \n"
        "precision highp float;                   \n"
        "layout(location = 0) in vec2 a_posOffset;\n"
        "layout(location = 1) in vec2 a_position; \n"
        "layout(location = 2) in vec2 a_speed;    \n"
        "layout(location = 3) in float a_length;  \n"
        "layout(location = 4) in float a_width;   \n"
        "out float v_t;                           \n"
        "out float v_depth;                       \n"
        "uniform float u_time;                    \n"
        "uniform vec2 u_screenSize;               \n"
        "uniform float u_snowSpeed;               \n"
        "void main() {                            \n"
        "    vec2 pos = a_position;               \n"
        "    float speedFactor = 0.5 + u_snowSpeed * 1.5; \n"
        "    pos.y -= u_time * a_speed.y * speedFactor * 0.2; \n"
        "    pos.x += sin(u_time * 0.3 + a_position.x * 10.0) * 0.003;\n"
        "    pos.y = fract(pos.y + 1.25);         \n"
        "    pos.x = fract(pos.x + 0.5);          \n"
        "    vec2 end = pos + a_speed * a_length * 2.0;\n"
        "    vec2 dir = normalize(end - pos);     \n"
        "    vec2 perp = vec2(-dir.y, dir.x);     \n"
        "    vec2 vertexPos = pos + dir * a_length * a_posOffset.y;\n"
        "    vertexPos += perp * a_width * a_posOffset.x;\n"
        "    vec2 clipPos = vertexPos * 2.0 - 1.0;\n"
        "    clipPos.x *= u_screenSize.x / u_screenSize.y;\n"
        "    gl_Position = vec4(clipPos, 0.0, 1.0);\n"
        "    v_t = a_posOffset.y;                 \n"
        "    v_depth = pos.y;                     \n"
        "}";

    const char *fShaderStr =
        "#version 300 es                          \n"
        "precision highp float;                   \n"
        "in float v_t;                            \n"
        "in float v_depth;                        \n"
        "out vec4 outColor;                       \n"
        "uniform float u_time;                    \n"
        "uniform float u_snowIntensity;           \n"
        "void main() {                            \n"
        "    float alpha = smoothstep(0.0, 0.2, v_depth) * smoothstep(1.0, 0.7, v_depth);\n"
        "    alpha *= smoothstep(0.5, 0.0, length(gl_PointCoord - vec2(0.5))) * u_snowIntensity;\n"
        "    float twinkle = sin(gl_FragCoord.x * 0.5 + gl_FragCoord.y * 0.5 + u_time * 2.0) * 0.2 + 0.8;\n"
        "    outColor = vec4(0.92, 0.96, 1.0, alpha * twinkle);\n"
        "}";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj == 0) {
        LOGCATE("SnowLayer::Init create program fail");
        return false;
    }

    m_PosOffsetLoc = glGetAttribLocation(m_ProgramObj, "a_posOffset");
    m_PositionLoc = glGetAttribLocation(m_ProgramObj, "a_position");
    m_SpeedLoc = glGetAttribLocation(m_ProgramObj, "a_speed");
    m_LengthLoc = glGetAttribLocation(m_ProgramObj, "a_length");
    m_WidthLoc = glGetAttribLocation(m_ProgramObj, "a_width");
    
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_SnowIntensityLoc = glGetUniformLocation(m_ProgramObj, "u_snowIntensity");
    m_SnowSpeedLoc = glGetUniformLocation(m_ProgramObj, "u_snowSpeed");

    LOGCATI("SnowLayer::Init: attrib locations - offset=%d, pos=%d, speed=%d, length=%d, width=%d", 
            m_PosOffsetLoc, m_PositionLoc, m_SpeedLoc, m_LengthLoc, m_WidthLoc);

    float quadVertices[] = {
        -1.0f, 0.0f,   // 左下
         1.0f, 0.0f,   // 右下
        -1.0f, 1.0f,   // 左上
         1.0f, 1.0f    // 右上
    };

    glGenBuffers(1, &m_QuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_PositionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_PositionVBO);
    glBufferData(GL_ARRAY_BUFFER, m_ParticleCount * sizeof(glm::vec2), m_ParticlePositions.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_SpeedVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SpeedVBO);
    glBufferData(GL_ARRAY_BUFFER, m_ParticleCount * sizeof(glm::vec2), m_ParticleSpeeds.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_LengthVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_LengthVBO);
    glBufferData(GL_ARRAY_BUFFER, m_ParticleCount * sizeof(float), m_ParticleLengths.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_WidthVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_WidthVBO);
    glBufferData(GL_ARRAY_BUFFER, m_ParticleCount * sizeof(float), m_ParticleWidths.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    LOGCATI("SnowLayer::Init success, m_ProgramObj=%d, particleCount=%d", m_ProgramObj, m_ParticleCount);
    return true;
}

void SnowLayer::Draw(int screenW, int screenH) {
    if (m_ProgramObj == 0 || !m_Enabled) {
        return;
    }

    m_ScreenWidth = screenW;
    m_ScreenHeight = screenH;
    m_Time += 0.016f;
    
    static int logFrameCount = 0;
    if (logFrameCount++ % 60 == 0) {
        LOGCATI("SnowLayer::Draw: intensity=%.2f, speed=%.2f, particleCount=%d", 
                m_SnowIntensity, m_SnowSpeed, m_ParticleCount);
    }

    glUseProgram(m_ProgramObj);

    glUniform1f(m_TimeLoc, m_Time);
    glUniform2f(m_ScreenSizeLoc, (float)screenW, (float)screenH);
    glUniform1f(m_SnowIntensityLoc, m_SnowIntensity);
    glUniform1f(m_SnowSpeedLoc, m_SnowSpeed);

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glVertexAttribPointer(m_PosOffsetLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(m_PosOffsetLoc);

    glBindBuffer(GL_ARRAY_BUFFER, m_PositionVBO);
    glVertexAttribPointer(m_PositionLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(m_PositionLoc);
    glVertexAttribDivisor(m_PositionLoc, 1);

    glBindBuffer(GL_ARRAY_BUFFER, m_SpeedVBO);
    glVertexAttribPointer(m_SpeedLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(m_SpeedLoc);
    glVertexAttribDivisor(m_SpeedLoc, 1);

    glBindBuffer(GL_ARRAY_BUFFER, m_LengthVBO);
    glVertexAttribPointer(m_LengthLoc, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(m_LengthLoc);
    glVertexAttribDivisor(m_LengthLoc, 1);

    glBindBuffer(GL_ARRAY_BUFFER, m_WidthVBO);
    glVertexAttribPointer(m_WidthLoc, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(m_WidthLoc);
    glVertexAttribDivisor(m_WidthLoc, 1);

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_ParticleCount);

    glVertexAttribDivisor(m_PositionLoc, 0);
    glVertexAttribDivisor(m_SpeedLoc, 0);
    glVertexAttribDivisor(m_LengthLoc, 0);
    glVertexAttribDivisor(m_WidthLoc, 0);

    glDisableVertexAttribArray(m_PosOffsetLoc);
    glDisableVertexAttribArray(m_PositionLoc);
    glDisableVertexAttribArray(m_SpeedLoc);
    glDisableVertexAttribArray(m_LengthLoc);
    glDisableVertexAttribArray(m_WidthLoc);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void SnowLayer::Destroy() {
    if (m_ProgramObj != 0) {
        glDeleteProgram(m_ProgramObj);
        m_ProgramObj = 0;
    }
    if (m_QuadVBO != 0) {
        glDeleteBuffers(1, &m_QuadVBO);
        m_QuadVBO = 0;
    }
    if (m_PositionVBO != 0) {
        glDeleteBuffers(1, &m_PositionVBO);
        m_PositionVBO = 0;
    }
    if (m_SpeedVBO != 0) {
        glDeleteBuffers(1, &m_SpeedVBO);
        m_SpeedVBO = 0;
    }
    if (m_LengthVBO != 0) {
        glDeleteBuffers(1, &m_LengthVBO);
        m_LengthVBO = 0;
    }
    if (m_WidthVBO != 0) {
        glDeleteBuffers(1, &m_WidthVBO);
        m_WidthVBO = 0;
    }
}

void SnowLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        default:
            break;
    }
}

void SnowLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_SNOW_INTENSITY:
            m_SnowIntensity = value;
            break;
        case PARAM_SNOW_SPEED:
            m_SnowSpeed = value;
            break;
        case PARAM_SNOW_SIZE:
            m_SnowSize = value;
            break;
        default:
            break;
    }
}
