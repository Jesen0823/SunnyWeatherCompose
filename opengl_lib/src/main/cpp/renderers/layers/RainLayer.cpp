#include "RainLayer.h"
#include "../../util/LogUtil.h"
#include "../../util/ShaderLoader.h"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <gtc/matrix_transform.hpp>

#define GRAVITY 3500.0f
#define MAX_MASS 300.0f
#define EVAPORATION_RATE 10.0f
#define MAX_RAINDROPS 800
#define MERGE_DISTANCE 15.0f
#define MIN_SIZE 1.0f
#define MAX_SIZE 25.0f
#define GRID_CELL_SIZE 60.0f

static float randomRange(float min, float max) {
    return min + (float)rand() / RAND_MAX * (max - min);
}

static GLuint compileShader(GLenum shaderType, const char *source) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    GLint compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char *infoLog = new char[infoLen];
            glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
            LOGCATD("RainLayer::compileShader: shader compile error: %s", infoLog);
            delete[] infoLog;
        }
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint createProgram(const char *vShaderSrc, const char *fShaderSrc) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vShaderSrc);
    if (!vertexShader) {
        LOGCATD("RainLayer::createProgram: vertex shader failed");
        return 0;
    }
    
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fShaderSrc);
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        LOGCATD("RainLayer::createProgram: fragment shader failed");
        return 0;
    }
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint infoLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            char *infoLog = new char[infoLen];
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
            glGetProgramInfoLog(program, infoLen, nullptr, infoLog);
            LOGCATD("RainLayer::createProgram: program link error: %s", infoLog);
            delete[] infoLog;
        }
        glDeleteProgram(program);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return program;
}

struct GridKey {
    int x;
    int y;
    
    bool operator==(const GridKey &other) const {
        return x == other.x && y == other.y;
    }
};

namespace std {
    template<> struct hash<GridKey> {
        size_t operator()(const GridKey &k) const {
            return hash<int>()(k.x) ^ (hash<int>()(k.y) << 1);
        }
    };
}

RainLayer::RainLayer() : GLLayerBase(LAYER_TYPE_RAIN) {}

RainLayer::~RainLayer() {}

bool RainLayer::Init() {
    LOGCATI("RainLayer::Init: called");
    
    std::string distortVShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/rain_layer_distort_v.glsl");
    std::string distortFShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/rain_layer_distort_f.glsl");
    std::string refractVShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/rain_layer_refract_v.glsl");
    std::string refractFShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/rain_layer_refract_f.glsl");

    if (distortVShaderStr.empty()) {
        LOGCATD("RainLayer::Init: failed to load distort vertex shader");
        return false;
    }
    if (distortFShaderStr.empty()) {
        LOGCATD("RainLayer::Init: failed to load distort fragment shader");
        return false;
    }
    if (refractVShaderStr.empty()) {
        LOGCATD("RainLayer::Init: failed to load refract vertex shader");
        return false;
    }
    if (refractFShaderStr.empty()) {
        LOGCATD("RainLayer::Init: failed to load refract fragment shader");
        return false;
    }
    
    m_DistortProgram = createProgram(distortVShaderStr.c_str(), distortFShaderStr.c_str());
    if (!m_DistortProgram) {
        LOGCATD("RainLayer::Init: distort program creation failed");
        return false;
    }
    
    m_ProgramObj = createProgram(refractVShaderStr.c_str(), refractFShaderStr.c_str());
    if (!m_ProgramObj) {
        LOGCATD("RainLayer::Init: refract program creation failed");
        glDeleteProgram(m_DistortProgram);
        return false;
    }
    
    // 缓存refract program的location
    m_PositionLoc = glGetAttribLocation(m_ProgramObj, "aPosition");
    m_TexCoordLoc = glGetAttribLocation(m_ProgramObj, "aTexCoord");
    m_RefractionLoc = glGetUniformLocation(m_ProgramObj, "uRefraction");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "uTime");
    m_BackgroundTexLoc = glGetUniformLocation(m_ProgramObj, "uBackground");
    m_DistortTexLoc = glGetUniformLocation(m_ProgramObj, "uDistort");
    
    // 缓存distort program的location
    m_DistortModelLoc = glGetUniformLocation(m_DistortProgram, "uModel");
    m_DistortProjLoc = glGetUniformLocation(m_DistortProgram, "uProjection");
    m_DistortPosLoc = glGetAttribLocation(m_DistortProgram, "aPosition");
    m_DistortTexAttrLoc = glGetAttribLocation(m_DistortProgram, "aTexCoord");
    
    LOGCATI("RainLayer::Init: locations cached - posLoc=%d, texLoc=%d, refractLoc=%d, timeLoc=%d, bgLoc=%d, distLoc=%d",
            m_PositionLoc, m_TexCoordLoc, m_RefractionLoc, m_TimeLoc, m_BackgroundTexLoc, m_DistortTexLoc);
    LOGCATI("RainLayer::Init: distort locations - modelLoc=%d, projLoc=%d, posLoc=%d, texLoc=%d",
            m_DistortModelLoc, m_DistortProjLoc, m_DistortPosLoc, m_DistortTexAttrLoc);
    
    float quadVertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 0.0f
    };
    
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    float quadVertices2D[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f
    };
    
    glGenBuffers(1, &m_QuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices2D), quadVertices2D, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    LOGCATI("RainLayer::Init: completed, program=%d, distortProgram=%d", m_ProgramObj, m_DistortProgram);
    
    return true;
}

