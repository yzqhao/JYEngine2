# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../CPlatform.txt)

set(PROJECT_NAME Engine)

################################################################################
# Source groups
################################################################################

set(__private
    "private/Define.h"
    "private/Define.cpp"
)
source_group("private" FILES ${__private})

set(__component
    "component/Animatable.h"
    "component/Animatable.cpp"
    "component/Component.h"
    "component/Component.cpp"
    "component/GizmoConfig.h"
)
source_group("component" FILES ${__component})

set(__component__transform
    "component/transform/TransformComponent.h"
    "component/transform/TransformComponent.cpp"
)
source_group("component\\transform" FILES ${__component__transform})

set(__component__camera
    "component/camera/CameraComponent.h"
    "component/camera/CameraComponent.cpp"
    "component/camera/CameraGizmo.h"
    "component/camera/CameraGizmo.cpp"
)
source_group("component\\camera" FILES ${__component__camera})

set(__component__light
    "component/light/LightInfo.h"
    "component/light/LightInfo.cpp"
    "component/light/LightComponent.h"
    "component/light/LightComponent.cpp"
    "component/light/LightGizmo.h"
    "component/light/LightGizmo.cpp"
)
source_group("component\\light" FILES ${__component__light})

set(__component__render
    "component/render/RenderComponent.h"
    "component/render/RenderComponent.cpp"
)
source_group("component\\render" FILES ${__component__render})

set(__object
    "object/IMessage.h"
    "object/GObject.h"
    "object/GObject.cpp"
    "object/ObjectIdAllocator.h"
    "object/ObjectIdAllocator.cpp"
    "object/Scene.h"
    "object/Scene.cpp"
    "object/SceneManager.h"
    "object/SceneManager.cpp"
)
source_group("object" FILES ${__object})

set(__pipeline
    "pipeline/ContextUntil.h"
    "pipeline/ContextUntil.cpp"
    "pipeline/InstanceBatcher.h"
    "pipeline/InstanceBatcher.cpp"
    "pipeline/IRenderLoop.h"
    "pipeline/IRenderLoop.cpp"
    "pipeline/PipelineContex.h"
    "pipeline/PipelineContex.cpp"
    "pipeline/PipelineSystem.h"
    "pipeline/PipelineSystem.cpp"
    "pipeline/RenderContext.h"
    "pipeline/RenderContext.cpp"
    "pipeline/RenderPipe.h"
    "pipeline/RenderPipe.cpp"
    "pipeline/RenderUntil.h"
    "pipeline/RenderUntil.cpp"
)
source_group("pipeline" FILES ${__pipeline})

set(__graphic
    "graphic/IAsyncResource.h"
    "graphic/IndicesBufferResource.h"
    "graphic/IndicesBufferResource.cpp"
    "graphic/ITextureResource.h"
    "graphic/ITextureResource.cpp"
    "graphic/MaterialResource.h"
    "graphic/MaterialResource.cpp"
    "graphic/RenderObjectResource.h"
    "graphic/RenderObjectResource.cpp"
    "graphic/RenderTargetResource.h"
    "graphic/RenderTargetResource.cpp"
    "graphic/VertexBufferResource.h"
    "graphic/VertexBufferResource.cpp"
)
source_group("graphic" FILES ${__graphic})

set(__queue
    "queue/ILightQueue.h"
    "queue/ILightQueue.cpp"
    "queue/IRenderQueue.h"
    "queue/IRenderQueue.cpp"
    "queue/LightQueueSystem.h"
    "queue/LightQueueSystem.cpp"
    "queue/RenderQueueSystem.h"
    "queue/RenderQueueSystem.cpp"
)
source_group("queue" FILES ${__queue})

set(__render
    "render/IndicesStream.h"
    "render/IndicesStream.cpp"
    "render/VertexStreamLayout.h"
    "render/VertexStream.h"
    "render/VertexStream.cpp"
    "render/IndicesBufferEntity.h"
    "render/IndicesBufferEntity.cpp"
    "render/RenderObjectEntity.h"
    "render/RenderObjectEntity.inl"
    "render/RenderObjectEntity.cpp"
    "render/RenderTargetEntity.h"
    "render/RenderTargetEntity.inl"
    "render/RenderTargetEntity.cpp"
    "render/VertexBufferEntity.h"
    "render/VertexBufferEntity.cpp"
    "render/SubMesh.h"
    "render/SubMesh.cpp"
)
source_group("render" FILES ${__render})

set(__render__texture
    "render/texture/TextureStream.h"
    "render/texture/TextureStream.cpp"
    "render/texture/TextureEntity.inl"
    "render/texture/TextureEntity.h"
    "render/texture/TextureEntity.cpp"
    "render/texture/TextureFormat.h"
    "render/texture/TextureFormat.cpp"
)
source_group("render\\texture" FILES ${__render__texture})

