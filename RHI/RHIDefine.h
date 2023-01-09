
#pragma once

#include "private/Define.h"
#include "Core/Type.h"

NS_JYE_BEGIN

namespace RHIDefine {

enum GraphicRenderer
{
	RendererOpenGL = 0,
	RendererGles,
	RendererGles31,
	RendererGles32,
	RendererMetal,
	RendererVulkan,
	RendererDirect3D11,
	RendererDirect3D12,
	RendererCount
};

enum GraphicFeatureLevel
{
	ES2,
	ES3_0,
	ES3_1,
	ES3_2,
	SM_4,
	SM_5,
	LEVELCOUNT
};

enum ShaderType
{
	VS,
	PS,
	CS,
};

enum MemoryUseage
{
	MU_DYNAMIC = 0,
	MU_STATIC,
	MU_STREAM,
	MU_READ,
	MU_WRITE,
	MU_READWRITE,
};

enum IndicesType
{
	IT_UINT16 = 0,
	IT_UINT32,
};

enum Boolean
{
	MB_NULL = -1,
	MB_FALSE = 0,
	MB_TRUE = 1,
};
enum Function
{
	FN_NULL = -1,
	FN_NEVER = 0,
	FN_LESS,
	FN_EQUAL,
	FN_LEQUAL,
	FN_GREATER,
	FN_NOTEQUAL,
	FN_GEQUAL,
	FN_ALWAYS,
};
enum CullFaceMode
{
	CFM_NULL = -1,
	CFM_OFF = 0,
	CFM_FRONT,
	CFM_BACK,
};
enum RasterizerCullMode
{
	CM_None,
	CM_CW,
	CM_CCW,
};
enum Operation
{
	ON_NULL = -1,
	ON_ZERO = 0,
	ON_ONE,
	ON_KEEP,
	ON_REPLACE,
	ON_INCR,
	ON_DECR,
	ON_INVERT,
	ON_INCR_WRAP,
	ON_DECR_WRAP,
};
enum Blend
{
	BL_NULL = -1,
	BL_ZERO = 0,
	BL_ONE,
	BL_SRC_COLOR,
	BL_ONE_MINUS_SRC_COLOR,
	BL_SRC_ALPHA,
	BL_ONE_MINUS_SRC_ALPHA,
	BL_DST_ALPHA,
	BL_ONE_MINUS_DST_ALPHA,
	BL_DST_COLOR,
	BL_ONE_MINUS_DST_COLOR,
};
enum PolygonMode {
	PM_NULL = -1,
	PM_POINT = 0,
	PM_LINE,
	PM_TRIANGLE,
};
enum ColorMask
{
	CM_COLOR_NONE = 0,
	CM_COLOR_R = 1 << 0,
	CM_COLOR_G = 1 << 1,
	CM_COLOR_B = 1 << 2,
	CM_COLOR_A = 1 << 3,
	CM_COLOR_RGBA = CM_COLOR_R | CM_COLOR_G | CM_COLOR_B | CM_COLOR_A,
	CM_COLOR_RGB = CM_COLOR_R | CM_COLOR_G | CM_COLOR_B,
};
enum RenderMode
{
	RM_POINTS = 0,
	RM_LINES,
	RM_LINE_LOOP,
	RM_LINE_STRIP,
	RM_TRIANGLES,
	RM_TRIANGLE_STRIP,
};

enum TargetType
{
	RT_RENDER_TARGET_MAIN = 0, //主屏幕
	RT_RENDER_TARGET_2D = 1,
	RT_RENDER_TARGET_CUBE = 6,
};

enum TargetFace
{
	TF_POSITIVE_X = 0,//x 正方向
	TF_NEGATIVE_X,
	TF_POSITIVE_Y,
	TF_NEGATIVE_Y,
	TF_POSITIVE_Z,
	TF_NEGATIVE_Z,
	TF_DEFAULT,//2d纹理的方向
};
enum RenderTargetAttachment
{
	TA_COLOR_0 = 0,
	TA_COLOR_1,
	TA_COLOR_2,
	TA_COLOR_3,
	TA_COLOR_4,
	TA_COLOR_5,
	TA_COLOR_6,
	TA_COLOR_7,
	TA_COLOR_8,
	TA_COLOR_9,
	TA_DEPTH,
	TA_DEPTH_STENCIL,
	TA_COUNT,
};

enum ClearFlag
{
	CF_None = 1 << 0,
	CF_COLOR = 1 << 1,
	CF_DEPTH = 1 << 2,
	CF_STENCIL = 1 << 3,
	CF_COLOR1 = 1 << 4,
	CF_COLOR2 = 1 << 5,
	CF_COLOR3 = 1 << 6,

