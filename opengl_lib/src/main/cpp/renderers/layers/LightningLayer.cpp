#include "LightningLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
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
      m_VaoId(GL_NONE),
      m_MVPMatLoc(GL_NONE),
      m_TimeLoc(GL_NONE),
      m_ScreenSizeLoc(GL_NONE),
      m_LightningEnabledLoc(GL_NONE),
      m_LightningIntervalLoc(GL_NONE),
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
            "precision highp int;                                                     \n"
            "layout(location = 0) out vec4 outColor;                                  \n"
            "uniform float u_time;                                                    \n"
            "uniform vec2 u_screenSize;                                               \n"
            "uniform bool u_lightningEnabled;                                         \n"
            "uniform float u_lightningInterval;                                       \n"
            "uniform bool u_isNight;                                                  \n"
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
            "float lightningSegment(vec2 p, vec2 start, vec2 end, float width) {       \n"
            "    vec2 dir = end - start;                                               \n"
            "    float len = length(dir);                                              \n"
            "    if (len < 0.001) return 0.0;                                         \n"
            "    dir /= len;                                                           \n"
            "    vec2 perp = vec2(-dir.y, dir.x);                                      \n"
            "    vec2 rel = p - start;                                                 \n"
            "    float proj = dot(rel, dir);                                           \n"
            "    float dist = abs(dot(rel, perp));                                     \n"
            "    if (proj < 0.0 || proj > len) return 0.0;                             \n"
            "    float line = smoothstep(width, 0.0, dist);                            \n"
            "    float fade = smoothstep(len * 0.05, len * 0.95, proj);                \n"
            "    return line * fade;                                                   \n"
            "}                                                                        \n"
            "                                                                         \n"
            "float lightningBranch(vec2 p, vec2 start, vec2 end, float width, int depth, float randSeed) {\n"
            "    float result = 0.0;                                                   \n"
            "    const int maxDepth = 4;                                               \n"
            "    \n"
            "    vec2 starts[16];                                                      \n"
            "    vec2 ends[16];                                                        \n"
            "    float widths[16];                                                     \n"
            "    int depths[16];                                                       \n"
            "    int stackSize = 0;                                                    \n"
            "    \n"
            "    starts[0] = start;                                                    \n"
            "    ends[0] = end;                                                        \n"
            "    widths[0] = width;                                                    \n"
            "    depths[0] = depth;                                                    \n"
            "    stackSize = 1;                                                        \n"
            "    \n"
            "    while (stackSize > 0) {                                               \n"
            "        stackSize--;                                                      \n"
            "        vec2 s = starts[stackSize];                                       \n"
            "        vec2 e = ends[stackSize];                                         \n"
            "        float w = widths[stackSize];                                      \n"
            "        int d = depths[stackSize];                                        \n"
            "        \n"
            "        result += lightningSegment(p, s, e, w);                           \n"
            "        \n"
            "        if (d < maxDepth) {                                               \n"
            "            vec2 mid = (s + e) * 0.5;                                     \n"
            "            float perturb = 0.08 * (1.0 + float(d) * 0.15);               \n"
            "            mid.x += noise(vec2(s.y * 200.0 + randSeed, float(d) * 50.0)) * perturb;\n"
            "            mid.y += noise(vec2(s.x * 200.0 + randSeed, float(d) * 30.0)) * 0.02;\n"
            "            \n"
            "            float widthRand = 0.85 + noise(vec2(float(d) * 70.0 + randSeed)) * 0.3;\n"
            "            \n"
            "            starts[stackSize] = s;                                        \n"
            "            ends[stackSize] = mid;                                        \n"
            "            widths[stackSize] = w * 0.75 * widthRand;                     \n"
            "            depths[stackSize] = d + 1;                                    \n"
            "            stackSize++;                                                  \n"
            "            \n"
            "            starts[stackSize] = mid;                                      \n"
            "            ends[stackSize] = e;                                          \n"
            "            widths[stackSize] = w * 0.75 * (1.3 - widthRand);             \n"
            "            depths[stackSize] = d + 1;                                    \n"
            "            stackSize++;                                                  \n"
            "            \n"
            "            if (noise(vec2(mid.y * 100.0 + randSeed, float(d) * 20.0)) > 0.6 && d < maxDepth - 1) {\n"
            "                float branchAngle = noise(vec2(float(d) * 30.0 + randSeed)) * 0.3 - 0.15;\n"
            "                float branchLength = (e.y - mid.y) * (0.3 + noise(vec2(float(d) * 40.0 + randSeed)) * 0.4);\n"
            "                vec2 branchEnd = mid + vec2(branchAngle, -branchLength);  \n"
            "                starts[stackSize] = mid;                                  \n"
            "                ends[stackSize] = branchEnd;                              \n"
            "                widths[stackSize] = w * 0.3 * (0.7 + noise(vec2(float(d) * 50.0)) * 0.5);\n"
            "                depths[stackSize] = d + 2;                                \n"
            "                stackSize++;                                              \n"
            "            }                                                             \n"
            "        }                                                                 \n"
            "    }                                                                     \n"
            "    return result;                                                        \n"
            "}                                                                        \n"
            "                                                                         \n"
            "float lightningFlash(float time, float interval) {                         \n"
            "    float result = 0.0;                                                   \n"
            "    float cycleTime = mod(time, interval);                                \n"
            "    \n"
            "    if (cycleTime < 0.25) {                                               \n"
            "        float t = cycleTime / 0.25;                                       \n"
            "        result = smoothstep(0.0, 0.3, t) * smoothstep(1.0, 0.6, t) * 0.1;\n"
            "    }                                                                    \n"
            "    \n"
            "    if (cycleTime >= 0.25 && cycleTime < 0.32) {                          \n"
            "        float t = (cycleTime - 0.25) / 0.07;                              \n"
            "        result += smoothstep(0.0, 0.2, t) * smoothstep(1.0, 0.8, t);      \n"
            "    }                                                                    \n"
            "    \n"
            "    if (cycleTime >= 0.35 && cycleTime < 0.4) {                           \n"
            "        float t = (cycleTime - 0.35) / 0.05;                              \n"
            "        result += smoothstep(0.0, 0.3, t) * smoothstep(1.0, 0.7, t) * 0.6;\n"
            "    }                                                                    \n"
            "    \n"
            "    if (cycleTime >= 0.43 && cycleTime < 0.47) {                          \n"
            "        float t = (cycleTime - 0.43) / 0.04;                              \n"
            "        result += smoothstep(0.0, 0.4, t) * smoothstep(1.0, 0.6, t) * 0.4;\n"
            "    }                                                                    \n"
            "    \n"
            "    return result;                                                        \n"
            "}                                                                        \n"
            "                                                                         \n"
            "void main() {                                                             \n"
            "    vec2 p = gl_FragCoord.xy / u_screenSize.xy;                          \n"
            "    vec3 result = vec3(0.0);                                             \n"
            "    float alpha = 0.0;                                                   \n"
            "    \n"
            "    vec3 lightningColor = u_isNight ? vec3(0.8, 0.88, 1.0) : vec3(0.95, 0.98, 1.0);\n"
            "    vec3 edgeColor = u_isNight ? vec3(0.5, 0.65, 1.0) : vec3(0.75, 0.82, 1.0);\n"
            "    vec3 glowColor = u_isNight ? vec3(0.3, 0.5, 0.9) : vec3(0.6, 0.75, 0.95);\n"
            "    \n"
            "    if (u_lightningEnabled) {                                            \n"
            "        float flash = lightningFlash(u_time, u_lightningInterval);        \n"
            "        if (flash > 0.005) {                                              \n"
            "            float lightningPath = 0.0;                                    \n"
            "            float randSeed = u_time * 1000.0;                             \n"
            "            float mainX = 0.15 + noise(vec2(randSeed)) * 0.7;             \n"
            "            float widthRand = 0.5 + noise(vec2(randSeed + 100.0)) * 0.5;  \n"
            "            float mainWidth = 0.006 * widthRand;                          \n"
            "            float flashIntensity = 0.6 + noise(vec2(randSeed + 200.0)) * 0.4;\n"
            "            \n"
            "            vec2 start = vec2(mainX, 1.1);                                \n"
            "            vec2 end = vec2(mainX + noise(vec2(randSeed + 300.0)) * 0.2, -0.1);\n"
            "            lightningPath += lightningBranch(p, start, end, mainWidth, 0, randSeed);\n"
            "            \n"
            "            if (noise(vec2(randSeed + 400.0)) > 0.45) {                   \n"
            "                float branchX1 = mainX + 0.15 + noise(vec2(randSeed + 500.0)) * 0.12;\n"
            "                vec2 branchStart1 = vec2(branchX1, 1.05 + noise(vec2(randSeed + 600.0)) * 0.05);\n"
            "                vec2 branchEnd1 = vec2(branchX1 - noise(vec2(randSeed + 700.0)) * 0.12, 0.3);\n"
            "                lightningPath += lightningBranch(p, branchStart1, branchEnd1, mainWidth * 0.55, 1, randSeed + 1000.0);\n"
            "            }                                                             \n"
            "            \n"
            "            if (noise(vec2(randSeed + 800.0)) > 0.55) {                   \n"
            "                float branchX2 = mainX - 0.12 + noise(vec2(randSeed + 900.0)) * 0.1;\n"
            "                vec2 branchStart2 = vec2(branchX2, 0.9 + noise(vec2(randSeed + 1000.0)) * 0.08);\n"
            "                vec2 branchEnd2 = vec2(branchX2 + noise(vec2(randSeed + 1100.0)) * 0.1, 0.25);\n"
            "                lightningPath += lightningBranch(p, branchStart2, branchEnd2, mainWidth * 0.4, 2, randSeed + 2000.0);\n"
            "            }                                                             \n"
            "            \n"
            "            float coreAlpha = lightningPath * flash * flashIntensity * 12.0;\n"
            "            float edgeAlpha = lightningPath * flash * flashIntensity * 6.0;\n"
            "            float glowAlpha = lightningPath * flash * flashIntensity * 3.0;\n"
            "            \n"
            "            float glowWidth = mainWidth * 3.0;\n"
            "            float glow = smoothstep(glowWidth, 0.0, lightningPath * 50.0);\n"
            "            glow *= smoothstep(0.5, 0.0, lightningPath * 30.0);\n"
            "            \n"
            "            result += lightningColor * coreAlpha;                          \n"
            "            result += edgeColor * edgeAlpha * 0.6;                        \n"
            "            result += glowColor * glow * flash * flashIntensity * 0.5;    \n"
            "            alpha += coreAlpha + edgeAlpha * 0.4 + glow * 0.3;            \n"
            "            \n"
            "            float ambientFlash = flash * flashIntensity * 0.4;             \n"
            "            vec3 ambientColor = u_isNight ? vec3(0.55, 0.65, 0.85) : vec3(0.82, 0.86, 0.92);\n"
            "            result += ambientColor * ambientFlash;                        \n"
            "            alpha += ambientFlash;                                         \n"
            "        }                                                                \n"
            "    }                                                                    \n"
            "    \n"
            "    alpha = clamp(alpha, 0.0, 1.0);                                      \n"
            "    outColor = vec4(result, alpha);                                       \n"
            "}";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("LightningLayer::Init create program fail");
        return false;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_LightningEnabledLoc = glGetUniformLocation(m_ProgramObj, "u_lightningEnabled");
    m_LightningIntervalLoc = glGetUniformLocation(m_ProgramObj, "u_lightningInterval");
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
    glUniform1i(m_LightningEnabledLoc, m_LightningEnabled ? 1 : 0);
    glUniform1f(m_LightningIntervalLoc, m_LightningInterval);
    glUniform1i(m_IsNightLoc, m_IsNight ? 1 : 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
