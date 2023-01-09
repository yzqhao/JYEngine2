# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../../CPlatform.txt)

set(PROJECT_NAME MeteorEditor)

################################################################################
# Source groups
################################################################################

set(__private
    "private/Define.h"
)
source_group("private" FILES ${__private})

set(__imguifunction
    "imguifunction/AdapterImgui.h"
    "imguifunction/AdapterImgui.cpp"
    "imguifunction/AdapterInput.h"
    "imguifunction/AdapterInput.cpp"
    "imguifunction/AdapterRender.h"
    "imguifunction/AdapterRender.cpp"
    "imguifunction/AdapterWindow.h"
    "imguifunction/AdapterWindow.cpp"
)
source_group("imguifunction" FILES ${__imguifunction})

set(__util 
    "util/MFileTree.h"
    "util/MFileTree.cpp"
    "util/MFileSearchTool.h"
    "util/MFileSearchTool.cpp"
    "util/MFileUtility.h"
    "util/MFileUtility.cpp"
    "util/MImGuiUtil.h"
    "util/MImGuiUtil.cpp"
    "util/MeteorUtil.h"
    "util/MeteorUtil.cpp"
    "util/MSceneExt.h"
    "util/MSceneExt.cpp"
    "util/MSceneManagerExt.h"
    "util/MSceneManagerExt.cpp"
)
source_group("util" FILES ${__util})

set(__window
    "window/MWindow.h"
    "window/MWindow.cpp"
    "window/MWindowBase.h"
    "window/MWindowBase.cpp"
)
source_group("window" FILES ${__window})

set(__window__project
    "window/project/MProject.h"
    "window/project/MProject.cpp"
    "window/project/MProjectManager.h"
    "window/project/MProjectManager.cpp"
    "window/project/MProjectOptions.h"
    "window/project/MProjectOptions.cpp"
)
source_group("window\\project" FILES ${__window__project})

set(__window__editor__command__commands
    "window/editor/command/commands/MCmdPropertyChange.h"
    "window/editor/command/commands/MCmdPropertyChange.cpp"
)
source_group("window\\editor\\command\\commands" FILES ${__window__editor__command__commands})

set(__window__editor__command__commands_rendercomponent
    "window/editor/command/commands/rendercomponent/MCmdAddMaterial.h"
    "window/editor/command/commands/rendercomponent/MCmdAddMaterial.cpp"
    "window/editor/command/commands/rendercomponent/MCmdAddRenderProperty.h"
    "window/editor/command/commands/rendercomponent/MCmdAddRenderProperty.cpp"
    "window/editor/command/commands/rendercomponent/MCmdChangeMaterial.h"
    "window/editor/command/commands/rendercomponent/MCmdChangeMaterial.cpp"
    "window/editor/command/commands/rendercomponent/MCmdChangeShader.h"
    "window/editor/command/commands/rendercomponent/MCmdChangeShader.cpp"
    "window/editor/command/commands/rendercomponent/MCmdChangeMesh.h"
    "window/editor/command/commands/rendercomponent/MCmdChangeMesh.cpp"
    "window/editor/command/commands/rendercomponent/MCmdRemoveMaterial.h"
    "window/editor/command/commands/rendercomponent/MCmdRemoveMaterial.cpp"
    "window/editor/command/commands/rendercomponent/MCmdRemoveRenderProperty.h"
    "window/editor/command/commands/rendercomponent/MCmdRemoveRenderProperty.cpp"
    "window/editor/command/commands/rendercomponent/MCmdSetParameter.h"
    "window/editor/command/commands/rendercomponent/MCmdSetParameter.cpp"
)
source_group("window\\editor\\command\\commands\\rendercomponent" FILES ${__window__editor__command__commands_rendercomponent})

set(__window__editor__command__commands_node
    "window/editor/command/commands/node/MCmdAttachNode.h"
    "window/editor/command/commands/node/MCmdAttachNode.cpp"
    "window/editor/command/commands/node/MCmdCloneNode.h"
    "window/editor/command/commands/node/MCmdCloneNode.cpp"
    "window/editor/command/commands/node/MCmdDeserializePrefab.h"
    "window/editor/command/commands/node/MCmdDeserializePrefab.cpp"
    "window/editor/command/commands/node/MCmdSerializePrefab.h"
    "window/editor/command/commands/node/MCmdSerializePrefab.cpp"
    "window/editor/command/commands/node/MCmdSetNodePrefab.h"
    "window/editor/command/commands/node/MCmdSetNodePrefab.cpp"
    "window/editor/command/commands/node/MCmdCreateDefaultCamera.h"
    "window/editor/command/commands/node/MCmdCreateDefaultCamera.cpp"
    "window/editor/command/commands/node/MCmdCreateDefaultLight.h"
    "window/editor/command/commands/node/MCmdCreateDefaultLight.cpp"
    "window/editor/command/commands/node/MCmdCreateRenderComponent.h"
    "window/editor/command/commands/node/MCmdCreateRenderComponent.cpp"
    "window/editor/command/commands/node/MCmdCreateComponent.h"
    "window/editor/command/commands/node/MCmdCreateComponent.cpp"
    "window/editor/command/commands/node/MCmdDeleteComponent.h"
    "window/editor/command/commands/node/MCmdDeleteComponent.cpp"
)
source_group("window\\editor\\command\\commands\\node" FILES ${__window__editor__command__commands_node})

