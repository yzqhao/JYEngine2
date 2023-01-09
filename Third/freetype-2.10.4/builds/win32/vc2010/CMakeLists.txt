include(../../../../../build/cmake/CPlatform.txt)

set(PROJECT_NAME freetype)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "../../../include/freetype/config/ftconfig.h"
    "../../../include/freetype/config/ftheader.h"
    "../../../include/freetype/config/ftmodule.h"
    "../../../include/freetype/config/ftoption.h"
    "../../../include/freetype/config/ftstdlib.h"
    "../../../include/ft2build.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "../../../src/autofit/autofit.c"
    "../../../src/base/ftbase.c"
    "../../../src/base/ftbitmap.c"
    "../../../src/base/ftfstype.c"
    "../../../src/base/ftgasp.c"
    "../../../src/base/ftglyph.c"
    "../../../src/base/ftinit.c"
    "../../../src/base/ftstroke.c"
    "../../../src/base/ftsystem.c"
    "../../../src/bdf/bdf.c"
    "../../../src/cache/ftcache.c"
    "../../../src/cff/cff.c"
    "../../../src/gzip/ftgzip.c"
    "../../../src/lzw/ftlzw.c"
    "../../../src/smooth/smooth.c"
    "../ftdebug.c"
)
source_group("Source Files" FILES ${Source_Files})

set(Source_Files__FT_MODULES
    "../../../src/base/ftbbox.c"
    "../../../src/base/ftgxval.c"
    "../../../src/base/ftlcdfil.c"
    "../../../src/base/ftmm.c"
    "../../../src/base/ftotval.c"
    "../../../src/base/ftpatent.c"
    "../../../src/base/ftpfr.c"
    "../../../src/base/ftsynth.c"
    "../../../src/base/fttype1.c"
    "../../../src/base/ftwinfnt.c"
    "../../../src/base/ftxf86.c"
    "../../../src/cid/type1cid.c"
    "../../../src/pcf/pcf.c"
    "../../../src/pfr/pfr.c"
    "../../../src/psaux/psaux.c"
    "../../../src/pshinter/pshinter.c"
    "../../../src/psnames/psmodule.c"
    "../../../src/raster/raster.c"
    "../../../src/sfnt/sfnt.c"
    "../../../src/truetype/truetype.c"
    "../../../src/type1/type1.c"
    "../../../src/type42/type42.c"
    "../../../src/winfonts/winfnt.c"
)
source_group("Source Files\\FT_MODULES" FILES ${Source_Files__FT_MODULES})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
    ${Source_Files__FT_MODULES}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "third")

# use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "freetype249_D"
        TARGET_NAME_EDITOR_DEBUG   "freetype249_D"
        TARGET_NAME_EDITOR_RELEASE "freetype249"
        TARGET_NAME_RELEASE        "freetype249"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "freetype249_D"
        TARGET_NAME_EDITOR_DEBUG   "freetype249_D"
        TARGET_NAME_EDITOR_RELEASE "freetype249"
        TARGET_NAME_RELEASE        "freetype249"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "freetype249_D"
        TARGET_NAME_EDITOR_DEBUG   "freetype249_D"
        TARGET_NAME_EDITOR_RELEASE "freetype249"
        TARGET_NAME_RELEASE        "freetype249"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG          "${CMAKE_CURRENT_SOURCE_DIR}/./../../../objs/win32/vc2010/"
        OUTPUT_DIRECTORY_EDITOR_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/./../../../objs/win32/vc2010/"
        OUTPUT_DIRECTORY_EDITOR_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/./../../../objs/win32/vc2010/"
        OUTPUT_DIRECTORY_RELEASE        "${CMAKE_CURRENT_SOURCE_DIR}/./../../../objs/win32/vc2010/"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG          "${CMAKE_CURRENT_SOURCE_DIR}/./../../../objs/win32/vc2010/"
        OUTPUT_DIRECTORY_EDITOR_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/./../../../objs/win32/vc2010/"
        OUTPUT_DIRECTORY_EDITOR_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/./../../../objs/win32/vc2010/"
        OUTPUT_DIRECTORY_RELEASE        "${CMAKE_CURRENT_SOURCE_DIR}/./../../../objs/win32/vc2010/"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Editor_Debug>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Editor_Release>:
            MultiThreadedDLL
        >
        $<$<CONFIG:Release>:
            MultiThreadedDLL
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Editor_Debug>,$<CONFIG:Editor_Release>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Editor_Debug>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Editor_Release>:
            MultiThreadedDLL
        >
        $<$<CONFIG:Release>:
            MultiThreadedDLL
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Editor_Debug>,$<CONFIG:Editor_Release>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Editor_Debug>:
            MultiThreadedDebugDLL
        >
        $<$<CONFIG:Editor_Release>:
            MultiThreadedDLL
        >
        $<$<CONFIG:Release>:
            MultiThreadedDLL
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Editor_Debug>,$<CONFIG:Editor_Release>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
if(MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})
endif()

