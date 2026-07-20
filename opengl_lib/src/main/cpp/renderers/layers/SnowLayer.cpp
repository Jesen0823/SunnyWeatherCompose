#include "SnowLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include <stdlib.h>
#include <time.h>

static float randomRange(float min, float max) {
    return min + (float)rand() / (float)RAND_MAX * (max - min);
}

SnowLayer::SnowLayer() : GLLayerBase(LAYER_TYPE_SNOW) {
    srand((unsigned int)time(nullptr));
}

SnowLayer::~SnowLayer() {
    Destroy();
}

void SnowLayer::GenerateSnowflakes() {
    m_ParticlePositions.resize(m_ParticleCount);
    m_ParticleSpeeds.resize(m_ParticleCount);
    m_ParticleSizes.resize(m_ParticleCount);
    m_SwingOffsets.resize(m_ParticleCount);
    m_SwingSpeeds.resize(m_ParticleCount);
    m_WindOffsets.resize(m_ParticleCount);
    
    for (int i = 0; i < m_ParticleCount; ++i) {
        m_ParticlePositions[i].x = randomRange(0.0f, 1.0f);
        m_ParticlePositions[i].y = randomRange(0.0f, 1.5f);
        
        float baseSpeed = randomRange(0.1f, 0.3f);
        m_ParticleSpeeds[i].x = randomRange(-0.02f, 0.02f);
        m_ParticleSpeeds[i].y = -baseSpeed;
        
        float depthFactor = 0.7f + m_ParticlePositions[i].y * 0.4f;
        float baseSize = randomRange(0.4f * depthFactor, 1.2f * depthFactor);
        float closeUpFactor = randomRange(0.0f, 1.0f);
        if (closeUpFactor < 0.15f) {
            baseSize *= randomRange(1.5f, 2.2f);
        }
        m_ParticleSizes[i] = baseSize;
        
        m_SwingOffsets[i] = randomRange(0.0f, 6.2831853f);
        m_SwingSpeeds[i] = randomRange(0.5f, 2.0f);
        
        m_WindOffsets[i] = randomRange(-0.3f, 1.0f);
    }
}

