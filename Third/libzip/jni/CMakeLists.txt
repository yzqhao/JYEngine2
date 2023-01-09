
include(../../../build/cmake/CPlatform.txt)

set ( LOCAL_PATH ${CMAKE_CURRENT_SOURCE_DIR} )

set ( SRC_PATH ${LOCAL_PATH}/../source )

set( LOCAL_SRC_FILES
    ${SRC_PATH}/zip_add.c
    ${SRC_PATH}/zip_add_dir.c
    ${SRC_PATH}/zip_add_entry.c
    ${SRC_PATH}/zip_algorithm_deflate.c
    ${SRC_PATH}/zip_buffer.c
    ${SRC_PATH}/zip_close.c
    ${SRC_PATH}/zip_delete.c
    ${SRC_PATH}/zip_dirent.c
    ${SRC_PATH}/zip_dir_add.c
    ${SRC_PATH}/zip_discard.c
    ${SRC_PATH}/zip_entry.c
    ${SRC_PATH}/zip_error.c
    ${SRC_PATH}/zip_error_clear.c
    ${SRC_PATH}/zip_error_get.c
    ${SRC_PATH}/zip_error_get_sys_type.c
    ${SRC_PATH}/zip_error_strerror.c
    ${SRC_PATH}/zip_error_to_str.c
    ${SRC_PATH}/zip_err_str.c
    ${SRC_PATH}/zip_extra_field.c
    ${SRC_PATH}/zip_extra_field_api.c
    ${SRC_PATH}/zip_fclose.c
    ${SRC_PATH}/zip_fdopen.c
    ${SRC_PATH}/zip_file_add.c
    ${SRC_PATH}/zip_file_error_clear.c
    ${SRC_PATH}/zip_file_error_get.c
    ${SRC_PATH}/zip_file_get_comment.c
    ${SRC_PATH}/zip_file_get_external_attributes.c
    ${SRC_PATH}/zip_file_get_offset.c
    ${SRC_PATH}/zip_file_rename.c
    ${SRC_PATH}/zip_file_replace.c
    ${SRC_PATH}/zip_file_set_comment.c
    ${SRC_PATH}/zip_file_set_encryption.c
    ${SRC_PATH}/zip_file_set_external_attributes.c
    ${SRC_PATH}/zip_file_set_mtime.c
    ${SRC_PATH}/zip_file_strerror.c
    ${SRC_PATH}/zip_fopen.c
    ${SRC_PATH}/zip_fopen_encrypted.c
    ${SRC_PATH}/zip_fopen_index.c
    ${SRC_PATH}/zip_fopen_index_encrypted.c
    ${SRC_PATH}/zip_fread.c
    ${SRC_PATH}/zip_fseek.c
    ${SRC_PATH}/zip_ftell.c
    ${SRC_PATH}/zip_get_archive_comment.c
    ${SRC_PATH}/zip_get_archive_flag.c
    ${SRC_PATH}/zip_get_encryption_implementation.c
    ${SRC_PATH}/zip_get_file_comment.c
    ${SRC_PATH}/zip_get_name.c
    ${SRC_PATH}/zip_get_num_entries.c
    ${SRC_PATH}/zip_get_num_files.c
    ${SRC_PATH}/zip_hash.c
    ${SRC_PATH}/zip_io_util.c
    ${SRC_PATH}/zip_libzip_version.c
    ${SRC_PATH}/zip_memdup.c
    ${SRC_PATH}/zip_name_locate.c
    ${SRC_PATH}/zip_new.c
    ${SRC_PATH}/zip_open.c
    ${SRC_PATH}/zip_pkware.c
    ${SRC_PATH}/zip_progress.c
    ${SRC_PATH}/zip_rename.c
    ${SRC_PATH}/zip_replace.c
    ${SRC_PATH}/zip_set_archive_comment.c
    ${SRC_PATH}/zip_set_archive_flag.c
    ${SRC_PATH}/zip_set_default_password.c
    ${SRC_PATH}/zip_set_file_comment.c
    ${SRC_PATH}/zip_set_file_compression.c
    ${SRC_PATH}/zip_set_name.c
    ${SRC_PATH}/zip_source_accept_empty.c
    ${SRC_PATH}/zip_source_begin_write.c
    ${SRC_PATH}/zip_source_begin_write_cloning.c
    ${SRC_PATH}/zip_source_buffer.c
    ${SRC_PATH}/zip_source_call.c
    ${SRC_PATH}/zip_source_close.c
    ${SRC_PATH}/zip_source_commit_write.c
    ${SRC_PATH}/zip_source_compress.c
    ${SRC_PATH}/zip_source_crc.c
    ${SRC_PATH}/zip_source_error.c
    ${SRC_PATH}/zip_source_file_common.c
    ${SRC_PATH}/zip_source_file_stdio.c
	${SRC_PATH}/zip_source_file_stdio_named.c
    ${SRC_PATH}/zip_source_free.c
    ${SRC_PATH}/zip_source_function.c
    ${SRC_PATH}/zip_source_get_file_attributes.c
    ${SRC_PATH}/zip_source_is_deleted.c
    ${SRC_PATH}/zip_source_layered.c
    ${SRC_PATH}/zip_source_open.c
    ${SRC_PATH}/zip_source_pkware_decode.c
    ${SRC_PATH}/zip_source_pkware_encode.c
    ${SRC_PATH}/zip_source_read.c
    ${SRC_PATH}/zip_source_remove.c
    ${SRC_PATH}/zip_source_rollback_write.c
    ${SRC_PATH}/zip_source_seek.c
    ${SRC_PATH}/zip_source_seek_write.c
    ${SRC_PATH}/zip_source_stat.c
    ${SRC_PATH}/zip_source_supports.c
    ${SRC_PATH}/zip_source_tell.c
    ${SRC_PATH}/zip_source_tell_write.c
    ${SRC_PATH}/zip_source_window.c
    ${SRC_PATH}/zip_source_write.c
    ${SRC_PATH}/zip_source_zip.c
    ${SRC_PATH}/zip_source_zip_new.c
    ${SRC_PATH}/zip_stat.c
    ${SRC_PATH}/zip_stat_index.c
    ${SRC_PATH}/zip_stat_init.c
    ${SRC_PATH}/zip_strerror.c
    ${SRC_PATH}/zip_string.c
    ${SRC_PATH}/zip_unchange.c
    ${SRC_PATH}/zip_unchange_all.c
    ${SRC_PATH}/zip_unchange_archive.c
    ${SRC_PATH}/zip_unchange_data.c
    ${SRC_PATH}/zip_utf-8.c    
    ${SRC_PATH}/zip_random_unix.c )
    