void RainLayer::SpawnRaindrop() {
    if (m_Raindrops.size() >= MAX_RAINDROPS) return;
    
    Raindrop drop;
    drop.pos.x = randomRange(0.0f, (float)m_ScreenWidth);
    drop.pos.y = randomRange((float)m_ScreenHeight + 20.0f, (float)m_ScreenHeight + 100.0f);
    
    float sizeFactor;
    float densityFactor;
    float dragFactor;
    
    if (m_RainIntensity > 1.5f) {
        sizeFactor = randomRange(0.5f, 1.0f);
        densityFactor = randomRange(0.35f, 0.75f);
        dragFactor = randomRange(0.005f, 0.03f);
    } else if (m_RainIntensity > 0.8f) {
        sizeFactor = randomRange(0.35f, 0.75f);
        densityFactor = randomRange(0.45f, 0.95f);
        dragFactor = randomRange(0.025f, 0.08f);
    } else {
        sizeFactor = randomRange(0.15f, 0.5f);
        densityFactor = randomRange(0.6f, 1.0f);
        dragFactor = randomRange(0.08f, 0.2f);
    }
    
    drop.size = MIN_SIZE + (MAX_SIZE - MIN_SIZE) * sizeFactor;
    drop.density = densityFactor;
    drop.mass = drop.size * drop.size * drop.density;
    drop.velocity = glm::vec2(randomRange(-10.0f, 10.0f), -randomRange(500.0f, 1500.0f));
    drop.drag = GRAVITY * MAX_MASS * dragFactor;
    
    drop.offset = randomRange(-0.2f, 0.2f) * m_RainIntensity;
    drop.spread = glm::vec2(0.02f);
    drop.parent = -1;
    drop.lastTrailPos = drop.pos;
    
    m_Raindrops.push_back(drop);
}