	CF_COLOR_DEPTH = CF_COLOR | CF_DEPTH,
	CF_COLOR_STENCIL = CF_COLOR | CF_STENCIL,
	CF_DEPTH_STENCIL = CF_DEPTH | CF_STENCIL,
	CF_COLOR_DEPTH_STENCIL = CF_COLOR | CF_DEPTH | CF_STENCIL,
	CF_COLOR123_DEPTH_STENCIL = CF_COLOR1 | CF_COLOR2 | CF_COLOR3 | CF_DEPTH | CF_STENCIL,
	CF_COLOR0123_DEPTH_STENCIL = CF_COLOR | CF_COLOR1 | CF_COLOR2 | CF_COLOR3 | CF_DEPTH | CF_STENCIL,
	CF_COLOR013_DEPTH_STENCIL = CF_COLOR | CF_COLOR1 | CF_COLOR3 | CF_DEPTH | CF_STENCIL,
};

enum TextureType
{
	TT_AUTO = 0,
	TEXTURE_1D,
	TEXTURE_2D,
	TEXTURE_3D,
	TEXTURE_CUBE_MAP,

	TT_TEXTURECUBE_FRONT,
	TT_TEXTURECUBE_BACK,
	TT_TEXTURECUBE_TOP, 
	TT_TEXTURECUBE_BOTTOM,
	TT_TEXTURECUBE_LEFT,
	TT_TEXTURECUBE_RIGHT,
};
enum TextureWarp
{
	TW_REPEAT,
	TW_CLAMP_TO_EDGE,
	TW_MIRRORED_REPEAT,
	TW_CLAMP_TO_BORDER,
};
enum TextureFilter
{
	TF_NEAREST,
	TF_LINEAR,
	TF_NEAREST_MIPMAP_NEAREST,
	TF_LINEAR_MIPMAP_NEAREST,
	TF_NEAREST_MIPMAP_LINEAR,
	TF_LINEAR_MIPMAP_LINEAR,
};
struct WarpType
{
	TextureWarp m_SWarp;
	TextureWarp m_TWarp;
	WarpType()
		:m_SWarp(TW_CLAMP_TO_EDGE),
		m_TWarp(TW_CLAMP_TO_EDGE)
	{}
};
struct FilterType
{
	TextureFilter m_MagFilter;
	TextureFilter m_MinFilter;
	FilterType()
		:m_MagFilter(TF_NEAREST),
		m_MinFilter(TF_NEAREST)
	{}
};
enum TextureSamplerCompare
{
	SAMPLER_COMPARE_NONE,
	SAMPLER_COMPARE_LESS,
	SAMPLER_COMPARE_LEQUAL,
	SAMPLER_COMPARE_EQUAL,
	SAMPLER_COMPARE_GEQUAL,
	SAMPLER_COMPARE_GREATER,
	SAMPLER_COMPARE_NOTEQUAL,
	SAMPLER_COMPARE_NEVER,
	SAMPLER_COMPARE_ALWAYS,
};
enum TextureUseage
{
	TU_STATIC = 0,
	TU_READ = 1,
	TU_WRITE = 2,
	TU_RT_WRITE_ONLY = 3,
	TU_COMPUTEWRITE = 4,
};

enum PixelFormat
{
	PF_AUTO = -1,
	PF_A8,
	PF_L8,	
	PF_L8A8,	
	PF_YUV420P,		
	PF_R8G8B8,		
	PF_R5G6B5,				
	PF_R8G8B8A8,	
	PF_R4G4B4A4,		
	PF_DEPTH16,
	PF_DEPTH32,				
	PF_DEPTH24_STENCIL8,

