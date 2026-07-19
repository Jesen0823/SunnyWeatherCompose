#include "LightningLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

LightningLayer::LightningLayer() 
    : GLLayerBase(LAYER_TYPE_LIGHTNING),
      m_LightningEnabled(false),
      m_LightningInterval(5.0f),
      m_IsNight(false),
      m_ScreenWidth(0),
      m_ScreenHeight(0),
      m_Time(0.0f),
      m_FrameIndex(0),
      m_CurrentFlashIntensity(0.0f),
      m_VaoId(GL_NONE),
      m_MVPMatLoc(GL_NONE),
      m_TimeLoc(GL_NONE),
      m_ScreenSizeLoc(GL_NONE),
      m_FlashIntensityLoc(GL_NONE),
      m_IsNightLoc(GL_NONE) {
    m_VboIds[0] = m_VboIds[1] = m_VboIds[2] = GL_NONE;
}

LightningLayer::~LightningLayer() {}

bool LightningLayer::Init() {
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
            "uniform float u_flashIntensity;                                          \n"
            "uniform bool u_isNight;                                                  \n"
            "                                                                         \n"
            "float rand(vec2 p) {                                                     \n"
            "    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);      \n"
            "}                                                                        \n"
            "                                                                         \n"
            "float sdSegment(vec2 p, vec2 a, vec2 b) {                                \n"
            "    vec2 ab = b - a;                                                     \n"
            "    float t = clamp(dot(p - a, ab) / dot(ab, ab), 0.0, 1.0);            \n"
            "    return length(p - a - ab * t);                                       \n"
            "}                                                                        \n"
            "                                                                         \n"
            "float getLightningValue(vec2 uv, vec2 start, vec2 end, float width) {    \n"
            "    float dist = sdSegment(uv, start, end);                              \n"
            "    float core = smoothstep(width * 0.2, 0.0, dist) * 1.5;               \n"
            "    float inner = smoothstep(width * 0.5, 0.0, dist);                    \n"
            "    float outer = smoothstep(width * 1.2, 0.0, dist);                   \n"
            "    float glow = smoothstep(width * 4.0, 0.0, dist);                    \n"
            "    inner *= (1.0 - core);                                              \n"
            "    outer *= (1.0 - inner);                                             \n"
            "    glow *= (1.0 - outer);                                              \n"
            "    return core + inner * 0.8 + outer * 0.4 + glow * 0.2;               \n"
            "}                                                                        \n"
            "                                                                         \n"
            "void main() {                                                             \n"
            "    vec2 uv = gl_FragCoord.xy / u_screenSize.xy;                          \n"
            "    vec3 col = vec3(0.0);                                                 \n"
            "    float alpha = 0.0;                                                    \n"
            "    \n"
            "    float flash = u_flashIntensity;                                       \n"
            "    \n"
            "    if (flash < 0.001) {                                                  \n"
            "        outColor = vec4(col, alpha);                                      \n"
            "        return;                                                           \n"
            "    }                                                                    \n"
            "    \n"
            "    float seed = floor(u_time * 500.0);                                   \n"
            "    float mainX = 0.1 + rand(vec2(seed)) * 0.8;                          \n"
            "    float widthVar = 0.5 + rand(vec2(seed + 100.0)) * 0.5;               \n"
            "    float brightnessVar = 0.5 + rand(vec2(seed + 200.0)) * 0.5;          \n"
            "    float heightVar = 0.4 + rand(vec2(seed + 300.0)) * 0.6;              \n"
            "    \n"
            "    float totalFlash = flash * brightnessVar;                            \n"
            "    float baseWidth = 0.015 * widthVar;                                  \n"
            "    \n"
            "    vec3 coreCol = vec3(1.0, 1.0, 0.95);                                 \n"
            "    vec3 innerCol = u_isNight ? vec3(0.85, 0.92, 1.0) : vec3(0.96, 0.98, 1.0);\n"
            "    vec3 outerCol = u_isNight ? vec3(0.6, 0.75, 1.0) : vec3(0.85, 0.9, 0.98);\n"
            "    vec3 glowCol = u_isNight ? vec3(0.4, 0.55, 0.9) : vec3(0.7, 0.8, 0.95);\n"
            "    \n"
            "    vec2 p0 = vec2(mainX, 1.1);                                          \n"
            "    vec2 p1 = vec2(mainX + (rand(vec2(seed + 400.0)) - 0.5) * 0.3, 0.85);\n"
            "    vec2 p2 = vec2(p1.x + (rand(vec2(seed + 500.0)) - 0.5) * 0.25, 0.6);\n"
            "    vec2 p3 = vec2(p2.x + (rand(vec2(seed + 600.0)) - 0.5) * 0.2, 0.4);  \n"
            "    vec2 p4 = vec2(p3.x + (rand(vec2(seed + 700.0)) - 0.5) * 0.15, 1.1 - heightVar);\n"
            "    \n"
            "    float s0 = getLightningValue(uv, p0, p1, baseWidth);                 \n"
            "    float s1 = getLightningValue(uv, p1, p2, baseWidth * 0.9);           \n"
            "    float s2 = getLightningValue(uv, p2, p3, baseWidth * 0.8);           \n"
            "    float s3 = getLightningValue(uv, p3, p4, baseWidth * 0.7);           \n"
            "    \n"
            "    float mainLightning = max(max(s0, s1), max(s2, s3));                 \n"
            "    col += coreCol * mainLightning * totalFlash * 2.0;                    \n"
            "    \n"
            "    if (rand(vec2(seed + 800.0)) > 0.3) {                                \n"
            "        float bx1 = p1.x + (rand(vec2(seed + 900.0)) - 0.5) * 0.35;      \n"
            "        vec2 bp0 = vec2(bx1, p1.y);                                      \n"
            "        vec2 bp1 = vec2(bx1 + (rand(vec2(seed + 1000.0)) - 0.5) * 0.3, bp0.y - rand(vec2(seed + 1100.0)) * 0.45);\n"
            "        vec2 bp2 = vec2(bp1.x + (rand(vec2(seed + 1200.0)) - 0.5) * 0.2, bp1.y - rand(vec2(seed + 1300.0)) * 0.35);\n"
            "        float bs0 = getLightningValue(uv, bp0, bp1, baseWidth * 0.5);    \n"
            "        float bs1 = getLightningValue(uv, bp1, bp2, baseWidth * 0.35);   \n"
            "        col += coreCol * max(bs0, bs1) * totalFlash * 0.8;               \n"
            "        \n"
            "        if (rand(vec2(seed + 1400.0)) > 0.5) {                           \n"
            "            float bx1a = bp1.x + (rand(vec2(seed + 1500.0)) - 0.5) * 0.2;\n"
            "            vec2 bp0a = vec2(bx1a, bp1.y);                               \n"
            "            vec2 bp1a = vec2(bx1a + (rand(vec2(seed + 1600.0)) - 0.5) * 0.15, bp0a.y - rand(vec2(seed + 1700.0)) * 0.3);\n"
            "            float bsa = getLightningValue(uv, bp0a, bp1a, baseWidth * 0.25);\n"
            "            col += coreCol * bsa * totalFlash * 0.5;                     \n"
            "        }                                                                \n"
            "    }                                                                    \n"
            "    \n"
            "    if (rand(vec2(seed + 1800.0)) > 0.4) {                              \n"
            "        float bx2 = p2.x + (rand(vec2(seed + 1900.0)) - 0.5) * 0.25;    \n"
            "        vec2 bp0 = vec2(bx2, p2.y);                                      \n"
            "        vec2 bp1 = vec2(bx2 + (rand(vec2(seed + 2000.0)) - 0.5) * 0.25, bp0.y - rand(vec2(seed + 2100.0)) * 0.4);\n"
            "        float bs = getLightningValue(uv, bp0, bp1, baseWidth * 0.45);    \n"
            "        col += coreCol * bs * totalFlash * 0.7;                          \n"
            "    }                                                                    \n"
            "    \n"
            "    if (rand(vec2(seed + 2200.0)) > 0.45) {                             \n"
            "        float bx3 = p3.x + (rand(vec2(seed + 2300.0)) - 0.5) * 0.2;     \n"
            "        vec2 bp0 = vec2(bx3, p3.y);                                      \n"
            "        vec2 bp1 = vec2(bx3 + (rand(vec2(seed + 2400.0)) - 0.5) * 0.15, bp0.y - rand(vec2(seed + 2500.0)) * 0.3);\n"
            "        float bs = getLightningValue(uv, bp0, bp1, baseWidth * 0.4);     \n"
            "        col += coreCol * bs * totalFlash * 0.6;                          \n"
            "    }                                                                    \n"
            "    \n"
            "    if (rand(vec2(seed + 2600.0)) > 0.5) {                              \n"
            "        float bx4 = p0.x + (rand(vec2(seed + 2700.0)) - 0.5) * 0.2;     \n"
            "        vec2 bp0 = vec2(bx4, p0.y - 0.05);                               \n"
            "        vec2 bp1 = vec2(bx4 + (rand(vec2(seed + 2800.0)) - 0.5) * 0.25, bp0.y - rand(vec2(seed + 2900.0)) * 0.35);\n"
            "        float bs = getLightningValue(uv, bp0, bp1, baseWidth * 0.5);     \n"
            "        col += coreCol * bs * totalFlash * 0.75;                         \n"
            "    }                                                                    \n"
            "    \n"
            "    float distToLightning = min(min(sdSegment(uv, p0, p1), sdSegment(uv, p1, p2)),\n"
            "                                min(sdSegment(uv, p2, p3), sdSegment(uv, p3, p4)));\n"
            "    float ambientGlow = smoothstep(baseWidth * 15.0, 0.0, distToLightning);\n"
            "    col += glowCol * ambientGlow * totalFlash * 0.4;                     \n"
            "    \n"
            "    float flicker = 0.8 + rand(vec2(u_time * 10000.0, uv.x)) * 0.2;     \n"
            "    col *= flicker;                                                       \n"
            "    \n"
            "    col = clamp(col, 0.0, 1.0);                                          \n"
            "    alpha = max(max(col.r, col.g), col.b);                               \n"
            "    alpha = clamp(alpha, 0.0, 1.0);                                      \n"
            "    \n"
            "    outColor = vec4(col, alpha);                                          \n"
            "}";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("LightningLayer::Init create program fail");
        return false;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_FlashIntensityLoc = glGetUniformLocation(m_ProgramObj, "u_flashIntensity");
    m_IsNightLoc = glGetUniformLocation(m_ProgramObj, "u_isNight");

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

void LightningLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj || !m_Enabled || !m_LightningEnabled) {
        m_CurrentFlashIntensity = 0.0f;
        return;
    }

    m_ScreenWidth = screenW;
    m_ScreenHeight = screenH;

    UpdateMVPMatrix();

    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    m_FrameIndex++;
    m_Time = m_FrameIndex * 0.04f;

    float cycleTime = fmod(m_Time, m_LightningInterval);
    float flashDuration = 1.2f;
    float flashPhase = cycleTime / flashDuration;
    float flash = 0.0f;
    
    if (flashPhase < 1.0f) {
        float preFlash = glm::smoothstep(0.0f, 0.08f, flashPhase) * glm::smoothstep(0.25f, 0.15f, flashPhase) * 0.1f;
        
        float mainFlash = 0.0f;
        if (flashPhase >= 0.25f && flashPhase < 0.35f) {
            float t = (flashPhase - 0.25f) / 0.1f;
            mainFlash = glm::smoothstep(0.0f, 0.1f, t) * glm::smoothstep(1.0f, 0.7f, t);
        }
        
        float bounce1 = 0.0f;
        if (flashPhase >= 0.38f && flashPhase < 0.48f) {
            float t = (flashPhase - 0.38f) / 0.1f;
            bounce1 = glm::smoothstep(0.0f, 0.15f, t) * glm::smoothstep(1.0f, 0.55f, t) * 0.7f;
        }
        
        float bounce2 = 0.0f;
        if (flashPhase >= 0.52f && flashPhase < 0.62f) {
            float t = (flashPhase - 0.52f) / 0.1f;
            bounce2 = glm::smoothstep(0.0f, 0.2f, t) * glm::smoothstep(1.0f, 0.5f, t) * 0.5f;
        }
        
        float bounce3 = 0.0f;
        if (flashPhase >= 0.66f && flashPhase < 0.76f) {
            float t = (flashPhase - 0.66f) / 0.1f;
            bounce3 = glm::smoothstep(0.0f, 0.25f, t) * glm::smoothstep(1.0f, 0.45f, t) * 0.35f;
        }
        
        float decay = glm::smoothstep(0.8f, 1.0f, flashPhase) * 0.2f;
        
        flash = preFlash + mainFlash + bounce1 + bounce2 + bounce3 + decay;
    }
    
    m_CurrentFlashIntensity = flash;

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniform1f(m_TimeLoc, m_Time);
    glUniform2f(m_ScreenSizeLoc, screenW, screenH);
    glUniform1f(m_FlashIntensityLoc, flash);
    glUniform1i(m_IsNightLoc, m_IsNight ? 1 : 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);

    glDisable(GL_BLEND);

    glBindVertexArray(GL_NONE);
}

void LightningLayer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        m_ProgramObj = GL_NONE;
    }
}

void LightningLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        case PARAM_LIGHTNING_ENABLED:
            m_LightningEnabled = (value != 0);
            break;
        case PARAM_LIGHTNING_IS_NIGHT:
            m_IsNight = (value != 0);
            break;
        default:
            break;
    }
}

void LightningLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_LIGHTNING_INTERVAL:
            m_LightningInterval = value;
            break;
        default:
            break;
    }
}

void LightningLayer::UpdateMVPMatrix() {
    float ratio = (float) m_ScreenWidth / m_ScreenHeight;
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 Model = glm::mat4(1.0f);
    m_MVPMatrix = Projection * View * Model;
}
