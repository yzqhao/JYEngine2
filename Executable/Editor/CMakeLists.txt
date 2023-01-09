# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../../CPlatform.txt)

set(PROJECT_NAME Editor)

################################################################################
# Source groups
################################################################################
set(Source
    "GlfwCallback.h"
    "Application.cpp"
    "Application.h"
    "AppWindow.h"
    "AppWindow.cpp"
    "main.cpp"
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
#add_library(${PROJECT_NAME} ${LIBRARY_TYPE_BY_PLATFORM} ${ALL_FILES})
add_executable(${PROJECT_NAME} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Executable")

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
	    "${ROOT_PATH}/Third/glfw-3.3.5/include"
        "${ROOT_PATH}/Third/glew-1.9.0-win32/include"
        "${ROOT_PATH}/Third/glad"
        "${ROOT_PATH}/Third/rapidjson/include"
        "${ROOT_PATH}/Third/nativefiledialog/src/include"
	    "${ROOT_PATH}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PRIVATE
	    "${ROOT_PATH}/Third/glfw-3.3.5/include"
        "${ROOT_PATH}/Third/glew-1.9.0-win64/include"
        "${ROOT_PATH}/Third/glad"
        "${ROOT_PATH}/Third/rapidjson/include"
        "${ROOT_PATH}/Third/nativefiledialog/src/include"
	    "${ROOT_PATH}"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PRIVATE
	    "${ROOT_PATH}/Third/glfw-3.3.5/include"
        "${ROOT_PATH}/Third/glew-1.9.0-win64/include"
        "${ROOT_PATH}/Third/glad"
        "${ROOT_PATH}/Third/rapidjson/include"
        "${ROOT_PATH}/Third/nativefiledialog/src/include"
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
    glfw
    nfd
    MeteorEditor
	)
target_link_libraries(${PROJECT_NAME} PUBLIC
    Core
    Engine
    RHI
    glfw
    nfd
    MeteorEditor
)	

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "glew32;"
        "glew32mx;"
        "glew32mxs;"
        "glew32s;"
        "opengl32;"
        "Dbghelp"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "glew32;"
        "glew32mx;"
        "glew32mxs;"
        "glew32s;"
        "opengl32;"
        "Dbghelp"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "glew32;"
        "glew32mx;"
        "glew32mxs;"
        "glew32s;"
        "opengl32"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${ROOT_PATH}/Third/glew-1.9.0-win32/lib"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${ROOT_PATH}/Third/glew-1.9.0-win32/lib"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${ROOT_PATH}/Third/glew-1.9.0-win64/lib"
    )
endif()

################################################################################
# Compile and link options
################################################################################
include(../../VSCompileLink.txt)