bool SnowLayer::Init() {
    const char *vShaderStr =
        "#version 300 es                          \n"
        "precision highp float;                   \n"
        "layout(location = 0) in vec2 a_position; \n"
        "layout(location = 1) in vec2 a_speed;    \n"
        "layout(location = 2) in float a_size;    \n"
        "layout(location = 3) in float a_swingOffset;\n"
        "layout(location = 4) in float a_swingSpeed;\n"
        "layout(location = 5) in float a_windOffset;\n"
        "uniform float u_time;                    \n"
        "uniform vec2 u_screenSize;               \n"
        "uniform float u_snowSpeed;               \n"
        "uniform float u_windForce;               \n"
        "uniform float u_gustWindX;               \n"
        "uniform float u_gustWindY;               \n"
        "uniform float u_gustStrength;            \n"
        "uniform float u_gustGroupSeed;           \n"
        "uniform float u_gustGroupRatio;          \n"
        "float randomHash(float x, float seed) {  \n"
        "    return fract(sin(x * 12.9898 + seed * 78.233) * 43758.5453);\n"
        "}\n"
        "void main() {                            \n"
        "    vec2 pos = a_position;               \n"
        "    float speedFactor = 0.5 + u_snowSpeed * 2.5; \n"
        "    pos.y += u_time * a_speed.y * speedFactor * 0.25; \n"
        "    float windBase = u_windForce * u_time * 0.18;\n"
        "    float windSwing = u_windForce * sin(u_time * 0.5 + a_swingOffset) * 0.08;\n"
        "    float randomPerturbX = sin(u_time * a_swingSpeed + a_swingOffset) * 0.02;\n"
        "    float randomPerturbY = cos(u_time * a_swingSpeed * 1.2 + a_swingOffset) * 0.01;\n"
        "    float particleHash = randomHash(a_swingOffset * 100.0, u_gustGroupSeed);\n"
        "    float gustParticipate = step(0.0, particleHash - (1.0 - u_gustGroupRatio));\n"
        "    float gustEffectX = u_gustWindX * u_gustStrength * u_time * 0.08 * gustParticipate;\n"
        "    float gustEffectY = u_gustWindY * u_gustStrength * u_time * 0.05 * gustParticipate;\n"
        "    pos.x += windBase + windSwing + randomPerturbX + gustEffectX;\n"
        "    pos.y += randomPerturbY + gustEffectY;\n"
        "    pos.x += a_windOffset * u_windForce * 0.10;\n"
        "    pos.y = fract(pos.y + 1.25);         \n"
        "    pos.x = fract(pos.x + 0.5);          \n"
        "    vec2 clipPos = pos * 2.0 - 1.0;      \n"
        "    clipPos.x *= u_screenSize.x / u_screenSize.y;\n"
        "    gl_Position = vec4(clipPos, 0.0, 1.0);\n"
        "    gl_PointSize = a_size * u_snowSpeed * 32.0;\n"
        "}";

    const char *fShaderStr =
        "#version 300 es                          \n"
        "precision highp float;                   \n"
        "out vec4 outColor;                       \n"
        "uniform float u_snowIntensity;           \n"
        "uniform float u_snowShape;               \n"
        "void main() {                            \n"
        "    vec2 coord = gl_PointCoord - vec2(0.5);\n"
        "    float dist = length(coord);          \n"
        "    if (dist > 0.45) discard;            \n"
        "    float alpha;\n"
        "    if (u_snowShape < 0.5) {\n"
        "        alpha = smoothstep(0.45, 0.0, dist);\n"
        "    } else {\n"
        "        float angle = atan(coord.y, coord.x);\n"
        "        float r = 0.38 + 0.08 * cos(6.0 * angle);\n"
        "        alpha = smoothstep(r + 0.05, r - 0.05, dist);\n"
        "    }\n"
        "    alpha *= u_snowIntensity;            \n"
        "    outColor = vec4(0.85, 0.88, 0.92, alpha);\n"
        "}";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("SnowLayer::Init: CreateProgram failed");
        return false;
    }

    m_PositionLoc = glGetAttribLocation(m_ProgramObj, "a_position");
    m_SpeedLoc = glGetAttribLocation(m_ProgramObj, "a_speed");
    m_SizeLoc = glGetAttribLocation(m_ProgramObj, "a_size");
    m_SwingOffsetLoc = glGetAttribLocation(m_ProgramObj, "a_swingOffset");
    m_SwingSpeedLoc = glGetAttribLocation(m_ProgramObj, "a_swingSpeed");
    m_WindOffsetLoc = glGetAttribLocation(m_ProgramObj, "a_windOffset");

    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_SnowIntensityLoc = glGetUniformLocation(m_ProgramObj, "u_snowIntensity");
    m_SnowSpeedLoc = glGetUniformLocation(m_ProgramObj, "u_snowSpeed");
    m_WindForceLoc = glGetUniformLocation(m_ProgramObj, "u_windForce");
    m_SnowShapeLoc = glGetUniformLocation(m_ProgramObj, "u_snowShape");
    m_GustWindXLoc = glGetUniformLocation(m_ProgramObj, "u_gustWindX");
    m_GustWindYLoc = glGetUniformLocation(m_ProgramObj, "u_gustWindY");
    m_GustStrengthLoc = glGetUniformLocation(m_ProgramObj, "u_gustStrength");
    m_GustGroupSeedLoc = glGetUniformLocation(m_ProgramObj, "u_gustGroupSeed");
    m_GustGroupRatioLoc = glGetUniformLocation(m_ProgramObj, "u_gustGroupRatio");

    GenerateSnowflakes();

    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glGenBuffers(1, &m_PositionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_PositionVBO);
    glBufferData(GL_ARRAY_BUFFER, m_ParticlePositions.size() * sizeof(glm::vec2), m_ParticlePositions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_PositionLoc, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    glEnableVertexAttribArray(m_PositionLoc);
    glVertexAttribDivisor(m_PositionLoc, 1);

    glGenBuffers(1, &m_SpeedVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SpeedVBO);
    glBufferData(GL_ARRAY_BUFFER, m_ParticleSpeeds.size() * sizeof(glm::vec2), m_ParticleSpeeds.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_SpeedLoc, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    glEnableVertexAttribArray(m_SpeedLoc);
    glVertexAttribDivisor(m_SpeedLoc, 1);

    glGenBuffers(1, &m_SizeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SizeVBO);
    glBufferData(GL_ARRAY_BUFFER, m_ParticleSizes.size() * sizeof(float), m_ParticleSizes.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_SizeLoc, 1, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    glEnableVertexAttribArray(m_SizeLoc);
    glVertexAttribDivisor(m_SizeLoc, 1);

    glGenBuffers(1, &m_SwingOffsetVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SwingOffsetVBO);
    glBufferData(GL_ARRAY_BUFFER, m_SwingOffsets.size() * sizeof(float), m_SwingOffsets.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_SwingOffsetLoc, 1, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    glEnableVertexAttribArray(m_SwingOffsetLoc);
    glVertexAttribDivisor(m_SwingOffsetLoc, 1);

    glGenBuffers(1, &m_SwingSpeedVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SwingSpeedVBO);
    glBufferData(GL_ARRAY_BUFFER, m_SwingSpeeds.size() * sizeof(float), m_SwingSpeeds.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_SwingSpeedLoc, 1, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    glEnableVertexAttribArray(m_SwingSpeedLoc);
    glVertexAttribDivisor(m_SwingSpeedLoc, 1);

    glGenBuffers(1, &m_WindOffsetVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_WindOffsetVBO);
    glBufferData(GL_ARRAY_BUFFER, m_WindOffsets.size() * sizeof(float), m_WindOffsets.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(m_WindOffsetLoc, 1, GL_FLOAT, GL_FALSE, 0, (const void *)0);
    glEnableVertexAttribArray(m_WindOffsetLoc);
    glVertexAttribDivisor(m_WindOffsetLoc, 1);

    glBindVertexArray(GL_NONE);

    m_IsInitialized = true;
    LOGCATI("SnowLayer::Init: initialized with %d particles", m_ParticleCount);
    return true;
}

void SnowLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj || !m_Enabled || !m_IsInitialized) {
        return;
    }

    m_ScreenWidth = screenW;
    m_ScreenHeight = screenH;

    glDisable(GL_DEPTH_TEST);
    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    m_Time += 0.04f;
    
    m_GustTimer += 0.04f;
    if (m_GustStrength <= 0.01f && m_GustTimer > m_GustDuration) {
        float angle = randomRange(0.0f, 6.2831853f);
        float speed = randomRange(0.15f, 0.4f);
        m_GustWindX = cos(angle) * speed;
        m_GustWindY = sin(angle) * speed * 0.5f;
        m_GustDuration = randomRange(2.0f, 3.5f);
        m_GustTimer = 0.0f;
        m_GustGroupSeed = randomRange(0.0f, 1000.0f);
        m_GustGroupRatio = randomRange(0.3f, 0.6f);
    }
    
    if (m_GustTimer < 0.3f) {
        m_GustStrength = m_GustTimer / 0.3f;
    } else if (m_GustTimer > m_GustDuration - 0.3f) {
        m_GustStrength = 1.0f - (m_GustTimer - (m_GustDuration - 0.3f)) / 0.3f;
    } else if (m_GustTimer > 0.3f) {
        m_GustStrength = 1.0f;
    }
    
    m_GustStrength = std::max(0.0f, std::min(1.0f, m_GustStrength));

    glUniform1f(m_TimeLoc, m_Time);
    glUniform2f(m_ScreenSizeLoc, (float)screenW, (float)screenH);
    glUniform1f(m_SnowIntensityLoc, m_SnowIntensity);
    glUniform1f(m_SnowSpeedLoc, m_SnowSpeed);
    glUniform1f(m_WindForceLoc, m_WindForce);
    glUniform1f(m_SnowShapeLoc, m_SnowShape);
    glUniform1f(m_GustWindXLoc, m_GustWindX);
    glUniform1f(m_GustWindYLoc, m_GustWindY);
    glUniform1f(m_GustStrengthLoc, m_GustStrength);
    glUniform1f(m_GustGroupSeedLoc, m_GustGroupSeed);
    glUniform1f(m_GustGroupRatioLoc, m_GustGroupRatio);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawArraysInstanced(GL_POINTS, 0, 1, m_ParticleCount);

    glDisable(GL_BLEND);

    glBindVertexArray(GL_NONE);
}

void SnowLayer::Destroy() {
    if (m_VaoId != 0) {
        glDeleteVertexArrays(1, &m_VaoId);
        m_VaoId = 0;
    }

    if (m_PositionVBO != 0) {
        glDeleteBuffers(1, &m_PositionVBO);
        m_PositionVBO = 0;
    }

    if (m_SpeedVBO != 0) {
        glDeleteBuffers(1, &m_SpeedVBO);
        m_SpeedVBO = 0;
    }

    if (m_SizeVBO != 0) {
        glDeleteBuffers(1, &m_SizeVBO);
        m_SizeVBO = 0;
    }

    if (m_SwingOffsetVBO != 0) {
        glDeleteBuffers(1, &m_SwingOffsetVBO);
        m_SwingOffsetVBO = 0;
    }

    if (m_SwingSpeedVBO != 0) {
        glDeleteBuffers(1, &m_SwingSpeedVBO);
        m_SwingSpeedVBO = 0;
    }

    if (m_WindOffsetVBO != 0) {
        glDeleteBuffers(1, &m_WindOffsetVBO);
        m_WindOffsetVBO = 0;
    }

    if (m_ProgramObj != 0) {
        glDeleteProgram(m_ProgramObj);
        m_ProgramObj = 0;
    }

    if (m_VertexShader != 0) {
        glDeleteShader(m_VertexShader);
        m_VertexShader = 0;
    }

    if (m_FragmentShader != 0) {
        glDeleteShader(m_FragmentShader);
        m_FragmentShader = 0;
    }

    m_IsInitialized = false;
}

void SnowLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        case PARAM_SNOW_PARTICLE_COUNT:
            m_ParticleCount = value;
            if (m_IsInitialized) {
                GenerateSnowflakes();
                glBindBuffer(GL_ARRAY_BUFFER, m_PositionVBO);
                glBufferData(GL_ARRAY_BUFFER, m_ParticlePositions.size() * sizeof(glm::vec2), m_ParticlePositions.data(), GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, m_SpeedVBO);
                glBufferData(GL_ARRAY_BUFFER, m_ParticleSpeeds.size() * sizeof(glm::vec2), m_ParticleSpeeds.data(), GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, m_SizeVBO);
                glBufferData(GL_ARRAY_BUFFER, m_ParticleSizes.size() * sizeof(float), m_ParticleSizes.data(), GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, m_SwingOffsetVBO);
                glBufferData(GL_ARRAY_BUFFER, m_SwingOffsets.size() * sizeof(float), m_SwingOffsets.data(), GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, m_SwingSpeedVBO);
                glBufferData(GL_ARRAY_BUFFER, m_SwingSpeeds.size() * sizeof(float), m_SwingSpeeds.data(), GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, m_WindOffsetVBO);
                glBufferData(GL_ARRAY_BUFFER, m_WindOffsets.size() * sizeof(float), m_WindOffsets.data(), GL_STATIC_DRAW);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            break;
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
        case PARAM_SNOW_WIND:
            m_WindForce = value;
            break;
        case PARAM_SNOW_SHAPE:
            m_SnowShape = value;
            break;
        default:
            break;
    }
}

void SnowLayer::SetWindForce(float force) {
    m_WindForce = force;
}

void SnowLayer::SetParticleCount(int count) {
    m_ParticleCount = count;
    if (m_IsInitialized) {
        GenerateSnowflakes();
        glBindBuffer(GL_ARRAY_BUFFER, m_PositionVBO);
        glBufferData(GL_ARRAY_BUFFER, m_ParticlePositions.size() * sizeof(glm::vec2), m_ParticlePositions.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, m_SpeedVBO);
        glBufferData(GL_ARRAY_BUFFER, m_ParticleSpeeds.size() * sizeof(glm::vec2), m_ParticleSpeeds.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, m_SizeVBO);
        glBufferData(GL_ARRAY_BUFFER, m_ParticleSizes.size() * sizeof(float), m_ParticleSizes.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, m_SwingOffsetVBO);
        glBufferData(GL_ARRAY_BUFFER, m_SwingOffsets.size() * sizeof(float), m_SwingOffsets.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, m_SwingSpeedVBO);
        glBufferData(GL_ARRAY_BUFFER, m_SwingSpeeds.size() * sizeof(float), m_SwingSpeeds.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
