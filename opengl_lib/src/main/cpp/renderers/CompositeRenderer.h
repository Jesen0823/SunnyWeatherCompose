#ifndef GLLIB_COMPOSITERENDERER_H
#define GLLIB_COMPOSITERENDERER_H

#include "base/GLLayerBase.h"
#include "config/WeatherProfile.h"
#include <vector>
#include <map>
#include <string>

/**
 * 组合渲染器
 * 
 * 负责管理多个渲染层，按顺序渲染各层，支持层的添加、移除和清空操作。
 * 
 * 渲染顺序（按 LayerType 排序，特殊层有优先级调整）：
 * 1. SkyBackgroundLayer（天空背景层）- 全屏天空渐变 + 太阳/月亮光晕
 * 2. StarLayer（星星层）- 星星粒子系统（仅夜间显示）
 * 3. CloudLayer（云层）- 程序化云层，遮挡星星和太阳/月亮
 * 4. WindLayer（风力层）- 风线效果，在雪层之前绘制
 * 5. SnowLayer（雪层）- 雪花粒子效果
 * 6. ParticleLayer（颗粒层）- 雾霾/雾/沙尘效果
 * 7. RainLayer（雨层）- 全屏程序化雨丝效果（使用FBO离屏渲染）
 * 8. LightningLayer（闪电层）- 闪电特效，单独绘制阶段
 * 9. AmbientOverlayLayer（环境光覆盖层）- 闪电时全局亮变，最后绘制
 * 
 * 特殊渲染流程：
 * - 雨天模式（requiresFBO=true）：先将除雨层外的所有层渲染到FBO纹理，然后雨层使用此纹理作为背景实现折射效果
 * - 闪电模式（requiresLightningLink=true）：闪电层与环境覆盖层联动，闪电亮度实时传递给环境层
 * 
 * 层堆栈按照 LayerType 自动排序，确保渲染顺序正确。
 * 
 * 设计模式：Facade模式 + Template Method模式
 * - Facade模式：为上层提供统一的渲染接口
 * - Template Method模式：各Layer实现统一的Init/Draw/Destroy接口
 */
class CompositeRenderer : public GLRendererBase {
public:
    CompositeRenderer();
    virtual ~CompositeRenderer();
    
    /**
     * 初始化渲染器
     */
    virtual bool Init() override;
    
    /**
     * 绘制所有层
     * @param screenW 屏幕宽度
     * @param screenH 屏幕高度
     */
    virtual void Draw(int screenW, int screenH) override;
    
    /**
     * 销毁所有层资源
     */
    virtual void Destroy() override;
    
    /**
     * 添加渲染层
     * @param layer 要添加的层（所有权转移）
     */
    void AddLayer(GLLayerBase *layer);
    
    /**
     * 移除渲染层
     * @param layerType 要移除的层类型
     */
    void RemoveLayer(LayerType layerType);
    
    /**
     * 清空所有层
     */
    void ClearLayers();
    
    /**
     * 获取指定类型的层
     * @param layerType 层类型
     * @return 层指针，不存在返回 nullptr
     */
    GLLayerBase *GetLayer(LayerType layerType);
    
    /**
     * 检查是否存在指定类型的层
     * @param layerType 层类型
     * @return 是否存在
     */
    bool HasLayer(LayerType layerType);
    
    /**
     * 获取所有层的数量
     * @return 层数量
     */
    int GetLayerCount() const;
    
    /**
     * 根据 skycon 配置层组合
     * @param skycon 天气类型字符串
     */
    void ConfigureLayers(const char *skycon);
    
    /**
     * 根据 WeatherProfile 配置层组合
     * @param profile 天气配置文件
     */
    void ConfigureLayers(const WeatherProfile& profile);
    
private:
    /**
     * 对层堆栈进行排序，确保正确的渲染顺序
     */
    void SortLayers();
    
    /**
     * 根据 LayerConfig 创建并配置 Layer
     * @param config Layer配置
     * @return 创建的Layer指针，创建失败返回nullptr
     */
    GLLayerBase* CreateLayer(const LayerConfig& config);
    
    /**
     * 应用Layer参数到Layer实例
     * @param layer Layer实例
     * @param config Layer配置
     */
    void ApplyLayerParams(GLLayerBase* layer, const LayerConfig& config);
    
    bool InitFBO(int width, int height);
    void DestroyFBO();
    
    GLuint m_FBO = 0;
    GLuint m_FBOTexture = 0;
    
    std::vector<GLLayerBase *> m_Layers;  // 层堆栈
    std::map<LayerType, GLLayerBase *> m_LayerMap;  // 层类型到层的映射
    
    int m_ScreenWidth;                    // 屏幕宽度
    int m_ScreenHeight;                   // 屏幕高度
    
    bool m_IsInitialized;                 // 是否已初始化
    
    RenderFlags m_RenderFlags;            // 渲染标记
};

#endif // GLLIB_COMPOSITERENDERER_H