void RainLayer::UpdateRaindrops(float dt) {
    float baseSpawnRate = m_RainIntensity > 1.5f ? 120.0f : m_RainIntensity > 0.8f ? 60.0f : 25.0f;
    float spawnRate = baseSpawnRate + m_RainIntensity * 50.0f;
    
    m_SpawnTimer += dt;
    while (m_SpawnTimer > 1.0f / spawnRate) {
        SpawnRaindrop();
        m_SpawnTimer -= 1.0f / spawnRate;
    }
    
    std::vector<Raindrop> newTrails;
    newTrails.reserve(128);
    
    size_t initialSize = m_Raindrops.size();
    for (size_t i = 0; i < initialSize; ++i) {
        Raindrop &drop = m_Raindrops[i];
        
        float acceleration = drop.mass * GRAVITY - drop.drag;
        drop.velocity.y -= acceleration * dt;
        drop.velocity.x = drop.offset * 100.0f + m_WindSpeed * 80.0f;
        
        if (drop.velocity.y < -8000.0f) drop.velocity.y = -8000.0f;
        if (drop.velocity.y > -200.0f && acceleration > 0) {
            drop.velocity.y = -500.0f;
        }
        
        glm::vec2 prevPos = drop.pos;
        drop.pos += drop.velocity * dt;
        
        float speed = glm::length(drop.velocity);
        
        if (m_RainIntensity > 1.5f) {
            drop.spread.y = std::max(drop.spread.y * std::pow(0.0005f, dt), speed * 0.0025f);
            drop.spread.x = std::max(drop.spread.x * std::pow(0.001f, dt), 0.01f);
        } else {
            drop.spread.y = std::max(drop.spread.y * std::pow(0.001f, dt), speed * 0.0008f);
            drop.spread.x *= std::pow(0.001f, dt);
        }
        
        float trailCount = 0.0f;
        if (m_RainIntensity > 1.5f) {
            trailCount = std::min(speed * 0.04f, 15.0f);
        } else if (m_RainIntensity > 0.8f) {
            trailCount = std::min(speed * 0.02f, 8.0f);
        } else {
            trailCount = std::min(speed * 0.008f, 3.0f);
        }
        
        for (int t = 0; t < (int)trailCount; t++) {
            float tFactor = (trailCount > 0.0f) ? (float)t / trailCount : 0.0f;
            float trailMass = drop.mass * 0.003f;
            if (trailMass < 0.3f) break;
            
            Raindrop trail;
            trail.pos = prevPos + (drop.pos - prevPos) * tFactor;
            trail.size = drop.size * (0.6f - tFactor * 0.3f);
            trail.density = drop.density * 0.2f;
            trail.mass = trailMass;
            trail.velocity = drop.velocity * (0.85f - tFactor * 0.45f);
            trail.drag = GRAVITY * MAX_MASS * 0.99f;
            trail.offset = drop.offset * (1.0f - tFactor * 0.5f);
            
            if (m_RainIntensity > 1.5f) {
                trail.spread = glm::vec2(0.008f, 1.2f + tFactor * 0.6f);
            } else {
                trail.spread = glm::vec2(0.015f, 0.5f + tFactor * 0.3f);
            }
            trail.parent = -1;
            newTrails.push_back(trail);
        }
        
        float evaporationRate = EVAPORATION_RATE * (1.0f + m_RainIntensity * 0.1f);
        drop.mass -= evaporationRate * dt * 0.0008f * drop.size;
    }
    
    // 批量插入拖尾雨滴
    if (!newTrails.empty()) {
        size_t capacityNeeded = m_Raindrops.size() + newTrails.size();
        if (capacityNeeded > MAX_RAINDROPS) {
            newTrails.resize(MAX_RAINDROPS - m_Raindrops.size());
        }
        m_Raindrops.insert(m_Raindrops.end(), newTrails.begin(), newTrails.end());
    }
    
    // 移除超出边界或质量过小的雨滴
    m_Raindrops.erase(
        std::remove_if(m_Raindrops.begin(), m_Raindrops.end(),
            [this](const Raindrop &drop) {
                return drop.mass <= 0.0f ||
                       drop.pos.y < -300.0f ||
                       drop.pos.x < -300.0f ||
                       drop.pos.x > m_ScreenWidth + 300.0f;
            }),
        m_Raindrops.end());
    
    CheckCollisions();
}