set(__window__editor__command__commands_scene
    "window/editor/command/commands/scene/MCmdCreateGenericNode.h"
    "window/editor/command/commands/scene/MCmdCreateGenericNode.cpp"
    "window/editor/command/commands/scene/MCmdDestoryNode.h"
    "window/editor/command/commands/scene/MCmdDestoryNode.cpp"
    "window/editor/command/commands/scene/MCmdDeserializeScene.h"
    "window/editor/command/commands/scene/MCmdDeserializeScene.cpp"
)
source_group("window\\editor\\command\\commands\\scene" FILES ${__window__editor__command__commands_scene})

set(__window__editor__command
    "window/editor/command/MCommandBase.h"
    "window/editor/command/MCommandBase.cpp"
    "window/editor/command/MCommandManager.h"
    "window/editor/command/MCommandManager.cpp"
)
source_group("window\\editor\\command" FILES ${__window__editor__command})

set(__window__editor__widget
    "window/editor/widget/MMemuWindowBase.h"
    "window/editor/widget/MMemuWindowBase.cpp"
)
source_group("window\\editor\\widget" FILES ${__window__editor__widget})

set(__window__editor__widget__assets
    "window/editor/widget/assets/FboTool.h"
    "window/editor/widget/assets/FboTool.cpp"
    "window/editor/widget/assets/MatTool.h"
    "window/editor/widget/assets/MatTool.cpp"
    "window/editor/widget/assets/MAssets.h"
    "window/editor/widget/assets/MAssets.cpp"
)
source_group("window\\editor\\widget\\assets" FILES ${__window__editor__widget__assets})

set(__window__editor__widget__game
    "window/editor/widget/game/MGame.h"
    "window/editor/widget/game/MGame.cpp"
)
source_group("window\\editor\\widget\\game" FILES ${__window__editor__widget__game})

set(__window__editor__widget__hierarchy
    "window/editor/widget/hierarchy/MHierarchy.h"
    "window/editor/widget/hierarchy/MHierarchy.cpp"
    "window/editor/widget/hierarchy/MHierarchyBaseView.h"
    "window/editor/widget/hierarchy/MHierarchyBaseView.cpp"
    "window/editor/widget/hierarchy/MHierarchyScene.h"
    "window/editor/widget/hierarchy/MHierarchyScene.cpp"
    "window/editor/widget/hierarchy/MHierarchyResManager.h"
    "window/editor/widget/hierarchy/MHierarchyResManager.cpp"
    "window/editor/widget/hierarchy/MHierarchyTool.h"
    "window/editor/widget/hierarchy/MHierarchyTool.cpp"
)
source_group("window\\editor\\widget\\hierarchy" FILES ${__window__editor__widget__hierarchy})

set(__window__editor__widget__inspector__panels__assets
    "window/editor/widget/inspector/panels/assets/MAssetsGui.h"
    "window/editor/widget/inspector/panels/assets/MAssetsGui.cpp"
    "window/editor/widget/inspector/panels/assets/MTexture.h"
    "window/editor/widget/inspector/panels/assets/MTexture.cpp"
)
source_group("window\\editor\\widget\\inspector\\panels\\assets" FILES ${__window__editor__widget__inspector__panels__assets})

set(__window__editor__widget__inspector__panels
    "window/editor/widget/inspector/panels/InspectorUtil.h"
    "window/editor/widget/inspector/panels/InspectorUtil.cpp"
    "window/editor/widget/inspector/panels/MPCameraComponent.h"
    "window/editor/widget/inspector/panels/MPCameraComponent.cpp"
    "window/editor/widget/inspector/panels/MPLightComponent.h"
    "window/editor/widget/inspector/panels/MPLightComponent.cpp"
    "window/editor/widget/inspector/panels/MPRenderComponent.h"
    "window/editor/widget/inspector/panels/MPRenderComponent.cpp"
    "window/editor/widget/inspector/panels/MPTransformComponent.h"
    "window/editor/widget/inspector/panels/MPTransformComponent.cpp"
    "window/editor/widget/inspector/panels/MPNode.h"
    "window/editor/widget/inspector/panels/MPNode.cpp"
)
source_group("window\\editor\\widget\\inspector\\panels" FILES ${__window__editor__widget__inspector__panels})

