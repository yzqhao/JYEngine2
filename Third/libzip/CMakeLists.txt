include(../../CPlatform.txt)

set(PROJECT_NAME libzip)

################################################################################
# Source groups
################################################################################
set(Src
    "source/zip_add.c"
    "source/zip_add_dir.c"
    "source/zip_add_entry.c"
    "source/zip_algorithm_deflate.c"
    "source/zip_buffer.c"
    "source/zip_close.c"
    "source/zip_crypto_win.c"
    "source/zip_delete.c"
    "source/zip_dir_add.c"
    "source/zip_dirent.c"
    "source/zip_discard.c"
    "source/zip_entry.c"
    "source/zip_err_str.c"
    "source/zip_error.c"
    "source/zip_error_clear.c"
    "source/zip_error_get.c"
    "source/zip_error_get_sys_type.c"
    "source/zip_error_strerror.c"
    "source/zip_error_to_str.c"
    "source/zip_extra_field.c"
    "source/zip_extra_field_api.c"
    "source/zip_fclose.c"
    "source/zip_fdopen.c"
    "source/zip_file_add.c"
    "source/zip_file_error_clear.c"
    "source/zip_file_error_get.c"
    "source/zip_file_get_comment.c"
    "source/zip_file_get_external_attributes.c"
    "source/zip_file_get_offset.c"
    "source/zip_file_rename.c"
    "source/zip_file_replace.c"
    "source/zip_file_set_comment.c"
    "source/zip_file_set_encryption.c"
    "source/zip_file_set_external_attributes.c"
    "source/zip_file_set_mtime.c"
    "source/zip_file_strerror.c"
    "source/zip_fopen.c"
    "source/zip_fopen_encrypted.c"
    "source/zip_fopen_index.c"
    "source/zip_fopen_index_encrypted.c"
    "source/zip_fread.c"
    "source/zip_fseek.c"
    "source/zip_ftell.c"
    "source/zip_get_archive_comment.c"
    "source/zip_get_archive_flag.c"
    "source/zip_get_encryption_implementation.c"
    "source/zip_get_file_comment.c"
    "source/zip_get_name.c"
    "source/zip_get_num_entries.c"
    "source/zip_get_num_files.c"
    "source/zip_hash.c"
    "source/zip_io_util.c"
    "source/zip_libzip_version.c"
    "source/zip_memdup.c"
    "source/zip_name_locate.c"
    "source/zip_new.c"
    "source/zip_open.c"
    "source/zip_pkware.c"
    "source/zip_progress.c"
    "source/zip_rename.c"
    "source/zip_replace.c"
    "source/zip_set_archive_comment.c"
    "source/zip_set_archive_flag.c"
    "source/zip_set_default_password.c"
    "source/zip_set_file_comment.c"
    "source/zip_set_file_compression.c"
    "source/zip_set_name.c"
    "source/zip_source_accept_empty.c"
    "source/zip_source_begin_write.c"
    "source/zip_source_begin_write_cloning.c"
    "source/zip_source_buffer.c"
    "source/zip_source_call.c"
    "source/zip_source_close.c"
    "source/zip_source_commit_write.c"
    "source/zip_source_compress.c"
    "source/zip_source_crc.c"
    "source/zip_source_error.c"
    "source/zip_source_file_common.c"
    "source/zip_source_file_stdio.c"
    "source/zip_source_file_win32.c"
    "source/zip_source_file_win32_ansi.c"
    "source/zip_source_file_win32_named.c"
    "source/zip_source_file_win32_utf16.c"
    "source/zip_source_file_win32_utf8.c"
    "source/zip_source_free.c"
    "source/zip_source_function.c"
    "source/zip_source_get_file_attributes.c"
    "source/zip_source_is_deleted.c"
    "source/zip_source_layered.c"
    "source/zip_source_open.c"
    "source/zip_source_pkware_decode.c"
    "source/zip_source_pkware_encode.c"
    "source/zip_source_read.c"
    "source/zip_source_remove.c"
    "source/zip_source_rollback_write.c"
    "source/zip_source_seek.c"
    "source/zip_source_seek_write.c"
    "source/zip_source_stat.c"
    "source/zip_source_supports.c"
    "source/zip_source_tell.c"
    "source/zip_source_tell_write.c"
    "source/zip_source_window.c"
    "source/zip_source_winzip_aes_decode.c"
    "source/zip_source_winzip_aes_encode.c"
    "source/zip_source_write.c"
    "source/zip_source_zip.c"
    "source/zip_source_zip_new.c"
    "source/zip_stat.c"
    "source/zip_stat_index.c"
    "source/zip_stat_init.c"
    "source/zip_strerror.c"
    "source/zip_string.c"
    "source/zip_unchange.c"
    "source/zip_unchange_all.c"
    "source/zip_unchange_archive.c"
    "source/zip_unchange_data.c"
    "source/zip_utf-8.c"
    "source/zip_winzip_aes.c"
)
source_group("Source Files" FILES ${Src})

set(ALL_FILES
    ${Src}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_DEBUG ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${ROOT_PATH}/bin/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_EDITOR_RELEASE ${ROOT_PATH}/bin/)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Third")

# use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE libzip)

################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "${PROJECT_NAME}_d"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG          "${PROJECT_NAME}_d"
        TARGET_NAME_EDITOR_DEBUG   "${PROJECT_NAME}_d"
    )
endif()

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Any CPU")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_EDITOR_DEBUG   "TRUE"
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
        "${CMAKE_CURRENT_SOURCE_DIR}/../zlib-1.2.8;"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../zlib-1.2.8;"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../zlib-1.2.8;"
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if(NOT MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "HAVE_UNISTD_H;"
        "HAVE_STRINGS_H;"
        "HAVE_STRCASECMP;" 
        "HAVE_FSEEKO;" 
        "HAVE_FTELLO;" 
        "HAVE_STRICMP;"
        "NDEBUG;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "ZIP_STATIC;"
        "WIN32;"
        "_WINDOWS;"
        "NDEBUG;"
        "_CRT_SECURE_NO_WARNINGS;"
        "_CRT_NONSTDC_NO_DEPRECATE;"
        "_MBCS"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "ZIP_STATIC;"
        "WIN32;"
        "_WINDOWS;"
        "NDEBUG;"
        "_CRT_SECURE_NO_WARNINGS;"
        "_CRT_NONSTDC_NO_DEPRECATE"
        "_MBCS"
    )
endif()

################################################################################
# Dependencies
################################################################################
if(MSVC)
    add_dependencies(${PROJECT_NAME}
        zlib
    )

    # Link with other targets.
    target_link_libraries(${PROJECT_NAME} PUBLIC
        zlib
    )
endif()












