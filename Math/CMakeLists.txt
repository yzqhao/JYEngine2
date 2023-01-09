# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
include(../CPlatform.txt)

set(PROJECT_NAME Math)

################################################################################
# Source groups
################################################################################
set(Source
    "3DMath.h"
    "AABB.cpp"
    "AABB.h"
    "Box2d.cpp"
    "Box2d.h"
    "CubicBezier.cpp"
    "CubicBezier.h"
    "Color.cpp"
    "Color.h"
    "Frustum.cpp"
    "Frustum.h"
    "IntRect.h"
    "IntRect.cpp"
    "IntVec2.cpp"
    "IntVec2.h"
    "IntVec3.cpp"
    "IntVec3.h"
    "IntVec4.cpp"
    "IntVec4.h"
    "Mat3.cpp"
    "Mat3.h"
    "Mat4.cpp"
    "Mat4.h"
    "Math.cpp"
    "Math.h"
    "MathUtil.cpp"
    "MathUtil.h"
	"MathUtil.inl"
    "Matrix4fArray.h"
    "Plane.cpp"
    "Plane.h"
    "Quaternion.cpp"
    "Quaternion.h"
    "Ray.cpp"
    "Ray.h"
    "Rect.cpp"
    "Rect.h"
    "Sphere.cpp"
    "Sphere.h"
    "Transform.h"
    "Transform.cpp"
    "Vec1.cpp"
    "Vec2.h"
    "Vec2.cpp"
    "Vec2.h"
    "Vec3.cpp"
    "Vec3.h"
    "Vec4.cpp"
    "Vec4.h"
    "VectorArray.h"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${Source}
)

add_compile_definitions (
    MATH_EXPORTS
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
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
	System
	)
target_link_libraries(${PROJECT_NAME} PUBLIC
    System
)	

################################################################################
# Compile and link options
################################################################################
include(../VSCompileLink.txt)

