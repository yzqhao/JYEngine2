
include(../../../CPlatform.txt)

set(PROJECT_NAME zipper)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "defs.h"
    "tools.h"
    "unzipper.h"
    "zipper.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "CDirEntry.cpp"
    "CDirEntry.h"
    "tools.cpp"
    "tps/dirent.c"
    "tps/dirent.h"
    "unzipper.cpp"
    "zipper.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(minizip
    "../minizip/crypt.h"
    "../minizip/ioapi.c"
    "../minizip/ioapi.h"
    "../minizip/ioapi_buf.c"
    "../minizip/ioapi_buf.h"
    "../minizip/ioapi_mem.c"
    "../minizip/ioapi_mem.h"
    "../minizip/iowin32.c"
    "../minizip/iowin32.h"
    "../minizip/unzip.c"
    "../minizip/unzip.h"
    "../minizip/zip.c"
    "../minizip/zip.h"
)
source_group("minizip" FILES ${minizip})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
    ${minizip}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Third")

# use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE zipper)

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
        "${CMAKE_CURRENT_SOURCE_DIR}/../minizip;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../zlib-1.2.8"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../minizip;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../zlib-1.2.8"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../minizip;"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../zlib-1.2.8"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_LIB"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "_LIB;"
        ">"
        "$<$<CONFIG:Editor_Release>:"
            "_WINDOWS"
        ">"
        "$<$<CONFIG:Release>:"
            "_WINDOWS"
        ">"
        "WIN32;"
        "_CRT_SECURE_NO_WARNINGS;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_LIB"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "_LIB;"
        ">"
        "$<$<CONFIG:Editor_Release>:"
            "_WINDOWS"
        ">"
        "$<$<CONFIG:Release>:"
            "_WINDOWS"
        ">"
        "WIN32;"
        "_CRT_SECURE_NO_WARNINGS;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "WIN32;"
        "_CRT_SECURE_NO_WARNINGS;"
        "_WINDOWS;"
        ";"
        "_MBCS"
    )
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    zlib
)

# Link with other targets.
target_link_libraries(${PROJECT_NAME} PUBLIC
    zlib
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "$<$<CONFIG:Debug>:"
            "zlib;"
            "kernel32;"
            "user32;"
            "gdi32;"
            "winspool;"
            "comdlg32;"
            "advapi32;"
            "shell32;"
            "ole32;"
            "oleaut32;"
            "uuid;"
            "odbc32;"
            "odbccp32"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "zlib;"
            "kernel32;"
            "user32;"
            "gdi32;"
            "winspool;"
            "comdlg32;"
            "advapi32;"
            "shell32;"
            "ole32;"
            "oleaut32;"
            "uuid;"
            "odbc32;"
            "odbccp32"
        ">"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "$ENV{ZLIBROOT}/lib/"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "$ENV{ZLIBROOT}/lib/"
        ">"
    )
endif()













