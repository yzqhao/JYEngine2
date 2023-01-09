set (
	BUILD_LIST_COMMON
    ./System/ system_dir
    ./Math/ math_dir
    ./Core/ core_dir
    ./Engine/ engine_dir
    ./RHI/ rhi_dir
    #./LuaScriptSystem/ lua_script_system_dir
    #./Application/ application_dir
    ./Executable/Editor/ editor_dir
    ./Plugin/BgfxRHI/ bgfx_rhi_dir
    ./Plugin/ConfigSystem/ config_system_dir
    ./Plugin/FileSystem/ file_system_dir
	./Plugin/FlatSerializeSystem flat_serialize_system_dir
    ./Plugin/LogSystem/ log_system_dir
    ./Plugin/RenderPipelineSystem/ render_pipeline_system_dir
    ./Plugin/ResourceSystem/ resource_system_dir
    ./Plugin/TimeSystem/ time_system_dir
	#./Function/imguifunction imgui_function_dir
	./Function/Meteor meteor_function_dir
    ./Test/TestModule/ test_testmodule_dir
    ./Third/libpng-1.5.30/projects/visualc71/ libpng_dir
    ./Third/zlib-1.2.8/ zlib_dir
	#./Third/glad/ glad_dir
	./Third/glfw-3.3.5/src/ glfw_dir
	./Third/bx/ bx_dir
	./Third/bimg/ bimg_dir
	./Third/bgfx/ bgfx_dir
	./Third/LuaVMLib/ lua_dir
	./Third/luasocket/ luasocket_dir
	./Third/lua-cjson/ lua_cjson_dir
	./Third/LuaBitOp-1.0.2/ lua_bit_dir
	./Third/luafilesystem/ luafilesystem_dir
	./Third/hlslcc_lib/ hlslcc_lib_dir
	./Third/mcpp-2.7.2/ mcpp_dir
	./Third/liblz4/ liblz4_dir
	./Third/libzip/ libzip_dir
	./Third/zipper/zipper/ zipper_dir
	./Third/freetype-2.10.4/ freetype_dir
	./Third/imgui/ imgui_dir
	./Third/libexif/ exif_lib
	./Third/libyuv/jni/ libyuv_lib
	./Tools/FbxExport fbx_export
	./Tools/FbxExportLib fbx_export_lib
	./Tools/CompilerShader compiler_shader
	./Tools/Lz4ZipCompression Lz4ZipCompression_lib
)


set ( BUILD_LIST_COMMON
        ${BUILD_LIST_COMMON}
    )
	
set ( BUILD_LIST_WINDOWS
	${BUILD_LIST_COMMON}
    #./Third/nativefiledialog/build/ nativefiledialog_binary_dir
    ./Third/nativefiledialog/build/vs2010/ nativefiledialog2010_binary_dir
)