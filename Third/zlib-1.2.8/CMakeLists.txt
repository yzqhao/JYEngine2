
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

project(zlib C)

set(VERSION "1.2.8")

option(ASM686 "Enable building i686 assembly implementation")
option(AMD64 "Enable building amd64 assembly implementation")

set(INSTALL_BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin" CACHE PATH "Installation directory for executables")
set(INSTALL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Installation directory for libraries")
set(INSTALL_INC_DIR "${CMAKE_INSTALL_PREFIX}/include" CACHE PATH "Installation directory for headers")
set(INSTALL_MAN_DIR "${CMAKE_INSTALL_PREFIX}/share/man" CACHE PATH "Installation directory for manual pages")
set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/share/pkgconfig" CACHE PATH "Installation directory for pkgconfig (.pc) files")

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)
enable_testing()

check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(stdint.h    HAVE_STDINT_H)
check_include_file(stddef.h    HAVE_STDDEF_H)

#
# Check to see if we have large file support
#
set(CMAKE_REQUIRED_DEFINITIONS -D_LARGEFILE64_SOURCE=1)
# We add these other definitions here because CheckTypeSize.cmake
# in CMake 2.4.x does not automatically do so and we want
# compatibility with CMake 2.4.x.
if(HAVE_SYS_TYPES_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_SYS_TYPES_H)
endif()
if(HAVE_STDINT_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDINT_H)
endif()
if(HAVE_STDDEF_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDDEF_H)
endif()
check_type_size(off64_t OFF64_T)
if(HAVE_OFF64_T)
   add_definitions(-D_LARGEFILE64_SOURCE=1)
endif()
set(CMAKE_REQUIRED_DEFINITIONS) # clear variable

#
# Check for fseeko
#
check_function_exists(fseeko HAVE_FSEEKO)
if(NOT HAVE_FSEEKO)
    add_definitions(-DNO_FSEEKO)
endif()

#
# Check for unistd.h
#
check_include_file(unistd.h Z_HAVE_UNISTD_H)

if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
    add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
    add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
endif()

if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    # If we're doing an out of source build and the user has a zconf.h
    # in their source tree...
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/zconf.h)
        message(STATUS "Renaming")
        message(STATUS "    ${CMAKE_CURRENT_SOURCE_DIR}/zconf.h")
        message(STATUS "to 'zconf.h.included' because this file is included with zlib")
        message(STATUS "but CMake generates it automatically in the build directory.")
        # file(RENAME ${CMAKE_CURRENT_SOURCE_DIR}/zconf.h ${CMAKE_CURRENT_SOURCE_DIR}/zconf.h.included)     # 这里注释掉
  endif()
endif()

set(ZLIB_PC ${CMAKE_CURRENT_BINARY_DIR}/zlib.pc)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/zlib.pc.cmakein
		${ZLIB_PC} @ONLY)
configure_file(	${CMAKE_CURRENT_SOURCE_DIR}/zconf.h.cmakein
		${CMAKE_CURRENT_BINARY_DIR}/zconf.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})


#============================================================================
# zlib
#============================================================================

set(ZLIB_PUBLIC_HDRS
    ${CMAKE_CURRENT_BINARY_DIR}/zconf.h
    zlib.h
)
set(ZLIB_PRIVATE_HDRS
    crc32.h
    deflate.h
    gzguts.h
    inffast.h
    inffixed.h
    inflate.h
    inftrees.h
    trees.h
    zutil.h
)
set(ZLIB_SRCS
    adler32.c
    compress.c
    crc32.c
    deflate.c
    gzclose.c
    gzlib.c
    gzread.c
    gzwrite.c
    inflate.c
    infback.c
    inftrees.c
    inffast.c
    trees.c
    uncompr.c
    zutil.c
)

if(NOT MINGW)
    set(ZLIB_DLL_SRCS
        win32/zlib1.rc # If present will override custom build rule below.
    )
endif()

if(CMAKE_COMPILER_IS_GNUCC)
    if(ASM686)
        set(ZLIB_ASMS contrib/asm686/match.S)
    elseif (AMD64)
        set(ZLIB_ASMS contrib/amd64/amd64-match.S)
    endif ()

	if(ZLIB_ASMS)
		add_definitions(-DASMV)
		set_source_files_properties(${ZLIB_ASMS} PROPERTIES LANGUAGE C COMPILE_FLAGS -DNO_UNDERLINE)
	endif()
endif()

if(MSVC)
    if(ASM686)
		ENABLE_LANGUAGE(ASM_MASM)
        set(ZLIB_ASMS
			contrib/masmx86/inffas32.asm
			contrib/masmx86/match686.asm
		)
    elseif (AMD64)
		ENABLE_LANGUAGE(ASM_MASM)
        set(ZLIB_ASMS
			contrib/masmx64/gvmat64.asm
			contrib/masmx64/inffasx64.asm
		)
    endif()

	if(ZLIB_ASMS)
		add_definitions(-DASMV -DASMINF)
	endif()
endif()