################################################################################
# Include directories
################################################################################
if(NOT MSVC)
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../../include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "FT_DEBUG_LEVEL_ERROR;"
            "FT_DEBUG_LEVEL_TRACE"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "FT_DEBUG_LEVEL_ERROR;"
            "FT_DEBUG_LEVEL_TRACE"
        ">"
        "WIN32;"
        "_LIB;"
        "_CRT_SECURE_NO_WARNINGS;"
        "FT2_BUILD_LIBRARY;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "FT_DEBUG_LEVEL_ERROR;"
            "FT_DEBUG_LEVEL_TRACE"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "FT_DEBUG_LEVEL_ERROR;"
            "FT_DEBUG_LEVEL_TRACE"
        ">"
        "WIN32;"
        "_LIB;"
        "_CRT_SECURE_NO_WARNINGS;"
        "FT2_BUILD_LIBRARY;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "FT_DEBUG_LEVEL_ERROR;"
            "FT_DEBUG_LEVEL_TRACE"
        ">"
        "$<$<CONFIG:Editor_Debug>:"
            "FT_DEBUG_LEVEL_ERROR;"
            "FT_DEBUG_LEVEL_TRACE"
        ">"
        "WIN32;"
        "_LIB;"
        "_CRT_SECURE_NO_WARNINGS;"
        "FT2_BUILD_LIBRARY;"
        "_MBCS"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /Zi
            >
            $<$<CONFIG:Editor_Debug>:
                /Od;
                /RTC1;
                /Zi
            >
            $<$<CONFIG:Editor_Release>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            /MP;
            /W4;
            /wd4001;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /Zi
            >
            $<$<CONFIG:Editor_Debug>:
                /Od;
                /RTC1;
                /Zi
            >
            $<$<CONFIG:Editor_Release>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            /MP;
            /W4;
            /wd4001;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /Od;
                /RTC1;
                /Zi
            >
            $<$<CONFIG:Editor_Debug>:
                /Od;
                /RTC1;
                /Zi
            >
            $<$<CONFIG:Editor_Release>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            $<$<CONFIG:Release>:
                /O2;
                /Ob1;
                /GF;
                /Gy;
                ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT}
            >
            /MP;
            /W4;
            /wd4001;
            ${DEFAULT_CXX_EXCEPTION_HANDLING};
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/bdf/bdf.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/cff/cff.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/base/ftbase.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/cache/ftcache.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../ftdebug.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/base/ftglyph.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/gzip/ftgzip.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/base/ftinit.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/lzw/ftlzw.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/base/ftsystem.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/smooth/smooth.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/base/ftmm.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/pcf/pcf.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/pfr/pfr.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/psaux/psaux.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/pshinter/pshinter.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/psnames/psmodule.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/raster/raster.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/sfnt/sfnt.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/truetype/truetype.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/type1/type1.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/cid/type1cid.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/type42/type42.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        string(CONCAT FILE_CL_OPTIONS
            "$<$<CONFIG:Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Debug>:"
                "/Od;/RTC1"
            ">"
            "$<$<CONFIG:Editor_Release>:"
                "/O2"
            ">"
            "$<$<CONFIG:Release>:"
                "/O2"
            ">"
        )
    endif()
    source_file_compile_options(../../../src/winfonts/winfnt.c ${FILE_CL_OPTIONS})
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
        target_link_options(${PROJECT_NAME} PRIVATE
            /NOLOGO
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_link_options(${PROJECT_NAME} PRIVATE
            /NOLOGO
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            /NOLOGO
        )
    endif()
endif()









