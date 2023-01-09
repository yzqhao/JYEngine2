# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../../CPlatform.txt)

set(PROJECT_NAME bgfx)

################################################################################
# Source groups
################################################################################

file(
    GLOB ALL_FILES
    "./include/bgfx/*.h"
    "./src/*.h"
    "./src/OpenGL/*.h"
    "./src/OpenGL/*.cpp"
    "./src/bgfx.idl.inl"
    "./src/bgfx.cpp"
    "./src/debug_renderdoc.cpp"
    "./src/dxgi.cpp"
    "./src/glcontext_egl.cpp"
    "./src/glcontext_glx.cpp"
    "./src/glcontext_html5.cpp"
    "./src/glcontext_wgl.cpp"
    "./src/nvapi.cpp"
    "./src/renderer_agc.cpp"
    "./src/renderer_d3d11.cpp"
    "./src/renderer_d3d12.cpp"
    "./src/renderer_gl.cpp"
    "./src/renderer_gnm.cpp"
    "./src/renderer_noop.cpp"
    "./src/renderer_nvn.cpp"
    "./src/renderer_vk.cpp"
    "./src/renderer_webgpu.cpp"
    "./src/shader.cpp"
    "./src/shader_dx9bc.cpp"
    "./src/shader_dxbc.cpp"
    "./src/shader_spirv.cpp"
    "./src/topology.cpp"
    "./src/vertexlayout.cpp"
)

add_compile_definitions (
	__STDC_LIMIT_MACROS
	__STDC_FORMAT_MACROS
	__STDC_CONSTANT_MACROS
)

if (CMAKE_BUILD_TYPE MATCHES "Debug" OR CMAKE_BUILD_TYPE MATCHES "")
	add_compile_definitions (
		BX_CONFIG_DEBUG=1
	)
else ()
	add_compile_definitions (
		BX_CONFIG_DEBUG=0
	)
endif ()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Third/bgfx")
	
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "${PROJECT_NAME}_d"
        TARGET_NAME_EDITOR_DEBUG   "${PROJECT_NAME}_d"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
	target_include_directories(${PROJECT_NAME} PRIVATE
		"${ROOT_PATH}/Third/bx/include/compat/msvc"
	)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "${PROJECT_NAME}_d"
        TARGET_NAME_EDITOR_DEBUG   "${PROJECT_NAME}_d"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
	target_include_directories(${PROJECT_NAME} PRIVATE
		"${ROOT_PATH}/Third/bx/include/compat/msvc"
	)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "${PROJECT_NAME}_d"
        TARGET_NAME_EDITOR_DEBUG   "${PROJECT_NAME}_d"
    )
endif()

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
	"${ROOT_PATH}/Third/bgfx/include"
	"${ROOT_PATH}/Third/bgfx/3rdparty"
	"${ROOT_PATH}/Third/bgfx/3rdparty/khronos"
	"${ROOT_PATH}/Third/bgfx/3rdparty/dxsdk/include"
	"${ROOT_PATH}/Third/bimg/include"
	"${ROOT_PATH}/Third/bimg/src"
	"${ROOT_PATH}/Third/bimg/3rdparty"
	"${ROOT_PATH}/Third/bimg/3rdparty/astc-codec/include"
	"${ROOT_PATH}/Third/bimg/3rdparty/iqa/include"
	"${ROOT_PATH}/Third/bimg/3rdparty/tinyexr/deps/miniz"
	"${ROOT_PATH}/Third/bx/include"
)

################################################################################
# Compile and link options
################################################################################
include(../../VSCompileLink.txt)
