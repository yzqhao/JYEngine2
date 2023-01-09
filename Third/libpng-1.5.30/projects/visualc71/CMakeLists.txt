
include(../../../../CPlatform.txt)

set(PROJECT_NAME libpng)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "PRJ0041.mak"
    "README.txt"
)
source_group("" FILES ${no_group_source_files})

set(Header_Files
    "../../png.h"
    "../../pngconf.h"
    "../../pngpriv.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "../../png.c"
    "../../pngerror.c"
    "../../pngget.c"
    "../../pngmem.c"
    "../../pngpread.c"
    "../../pngread.c"
    "../../pngrio.c"
    "../../pngrtran.c"
    "../../pngrutil.c"
    "../../pngset.c"
    "../../pngtrans.c"
    "../../pngwio.c"
    "../../pngwrite.c"
    "../../pngwtran.c"
    "../../pngwutil.c"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${no_group_source_files}
    ${Header_Files}
    ${Source_Files}
)

set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_EDITOR_DEBUG_POSTFIX "d")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Third")

target_precompile_headers(${PROJECT_NAME} PRIVATE "../../pngpriv.h")
	
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
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../zlib-1.2.8"
)

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "$<$<CONFIG:Debug>:"
        "DEBUG;"
        "PNG_DEBUG=1"
    ">"
    "$<$<CONFIG:Editor_Debug>:"
        "DEBUG;"
        "PNG_DEBUG=1"
    ">"
    "$<$<CONFIG:Editor_Release>:"
    ">"
    "$<$<CONFIG:Release>:"
    ">"
    "WIN32;"
    "_CRT_SECURE_NO_WARNINGS;"
    "UNICODE;"
    "_UNICODE"
)

	