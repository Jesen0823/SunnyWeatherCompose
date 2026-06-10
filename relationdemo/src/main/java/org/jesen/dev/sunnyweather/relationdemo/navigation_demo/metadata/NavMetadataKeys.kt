package org.jesen.dev.sunnyweather.relationdemo.navigation_demo.metadata

/**
 * NavMetadata 键定义
 * 
 * 【NavMetadata 说明】
 * 
 * Navigation3 1.1.0+ 引入的类型安全元数据 DSL，用于在导航过程中传递额外信息。
 * 
 * 【MetadataKey 接口】
 * 允许用户定义希望提供给元数据映射的键和值类型，提供类型安全的访问。
 * 
 * 【使用场景】
 * - 传递页面标题
 * - 传递过渡动画参数
 * - 传递页面配置信息
 * - 传递场景装饰器所需的配置
 * 
 * 【优势】
 * - 类型安全：编译时检查类型
 * - 灵活：可以传递任意类型的数据
 * - 可扩展：轻松添加新的元数据键
 */
object NavMetadataKeys {
    /** 页面标题 */
    const val TITLE = "title"
    
    /** 是否显示返回按钮 */
    const val SHOW_BACK_BUTTON = "showBackButton"
    
    /** 过渡动画类型 */
    const val TRANSITION_ANIMATION = "transitionAnimation"
    
    /**
     * 【Navigation3 1.1.0+ 推荐用法】
     * 使用 MetadataKey.create<T>() 创建类型安全的元数据键：
     * 
     * val TitleKey = MetadataKey.create<String>("title")
     * val ShowBackButton = MetadataKey.create<Boolean>("showBackButton")
     * 
     * 在 NavEntry 中设置：
     * metadata = {
     *     it[TitleKey] = "Page Title"
     *     it[ShowBackButton] = true
     * }
     * 
     * 在目标页面读取：
     * val title = metadata[TitleKey] ?: "Default Title"
     */
}