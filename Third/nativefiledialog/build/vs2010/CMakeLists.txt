include(../../../../CPlatform.txt)

set(PROJECT_NAME nfd)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "../../src/common.h"
    "../../src/nfd_common.c"
    "../../src/nfd_common.h"
    "../../src/nfd_win.cpp"
    "../../src/simple_exec.h"
)
source_group("" FILES ${no_group_source_files})

set(include
    "../../src/include/nfd.h"
)
source_group("include" FILES ${include})

set(ALL_FILES
    ${no_group_source_files}
    ${include}
)

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_CURRENT_SOURCE_DIR}/../lib/Debug/x86/")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_EDITOR_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../lib/Debug/x86/")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_EDITOR_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../lib/Release/x86/")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_CURRENT_SOURCE_DIR}/../lib/Release/x86/")
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_CURRENT_SOURCE_DIR}/../lib/Debug/x64/")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_EDITOR_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/../lib/Debug/x64/")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_EDITOR_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/../lib/Release/x64/")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_CURRENT_SOURCE_DIR}/../lib/Release/x64/")
endif()
################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Third")

# use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE nfd)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "nfd_d"
        TARGET_NAME_EDITOR_DEBUG   "nfd_d"
        TARGET_NAME_EDITOR_RELEASE "nfd"
        TARGET_NAME_RELEASE        "nfd"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "nfd_d"
        TARGET_NAME_EDITOR_DEBUG   "nfd_d"
        TARGET_NAME_EDITOR_RELEASE "nfd"
        TARGET_NAME_RELEASE        "nfd"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "nfd_d"
        TARGET_NAME_EDITOR_DEBUG   "nfd_d"
        TARGET_NAME_EDITOR_RELEASE "nfd"
        TARGET_NAME_RELEASE        "nfd"
    )
endif()
################################################################################
# Include directories
################################################################################
if(NOT MSVC)
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../src/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../src/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../src/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "_CRT_SECURE_NO_WARNINGS;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "_CRT_SECURE_NO_WARNINGS;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "_CRT_SECURE_NO_WARNINGS;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /MP;
                /Od;
                /ZI
            >
            $<$<CONFIG:Editor_Debug>:
                /Od;
                /ZI
            >
            $<$<CONFIG:Editor_Release>:
                /Gm-;
                /Ox;
                /Oi;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /MP;
                /Gm-;
                /Ox;
                /Oi;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /MP;
                /Od;
                /ZI
            >
            $<$<CONFIG:Editor_Debug>:
                /Od;
                /ZI
            >
            $<$<CONFIG:Editor_Release>:
                /Gm-;
                /Ox;
                /Oi;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /MP;
                /Gm-;
                /Ox;
                /Oi;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /MP;
                /Od;
                /Zi
            >
            $<$<CONFIG:Editor_Debug>:
                /Od;
                /Zi
            >
            $<$<CONFIG:Editor_Release>:
                /Gm-;
                /Ox;
                /Oi;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /MP;
                /Gm-;
                /Ox;
                /Oi;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            /W3;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
            /Y-
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Editor_Release>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Editor_Release>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Editor_Release>:
                /OPT:REF;
                /OPT:ICF
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()











