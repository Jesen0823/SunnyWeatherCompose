#ifndef GLLIB_WEATHERPROFILE_H
#define GLLIB_WEATHERPROFILE_H

#include "../base/GLLayerBase.h"
#include <vector>
#include <map>
#include <string>
#include <glm.hpp>

/**
 * 参数值类型枚举
 */
enum ParamValueType {
    PARAM_VALUE_INT = 0,
    PARAM_VALUE_FLOAT = 1,
    PARAM_VALUE_VEC3 = 2
};

/**
 * Layer参数值的类型安全存储
 * 使用分离字段方式实现，避免union导致的复制赋值问题
 */
struct LayerParamValue {
    ParamValueType type;
    int intValue;
    float floatValue;
    glm::vec3 vec3Value;

    LayerParamValue() : type(PARAM_VALUE_INT), intValue(0), floatValue(0.0f), vec3Value(0.0f) {}
    LayerParamValue(int v) : type(PARAM_VALUE_INT), intValue(v), floatValue(0.0f), vec3Value(0.0f) {}
    LayerParamValue(float v) : type(PARAM_VALUE_FLOAT), intValue(0), floatValue(v), vec3Value(0.0f) {}
    LayerParamValue(glm::vec3 v) : type(PARAM_VALUE_VEC3), intValue(0), floatValue(0.0f), vec3Value(v) {}
};

/**
 * 单个Layer的配置
 * 包含Layer类型和该Layer所需的所有参数
 */
struct LayerConfig {
    LayerType layerType;
    std::map<LayerParamType, LayerParamValue> params;

    LayerConfig(LayerType type) : layerType(type) {}

    void SetParamInt(LayerParamType paramType, int value) {
        params[paramType] = LayerParamValue(value);
    }

    void SetParamFloat(LayerParamType paramType, float value) {
        params[paramType] = LayerParamValue(value);
    }

    void SetParamVec2(LayerParamType paramType, float x, float y) {
        params[paramType] = LayerParamValue(glm::vec3(x, y, 0.0f));
    }

    void SetParamVec3(LayerParamType paramType, float x, float y, float z) {
        params[paramType] = LayerParamValue(glm::vec3(x, y, z));
    }

    void SetParamVec3(LayerParamType paramType, glm::vec3 value) {
        params[paramType] = LayerParamValue(value);
    }
};

/**
 * 特殊渲染标记
 * 用于标记该天气是否需要特殊渲染流程
 */
struct RenderFlags {
    bool requiresFBO;              // 是否需要FBO离屏渲染（用于雨滴折射）
    bool requiresLightningLink;    // 是否需要闪电与环境光联动
    bool hasDayNightVariation;     // 是否有日夜变化差异

    RenderFlags() : requiresFBO(false), requiresLightningLink(false), hasDayNightVariation(true) {}
};

/**
 * 天气配置文件
 * 完整描述一个天气场景所需的所有配置：
 * - Layer列表及各自参数
 * - 渲染顺序控制
 * - 特殊渲染标记
 * 
 * 使用场景：
 * - 通过ProfileBuilder构建具体天气的配置
 * - CompositeRenderer根据Profile创建和配置Layer
 * - 支持动态切换天气效果
 */
struct WeatherProfile {
    std::string skycon;
    std::vector<LayerConfig> layers;
    RenderFlags flags;

    /**
     * 添加Layer配置
     * @param config Layer配置
     * @return 返回该Layer配置的引用，便于链式调用设置参数
     */
    LayerConfig& AddLayer(LayerType layerType) {
        layers.emplace_back(layerType);
        return layers.back();
    }

    /**
     * 获取指定类型的Layer配置
     * @param layerType Layer类型
     * @return 配置指针，不存在返回nullptr
     */
    LayerConfig* GetLayerConfig(LayerType layerType) {
        for (auto& layer : layers) {
            if (layer.layerType == layerType) {
                return &layer;
            }
        }
        return nullptr;
    }

    /**
     * 检查是否包含指定类型的Layer
     * @param layerType Layer类型
     * @return 是否包含
     */
    bool HasLayer(LayerType layerType) const {
        for (const auto& layer : layers) {
            if (layer.layerType == layerType) {
                return true;
            }
        }
        return false;
    }

    /**
     * 获取Layer数量
     * @return Layer数量
     */
    size_t GetLayerCount() const {
        return layers.size();
    }
};

#endif // GLLIB_WEATHERPROFILE_H
