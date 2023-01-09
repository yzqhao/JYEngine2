#pragma once

struct TextureFormatInfo
{
    GLenum m_internalFmt;
    GLenum m_internalFmtSrgb;
    GLenum m_fmt;
    GLenum m_fmtSrgb;
    GLenum m_type;
    bool m_supported;
};

struct Blend
{
	GLenum m_src;
	GLenum m_dst;
	bool m_factor;
};

struct Extension
{
    enum Enum
    {
        AMD_conservative_depth,
        AMD_multi_draw_indirect,

        ANGLE_depth_texture,
        ANGLE_framebuffer_blit,
        ANGLE_framebuffer_multisample,
        ANGLE_instanced_arrays,
        ANGLE_texture_compression_dxt1,
        ANGLE_texture_compression_dxt3,
        ANGLE_texture_compression_dxt5,
        ANGLE_timer_query,
        ANGLE_translated_shader_source,

        APPLE_texture_format_BGRA8888,
        APPLE_texture_max_level,

        ARB_clip_control,
        ARB_compute_shader,
        ARB_conservative_depth,
        ARB_copy_image,
        ARB_debug_label,
        ARB_debug_output,
        ARB_depth_buffer_float,
        ARB_depth_clamp,
        ARB_draw_buffers_blend,
        ARB_draw_indirect,
        ARB_draw_instanced,
        ARB_ES3_compatibility,
        ARB_framebuffer_object,
        ARB_framebuffer_sRGB,
        ARB_get_program_binary,
        ARB_half_float_pixel,
        ARB_half_float_vertex,
        ARB_instanced_arrays,
        ARB_internalformat_query,
        ARB_internalformat_query2,
        ARB_invalidate_subdata,
        ARB_map_buffer_range,
        ARB_multi_draw_indirect,
        ARB_multisample,
        ARB_occlusion_query,
        ARB_occlusion_query2,
        ARB_program_interface_query,
        ARB_provoking_vertex,
        ARB_sampler_objects,
        ARB_seamless_cube_map,
        ARB_shader_bit_encoding,
        ARB_shader_image_load_store,
        ARB_shader_storage_buffer_object,
        ARB_shader_texture_lod,
        ARB_shader_viewport_layer_array,
        ARB_texture_compression_bptc,
        ARB_texture_compression_rgtc,
        ARB_texture_cube_map_array,
        ARB_texture_float,
        ARB_texture_multisample,
        ARB_texture_rg,
        ARB_texture_rgb10_a2ui,
        ARB_texture_stencil8,
        ARB_texture_storage,
        ARB_texture_swizzle,
        ARB_timer_query,
        ARB_uniform_buffer_object,
        ARB_vertex_array_object,
        ARB_vertex_type_2_10_10_10_rev,

        ATI_meminfo,

        CHROMIUM_color_buffer_float_rgb,
        CHROMIUM_color_buffer_float_rgba,
        CHROMIUM_depth_texture,
        CHROMIUM_framebuffer_multisample,
        CHROMIUM_texture_compression_dxt3,
        CHROMIUM_texture_compression_dxt5,

        EXT_bgra,
        EXT_blend_color,
        EXT_blend_minmax,
        EXT_blend_subtract,
        EXT_color_buffer_half_float,
        EXT_color_buffer_float,
        EXT_copy_image,
        EXT_compressed_ETC1_RGB8_sub_texture,
        EXT_debug_label,
        EXT_debug_marker,
        EXT_debug_tool,
        EXT_discard_framebuffer,
        EXT_disjoint_timer_query,
        EXT_draw_buffers,
        EXT_draw_instanced,
        EXT_instanced_arrays,
        EXT_frag_depth,
        EXT_framebuffer_blit,
        EXT_framebuffer_object,
        EXT_framebuffer_sRGB,
        EXT_gpu_shader4,
        EXT_multi_draw_indirect,
        EXT_occlusion_query_boolean,
        EXT_packed_float,
        EXT_read_format_bgra,
        EXT_shader_image_load_store,
        EXT_shader_texture_lod,
        EXT_shadow_samplers,
        EXT_sRGB_write_control,
        EXT_texture_array,
        EXT_texture_compression_dxt1,
        EXT_texture_compression_latc,
        EXT_texture_compression_rgtc,
        EXT_texture_compression_s3tc,
        EXT_texture_cube_map_array,
        EXT_texture_filter_anisotropic,
        EXT_texture_format_BGRA8888,
        EXT_texture_rg,
        EXT_texture_shared_exponent,
        EXT_texture_snorm,
        EXT_texture_sRGB,
        EXT_texture_storage,
        EXT_texture_swizzle,
        EXT_texture_type_2_10_10_10_REV,
        EXT_timer_query,
        EXT_unpack_subimage,
        EXT_sRGB,
        EXT_multisampled_render_to_texture,

        GOOGLE_depth_texture,

        IMG_multisampled_render_to_texture,
        IMG_read_format,
        IMG_shader_binary,
        IMG_texture_compression_pvrtc,
        IMG_texture_compression_pvrtc2,
        IMG_texture_format_BGRA8888,

        INTEL_fragment_shader_ordering,

        KHR_debug,
        KHR_no_error,

        MOZ_WEBGL_compressed_texture_s3tc,
        MOZ_WEBGL_depth_texture,

        NV_conservative_raster,
        NV_copy_image,
        NV_draw_buffers,
        NV_draw_instanced,
        NV_instanced_arrays,
        NV_occlusion_query,
        NV_texture_border_clamp,
        NVX_gpu_memory_info,

        OES_copy_image,
        OES_compressed_ETC1_RGB8_texture,
        OES_depth24,
        OES_depth32,
        OES_depth_texture,
        OES_element_index_uint,
        OES_fragment_precision_high,
        OES_fbo_render_mipmap,
        OES_get_program_binary,
        OES_required_internalformat,
        OES_packed_depth_stencil,
        OES_read_format,
        OES_rgb8_rgba8,
        OES_standard_derivatives,
        OES_texture_3D,
        OES_texture_float,
        OES_texture_float_linear,
        OES_texture_npot,
        OES_texture_half_float,
        OES_texture_half_float_linear,
        OES_texture_stencil8,
        OES_texture_storage_multisample_2d_array,
        OES_vertex_array_object,
        OES_vertex_half_float,
        OES_vertex_type_10_10_10_2,

        WEBGL_color_buffer_float,
        WEBGL_compressed_texture_etc1,
        WEBGL_compressed_texture_s3tc,
        WEBGL_compressed_texture_pvrtc,
        WEBGL_depth_texture,
        WEBGL_draw_buffers,

        WEBKIT_EXT_texture_filter_anisotropic,
        WEBKIT_WEBGL_compressed_texture_s3tc,
        WEBKIT_WEBGL_depth_texture,

        Count
    };

    const char* m_name;
    bool m_supported;
    bool m_initialize;
};

		
