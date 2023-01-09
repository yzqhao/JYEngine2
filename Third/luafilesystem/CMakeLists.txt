
include(../../CPlatform.txt)

set(PROJECT_NAME luafilesystem)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "src/lfs.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "src/lfs.c"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

set(CMAKE_DEBUG_POSTFIX "")
set(CMAKE_EDITOR_DEBUG_POSTFIX "")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} ${LIBRARY_TYPE_BY_PLATFORM} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Third/Script")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "lfs")

# use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE luafilesystem)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "lfs"
        TARGET_NAME_EDITOR_DEBUG   "lfs"
        TARGET_NAME_EDITOR_RELEASE "lfs"
        TARGET_NAME_RELEASE        "lfs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "lfs"
        TARGET_NAME_EDITOR_DEBUG   "lfs"
        TARGET_NAME_EDITOR_RELEASE "lfs"
        TARGET_NAME_RELEASE        "lfs"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "lfs"
        TARGET_NAME_EDITOR_DEBUG   "lfs"
        TARGET_NAME_EDITOR_RELEASE "lfs"
        TARGET_NAME_RELEASE        "lfs"
    )
endif()

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_EDITOR_RELEASE "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE        "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_EDITOR_RELEASE "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE        "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_EDITOR_RELEASE "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELEASE        "TRUE"
    )
endif()
################################################################################
# Include directories
################################################################################
if(NOT MSVC)
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${ROOT_PATH}/Third/luaVMLib"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${ROOT_PATH}/Third/luaVMLib"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${ROOT_PATH}/Third/luaVMLib"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "WIN32;"
        "_WINDOWS;"
        "_USRDLL;"
        "LUAFILESYSTEM_EXPORTS;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "WIN32;"
        "_WINDOWS;"
        "_USRDLL;"
        "LUAFILESYSTEM_EXPORTS;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "WIN32;"
        "_WINDOWS;"
        "_USRDLL;"
        "LUAFILESYSTEM_EXPORTS;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    lua
	)
target_link_libraries(${PROJECT_NAME} PUBLIC
    lua
)	

################################################################################
# Compile and link options
################################################################################
include(../../VSCompileLink.txt)









