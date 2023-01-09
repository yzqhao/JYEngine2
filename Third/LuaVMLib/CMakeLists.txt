# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../../CPlatform.txt)

set(PROJECT_NAME lua)

################################################################################
# Source groups
################################################################################

file(
    GLOB ALL_FILES
    "./*.h"
    "./lapi.c"
    "./lauxlib.c"
    "./lbaselib.c"
    "./lbitlib.c"
    "./lcode.c"
    "./lcorolib.c"
    "./lctype.c"
    "./ldblib.c"
    "./ldebug.c"
    "./ldo.c"
    "./ldump.c"
    "./lfunc.c"
    "./lgc.c"
    "./linit.c"
    "./liolib.c"
    "./llex.c"
    "./lmathlib.c"
    "./lmem.c"
    "./loadlib.c"
    "./lobject.c"
    "./lopcodes.c"
    "./loslib.c"
    "./lparser.c"
    "./lstate.c"
    "./lstring.c"
    "./lstrlib.c"
    "./ltable.c"
    "./ltablib.c"
    "./ltm.c"
    "./lundump.c"
    "./lutf8lib.c"
    "./lvm.c"
    "./lzio.c"
)

add_compile_definitions (
	LUA_BUILD_AS_DLL
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} ${LIBRARY_TYPE_BY_PLATFORM} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Third/Script")
	
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "${PROJECT_NAME}_d"
        TARGET_NAME_EDITOR_DEBUG   "${PROJECT_NAME}_d"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
	target_include_directories(${PROJECT_NAME} PRIVATE
		"${ROOT_PATH}/Third/bx/include/compat/msvc"
	)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "${PROJECT_NAME}_d"
        TARGET_NAME_EDITOR_DEBUG   "${PROJECT_NAME}_d"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
	target_include_directories(${PROJECT_NAME} PRIVATE
		"${ROOT_PATH}/Third/bx/include/compat/msvc"
	)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "${PROJECT_NAME}_d"
        TARGET_NAME_EDITOR_DEBUG   "${PROJECT_NAME}_d"
    )
endif()

################################################################################
# Compile and link options
################################################################################
include(../../VSCompileLink.txt)