set(__window__editor__widget__inspector
    "window/editor/widget/inspector/MInspector.h"
    "window/editor/widget/inspector/MInspector.cpp"
    "window/editor/widget/inspector/MPanelBase.h"
    "window/editor/widget/inspector/MPanelBase.cpp"
)
source_group("window\\editor\\widget\\inspector" FILES ${__window__editor__widget__inspector})

set(__window__editor__widget__mainmenu
    "window/editor/widget/mainmenu/MMainmenu.h"
    "window/editor/widget/mainmenu/MMainmenu.cpp"
)
source_group("window\\editor\\widget\\mainmenu" FILES ${__window__editor__widget__mainmenu})

set(__window__editor__widget__scene
    "window/editor/widget/scene/MDrag.h"
    "window/editor/widget/scene/MDrag.cpp"
    "window/editor/widget/scene/MScene.h"
    "window/editor/widget/scene/MScene.cpp"
)
source_group("window\\editor\\widget\\scene" FILES ${__window__editor__widget__scene})

set(__window__editor__widget__selectwindow
    "window/editor/widget/selectwindow/MInputBox.h"
    "window/editor/widget/selectwindow/MInputBox.cpp"
    "window/editor/widget/selectwindow/MSelectScene.h"
    "window/editor/widget/selectwindow/MSelectScene.cpp"
    "window/editor/widget/selectwindow/MSelectWindow.h"
    "window/editor/widget/selectwindow/MSelectWindow.cpp"
)
source_group("window\\editor\\widget\\selectwindow" FILES ${__window__editor__widget__selectwindow})

set(__window__editor__widget__toolbar
    "window/editor/widget/toolbar/MToolbar.h"
    "window/editor/widget/toolbar/MToolbar.cpp"
)
source_group("window\\editor\\widget\\toolbar" FILES ${__window__editor__widget__toolbar})

set(__window__editor__system 
    "window/editor/system/BundleManager.h"
    "window/editor/system/BundleManager.cpp"
    "window/editor/system/MDefined.h"
    "window/editor/system/EditorSystem.h"
    "window/editor/system/EditorSystem.cpp"
    "window/editor/system/WindowSystem.h"
    "window/editor/system/WindowSystem.cpp"
)
source_group("window\\editor\\system" FILES ${__window__editor__system})

set(__window__editor
    "window/editor/MEditor.h"
    "window/editor/MEditor.cpp"
    "window/editor/MEditorService.h"
    "window/editor/MEditorService.cpp"
)
source_group("window\\editor" FILES ${__window__editor})

set(Source
    "Meteor.h"
    "Meteor.cpp"
    "PluginEntry.h"
    "PluginEntry.cpp"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${__private}
    ${__window}
    ${__window__project}
    ${__window__editor}
    ${__window__editor__command__commands}
    ${__window__editor__command__commands_rendercomponent}
    ${__window__editor__command__commands_node}
    ${__window__editor__command__commands_scene}
    ${__window__editor__command}
    ${__window__editor__widget}
    ${__window__editor__widget__assets}
    ${__window__editor__widget__game}
    ${__window__editor__widget__hierarchy}
    ${__window__editor__widget__inspector__panels__assets}
    ${__window__editor__widget__inspector__panels}
    ${__window__editor__widget__inspector}
    ${__window__editor__widget__mainmenu}
    ${__window__editor__widget__scene}
    ${__window__editor__widget__selectwindow}
    ${__window__editor__widget__toolbar}
    ${__window__editor__system}
    ${__imguifunction}
    ${__util}
    ${Source}
)

add_compile_definitions (
    METEOR_EXPORTS
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} ${LIBRARY_TYPE_BY_PLATFORM} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Function")
	
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
	"${ROOT_PATH}"
	"${ROOT_PATH}/Third/imgui"
	"${ROOT_PATH}/Third/freetype-2.10.4/include"
    "${ROOT_PATH}/Function/Meteor"
)

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	Engine
    imgui
    freetype
	)
target_link_libraries(${PROJECT_NAME} PUBLIC
    Engine
    imgui
    freetype
)	

################################################################################
# Compile and link options
################################################################################
include(../../VSCompileLink.txt)
	