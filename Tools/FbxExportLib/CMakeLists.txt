# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../../CPlatform.txt)

set(PROJECT_NAME FbxExportLib)

################################################################################
# Source groups
################################################################################

source_group("private" FILES ${__private})

set(Source
    "cxxopts.hpp"
    "Fbx2Raw.h"
    "Fbx2Raw.cpp"
    "FbxExporter.h"
    "FbxExporter.cpp"
    "FbxMathUtil.h"
    "File_Utils.h"
    "File_Utils.cpp"
    "Image_Utils.h"
    "Image_Utils.cpp"
    "ObjExporter.h"
    "ObjExporter.cpp"
    "Raw2Apollo.h"
    "Raw2Apollo.cpp"
    "RawModel.h"
    "RawModel.cpp"
    "RawMorph.h"
    "String_Utils.h"
    "String_Utils.cpp"
    "tiny_obj_loader.h"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${Source}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
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
	"${ROOT_PATH}/Third/FBX_SDK/2020.2.1/include"
	"${ROOT_PATH}/Third/draco/include"
	"${ROOT_PATH}/Third/rapidjson/include"
)	
	
################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	Math
	)
target_link_libraries(${PROJECT_NAME} PUBLIC
    Math
)	

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco.lib;"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/debug/libfbxsdk-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/debug/zlib-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/debug/libxml2-md.lib"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco.lib;"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/debug/libfbxsdk-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/debug/zlib-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/debug/libxml2-md.lib"
        ">"
        "$<$<CONFIG:Editor_Release>:"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco.lib;"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/release/libfbxsdk-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/release/zlib-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/release/libxml2-md.lib"
        ">"
        "$<$<CONFIG:Release>:"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco.lib;"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/release/libfbxsdk-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/release/zlib-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/release/libxml2-md.lib"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco.lib;"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/debug/libfbxsdk-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/debug/zlib-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/debug/libxml2-md.lib"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco.lib;"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/debug/libfbxsdk-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/debug/zlib-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/debug/libxml2-md.lib"
        ">"
        "$<$<CONFIG:Editor_Release>:"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco.lib;"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/release/libfbxsdk-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/release/zlib-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/release/libxml2-md.lib"
        ">"
        "$<$<CONFIG:Release>:"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco.lib;"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/release/libfbxsdk-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/release/zlib-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x86/release/libxml2-md.lib"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "${ROOT_PATH}/Third/draco/lib/x64/debug/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x64/debug/draco.lib;"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x64/debug/libfbxsdk-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x64/debug/zlib-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x64/debug/libxml2-md.lib"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "${ROOT_PATH}/Third/draco/lib/x64/debug/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x64/debug/draco.lib;"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x64/debug/libfbxsdk-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x64/debug/zlib-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x64/debug/libxml2-md.lib"
        ">"
        "$<$<CONFIG:Editor_Release>:"
            "${ROOT_PATH}/Third/draco/lib/x64/release/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x64/release/draco.lib;"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x64/release/libfbxsdk-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x64/release/zlib-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x64/release/libxml2-md.lib"
        ">"
        "$<$<CONFIG:Release>:"
            "${ROOT_PATH}/Third/draco/lib/x64/release/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x64/release/draco.lib;"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x64/release/libfbxsdk-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x64/release/zlib-md.lib"
            "${ROOT_PATH}/Third/FBX_SDK/2020.2.1/lib/vs2019/x64/release/libxml2-md.lib"
        ">"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Editor_Release>:"
            "${ROOT_PATH}/Third/draco/lib/x86"
        ">"
        "$<$<CONFIG:Release>:"
            "${ROOT_PATH}/Third/draco/lib/x86"
        ">"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Editor_Release>:"
            "${ROOT_PATH}/Third/draco/lib/x86"
        ">"
        "$<$<CONFIG:Release>:"
            "${ROOT_PATH}/Third/draco/lib/x86"
        ">"
    )
endif()