	PF_RGBAFLOAT,		
	PF_RGBAHALF,
	PF_RG11B10FLOAT,	

	PF_R16_FLOAT,
	PF_R32_UINT,
	PF_R32_FLOAT,
};

enum ShaderAttribute
{
	PS_ATTRIBUTE_POSITION = 1000,
	PS_ATTRIBUTE_COORDNATE0,
	PS_ATTRIBUTE_COORDNATE1, 
	PS_ATTRIBUTE_COORDNATE2, 
	PS_ATTRIBUTE_COORDNATE3, 
	PS_ATTRIBUTE_INSTANCE0, 
	PS_ATTRIBUTE_INSTANCE1, 
	PS_ATTRIBUTE_INSTANCE2, 
	PS_ATTRIBUTE_INSTANCE3, 
	PS_ATTRIBUTE_NORMAL,
	PS_ATTRIBUTE_COLOR0,
	PS_ATTRIBUTE_COLOR1,
	PS_ATTRIBUTE_TANGENT,
	PS_ATTRIBUTE_BINORMAL,
	PS_ATTRIBUTE_BONE_INEX,
	PS_ATTRIBUTE_BONE_WEIGHT,
};
enum DataType
{
	DT_FLOAT = 0,
	DT_HALF_FLOAT,
	DT_INT_8_8_8_8,
	DT_UINT_8_8_8_8,
	DT_INT_2_10_10_10,
	DT_UINT_10_10_10_2,
};
enum VariablesType
{
	VT_ERRORCODE = 0,
	VT_FLOAT,
	VT_VEC2,
	VT_VEC3,
	VT_VEC4,
	VT_FLOAT_ARRAY,
	VT_VEC2_ARRAY,
	VT_VEC3_ARRAY,
	VT_VEC4_ARRAY,
	VT_MAT3,
	VT_MAT3_ARRAY,
	VT_MAT4,
	VT_MAT4_ARRAY,
	VT_TEXTURE1D,
	VT_TEXTURE2D,
	VT_TEXTURE3D,
	VT_TEXTURECUBE,
	VT_BUFFER,
	VT_SAMPLERBUFFER,
	VT_CONSTBUFFER,
	VT_COUNT,
};

enum ParameterUsage
{
	SU_ATTRIBUTE = 0,	//定义这个类型是一个顶点数据
	SU_UNIFORM,			//是一个uniform
	SU_INTERNAL,		//只在内部拼装时进行数据传递
	SU_UNKNOWN,			//预加载参数时不知道参数类型
};

enum ParameterSlot//所对应的Slot
{
	PS_ERRORCODE = -1,
	//系统寄存器
	PS_SYSTEM_TIME = 2001,//渲染线程自启动后到现在的时间
	PS_SYSTEM_TIME_SPAN,//渲染线程当前帧的时间

	PS_ANIMATION_REAL_ARRAY,//骨骼数组//模型寄存器
	PS_ANIMATION_DUAL_ARRAY,//骨骼数组
	PS_ANIMATION_SCALE_ARRAY,//骨骼动画支持缩放
	PS_ANIMATION_MATRIX_ARRAY,//骨骼矩阵
	PS_INSTANCE_CBUFFER0, /**<ConstBuffer0*/
	PS_INSTANCE_CBUFFER1, /**<ConstBuffer1*/
	PS_LOCALWORLD_TRANSFORM,//本地到世界的变换矩阵
	PS_WORLDLOCAL_TRANSFROM,/**<transfrom point form world to local*/
	PS_LOCALWORLD_ROTATION,//本地的旋转矩阵
	PS_LOCALSCREEN_TRANSVIEWPROJ,//本地到屏幕的mvp