void RainLayer::CheckCollisions() {
    std::unordered_map<GridKey, std::vector<size_t>> grid;
    
    for (size_t i = 0; i < m_Raindrops.size(); ++i) {
        const Raindrop &drop = m_Raindrops[i];
        GridKey key;
        key.x = (int)(drop.pos.x / GRID_CELL_SIZE);
        key.y = (int)(drop.pos.y / GRID_CELL_SIZE);
        grid[key].push_back(i);
    }
    
    std::vector<bool> merged(m_Raindrops.size(), false);
    
    for (auto &entry : grid) {
        auto &indices = entry.second;
        for (size_t i = 0; i < indices.size(); ++i) {
            if (merged[indices[i]]) continue;
            
            for (size_t j = i + 1; j < indices.size(); ++j) {
                if (merged[indices[j]]) continue;
                
                Raindrop &d1 = m_Raindrops[indices[i]];
                Raindrop &d2 = m_Raindrops[indices[j]];
                
                float dist = glm::distance(d1.pos, d2.pos);
                float radiusSum = d1.size * 0.5f + d2.size * 0.5f;
                
                // 只有真正接触时才合并，避免拖尾立即合并
                if (dist < radiusSum + 5.0f) {
                    if (d1.mass >= d2.mass) {
                        glm::vec2 momentum = d1.mass * d1.velocity + d2.mass * d2.velocity;
                        d1.mass += d2.mass;
                        d1.size = std::sqrt(d1.mass / d1.density);
                        d1.velocity = momentum / d1.mass;
                        d1.pos = (d1.pos * d1.mass + d2.pos * d2.mass) / (d1.mass + d2.mass);
                        merged[indices[j]] = true;
                    } else {
                        glm::vec2 momentum = d1.mass * d1.velocity + d2.mass * d2.velocity;
                        d2.mass += d1.mass;
                        d2.size = std::sqrt(d2.mass / d2.density);
                        d2.velocity = momentum / d2.mass;
                        d2.pos = (d1.pos * d1.mass + d2.pos * d2.mass) / (d1.mass + d2.mass);
                        merged[indices[i]] = true;
                        break;
                    }
                }
            }
        }
    }
    
    // 使用erase-remove惯用法批量删除已合并的雨滴
    size_t writeIdx = 0;
    for (size_t readIdx = 0; readIdx < m_Raindrops.size(); ++readIdx) {
        if (!merged[readIdx]) {
            if (writeIdx != readIdx) {
                m_Raindrops[writeIdx] = std::move(m_Raindrops[readIdx]);
            }
            ++writeIdx;
        }
    }
    m_Raindrops.resize(writeIdx);
}

