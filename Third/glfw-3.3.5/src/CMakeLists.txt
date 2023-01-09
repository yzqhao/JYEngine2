
include(../../../CPlatform.txt)

set(PROJECT_NAME glfw)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "../include/GLFW/glfw3.h"
    "../include/GLFW/glfw3native.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "context.c"
    "egl_context.c"
    "init.c"
    "input.c"
    "monitor.c"
    "osmesa_context.c"
    "vulkan.c"
    "wgl_context.c"
    "win32_init.c"
    "win32_joystick.c"
    "win32_monitor.c"
    "win32_thread.c"
    "win32_time.c"
    "win32_window.c"
    "window.c"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")			
	ADD_DEFINITIONS(-D_GLFW_X11)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	ADD_DEFINITIONS(-D_GLFW_WIN32)
	ADD_DEFINITIONS(-DGLFW_EXPOSE_NATIVE_WIN32)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
	ADD_DEFINITIONS(-D_GLFW_COCOA)
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Third")

################################################################################
# Include directories
################################################################################
if(NOT MSVC)
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/."
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/."
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../include;"
        "${CMAKE_CURRENT_SOURCE_DIR}/."
    )
endif()


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
