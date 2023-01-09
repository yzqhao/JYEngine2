# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../../CPlatform.txt)

set(CMAKE_CXX_STANDARD 17)  # SOL库必须要支持c++17

set(PROJECT_NAME LuaScriptSystem)

################################################################################
# Source groups
################################################################################
set(__private
    "private/Define.h"
)
source_group("private" FILES ${__private})

set(__Script
    "Script/LuaFunctionInvoker.h"
    "Script/LuaFunctionInvoker.cpp"
    "Script/LuaVirtualMachine.h"
    "Script/LuaVirtualMachine.cpp"
)
source_group("Script" FILES ${__Script})

set(Source
    "BindingCore.h"
    "BindingCore.cpp"
    "BindingEngine.h"
    "BindingEngine.cpp"
    "BindingEngineEntity.cpp"
    #"BindingImgui.h"
    #"BindingImgui.cpp"
    "BindingMath.h"
    "BindingMath.cpp"
    "LuaScriptSystem.h"
    "LuaScriptSystem.cpp"
    "PluginEntry.h"
    "PluginEntry.cpp"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${__private}
    ${__Script}
    ${Source}
)

ADD_DEFINITIONS(-DLUA_SCRIPT_EXPORTS)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

################################################################################
# Target
################################################################################
#add_library(${PROJECT_NAME} ${LIBRARY_TYPE_BY_PLATFORM} ${ALL_FILES})
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
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${ROOT_PATH}/Third/luaVMLib"
        "${ROOT_PATH}/Third/sol/include"
	    "${ROOT_PATH}/Third/imgui"
	    "${ROOT_PATH}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${ROOT_PATH}/Third/luaVMLib"
        "${ROOT_PATH}/Third/sol/include"
	    "${ROOT_PATH}/Third/imgui"
	    "${ROOT_PATH}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${ROOT_PATH}/Third/luaVMLib"
        "${ROOT_PATH}/Third/sol/include"
	    "${ROOT_PATH}/Third/imgui"
	    "${ROOT_PATH}"
    )
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	Core
    Engine
    RHI
    lua
    socket
    imguifunction
	)
target_link_libraries(${PROJECT_NAME} PUBLIC
    Core
    Engine
    RHI
    lua
    socket
    imguifunction
)	

################################################################################
# Compile and link options
################################################################################
include(../../VSCompileLink.txt)

