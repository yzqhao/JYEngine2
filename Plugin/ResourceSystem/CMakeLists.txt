# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../../CPlatform.txt)

set(PROJECT_NAME ResourceSystem)

################################################################################
# Source groups
################################################################################

set(__private
    "private/Define.h"
)
source_group("private" FILES ${__private})

set(Source
    "BundleReader.h"
    "BundleReader.cpp"
    "MatLoader.h"
    "MatLoader.cpp"
    "MeshCache.h"
    "MeshCache.cpp"
    "MeshLoader.h"
    "MeshLoader.cpp"
    "TextureLoader.h"
    "TextureLoader.cpp"
    "TexMetaLoader.h"
    "TexMetaLoader.cpp"
    "ResourceSystem.h"
    "ResourceSystem.cpp"
    "PluginEntry.h"
    "PluginEntry.cpp"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${__private}
    ${Source}
)

add_compile_definitions (
    RESOURCE_SYSTEM_EXPORTS
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} ${LIBRARY_TYPE_BY_PLATFORM} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Plugin")
	
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
	"${ROOT_PATH}/Third/libpng-1.5.30"
	"${ROOT_PATH}/Third/zlib-1.2.8"
	"${ROOT_PATH}/Third/draco/include"
	"${ROOT_PATH}/Third/libexif"
	"${ROOT_PATH}/Third/libyuv/include"
)

if(NOT MSVC)
    if (IOS)
        target_include_directories(${PROJECT_NAME} PRIVATE
            "${ROOT_PATH}/Third/libjpeg-turbo/include;"
        )
    elseif (ANDROID)
        target_include_directories(${PROJECT_NAME} PRIVATE
            "${ROOT_PATH}/Third/android-libjpeg-turbo/jni/include;"
            "${ROOT_PATH}/Third/android-libjpeg-turbo/jni/libjpeg-turbo-1.4.1;"
        )
    endif ()
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PRIVATE
	    "${ROOT_PATH}/Third/libjpeg-turbo-win32/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PRIVATE
	    "${ROOT_PATH}/Third/libjpeg-turbo-win64/include"
    )
else()
    include_directories(${PROJECT_NAME} PRIVATE
	    "${ROOT_PATH}/Third/libjpeg-turbo-win64/include"
    )
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	Engine
    libpng
    zlib
    libexif
    yuv_static
	)
target_link_libraries(${PROJECT_NAME} PUBLIC
    Engine
    libpng
    zlib
    libexif
    yuv_static
)	

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco.lib;"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco.lib;"
        ">"
        "$<$<CONFIG:Editor_Release>:"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco.lib;"
        ">"
        "$<$<CONFIG:Release>:"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco.lib;"
        ">"
        "${ROOT_PATH}/Third/libjpeg-turbo-win64/lib"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco.lib;"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/debug/draco.lib;"
        ">"
        "$<$<CONFIG:Editor_Release>:"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco.lib;"
        ">"
        "$<$<CONFIG:Release>:"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x86/release/draco.lib;"
        ">"
        "${ROOT_PATH}/Third/libjpeg-turbo-win32/lib/turbojpeg-static.lib;"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "${ROOT_PATH}/Third/draco/lib/x64/debug/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x64/debug/draco.lib;"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "${ROOT_PATH}/Third/draco/lib/x64/debug/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x64/debug/draco.lib;"
        ">"
        "$<$<CONFIG:Editor_Release>:"
            "${ROOT_PATH}/Third/draco/lib/x64/release/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x64/release/draco.lib;"
        ">"
        "$<$<CONFIG:Release>:"
            "${ROOT_PATH}/Third/draco/lib/x64/release/draco_encoder.lib;"
            "${ROOT_PATH}/Third/draco/lib/x64/release/draco.lib;"
        ">"
        "${ROOT_PATH}/Third/libjpeg-turbo-win64/lib/turbojpeg-static.lib;"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

################################################################################
# Compile and link options
################################################################################
include(../../VSCompileLink.txt)
	