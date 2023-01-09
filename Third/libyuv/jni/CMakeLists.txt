
include(../../../CPlatform.txt)

set ( CMAKE_ANDROID_ARM_NEON ON )

set ( LOCAL_PATH ${CMAKE_CURRENT_SOURCE_DIR} )

set ( src_path ${LOCAL_PATH}/../source )

set(PROJECT_NAME yuv_static)

set( LOCAL_SRC_FILES
    ${src_path}/compare.cc          
    ${src_path}/compare_common.cc   
    ${src_path}/compare_gcc.cc      
    ${src_path}/compare_mmi.cc      
    ${src_path}/compare_msa.cc      
    ${src_path}/compare_neon64.cc   
    ${src_path}/convert.cc          
    ${src_path}/convert_argb.cc     
    ${src_path}/convert_from.cc     
    ${src_path}/convert_from_argb.cc
    ${src_path}/convert_to_argb.cc  
    ${src_path}/convert_to_i420.cc  
    ${src_path}/cpu_id.cc           
    ${src_path}/planar_functions.cc 
    ${src_path}/rotate.cc           
    ${src_path}/rotate_any.cc       
    ${src_path}/rotate_argb.cc      
    ${src_path}/rotate_common.cc    
    ${src_path}/rotate_gcc.cc       
    ${src_path}/rotate_mmi.cc       
    ${src_path}/rotate_msa.cc       
    ${src_path}/rotate_neon64.cc    
    ${src_path}/row_any.cc          
    ${src_path}/row_common.cc       
    ${src_path}/row_gcc.cc          
    ${src_path}/row_mmi.cc          
    ${src_path}/row_msa.cc          
    ${src_path}/row_neon64.cc       
    ${src_path}/scale.cc            
    ${src_path}/scale_any.cc        
    ${src_path}/scale_argb.cc       
    ${src_path}/scale_common.cc     
    ${src_path}/scale_gcc.cc        
    ${src_path}/scale_mmi.cc        
    ${src_path}/scale_msa.cc        
    ${src_path}/scale_neon64.cc     
    ${src_path}/video_common.cc )

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(LOCAL_SRC_FILES ${LOCAL_SRC_FILES}
    ${src_path}/row_win.cc
    ${src_path}/scale_win.cc
    )
endif()

if (($(TARGET_ARCH_ABI) STREQUAL armeabi-v7a) OR (CMAKE_OSX_ARCHITECTURES MATCHES ".*armv7.*"))
    add_definitions(-DLIBYUV_NEON)
    set (LOCAL_SRC_FILES
        ${LOCAL_SRC_FILES}
        ${src_path}/compare_neon.cc   
        ${src_path}/rotate_neon.cc    
        ${src_path}/row_neon.cc       
        ${src_path}/scale_neon.cc
    )
endif()

add_library(yuv_static STATIC ${LOCAL_SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Third")

IF (CMAKE_BUILD_TYPE STREQUAL Debug)
	target_compile_options(yuv_static PRIVATE -O0 -g -D_DEBUG)
else()
	target_compile_options(yuv_static PRIVATE -O3 -DNDEBUG)
endif()

target_compile_options(yuv_static PRIVATE -fPIC -Wall -fexceptions -D${PLATFORM} -D${PLATFORM_LONG})

#MESSAGE( "yuv_static LOCAL_PATH = ${LOCAL_PATH}")
include_directories(${PROJECT_NAME} PRIVATE
    ${ROOT_PATH}/Third/libyuv/include
	)
target_link_libraries(yuv_static     
	)

target_link_libraries(yuv_static
	)

################################################################################
# Compile and link options
################################################################################
include(../../../VSCompileLink.txt)