set(__render__material_json
    "render/material/json/sajson.h"
    "render/material/json/JsonParserUtil.h"
    "render/material/json/JsonParserUtil.cpp"
)
source_group("render\\material\\json" FILES ${__render__material_json})

set(__render__material
    "render/material/ApplyKeyWord.h"
    "render/material/BuiltinParams.h"
    "render/material/BuiltinParams.cpp"
    "render/material/DecompressShaderAnalyzer.h"
    "render/material/DecompressShaderAnalyzer.cpp"
    "render/material/DefProperty.h"
    "render/material/DefProperty.cpp"
    "render/material/IMaterialSystem.h"
    "render/material/IMaterialSystem.cpp"
    "render/material/IShaderResource.h"
    "render/material/IShaderResource.cpp"
    "render/material/JsonMaterialParse.h"
    "render/material/JsonMaterialParse.cpp"
    "render/material/KeyWordUtil.h"
    "render/material/KeyWordUtil.cpp"
    "render/material/MaterialEntity.h"
    "render/material/MaterialEntity.cpp"
    "render/material/MaterialParameter.h"
    "render/material/MaterialParameter.cpp"
    "render/material/MaterialUtil.h"
    "render/material/MaterialUtil.cpp"
    "render/material/ParameterArray.h"
    "render/material/ParameterArray.cpp"
    "render/material/ParserMaterial.h"
    "render/material/ParserStateType.h"
    "render/material/RenderState.h"
    "render/material/RenderState.cpp"
    "render/material/ShaderData.h"
    "render/material/ShaderKeyWords.h"
    "render/material/ShaderKeyWords.cpp"
    "render/material/ShaderPass.h"
    "render/material/ShaderResource.h"
    "render/material/ShaderResource.cpp"
    "render/material/ShaderState.h"
    "render/material/ShaderState.cpp"
)
source_group("render\\material" FILES ${__render__material})

set(__platform
    "platform/PlatformEngine.h"
    "platform/windows/ModuleManager.h"
    "platform/windows/ModuleManager.cpp"
)
source_group("platform" FILES ${__platform})

set(__resource
    "resource/IResourceEntity.h"
    "resource/IResourceEntity.cpp"
    "resource/IAsyncEntity.h"
    "resource/IAsyncEntity.cpp"
    "resource/IAsyncSource.h"
    "resource/IAsyncSource.cpp"
    "resource/ISharedSource.h"
    "resource/ISharedSource.cpp"
    "resource/IMetadata.h"
    "resource/IMetadata.cpp"
    "resource/GeneralMetadata.h"
    "resource/GeneralMetadata.cpp"
    "resource/IndicesMetadata.h"
    "resource/IndicesMetadata.cpp"
    "resource/MaterialMetadata.h"
    "resource/MaterialMetadata.cpp"
    "resource/MaterialStreamData.h"
    "resource/MaterialStreamData.cpp"
    "resource/TextureMetadata.h"
    "resource/TextureMetadata.cpp"
    "resource/RenderObjectMetadata.h"
    "resource/RenderObjectMetadata.cpp"
    "resource/RenderTargetMetadata.h"
    "resource/RenderTargetMetadata.cpp"
    "resource/ShapeBufferUtility.h"
    "resource/ShapeBufferUtility.cpp"
    "resource/VertexMetadata.h"
    "resource/VertexMetadata.cpp"
)
source_group("resource" FILES ${__resource})

set(Source
    "ViewResolution.h"
    "Engine.cpp"
    "Engine.h"
    "IGraphicSystem.cpp"
    "IGraphicSystem.h"
    "ProjectSetting.cpp"
    "ProjectSetting.h"
    "GraphicDefine.h"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${__private}
    ${__component}
    ${__component__camera}
    ${__component__light}
    ${__component__render}
    ${__component__transform}
    ${__object}
    ${__pipeline}
    ${__graphic}
    ${__queue}
    ${__render}
    ${__render__texture}
    ${__render__material_json}
    ${__render__material}
    ${__platform}
    ${__resource}
    ${Source}
)

add_compile_definitions (
    ENGINE_EXPORTS
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} ${LIBRARY_TYPE_BY_PLATFORM} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Engine")
	
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "${PROJECT_NAME}_d"
        TARGET_NAME_EDITOR_DEBUG   "${PROJECT_NAME}_d"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "${PROJECT_NAME}_d"
        TARGET_NAME_EDITOR_DEBUG   "${PROJECT_NAME}_d"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "${PROJECT_NAME}_d"
        TARGET_NAME_EDITOR_DEBUG   "${PROJECT_NAME}_d"
    )
endif()

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
    "${ROOT_PATH}/Third/rapidjson/include"
	"${ROOT_PATH}"
)

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	Core
    RHI
	)
target_link_libraries(${PROJECT_NAME} PUBLIC
    Core
    RHI
)	

################################################################################
# Compile and link options
################################################################################
include(../VSCompileLink.txt)
	