if (NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set( LOCAL_SRC_FILES 
        ${LOCAL_SRC_FILES}
        ${SRC_PATH}/zip_mkstempm.c
    )
endif()

add_library(zip ${VENUS_LIBRARY_TYPE} ${LOCAL_SRC_FILES})

if(ENGINE_DEBUG)
	target_compile_options(zip PRIVATE -O0 -g -D_DEBUG)
else()
	target_compile_options(zip PRIVATE -O3 -DNDEBUG)
endif()

target_compile_options(zip PRIVATE -fPIC -D${PLATFORM} -D${PLATFORM_LONG})
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_compile_options(zip PRIVATE -DZIP_STATIC -DWIN32 -D_WINDOWS -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
else()
    target_compile_options(zip PRIVATE -DHAVE_UNISTD_H -DHAVE_STRINGS_H -DHAVE_STRCASECMP -DHAVE_FSEEKO -DHAVE_FTELLO -DHAVE_STRICMP)
endif()

#MESSAGE( "zip LOCAL_PATH = ${LOCAL_PATH}")
set( ZIP_INC ${LOCAL_PATH}/.. CACHE INTERNAL "DIRS" )
include_directories(${ZIP_INC}
	${LOCAL_PATH}/..
    ${LOCAL_PATH}/../3rdparty
    ${LOCAL_PATH}/../include
    ${LOCAL_PATH}/../src
    ${LOCAL_PATH}/../../../core/include
    ${LOCAL_PATH}/../../../function/apolloengine/include
    ${LOCAL_PATH}/../../../function/mathfunction
    ${LOCAL_PATH}/../../../interface/venus
    ${LOCAL_PATH}/../../../jni/sharedcontext/android/include
    ${LOCAL_PATH}/../../../third/concurrentqueue
    ${LOCAL_PATH}/../../../third/zlib-1.2.8
	)

if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(zip     
            z
        )
endif()

target_link_libraries(zip
	)