# parse the full version number from zlib.h and include in ZLIB_FULL_VERSION
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/zlib.h _zlib_h_contents)
string(REGEX REPLACE ".*#define[ \t]+ZLIB_VERSION[ \t]+\"([-0-9A-Za-z.]+)\".*"
    "\\1" ZLIB_FULL_VERSION ${_zlib_h_contents})

if(MINGW)
    # This gets us DLL resource information when compiling on MinGW.
    if(NOT CMAKE_RC_COMPILER)
        set(CMAKE_RC_COMPILER windres.exe)
    endif()

    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/zlib1rc.obj
                       COMMAND ${CMAKE_RC_COMPILER}
                            -D GCC_WINDRES
                            -I ${CMAKE_CURRENT_SOURCE_DIR}
                            -I ${CMAKE_CURRENT_BINARY_DIR}
                            -o ${CMAKE_CURRENT_BINARY_DIR}/zlib1rc.obj
                            -i ${CMAKE_CURRENT_SOURCE_DIR}/win32/zlib1.rc)
    set(ZLIB_DLL_SRCS ${CMAKE_CURRENT_BINARY_DIR}/zlib1rc.obj)
endif(MINGW)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

add_library(zlib SHARED ${ZLIB_SRCS} ${ZLIB_ASMS} ${ZLIB_DLL_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
add_library(zlibstatic STATIC ${ZLIB_SRCS} ${ZLIB_ASMS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})
set_target_properties(zlib PROPERTIES DEFINE_SYMBOL ZLIB_DLL)
set_target_properties(zlib PROPERTIES SOVERSION 1)

set_target_properties(zlib PROPERTIES FOLDER "Third")
set_target_properties(zlibstatic PROPERTIES FOLDER "Third")

if(NOT CYGWIN)
    # This property causes shared libraries on Linux to have the full version
    # encoded into their final filename.  We disable this on Cygwin because
    # it causes cygz-${ZLIB_FULL_VERSION}.dll to be created when cygz.dll
    # seems to be the default.
    #
    # This has no effect with MSVC, on that platform the version info for
    # the DLL comes from the resource file win32/zlib1.rc
    set_target_properties(zlib PROPERTIES VERSION ${ZLIB_FULL_VERSION})
endif()

if(UNIX)
    # On unix-like platforms the library is almost always called libz
   set_target_properties(zlib zlibstatic PROPERTIES OUTPUT_NAME zlib)
   if(NOT APPLE)
     set_target_properties(zlib PROPERTIES LINK_FLAGS "-Wl,--version-script,\"${CMAKE_CURRENT_SOURCE_DIR}/zlib.map\"")
   endif()
elseif(BUILD_SHARED_LIBS AND WIN32)
    # Creates zlib1.dll when building shared library version
    set_target_properties(zlib PROPERTIES SUFFIX "1.dll")
endif()

if(NOT SKIP_INSTALL_LIBRARIES AND NOT SKIP_INSTALL_ALL )
    install(TARGETS zlib zlibstatic
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" )
endif()
if(NOT SKIP_INSTALL_HEADERS AND NOT SKIP_INSTALL_ALL )
    install(FILES ${ZLIB_PUBLIC_HDRS} DESTINATION "${INSTALL_INC_DIR}")
endif()
if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL )
    install(FILES zlib.3 DESTINATION "${INSTALL_MAN_DIR}/man3")
endif()
if(NOT SKIP_INSTALL_FILES AND NOT SKIP_INSTALL_ALL )
    install(FILES ${ZLIB_PC} DESTINATION "${INSTALL_PKGCONFIG_DIR}")
endif()
























# include(../../build/cmake/CPlatform.txt)

# set(PROJECT_NAME zlib)

# ################################################################################
# # Source groups
# ################################################################################
# set(Headers
#     "crc32.h"
#     "deflate.h"
#     "gzguts.h"
#     "inffast.h"
#     "inffixed.h"
#     "inflate.h"
#     "inftrees.h"
#     "trees.h"
#     "zlib.h"
#     "zutil.h"
# )
# source_group("Headers" FILES ${Headers})

# set(Sources
#     "adler32.c"
#     "compress.c"
#     "crc32.c"
#     "deflate.c"
#     "gzclose.c"
#     "gzlib.c"
#     "gzread.c"
#     "gzwrite.c"
#     "infback.c"
#     "inffast.c"
#     "inflate.c"
#     "inftrees.c"
#     "trees.c"
#     "uncompr.c"
#     "zutil.c"
# )
# source_group("Sources" FILES ${Sources})

# set(ALL_FILES
#     ${Headers}
#     ${Sources}
# )

# ################################################################################
# # Target
# ################################################################################
# add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
# set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "third/image")

# # use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
# set(ROOT_NAMESPACE zlib)

# set_target_properties(${PROJECT_NAME} PROPERTIES
#     VS_GLOBAL_KEYWORD "Win32Proj"
# )
# ################################################################################
# # MSVC runtime library
# ################################################################################
# get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
# if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
#     string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
#         $<$<CONFIG:Debug>:
#             MultiThreadedDebugDLL
#         >
#         $<$<CONFIG:Editor_Debug>:
#             MultiThreadedDebugDLL
#         >
#         $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Editor_Debug>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
#     )
# elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
#     string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
#         $<$<CONFIG:Debug>:
#             MultiThreadedDebugDLL
#         >
#         $<$<CONFIG:Editor_Debug>:
#             MultiThreadedDebugDLL
#         >
#         $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Editor_Debug>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
#     )
# elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
#     string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
#         $<$<CONFIG:Debug>:
#             MultiThreadedDebugDLL
#         >
#         $<$<CONFIG:Editor_Debug>:
#             MultiThreadedDebugDLL
#         >
#         $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Editor_Debug>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
#     )
# endif()
# if(MSVC)
# 	set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})
# endif()

# ################################################################################
# # Compile definitions
# ################################################################################
# if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
#     target_compile_definitions(${PROJECT_NAME} PRIVATE
#         "WIN32;"
#         "_WINDOWS"
#     )
# elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
#     target_compile_definitions(${PROJECT_NAME} PRIVATE
#         "WIN32;"
#         "_WINDOWS"
#     )
# elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
#     target_compile_definitions(${PROJECT_NAME} PRIVATE
#         "WIN32;"
#         "_WINDOWS"
#     )
# endif()

# ################################################################################
# # Compile and link options
# ################################################################################
# if(MSVC)
#     if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
#         target_compile_options(${PROJECT_NAME} PRIVATE
#             $<$<CONFIG:Debug>:
#                 /MP;
#                 /Od
#             >
#             $<$<CONFIG:Editor_Debug>:
#                 /Od
#             >
#             $<$<CONFIG:Editor_Release>:
#                 /GL;
#                 /Ox;
#                 /Oi;
#                 /GS-
#             >
#             $<$<CONFIG:Release>:
#                 /MP;
#                 /GL;
#                 /Ox;
#                 /Oi;
#                 /GS-
#             >
#             /Gy;
#             /Zi;
#             /wd$(DisableSpecificWarnings);
#             /wd4127;
#             /wd4131;
#             /wd4242;
#             /wd4244;
#             ${DEFAULT_CXX_EXCEPTION_HANDLING}
#         )
#     elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
#         target_compile_options(${PROJECT_NAME} PRIVATE
#             $<$<CONFIG:Debug>:
#                 /MP;
#                 /Od
#             >
#             $<$<CONFIG:Editor_Debug>:
#                 /Od
#             >
#             $<$<CONFIG:Editor_Release>:
#                 /GL;
#                 /Ox;
#                 /Oi;
#                 /GS-
#             >
#             $<$<CONFIG:Release>:
#                 /MP;
#                 /GL;
#                 /Ox;
#                 /Oi;
#                 /GS-
#             >
#             /Gy;
#             /Zi;
#             /wd$(DisableSpecificWarnings);
#             /wd4127;
#             /wd4131;
#             /wd4242;
#             /wd4244;
#             ${DEFAULT_CXX_EXCEPTION_HANDLING}
#         )
#     elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
#         target_compile_options(${PROJECT_NAME} PRIVATE
#             $<$<CONFIG:Debug>:
#                 /MP;
#                 /Od
#             >
#             $<$<CONFIG:Editor_Debug>:
#                 /Od
#             >
#             $<$<CONFIG:Editor_Release>:
#                 /GL;
#                 /Ox;
#                 /Oi;
#                 /GS-
#             >
#             $<$<CONFIG:Release>:
#                 /MP;
#                 /GL;
#                 /Ox;
#                 /Oi;
#                 /GS-
#             >
#             /Gy;
#             /Zi;
#             /wd$(DisableSpecificWarnings);
#             /wd4127;
#             /wd4131;
#             /wd4242;
#             /wd4244;
#             ${DEFAULT_CXX_EXCEPTION_HANDLING}
#         )
#     endif()
#     if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
#         target_link_options(${PROJECT_NAME} PRIVATE
#             $<$<CONFIG:Editor_Release>:
#                 /OPT:REF;
#                 /OPT:ICF
#             >
#             $<$<CONFIG:Release>:
#                 /OPT:REF;
#                 /OPT:ICF
#             >
#             /MACHINE:X86;
#             /SUBSYSTEM:WINDOWS
#         )
#     elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
#         target_link_options(${PROJECT_NAME} PRIVATE
#             $<$<CONFIG:Editor_Release>:
#                 /OPT:REF;
#                 /OPT:ICF
#             >
#             $<$<CONFIG:Release>:
#                 /OPT:REF;
#                 /OPT:ICF
#             >
#             /MACHINE:X86;
#             /SUBSYSTEM:WINDOWS
#         )
#     elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
#         target_link_options(${PROJECT_NAME} PRIVATE
#             $<$<CONFIG:Editor_Release>:
#                 /OPT:REF;
#                 /OPT:ICF
#             >
#             $<$<CONFIG:Release>:
#                 /OPT:REF;
#                 /OPT:ICF
#             >
#             /SUBSYSTEM:WINDOWS
#         )
#     endif()
# endif()