	PS_CAMERA_WORLDROTATION,//摄像机旋转//摄像机寄存器数据
	PS_CAMERA_WORLDPOSITION,//摄像机位置
	PS_CAMERA_WORLDDIRECTION,//摄像机朝向
	PS_CAMERA_VIEW,//摄像机view矩阵
	PS_CAMERA_PROJECTION,//摄像机投影
	PS_CAMERA_VIEWPROJ,//摄像机vp矩阵
	PS_CAMERA_VIEWPROJ_INV,//摄像机vp逆矩阵
	PS_CAMERA_LINERPARAM,//摄像机线性化参数
	PS_CAMERA_LINEARPARAMBIAS,//
	PS_CAMERA_RESOLUTION,//屏幕分辨率
	PS_CAMERA_RESOLUTION_INV,
	PS_CAMERA_RESOLUTION_HALF_INV,
	PS_CAMERA_SHADOWRANGE,
	PS_LIGHT_CAMERA_VIEW, //光相机view矩阵
	PS_LIGHT_CAMERA_PROJECTION, //光相机投影矩阵
	PS_LIGHT_CAMERA_LINEARPARAM, //光相机线性化参数
	PS_LIGHT_CAMERA_POSITION,    //光相机位置


	PS_LIGHT_POSITION,//光源的属性
	PS_LIGHT_COLOR,
	PS_AMBIENT_COLOR,
	PS_LIGHT_ANGLE,//聚光灯角度
	PS_LIGHT_INNER_DIFF_INV,//角差
	PS_LIGHT_RANGE_INV,//距离的逆
	PS_LIGHT_ATTENUATION,//衰减
	PS_LIGHT_PARAM,
	PS_LIGHT_VERTEX_DIRECTION,//顶点计算出来的光照方向
	PS_LIGHT_GIVEN_DIRECTION,//外部传递光照方向
	PS_GAUSSIAN_BLUR_STEP,    //高斯模糊步长

	PS_WORLD_POSITION,//对象在世界的位置
	PS_WORLD_SCALE,//对象在世界的缩放

	PS_DYNAMIC_ATTRIBUTE_BEGIN = 3000,//用户自定义的属性

	PS_MATERIAL_BEGIN = 3001,//用户自定义的起点//中间是用户定义的，参数和纹理都用这个区间
	PS_TEXTURE_AMBIENT,//环境贴图
	PS_TEXTURE_DIFFUSE,//纹理属性
	PS_TEXTURE_SPECULAR,//反射颜色
	PS_TEXTURE_SPLEVEL,//发射等级
	PS_TEXTURE_GLOSSINESS,//发光
	PS_TEXTURE_ILLUMINATION,//自发光
	PS_TEXTURE_OPACITY,//透明贴图
	PS_TEXTURE_FITER,//过滤贴图
	PS_TEXTURE_BUMP,//法线贴图
	PS_TEXTURE_REFLECTION,//发射贴图
	PS_TEXTURE_REFRACTION,//折射贴图
	PS_TEXTURE_DISPLACEMENT,//视察贴图
	PS_TEXTURE_SHADOW_DEPTH, //阴影图
	PS_TEXTURE_TRANSPARENT_SHADOW,//透明物体阴影
	PS_TEXTURE_HAIR_SHADOW_DEPTH, // hair shadow map for self-shadow rendering
	PS_TEXTURE_HAIR_SHADOW_MASK, // hair shadow mask for casting shadow onto other objects
	PS_TEXTURE_GRAP, //当前屏幕的纹理
	// GBufferA.rgb = Base Color, alpha = Metalness
	PS_TEXTURE_GBUFFER_A,
	// GBufferB.rgb = World Normal, alpha = Smoothness
	PS_TEXTURE_GBUFFER_B,
	// GBufferC.rgb = emissive + iblDiffuseTerm + specularTerm, alpha = 1.0
	PS_TEXTURE_GBUFFER_C,
	// GBuffer深度
	PS_TEXTURE_GBUFFER_DEPTH,
	PS_TEXTURE_SCENE_DEPTH, /**<scene depth texture */

	PS_HAIR_FIRSTLAYER_DEPTH,/**< first layer depth texture in hair rendering pipeline*/
	PS_HAIR_SECONDLAYER_DEPTH,/**< second layer depth texture in hair rendering pipeline*/
	PS_HAIR_THIRDLAYER_DEPTH,/**< third layer depth texture in hair rendering pipeline*/


