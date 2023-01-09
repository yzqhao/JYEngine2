# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../../CPlatform.txt)

set(PROJECT_NAME FlatSerializeSystem)

################################################################################
# Source groups
################################################################################

set(__private
    "private/Define.h"
)
source_group("private" FILES ${__private})

set(Source
    "FlatDecoder.h"
    "FlatDecoder.cpp"
    "FlatEncoder.h"
    "FlatEncoder.cpp"
    "FlatSerializer.h"
    "FlatSerializer.cpp"
    "FlatSerializerSystem.h"
    "FlatSerializerSystem.cpp"
    "PluginEntry.h"
    "PluginEntry.cpp"
    "generic_generated.h"
    "generic.fbs"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${__private}
    ${Source}
)

add_compile_definitions (
    FLAT_SERIALIZE_SYSTEM_EXPORTS
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
	"${ROOT_PATH}/Third/flatbuffers/include"
)

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	Core
	)
target_link_libraries(${PROJECT_NAME} PUBLIC
    Core
)	

################################################################################
# Compile and link options
################################################################################
include(../../VSCompileLink.txt)
	