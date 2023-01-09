include(../../CPlatform.txt)

set ( LOCAL_PATH ${CMAKE_CURRENT_SOURCE_DIR} )
set ( SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libexif )

set(PROJECT_NAME libexif)

set( LOCAL_SRC_FILES
    ${SRC_PATH}/exif-byte-order.c
    ${SRC_PATH}/exif-content.c
    ${SRC_PATH}/exif-data.c
    ${SRC_PATH}/exif-entry.c
    ${SRC_PATH}/exif-format.c
    ${SRC_PATH}/exif-ifd.c
    ${SRC_PATH}/exif-loader.c
    ${SRC_PATH}/exif-log.c
    ${SRC_PATH}/exif-mem.c
    ${SRC_PATH}/exif-mnote-data.c
    ${SRC_PATH}/exif-tag.c
    ${SRC_PATH}/exif-utils.c
    ${SRC_PATH}/canon/exif-mnote-data-canon.c
    ${SRC_PATH}/canon/mnote-canon-entry.c
    ${SRC_PATH}/canon/mnote-canon-tag.c
    ${SRC_PATH}/olympus/exif-mnote-data-olympus.c
    ${SRC_PATH}/olympus/mnote-olympus-tag.c
    ${SRC_PATH}/olympus/mnote-olympus-entry.c
    ${SRC_PATH}/fuji/exif-mnote-data-fuji.c
    ${SRC_PATH}/fuji/mnote-fuji-entry.c
    ${SRC_PATH}/fuji/mnote-fuji-tag.c
    ${SRC_PATH}/pentax/exif-mnote-data-pentax.c
    ${SRC_PATH}/pentax/mnote-pentax-entry.c
    ${SRC_PATH}/pentax/mnote-pentax-tag.c
)

add_library(${PROJECT_NAME} STATIC ${LOCAL_SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Third")

IF (CMAKE_BUILD_TYPE STREQUAL Debug)
	target_compile_options(${PROJECT_NAME} PRIVATE -O0 -g)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -O3)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -fPIC -D${PLATFORM} -D${PLATFORM_LONG})

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /GL)
endif()

#MESSAGE( "exif LOCAL_PATH = ${LOCAL_PATH}")
set( EXIF_INC ${LOCAL_PATH} CACHE INTERNAL "DIRS" )
include_directories(${EXIF_INC}
)
target_link_libraries(${PROJECT_NAME} 
)

################################################################################
# Compile and link options
################################################################################
include(../../VSCompileLink.txt)
