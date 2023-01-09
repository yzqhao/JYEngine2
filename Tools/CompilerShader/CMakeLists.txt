# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../../CPlatform.txt)

set(PROJECT_NAME CompilerShader)

################################################################################
# Source groups
################################################################################

set(__glsl
    "glsl/GlslBackend.h"
    "glsl/GlslBackend.cpp"
    "glsl/GLSLCompiler.h"
    "glsl/GLSLCompiler.cpp"
    "glsl/ShaderPreprocessor.h"
    "glsl/ShaderPreprocessor.cpp"
)
source_group("glsl" FILES ${__glsl})

set(__hlsl
    "hlsl/hlslCompiler.h"
    "hlsl/hlslCompiler.cpp"
)
source_group("hlsl" FILES ${__hlsl})

set(Source
    "ByteArrayEncoder.h"
    "CommAttri.h"
    "ComplierParam.h"
    "ComplierParam.cpp"
    "ComplierState.h"
    "ComplierState.cpp"
    "ComplierParam.h"
    "ComplierParam.cpp"
    "luaCodes.cpp"
    "MacroEnumer.h"
    "MacroEnumer.cpp"
    "Macros.h"
    "Macros.cpp"
    "main.cpp"
    "shaderpack.h"
    "shaderpack.cpp"
    "Utils.h"
    "Utils.cpp"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${__glsl}
    ${__hlsl}
    ${Source}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Tools")
	
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
	"${ROOT_PATH}/Third"
	"${ROOT_PATH}/Third/liblz4/include"
	"${ROOT_PATH}/Third/libzip/include"
	"${ROOT_PATH}/Third/zlib-1.2.8"
)

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    lua
	mcpp
    hlslcc_lib
	liblz4
    zipper
    libzip
	)
target_link_libraries(${PROJECT_NAME} PUBLIC
    lua
	mcpp
    hlslcc_lib
	liblz4
    zipper
    libzip
)	

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "_CRT_SECURE_NO_WARNINGS;"
        "_CRT_NONSTDC_NO_DEPRECATE;"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "_CRT_SECURE_NO_WARNINGS;"
        "_CRT_NONSTDC_NO_DEPRECATE;"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "_CRT_SECURE_NO_WARNINGS;"
        "_CRT_NONSTDC_NO_DEPRECATE;"
    )
endif()

################################################################################
# Compile and link options
################################################################################
include(../../VSCompileLink.txt)