void RainLayer::Draw(int screenW, int screenH) {
    // 安全检查：确保程序对象有效
    if (m_ProgramObj == 0 || m_DistortProgram == 0) {
        LOGCATD("RainLayer::Draw: program invalid, distortProgram=%d, program=%d", m_DistortProgram, m_ProgramObj);
        return;
    }
    
    if (m_ScreenWidth != screenW || m_ScreenHeight != screenH) {
        m_ScreenWidth = screenW;
        m_ScreenHeight = screenH;
        
        if (m_FBO) {
            glDeleteFramebuffers(1, &m_FBO);
            glDeleteTextures(1, &m_DistortTexture);
            m_FBO = 0;
            m_DistortTexture = 0;
        }
        
        glGenTextures(1, &m_DistortTexture);
        glBindTexture(GL_TEXTURE_2D, m_DistortTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenW, screenH, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glGenFramebuffers(1, &m_FBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_DistortTexture, 0);
        
        GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE) {
            LOGCATD("RainLayer::Draw: FBO not complete, status=0x%x, FBO=%d, texture=%d", fboStatus, m_FBO, m_DistortTexture);
        } else {
            LOGCATI("RainLayer::Draw: FBO created, FBO=%d, texture=%d, screen=%dx%d", m_FBO, m_DistortTexture, screenW, screenH);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    // 安全检查：FBO必须存在
    if (m_FBO == 0 || m_DistortTexture == 0) {
        LOGCATD("RainLayer::Draw: FBO or texture invalid, FBO=%d, distortTex=%d", m_FBO, m_DistortTexture);
        return;
    }
    
    float dt = 0.016f;
    UpdateRaindrops(dt);
    m_Time += dt;
    
    static int frameCount = 0;
    static float logTimer = 0.0f;
    frameCount++;
    logTimer += dt;
    if (logTimer >= 1.0f) {
        LOGCATI("RainLayer::Draw: frameCount=%d, raindrops=%zu, intensity=%.2f, refraction=%.2f", 
                frameCount, m_Raindrops.size(), m_RainIntensity, m_Refraction);
        frameCount = 0;
        logTimer = 0.0f;
    }
    
    // ===== Pass 1: 渲染雨滴法线/遮罩到DistortTexture =====
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(m_DistortProgram);
    
    glm::mat4 proj = glm::ortho(0.0f, (float)screenW, (float)screenH, 0.0f, -1.0f, 1.0f);
    if (m_DistortProjLoc != (GLuint)-1) glUniformMatrix4fv(m_DistortProjLoc, 1, GL_FALSE, &proj[0][0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    if (m_DistortPosLoc != (GLuint)-1) {
        glEnableVertexAttribArray(m_DistortPosLoc);
        glVertexAttribPointer(m_DistortPosLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    }
    if (m_DistortTexAttrLoc != (GLuint)-1) {
        glEnableVertexAttribArray(m_DistortTexAttrLoc);
        glVertexAttribPointer(m_DistortTexAttrLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    
    for (const Raindrop &drop : m_Raindrops) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(drop.pos, 0.0f));
        float scaleX = drop.size * (0.3f + drop.spread.x);
        float scaleY = drop.size * (1.0f + drop.spread.y);
        model = glm::scale(model, glm::vec3(scaleX, scaleY, 1.0f));
        if (m_DistortModelLoc != (GLuint)-1) glUniformMatrix4fv(m_DistortModelLoc, 1, GL_FALSE, &model[0][0]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    
    if (m_DistortPosLoc != (GLuint)-1) glDisableVertexAttribArray(m_DistortPosLoc);
    if (m_DistortTexAttrLoc != (GLuint)-1) glDisableVertexAttribArray(m_DistortTexAttrLoc);
    
    // ===== Pass 2: 应用折射效果到最终输出 =====
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glUseProgram(m_ProgramObj);
    
    if (m_RefractionLoc != (GLuint)-1) glUniform1f(m_RefractionLoc, m_Refraction);
    if (m_TimeLoc != (GLuint)-1) glUniform1f(m_TimeLoc, m_Time);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_BackgroundTexture);
    if (m_BackgroundTexLoc != (GLuint)-1) glUniform1i(m_BackgroundTexLoc, 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_DistortTexture);
    if (m_DistortTexLoc != (GLuint)-1) glUniform1i(m_DistortTexLoc, 1);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    if (m_PositionLoc != (GLuint)-1) {
        glEnableVertexAttribArray(m_PositionLoc);
        glVertexAttribPointer(m_PositionLoc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    }
    if (m_TexCoordLoc != (GLuint)-1) {
        glEnableVertexAttribArray(m_TexCoordLoc);
        glVertexAttribPointer(m_TexCoordLoc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    if (m_PositionLoc != (GLuint)-1) glDisableVertexAttribArray(m_PositionLoc);
    if (m_TexCoordLoc != (GLuint)-1) glDisableVertexAttribArray(m_TexCoordLoc);
}

void RainLayer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        m_ProgramObj = 0;
    }
    if (m_DistortProgram) {
        glDeleteProgram(m_DistortProgram);
        m_DistortProgram = 0;
    }
    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_QuadVBO) {
        glDeleteBuffers(1, &m_QuadVBO);
        m_QuadVBO = 0;
    }
    if (m_FBO) {
        glDeleteFramebuffers(1, &m_FBO);
        m_FBO = 0;
    }
    if (m_DistortTexture) {
        glDeleteTextures(1, &m_DistortTexture);
        m_DistortTexture = 0;
    }
    m_Raindrops.clear();
}

void RainLayer::SetParamInt(LayerParamType paramType, int value) {}

void RainLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_RAIN_INTENSITY:
            m_RainIntensity = value;
            LOGCATI("RainLayer::SetParamFloat: PARAM_RAIN_INTENSITY=%.2f", value);
            break;
        case PARAM_RAIN_WIND_SPEED:
            m_WindSpeed = value;
            LOGCATI("RainLayer::SetParamFloat: PARAM_RAIN_WIND_SPEED=%.2f", value);
            break;
        case PARAM_RAIN_REFRACTION:
            m_Refraction = value;
            LOGCATI("RainLayer::SetParamFloat: PARAM_RAIN_REFRACTION=%.2f", value);
            break;
        default:
            break;
    }
}

void RainLayer::SetBackgroundTexture(GLuint textureId) {
    m_BackgroundTexture = textureId;
    LOGCATI("RainLayer::SetBackgroundTexture: textureId=%d", textureId);
}
