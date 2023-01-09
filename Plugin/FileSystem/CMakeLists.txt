# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../../CPlatform.txt)

set(PROJECT_NAME FileSystem)

################################################################################
# Source groups
################################################################################

set(__private
    "private/Define.h"
)
source_group("private" FILES ${__private})

set(Source
    "BaseFile.h"
    "BaseFile.cpp"
    "CDirEntry.h"
    "CDirEntry.cpp"
    "FileSystem.h"
    "FileSystem.cpp"
    "PackageFile.h"
    "PackageFile.cpp"
    "UsualFile.h"
    "UsualFile.cpp"
    "ZippedFile.h"
    "ZippedFile.cpp"
    "PluginEntry.h"
    "PluginEntry.cpp"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${__private}
    ${Source}
)

add_compile_definitions (
    FILE_SYSTEM_EXPORTS
    ZIP_STATIC
    LZ4F_STATIC_LINKING_ONLY
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
	"${ROOT_PATH}/Third/liblz4/include"
	"${ROOT_PATH}/Third/libzip/include"
)

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	Core
	liblz4
    zipper
    libzip
	)
target_link_libraries(${PROJECT_NAME} PUBLIC
    Core
	liblz4
    zipper
    libzip
)	

################################################################################
# Compile and link options
################################################################################
include(../../VSCompileLink.txt)
	