# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../CPlatform.txt)

set(PROJECT_NAME Core)

################################################################################
# Source groups
################################################################################
set(__Interface__Script
    "Interface/Script/IFunctionInvoker.h"
    "Interface/Script/IVirtualMachine.h"
    "Interface/Script/IVirtualMachine.cpp"
    "Interface/Script/IScriptInstance.h"
    "Interface/Script/IScriptInstance.cpp"
)
source_group("Interface\\Script" FILES ${__Interface__Script})

set(__Interface__Serialize
    "Interface/Serialize/DataType.h"
    "Interface/Serialize/IDecoder.h"
    "Interface/Serialize/IEncoder.h"
    "Interface/Serialize/ISerializer.h"
    "Interface/Serialize/ISerializeSystem.h"
    "Interface/Serialize/ISerializeSystem.cpp"
)
source_group("Interface\\Serialize" FILES ${__Interface__Serialize})

set(__Interface
    "Interface/IConfig.h"
    "Interface/IConfigSystem.h"
    "Interface/IConfigSystem.cpp"
    "Interface/IFile.h"
    "Interface/IFileSystem.h"
    "Interface/IFileSystem.cpp"
    "Interface/ILogSystem.h"
    "Interface/ILogSystem.cpp"
    "Interface/ILoader.h"
    "Interface/IResourceSystem.h"
    "Interface/IResourceSystem.cpp"
    "Interface/IWindow.h"
    "Interface/IWindow.cpp"
    "Interface/IApplication.h"
    "Interface/IApplication.cpp"
    "Interface/IModuleInterface.h"
    "Interface/IModuleManager.h"
    "Interface/IModuleManager.cpp"
    "Interface/IScriptSystem.h"
    "Interface/IScriptSystem.cpp"
    "Interface/ITimeSystem.h"
    "Interface/ITimeSystem.cpp"
)
source_group("Interface" FILES ${__Interface})

set(__Propery
    "Propery/InputPropery.h"
    "Propery/ResourceProperty.h"
    "Propery/TextureProperty.h"
    "Propery/TouchInput.h"
)
source_group("Propery" FILES ${__Propery})

set(Source
    "Configure.h"
    "Core.h"
    "EnableIf.h"
    "EnumClassFlags.h"
    "EnumTrans.cpp"
    "EnumTrans.h"
    "Main.cpp"
    "Main.h"
    "MainMacro.h"
    "Nonclearvector.hpp"
    "Object.cpp"
    "Object.h"
    "Pointer.h"
    "Priority.cpp"
    "Priority.h"
    "Property.h"
    "PropertyMacro.h"
    "Reference.h"
    "Rtti.cpp"
    "Rtti.h"
    "RttiMacro.h"
    "RttiManager.cpp"
    "RttiManager.h"
    "Stream.cpp"
    "Stream.h"
    "StringHash.cpp"
    "StringHash.h"
    "Type.h"
    "TEnumAsByte.h"
    "Resource.cpp"
    "Resource.h"
    "ResourceArray.h"
    "ThreadSafeCounter.h"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${__Interface__Script}
    ${__Interface__Serialize}
    ${__Interface}
    ${__Propery}
    ${Source}
)

add_compile_definitions (
    CORE_EXPORTS
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} ${LIBRARY_TYPE_BY_PLATFORM} ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Engine")
	
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
)

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	System
	Math
	)
target_link_libraries(${PROJECT_NAME} PUBLIC
    System
	Math
)	

################################################################################
# Compile and link options
################################################################################
include(../VSCompileLink.txt)
	