	PS_MATERIAL_AMBIENT,//环境光//材质接受光照强度
	PS_MATERIAL_DIFFUSE,//满反射
	PS_MATERIAL_SPECULAR,//反射
	PS_MATERIAL_SPECULAR_LEVEL,//反射强度
	PS_MATERIAL_SHININESS,//反射面积
	PS_POINT_COLOR,   //点的颜色


	PS_MATERIAL_USER_BEGIN,//用户定义的开始
	PS_MATERIAL_END = 4000,

	PS_INTERNAL_BEGIN = 4001,
	PS_WORLDSPACE_POSITION,//世界坐标下的位置
	PS_WORLDSPACE_NORMAL,//世界坐标下的发现
	PS_WORLDSPACE_TANGENT,//世界坐标下的切线
	PS_WORLDSPACE_BINORMAL,//世界坐标下的副法线
	PS_CAMERASPACE_POSITION, //相机空间下位置
	PS_WORLDSPACE_VIEWDIRECTION,//世界坐标下的视觉射线方向
	PS_SCREENSPACE_POSITION,//屏幕空间位置[-1,1]
	PS_SCREENSPACE_COORDNATE,//屏幕空间纹理坐标，铺满整个屏幕[0,1]
	PS_SCREENSPACE_NORMAL,//屏幕空间的法线，一般没用
	PS_SCREENSPACE_TANGENT,//屏幕空间的切线，一般没用
	PS_SCREENSPACE_BINORMAL,//屏幕空间的副法线，一般没用
	PS_WORLDSPACE_LIGHT_DISTANCE,//光源和顶点的距离
	PS_WORLDSPACE_DEPTH,//深度
	PS_LIGHTING_DIFFUSE,//漫反射光照颜色
	PS_LIGHTING_SPECULAR,//高光光照颜色
	PS_SURFACE_COLOR,//表面颜色
	PS_LIGHTSPACE_POSITION, //光相机空间位置
	PS_LIGHTSPACE_COORD, //光相机空间采样纹理坐标
	PS_LIGHTSPACE_DEPTH, //阴影图深度
	PS_WORLDSPACE_LIGHTCAMERA_DISTANCE, //光相机和顶点的距离
	PS_GAUSSIAN_COORD_ARRAY, //高斯模糊的采样点数组
	PS_LOCALSPACE_POSITION, //依旧是原始位置
	PS_SHADOWMAP_COLOR,//半透明阴影颜色
	PS_DEVICE_COORDINATE_Y_FLIP,//统一不同图形系统下FBO渲染y方向


	PS_INTERNAL_USE_BEING,//用户自定义的开始
	PS_INTERNAL_END = 5000,

	PS_UNKNOWN_BEGIN = 8000,
	PS_DYNAMIC_ATTRIBUTE_END = 9999
};

enum PassType
{
	PT_UNIVERSAL_POST_EFFECT,	// 后处理
	PT_FORWARDBASE,				// forward base
	PT_FORWARDADD,				// forward add
	PT_ALWAYS,					// 不计算光照
	PT_DEPTHPASS,				// 深度
	PR_COMPUTE,					// 计算着色器
	PT_GBUFFER,
	PT_DEFERRED_SHADING,
};

enum RenderQueue
{
	MRQ_NULL = 0XFFFF,
	MRQ_BACKGROUND = 0,
	MRQ_OPAQUE = 1000,
	MRQ_TRANSPARENT_CUTOUT = 2000,
	MRQ_TRANSPARENT = 3000,
	MRQ_POSTEFFECT = 4000,
	MRQ_OVERLAY = 5000,
};

using Uint = unsigned;
using BitMask = unsigned;

}

template<>	struct TIsNoNeedLoop <RHIDefine::TextureType> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::TextureWarp> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::TextureFilter> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::WarpType> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::FilterType> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::TextureSamplerCompare> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::TextureUseage> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::PixelFormat> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::ShaderAttribute> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::DataType> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::VariablesType> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::ParameterUsage> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::ParameterSlot> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::PassType> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <RHIDefine::RenderQueue> { enum { Value = true }; };

NS_JYE_END