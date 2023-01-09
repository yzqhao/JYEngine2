
#include "BindingEngine.h"
#include "BindingCore.h"

#include "Engine/render/material/IMaterialSystem.h"
#include "Engine/component/camera/CameraComponent.h"
#include "Engine/component/light/LightComponent.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/component/transform/TransformComponent.h"
#include "Engine/render/RenderTargetEntity.h"
#include "Engine/object/GObject.h"
#include "Engine/object/Scene.h"
#include "Engine/object/SceneManager.h"
#include "Engine/Engine.h"
#include "Engine/ProjectSetting.h"
#include "Core/Interface/IWindow.h"

NS_JYE_BEGIN

void Bind_Engine(sol::state& sol_state)
{
	auto ns_table = sol_state["Engine"].get_or_create<sol::table>();

#define REGISTER_MEMBER_FUNC(classname, funname)   #funname, &classname::funname
#define REGISTER_MEMBER_FUNC_TransformComponent(funname)		REGISTER_MEMBER_FUNC(TransformComponent, funname)
#define REGISTER_MEMBER_FUNC_CameraComponent(funname)			REGISTER_MEMBER_FUNC(CameraComponent, funname)
#define REGISTER_MEMBER_FUNC_RenderComponent(funname)			REGISTER_MEMBER_FUNC(RenderComponent, funname)
#define REGISTER_MEMBER_FUNC_LightComponent(funname)			REGISTER_MEMBER_FUNC(LightComponent, funname)

	{	// RHIDefine
#define DEFINE_GraphicRenderer(x)			{#x, RHIDefine::GraphicRenderer::x},
#define DEFINE_GraphicFeatureLevel(x)		{#x, RHIDefine::GraphicFeatureLevel::x},
#define DEFINE_ShaderType(x)				{#x, RHIDefine::ShaderType::x},
#define DEFINE_MemoryUseage(x)				{#x, RHIDefine::MemoryUseage::x},
#define DEFINE_IndicesType(x)				{#x, RHIDefine::IndicesType::x},
#define DEFINE_Boolean(x)					{#x, RHIDefine::Boolean::x},
#define DEFINE_Function(x)					{#x, RHIDefine::Function::x},
#define DEFINE_CullFaceMode(x)				{#x, RHIDefine::CullFaceMode::x},
#define DEFINE_RasterizerCullMode(x)		{#x, RHIDefine::RasterizerCullMode::x},
#define DEFINE_Operation(x)					{#x, RHIDefine::Operation::x},
#define DEFINE_Blend(x)						{#x, RHIDefine::Blend::x},
#define DEFINE_PolygonMode(x)				{#x, RHIDefine::PolygonMode::x},
#define DEFINE_ColorMask(x)					{#x, RHIDefine::ColorMask::x},
#define DEFINE_RenderMode(x)				{#x, RHIDefine::RenderMode::x},
#define DEFINE_TargetType(x)				{#x, RHIDefine::TargetType::x},
#define DEFINE_TargetFace(x)				{#x, RHIDefine::TargetFace::x},
#define DEFINE_RenderTargetAttachment(x)	{#x, RHIDefine::RenderTargetAttachment::x},
#define DEFINE_ClearFlag(x)					{#x, RHIDefine::ClearFlag::x},
#define DEFINE_TextureType(x)				{#x, RHIDefine::TextureType::x},
#define DEFINE_TextureWarp(x)				{#x, RHIDefine::TextureWarp::x},
#define DEFINE_TextureFilter(x)				{#x, RHIDefine::TextureFilter::x},
#define DEFINE_TextureSamplerCompare(x)		{#x, RHIDefine::TextureSamplerCompare::x},
#define DEFINE_TextureUseage(x)				{#x, RHIDefine::TextureUseage::x},
#define DEFINE_PixelFormat(x)				{#x, RHIDefine::PixelFormat::x},
#define DEFINE_ShaderAttribute(x)			{#x, RHIDefine::ShaderAttribute::PS_##x},
#define DEFINE_DataType(x)					{#x, RHIDefine::DataType::x},
#define DEFINE_VariablesType(x)				{#x, RHIDefine::VariablesType::x},
#define DEFINE_ParameterUsage(x)			{#x, RHIDefine::ParameterUsage::x},
#define DEFINE_ParameterSlot(x)				{#x, RHIDefine::ParameterSlot::PS_##x},
#define DEFINE_PassType(x)					{#x, RHIDefine::PassType::x},
#define DEFINE_RenderQueue(x)				{#x, RHIDefine::RenderQueue::x},

		ns_table.new_enum<int, true>("RHIDefine", {
				DEFINE_GraphicRenderer(RendererOpenGL)
				DEFINE_GraphicRenderer(RendererGles)
				DEFINE_GraphicRenderer(RendererGles31)
				DEFINE_GraphicRenderer(RendererGles32)
				DEFINE_GraphicRenderer(RendererMetal)
				DEFINE_GraphicRenderer(RendererVulkan)
				DEFINE_GraphicRenderer(RendererDirect3D11)
				DEFINE_GraphicRenderer(RendererDirect3D12)
				DEFINE_GraphicRenderer(RendererCount)

				DEFINE_GraphicFeatureLevel(ES2)
				DEFINE_GraphicFeatureLevel(ES3_0)
				DEFINE_GraphicFeatureLevel(ES3_1)
				DEFINE_GraphicFeatureLevel(ES3_2)
				DEFINE_GraphicFeatureLevel(SM_4)
				DEFINE_GraphicFeatureLevel(SM_5)
				DEFINE_GraphicFeatureLevel(LEVELCOUNT)

				DEFINE_ShaderType(VS)
				DEFINE_ShaderType(PS)
				DEFINE_ShaderType(CS)

				DEFINE_MemoryUseage(MU_DYNAMIC)
				DEFINE_MemoryUseage(MU_STATIC)
				DEFINE_MemoryUseage(MU_STREAM)
				DEFINE_MemoryUseage(MU_READ)
				DEFINE_MemoryUseage(MU_WRITE)
				DEFINE_MemoryUseage(MU_READWRITE)

				DEFINE_Boolean(MB_FALSE)
				DEFINE_Boolean(MB_TRUE)

				DEFINE_IndicesType(IT_UINT16)
				DEFINE_IndicesType(IT_UINT32)

				DEFINE_Function(FN_NEVER)
				DEFINE_Function(FN_LESS)
				DEFINE_Function(FN_EQUAL)
				DEFINE_Function(FN_LEQUAL)
				DEFINE_Function(FN_GREATER)
				DEFINE_Function(FN_NOTEQUAL)
				DEFINE_Function(FN_GEQUAL)
				DEFINE_Function(FN_ALWAYS)

				DEFINE_CullFaceMode(CFM_OFF)
				DEFINE_CullFaceMode(CFM_FRONT)
				DEFINE_CullFaceMode(CFM_BACK)

				DEFINE_RasterizerCullMode(CM_CW)
				DEFINE_RasterizerCullMode(CM_CCW)

				DEFINE_Operation(ON_ZERO)
				DEFINE_Operation(ON_ONE)
				DEFINE_Operation(ON_KEEP)
				DEFINE_Operation(ON_REPLACE)
				DEFINE_Operation(ON_INCR)
				DEFINE_Operation(ON_DECR)
				DEFINE_Operation(ON_INVERT)
				DEFINE_Operation(ON_INCR_WRAP)
				DEFINE_Operation(ON_DECR_WRAP)

				DEFINE_Blend(BL_ZERO)
				DEFINE_Blend(BL_ONE)
				DEFINE_Blend(BL_SRC_COLOR)
				DEFINE_Blend(BL_ONE_MINUS_SRC_COLOR)
				DEFINE_Blend(BL_SRC_ALPHA)
				DEFINE_Blend(BL_ONE_MINUS_SRC_ALPHA)
				DEFINE_Blend(BL_DST_ALPHA)
				DEFINE_Blend(BL_ONE_MINUS_DST_ALPHA)
				DEFINE_Blend(BL_DST_COLOR)
				DEFINE_Blend(BL_ONE_MINUS_DST_COLOR)

				DEFINE_PolygonMode(PM_POINT)
				DEFINE_PolygonMode(PM_LINE)
				DEFINE_PolygonMode(PM_TRIANGLE)

				DEFINE_ColorMask(CM_COLOR_R)
				DEFINE_ColorMask(CM_COLOR_G)
				DEFINE_ColorMask(CM_COLOR_B)
				DEFINE_ColorMask(CM_COLOR_A)
				DEFINE_ColorMask(CM_COLOR_RGBA)
				DEFINE_ColorMask(CM_COLOR_RGB)

				DEFINE_RenderMode(RM_POINTS)
				DEFINE_RenderMode(RM_LINES)
				DEFINE_RenderMode(RM_LINE_LOOP)
				DEFINE_RenderMode(RM_LINE_STRIP)
				DEFINE_RenderMode(RM_TRIANGLES)
				DEFINE_RenderMode(RM_TRIANGLE_STRIP)

				DEFINE_TargetType(RT_RENDER_TARGET_MAIN)
				DEFINE_TargetType(RT_RENDER_TARGET_2D)
				DEFINE_TargetType(RT_RENDER_TARGET_CUBE)

				DEFINE_TargetFace(TF_POSITIVE_X)
				DEFINE_TargetFace(TF_NEGATIVE_X)
				DEFINE_TargetFace(TF_POSITIVE_Y)
				DEFINE_TargetFace(TF_NEGATIVE_Y)
				DEFINE_TargetFace(TF_POSITIVE_Z)
				DEFINE_TargetFace(TF_NEGATIVE_Z)

				DEFINE_RenderTargetAttachment(TA_COLOR_0)
				DEFINE_RenderTargetAttachment(TA_COLOR_1)
				DEFINE_RenderTargetAttachment(TA_COLOR_2)
				DEFINE_RenderTargetAttachment(TA_COLOR_3)
				DEFINE_RenderTargetAttachment(TA_COLOR_4)
				DEFINE_RenderTargetAttachment(TA_COLOR_5)
				DEFINE_RenderTargetAttachment(TA_COLOR_6)
				DEFINE_RenderTargetAttachment(TA_COLOR_7)
				DEFINE_RenderTargetAttachment(TA_COLOR_8)
				DEFINE_RenderTargetAttachment(TA_COLOR_9)
				DEFINE_RenderTargetAttachment(TA_DEPTH)
				DEFINE_RenderTargetAttachment(TA_DEPTH_STENCIL)

				DEFINE_ClearFlag(CF_COLOR)
				DEFINE_ClearFlag(CF_DEPTH)
				DEFINE_ClearFlag(CF_STENCIL)
				DEFINE_ClearFlag(CF_COLOR1)
				DEFINE_ClearFlag(CF_COLOR2)
				DEFINE_ClearFlag(CF_COLOR3)
				DEFINE_ClearFlag(CF_COLOR_DEPTH)
				DEFINE_ClearFlag(CF_COLOR_STENCIL)
				DEFINE_ClearFlag(CF_DEPTH_STENCIL)
				DEFINE_ClearFlag(CF_COLOR_DEPTH_STENCIL)
				DEFINE_ClearFlag(CF_COLOR123_DEPTH_STENCIL)
				DEFINE_ClearFlag(CF_COLOR0123_DEPTH_STENCIL)
				DEFINE_ClearFlag(CF_COLOR013_DEPTH_STENCIL)

				DEFINE_TextureType(TEXTURE_1D)
				DEFINE_TextureType(TEXTURE_2D)
				DEFINE_TextureType(TEXTURE_3D)
				DEFINE_TextureType(TEXTURE_CUBE_MAP)
				DEFINE_TextureType(TT_TEXTURECUBE_FRONT)
				DEFINE_TextureType(TT_TEXTURECUBE_BACK)
				DEFINE_TextureType(TT_TEXTURECUBE_TOP)
				DEFINE_TextureType(TT_TEXTURECUBE_BOTTOM)
				DEFINE_TextureType(TT_TEXTURECUBE_LEFT)
				DEFINE_TextureType(TT_TEXTURECUBE_RIGHT)

				DEFINE_TextureWarp(TW_REPEAT)
				DEFINE_TextureWarp(TW_CLAMP_TO_EDGE)
				DEFINE_TextureWarp(TW_MIRRORED_REPEAT)
				DEFINE_TextureWarp(TW_CLAMP_TO_BORDER)

				DEFINE_TextureFilter(TF_NEAREST)
				DEFINE_TextureFilter(TF_LINEAR)
				DEFINE_TextureFilter(TF_NEAREST_MIPMAP_NEAREST)
				DEFINE_TextureFilter(TF_LINEAR_MIPMAP_NEAREST)
				DEFINE_TextureFilter(TF_NEAREST_MIPMAP_LINEAR)
				DEFINE_TextureFilter(TF_LINEAR_MIPMAP_LINEAR)

				DEFINE_TextureSamplerCompare(SAMPLER_COMPARE_LESS)
				DEFINE_TextureSamplerCompare(SAMPLER_COMPARE_LEQUAL)
				DEFINE_TextureSamplerCompare(SAMPLER_COMPARE_EQUAL)
				DEFINE_TextureSamplerCompare(SAMPLER_COMPARE_GEQUAL)
				DEFINE_TextureSamplerCompare(SAMPLER_COMPARE_GREATER)
				DEFINE_TextureSamplerCompare(SAMPLER_COMPARE_NOTEQUAL)
				DEFINE_TextureSamplerCompare(SAMPLER_COMPARE_NEVER)
				DEFINE_TextureSamplerCompare(SAMPLER_COMPARE_ALWAYS)

				DEFINE_TextureUseage(TU_STATIC)
				DEFINE_TextureUseage(TU_READ)
				DEFINE_TextureUseage(TU_WRITE)
				DEFINE_TextureUseage(TU_RT_WRITE_ONLY)
				DEFINE_TextureUseage(TU_COMPUTEWRITE)

				DEFINE_PixelFormat(PF_AUTO)
				DEFINE_PixelFormat(PF_A8)
				DEFINE_PixelFormat(PF_L8)
				DEFINE_PixelFormat(PF_L8A8)
				DEFINE_PixelFormat(PF_YUV420P)
				DEFINE_PixelFormat(PF_R8G8B8)
				DEFINE_PixelFormat(PF_R5G6B5)
				DEFINE_PixelFormat(PF_R8G8B8A8)
				DEFINE_PixelFormat(PF_R4G4B4A4)
				DEFINE_PixelFormat(PF_DEPTH16)
				DEFINE_PixelFormat(PF_DEPTH32)
				DEFINE_PixelFormat(PF_DEPTH24_STENCIL8)
				DEFINE_PixelFormat(PF_RGBAFLOAT)
				DEFINE_PixelFormat(PF_RGBAHALF)
				DEFINE_PixelFormat(PF_RG11B10FLOAT)
				DEFINE_PixelFormat(PF_R16_FLOAT)
				DEFINE_PixelFormat(PF_R32_UINT)
				DEFINE_PixelFormat(PF_R32_FLOAT)

				DEFINE_ShaderAttribute(ATTRIBUTE_POSITION)
				DEFINE_ShaderAttribute(ATTRIBUTE_COORDNATE0)
				DEFINE_ShaderAttribute(ATTRIBUTE_COORDNATE1)
				DEFINE_ShaderAttribute(ATTRIBUTE_COORDNATE2)
				DEFINE_ShaderAttribute(ATTRIBUTE_COORDNATE3)
				DEFINE_ShaderAttribute(ATTRIBUTE_INSTANCE0)
				DEFINE_ShaderAttribute(ATTRIBUTE_INSTANCE1)
				DEFINE_ShaderAttribute(ATTRIBUTE_INSTANCE2)
				DEFINE_ShaderAttribute(ATTRIBUTE_INSTANCE3)
				DEFINE_ShaderAttribute(ATTRIBUTE_NORMAL)
				DEFINE_ShaderAttribute(ATTRIBUTE_COLOR0)
				DEFINE_ShaderAttribute(ATTRIBUTE_COLOR1)
				DEFINE_ShaderAttribute(ATTRIBUTE_TANGENT)
				DEFINE_ShaderAttribute(ATTRIBUTE_BINORMAL)
				DEFINE_ShaderAttribute(ATTRIBUTE_BONE_INEX)
				DEFINE_ShaderAttribute(ATTRIBUTE_BONE_WEIGHT)

				DEFINE_DataType(DT_FLOAT)
				DEFINE_DataType(DT_HALF_FLOAT)
				DEFINE_DataType(DT_INT_8_8_8_8)
				DEFINE_DataType(DT_UINT_8_8_8_8)
				DEFINE_DataType(DT_INT_2_10_10_10)
				DEFINE_DataType(DT_UINT_10_10_10_2)

				DEFINE_VariablesType(VT_FLOAT)
				DEFINE_VariablesType(VT_VEC2)
				DEFINE_VariablesType(VT_VEC3)
				DEFINE_VariablesType(VT_VEC4)
				DEFINE_VariablesType(VT_FLOAT_ARRAY)
				DEFINE_VariablesType(VT_VEC2_ARRAY)
				DEFINE_VariablesType(VT_VEC3_ARRAY)
				DEFINE_VariablesType(VT_VEC4_ARRAY)
				DEFINE_VariablesType(VT_MAT3)
				DEFINE_VariablesType(VT_MAT3_ARRAY)
				DEFINE_VariablesType(VT_MAT4)
				DEFINE_VariablesType(VT_MAT4_ARRAY)
				DEFINE_VariablesType(VT_TEXTURE1D)
				DEFINE_VariablesType(VT_TEXTURE2D)
				DEFINE_VariablesType(VT_TEXTURE3D)
				DEFINE_VariablesType(VT_TEXTURECUBE)
				DEFINE_VariablesType(VT_BUFFER)
				DEFINE_VariablesType(VT_SAMPLERBUFFER)
				DEFINE_VariablesType(VT_CONSTBUFFER)

				DEFINE_ParameterUsage(SU_ATTRIBUTE)
				DEFINE_ParameterUsage(SU_UNIFORM)
				DEFINE_ParameterUsage(SU_INTERNAL)

				DEFINE_ParameterSlot(SYSTEM_TIME)
				DEFINE_ParameterSlot(SYSTEM_TIME_SPAN)
				DEFINE_ParameterSlot(ANIMATION_REAL_ARRAY)
				DEFINE_ParameterSlot(ANIMATION_DUAL_ARRAY)
				DEFINE_ParameterSlot(ANIMATION_SCALE_ARRAY)
				DEFINE_ParameterSlot(ANIMATION_MATRIX_ARRAY)
				DEFINE_ParameterSlot(INSTANCE_CBUFFER0)
				DEFINE_ParameterSlot(INSTANCE_CBUFFER1)
				DEFINE_ParameterSlot(LOCALWORLD_TRANSFORM)
				DEFINE_ParameterSlot(WORLDLOCAL_TRANSFROM)
				DEFINE_ParameterSlot(LOCALWORLD_ROTATION)
				DEFINE_ParameterSlot(LOCALSCREEN_TRANSVIEWPROJ)
				DEFINE_ParameterSlot(CAMERA_WORLDROTATION)
				DEFINE_ParameterSlot(CAMERA_WORLDPOSITION)
				DEFINE_ParameterSlot(CAMERA_WORLDDIRECTION)
				DEFINE_ParameterSlot(CAMERA_VIEW)
				DEFINE_ParameterSlot(CAMERA_PROJECTION)
				DEFINE_ParameterSlot(CAMERA_VIEWPROJ)
				DEFINE_ParameterSlot(CAMERA_VIEWPROJ_INV)
				DEFINE_ParameterSlot(CAMERA_LINERPARAM)
				DEFINE_ParameterSlot(CAMERA_LINEARPARAMBIAS)
				DEFINE_ParameterSlot(CAMERA_RESOLUTION)
				DEFINE_ParameterSlot(CAMERA_RESOLUTION_INV)
				DEFINE_ParameterSlot(CAMERA_RESOLUTION_HALF_INV)
				DEFINE_ParameterSlot(CAMERA_SHADOWRANGE)
				DEFINE_ParameterSlot(LIGHT_CAMERA_VIEW)
				DEFINE_ParameterSlot(LIGHT_CAMERA_PROJECTION)
				DEFINE_ParameterSlot(LIGHT_CAMERA_LINEARPARAM)
				DEFINE_ParameterSlot(LIGHT_CAMERA_POSITION)

				DEFINE_ParameterSlot(LIGHT_POSITION)
				DEFINE_ParameterSlot(LIGHT_COLOR)
				DEFINE_ParameterSlot(AMBIENT_COLOR)
				DEFINE_ParameterSlot(LIGHT_ANGLE)
				DEFINE_ParameterSlot(LIGHT_INNER_DIFF_INV)
				DEFINE_ParameterSlot(LIGHT_RANGE_INV)
				DEFINE_ParameterSlot(LIGHT_ATTENUATION)
				DEFINE_ParameterSlot(LIGHT_PARAM)
				DEFINE_ParameterSlot(LIGHT_VERTEX_DIRECTION)
				DEFINE_ParameterSlot(LIGHT_GIVEN_DIRECTION)
				DEFINE_ParameterSlot(GAUSSIAN_BLUR_STEP)

				DEFINE_ParameterSlot(WORLD_POSITION)
				DEFINE_ParameterSlot(WORLD_SCALE)

				DEFINE_PassType(PT_UNIVERSAL_POST_EFFECT)
				DEFINE_PassType(PT_FORWARDBASE)
				DEFINE_PassType(PT_FORWARDADD)
				DEFINE_PassType(PT_ALWAYS)
				DEFINE_PassType(PT_DEPTHPASS)
				DEFINE_PassType(PR_COMPUTE)
				DEFINE_PassType(PT_GBUFFER)
				DEFINE_PassType(PT_DEFERRED_SHADING)

				DEFINE_RenderQueue(MRQ_BACKGROUND)
				DEFINE_RenderQueue(MRQ_OPAQUE)
				DEFINE_RenderQueue(MRQ_TRANSPARENT_CUTOUT)
				DEFINE_RenderQueue(MRQ_TRANSPARENT)
				DEFINE_RenderQueue(MRQ_POSTEFFECT)
				DEFINE_RenderQueue(MRQ_OVERLAY)
			});

#undef DEFINE_GraphicRenderer(x)			
#undef DEFINE_GraphicFeatureLevel(x)		
#undef DEFINE_ShaderType(x)				
#undef DEFINE_MemoryUseage(x)				
#undef DEFINE_IndicesType(x)				
#undef DEFINE_Boolean(x)					
#undef DEFINE_Function(x)					
#undef DEFINE_CullFaceMode(x)				
#undef DEFINE_RasterizerCullMode(x)		
#undef DEFINE_Operation(x)					
#undef DEFINE_Blend(x)						
#undef DEFINE_PolygonMode(x)				
#undef DEFINE_ColorMask(x)					
#undef DEFINE_RenderMode(x)				
#undef DEFINE_TargetType(x)				
#undef DEFINE_TargetFace(x)				
#undef DEFINE_RenderTargetAttachment(x)	
#undef DEFINE_ClearFlag(x)					
#undef DEFINE_TextureType(x)				
#undef DEFINE_TextureWarp(x)				
#undef DEFINE_TextureFilter(x)				
#undef DEFINE_TextureSamplerCompare(x)		
#undef DEFINE_TextureUseage(x)				
#undef DEFINE_PixelFormat(x)				
#undef DEFINE_ShaderAttribute(x)			
#undef DEFINE_DataType(x)					
#undef DEFINE_VariablesType(x)				
#undef DEFINE_ParameterUsage(x)			
#undef DEFINE_ParameterSlot(x)				
#undef DEFINE_PassType(x)					
#undef DEFINE_RenderQueue(x)				
	}

	{
#define DEFINE_RenderProperty(x)				{#x, GraphicDefine::RenderProperty::x},
#define DEFINE_CameraProjectionType(x)			{#x, GraphicDefine::CameraProjectionType::x},
#define DEFINE_LightType(x)						{#x, GraphicDefine::LightType::x},
#define DEFINE_MaskConstant(x)					{#x, MaskConstant::x},
		// GraphicDefine
		ns_table.new_enum<int, true>("GraphicDefine", {
			DEFINE_RenderProperty(RP_SHOW)
			DEFINE_RenderProperty(RP_CULL)
			DEFINE_RenderProperty(RP_LIGHT)
			DEFINE_RenderProperty(RP_SHADOW_CASTER)
			DEFINE_RenderProperty(RP_SHADOW_RECEIVER)
			DEFINE_RenderProperty(RP_IGNORE_PICK)
			DEFINE_RenderProperty(RP_SKINANI)
			DEFINE_RenderProperty(RP_DEFAULT)

			DEFINE_CameraProjectionType(CPT_PERSPECTIVE)
			DEFINE_CameraProjectionType(CPT_ORTHOGRAPHI)
			DEFINE_CameraProjectionType(CPT_UICAMERA)

			DEFINE_LightType(LT_AMBIENT)
			DEFINE_LightType(LT_DIRECTIONAL)
			DEFINE_LightType(LT_POINT)
			DEFINE_LightType(LT_SPOT)

			DEFINE_MaskConstant(MC_MASK_DEFAULT_LAYER)
			DEFINE_MaskConstant(MC_MASK_EDITOR_UI_LAYER)
			DEFINE_MaskConstant(MC_MASK_EDITOR_SCENE_LAYER)
			DEFINE_MaskConstant(MC_MASK_EDITOR_CLOTHDEBUG_LAYER)
			DEFINE_MaskConstant(MC_MASK_MAX)
			});
#undef DEFINE_RenderProperty
#undef DEFINE_CameraProjectionType
#undef DEFINE_LightType
#undef DEFINE_MaskConstant
	}

	{	// Component
		ns_table.new_usertype<Component>("Component",
			sol::base_classes, sol::bases<Object>(),
			REGISTER_OBJECT_FUNC(Component),
			"GetParentObject", &Component::GetParentObject,
			"isActiveHierarchy", &Component::isActiveHierarchy
			);
	}

	{	// GObject
		ns_table.new_usertype<GObject>("GObject",
			sol::base_classes, sol::bases<Object>(),
			REGISTER_OBJECT_FUNC(GObject),
			"isActiveHierarchy", &GObject::isActiveHierarchy,
			"GetScene", &GObject::GetScene,
			"GetRoot", (GObject * (GObject::*)()) & GObject::GetRoot,
			"NodeDepth", &GObject::NodeDepth,
			"SetLayer", &GObject::SetLayer,
			"isLayer", &GObject::isLayer,
			"GetLayer", &GObject::GetLayer,
			"HasComponent", &GObject::HasComponent,
			"GetComponent", &GObject::GetComponent,
			"CreateComponent", (Component * (GObject::*)(const std::string&)) & GObject::CreateComponent,
			"RemoveComponent", sol::overload(
				(void (GObject::*)(Component*)) & GObject::RemoveComponent,
				(void (GObject::*)(const std::string&)) & GObject::RemoveComponent),
			"GetComponents", &GObject::GetComponents,
			"GetChildren", &GObject::GetChildren,
			"AddChild", &GObject::AddChild,
			"DetachNode", &GObject::DetachNode
			);
	}

	{	// Scene
		ns_table.new_usertype<Scene>("Scene",
			sol::base_classes, sol::bases<Object>(),
			REGISTER_OBJECT_FUNC(Scene),
#ifdef _EDITOR
			"SetEditorCamera", & Scene::SetEditorCamera,
			"CreateDefaultRenderTarget", & Scene::CreateDefaultRenderTarget,
			"GetDefaultRenderTarget", & Scene::GetDefaultRenderTarget,
			"ChangeDefaultResolution", & Scene::ChangeDefaultResolution,
#endif
			"GetName", &Scene::GetName,
			"GetSequence", &Scene::GetSequence,
			"SetSequence", &Scene::SetSequence,
			"GetStaticID", &Scene::GetStaticID,
			"GetStaticID", sol::overload(
				[](Scene& sce) -> LuaUInt64 { return LuaUInt64(sce.GetStaticID()); }
			),
			"CreateObject", &Scene::CreateObject,
			"DeleteGObject", &Scene::DeleteGObject,
			"DeleteGObjectByID", &Scene::DeleteGObjectByID
			);
	}

	{	// SceneManager
		ns_table.new_usertype<SceneManager>("SceneManager",
			"Instance", &SceneManager::Instance,
			"CreateScene", &SceneManager::CreateScene,
			"GetScene", &SceneManager::GetScene,
			"DeleteScene", &SceneManager::DeleteScene, 
			"GetScenebyId", sol::overload(
				[](SceneManager& mng, LuaUInt64& id)  { return mng.GetScenebyId(id.GetVal()); }
			),
			"GetUtilityScene", &SceneManager::GetUtilityScene,
			"GetAllScenes", &SceneManager::GetAllScenes
			);
	}

	{	// ViewResolution
		ns_table.new_usertype<ViewResolution>("ViewResolution", sol::call_constructor, sol::constructors<
			sol::types<>>(),
			"m_ViewSize", &ViewResolution::m_ViewSize,
			"m_Resolution", &ViewResolution::m_Resolution
			);
	}

	{	// IMaterialSystem
		ns_table.new_usertype<IMaterialSystem>("IMaterialSystem",
			"Instance", &IMaterialSystem::Instance,
			"NewParameterSlot", sol::overload((RHIDefine::ParameterSlot(IMaterialSystem::*)(RHIDefine::ParameterUsage, const String&))& IMaterialSystem::NewParameterSlot),
			"GetParameterSlot", &IMaterialSystem::GetParameterSlot,
			"ParameterSlotExist", &IMaterialSystem::ParameterSlotExist
			);
	}

	{	// TransformComponent
		ns_table.new_usertype<TransformComponent>("TransformComponent",
			sol::base_classes, sol::bases<Component, Object>(),
			REGISTER_OBJECT_FUNC(TransformComponent),
			REGISTER_MEMBER_FUNC_TransformComponent(SetLocalRotation),
			REGISTER_MEMBER_FUNC_TransformComponent(SetLocalPosition),
			REGISTER_MEMBER_FUNC_TransformComponent(SetLocalScale),
			REGISTER_MEMBER_FUNC_TransformComponent(SetWorldRotation),
			REGISTER_MEMBER_FUNC_TransformComponent(SetWorldPosition),
			REGISTER_MEMBER_FUNC_TransformComponent(SetWorldScale),
			REGISTER_MEMBER_FUNC_TransformComponent(GetLocalRotation),
			REGISTER_MEMBER_FUNC_TransformComponent(GetLocalPosition),
			REGISTER_MEMBER_FUNC_TransformComponent(GetLocalScale),
			REGISTER_MEMBER_FUNC_TransformComponent(GetWorldRotation),
			REGISTER_MEMBER_FUNC_TransformComponent(GetWorldPosition),
			REGISTER_MEMBER_FUNC_TransformComponent(GetWorldScale)
			);
	}

	{
#define REG_CAMERACOMPONENT_SET_PARAM_FUNC(valtype) \
	[](RenderComponent& mat, RHIDefine::ParameterSlot slot, valtype val) { mat.SetParameter(slot, MakeMaterialParam(val)); },	\
	[](RenderComponent& mat, const std::string& slot, valtype val) { mat.SetParameter(slot, MakeMaterialParam(val)); },			\
	[](RenderComponent& mat, int idx, RHIDefine::ParameterSlot slot, valtype val) { mat.SetParameter(idx, slot, MakeMaterialParam(val)); },	\
	[](RenderComponent& mat, int idx, const std::string& slot, valtype val) { mat.SetParameter(idx, slot, MakeMaterialParam(val)); }
		// RenderComponent
		ns_table.new_usertype<RenderComponent>("RenderComponent",
			sol::base_classes, sol::bases<Component, Object>(),
			REGISTER_OBJECT_FUNC(RenderComponent),
			REGISTER_MEMBER_FUNC_RenderComponent(AddMaterialEntity),
			REGISTER_MEMBER_FUNC_RenderComponent(RemoveMaterialEntity),
			REGISTER_MEMBER_FUNC_RenderComponent(ChangeMaterialEntity),
			REGISTER_MEMBER_FUNC_RenderComponent(GetMaterialCount),
			REGISTER_MEMBER_FUNC_RenderComponent(GetMaterialEntity),
			REGISTER_MEMBER_FUNC_RenderComponent(GetMaterialEntities),
			REGISTER_MEMBER_FUNC_RenderComponent(PushMetadata),
			REGISTER_MEMBER_FUNC_RenderComponent(GetRenderMode),
			REGISTER_MEMBER_FUNC_RenderComponent(isKeepSource),
			REGISTER_MEMBER_FUNC_RenderComponent(GetRenderObjectEntity),
			REGISTER_MEMBER_FUNC_RenderComponent(CreateResource),
			REGISTER_MEMBER_FUNC_RenderComponent(CreateSubMesh),
			REGISTER_MEMBER_FUNC_RenderComponent(GetSubMeshByIndex),
			REGISTER_MEMBER_FUNC_RenderComponent(DisableSubMeshs),
			REGISTER_MEMBER_FUNC_RenderComponent(GetDrawCount),
			REGISTER_MEMBER_FUNC_RenderComponent(SetRenderOrder),
			REGISTER_MEMBER_FUNC_RenderComponent(GetRenderOrder),
			REGISTER_MEMBER_FUNC_RenderComponent(SetRenderProperty),
			REGISTER_MEMBER_FUNC_RenderComponent(EraseRenderProperty),
			REGISTER_MEMBER_FUNC_RenderComponent(isRenderProperty),
			REGISTER_MEMBER_FUNC_RenderComponent(isNotRenderProperty),
			"SetParameter", sol::overload(
				REG_CAMERACOMPONENT_SET_PARAM_FUNC(float),
				REG_CAMERACOMPONENT_SET_PARAM_FUNC(Math::VectorArrayF),
				REG_CAMERACOMPONENT_SET_PARAM_FUNC(Math::Vec2),
				REG_CAMERACOMPONENT_SET_PARAM_FUNC(Math::Vector2ArrayF),
				REG_CAMERACOMPONENT_SET_PARAM_FUNC(Math::Vec3),
				REG_CAMERACOMPONENT_SET_PARAM_FUNC(Math::Vector3ArrayF),
				REG_CAMERACOMPONENT_SET_PARAM_FUNC(Math::Vec4),
				REG_CAMERACOMPONENT_SET_PARAM_FUNC(Math::Vector4ArrayF),
				REG_CAMERACOMPONENT_SET_PARAM_FUNC(Math::FColor),
				REG_CAMERACOMPONENT_SET_PARAM_FUNC(Math::Mat4),
				REG_CAMERACOMPONENT_SET_PARAM_FUNC(TextureEntity*))
			);
#undef REG_CAMERACOMPONENT_SET_PARAM_FUNC
	}

	{   // LightComponent
		ns_table.new_usertype<LightComponent>("LightComponent",
			sol::base_classes, sol::bases<Component, Object>(),
			REGISTER_OBJECT_FUNC(LightComponent),
			REGISTER_MEMBER_FUNC_LightComponent(GetColor),
			REGISTER_MEMBER_FUNC_LightComponent(GetWorldPosition),
			REGISTER_MEMBER_FUNC_LightComponent(SetColor),
			REGISTER_MEMBER_FUNC_LightComponent(SetRange),
			REGISTER_MEMBER_FUNC_LightComponent(GetRange),
			REGISTER_MEMBER_FUNC_LightComponent(GetAttenuation),
			REGISTER_MEMBER_FUNC_LightComponent(GetWorldDirection),
			REGISTER_MEMBER_FUNC_LightComponent(GetWorldUp),
			REGISTER_MEMBER_FUNC_LightComponent(SetLightAngle),
			REGISTER_MEMBER_FUNC_LightComponent(GetLightInnerDiffInv),
			REGISTER_MEMBER_FUNC_LightComponent(GetLightAngle),
			REGISTER_MEMBER_FUNC_LightComponent(SetIntensity),
			REGISTER_MEMBER_FUNC_LightComponent(GetIntensity),
			REGISTER_MEMBER_FUNC_LightComponent(GetLightType),
			"SetAttenuation", sol::overload(
				(void (LightComponent::*)(float, float, float, float)) & LightComponent::SetAttenuation,
				(void (LightComponent::*)(const Math::Vec4&)) & LightComponent::SetAttenuation)
			);
	}

	{	// CameraComponent
		ns_table.new_usertype<CameraComponent>("CameraComponent",
			sol::base_classes, sol::bases<Component, Object>(),
			REGISTER_OBJECT_FUNC(CameraComponent),
			REGISTER_MEMBER_FUNC_CameraComponent(GetClearColor),
			REGISTER_MEMBER_FUNC_CameraComponent(SetClearColor),
			REGISTER_MEMBER_FUNC_CameraComponent(GetProjType),
			REGISTER_MEMBER_FUNC_CameraComponent(SetProjType),
			REGISTER_MEMBER_FUNC_CameraComponent(GetCameraResolution),
			REGISTER_MEMBER_FUNC_CameraComponent(GetFov),
			REGISTER_MEMBER_FUNC_CameraComponent(SetFov),
			REGISTER_MEMBER_FUNC_CameraComponent(GetNear),
			REGISTER_MEMBER_FUNC_CameraComponent(SetNear),
			REGISTER_MEMBER_FUNC_CameraComponent(GetFar),
			REGISTER_MEMBER_FUNC_CameraComponent(SetFar),
			REGISTER_MEMBER_FUNC_CameraComponent(LookAt),
			REGISTER_MEMBER_FUNC_CameraComponent(GetForward),
			REGISTER_MEMBER_FUNC_CameraComponent(GetPosition),
			REGISTER_MEMBER_FUNC_CameraComponent(GetLookAt),
			REGISTER_MEMBER_FUNC_CameraComponent(GetUp),
			REGISTER_MEMBER_FUNC_CameraComponent(GetProject),
			REGISTER_MEMBER_FUNC_CameraComponent(GetViewProj),
			REGISTER_MEMBER_FUNC_CameraComponent(GetView),
			REGISTER_MEMBER_FUNC_CameraComponent(GetUnViewProj),
			REGISTER_MEMBER_FUNC_CameraComponent(GetUnView),
			REGISTER_MEMBER_FUNC_CameraComponent(GetUnProject),
			REGISTER_MEMBER_FUNC_CameraComponent(AddLayerMask),
			REGISTER_MEMBER_FUNC_CameraComponent(EraseLayerMask),
			REGISTER_MEMBER_FUNC_CameraComponent(SetLayerMaskNothing),
			REGISTER_MEMBER_FUNC_CameraComponent(SetLayerMaskEverything),
			REGISTER_MEMBER_FUNC_CameraComponent(isLayerMask),
			REGISTER_MEMBER_FUNC_CameraComponent(GetLayerMask),
			REGISTER_MEMBER_FUNC_CameraComponent(SetLayerMask),
			REGISTER_MEMBER_FUNC_CameraComponent(SetSequence),
			REGISTER_MEMBER_FUNC_CameraComponent(GetSequence),
			REGISTER_MEMBER_FUNC_CameraComponent(SetSequenceCulling),
			REGISTER_MEMBER_FUNC_CameraComponent(isSequenceCulling),
			REGISTER_MEMBER_FUNC_CameraComponent(SetPosition),
			REGISTER_MEMBER_FUNC_CameraComponent(SetForward),
			REGISTER_MEMBER_FUNC_CameraComponent(SetUp),
			REGISTER_MEMBER_FUNC_CameraComponent(CreateProjection),
			REGISTER_MEMBER_FUNC_CameraComponent(CreatePerspectiveProjection),
			REGISTER_MEMBER_FUNC_CameraComponent(CreateOrthographiProjection),
			REGISTER_MEMBER_FUNC_CameraComponent(SetPipelineType),
			REGISTER_MEMBER_FUNC_CameraComponent(ChangeResolution),
			REGISTER_MEMBER_FUNC_CameraComponent(AttachRenderTarget),
			REGISTER_MEMBER_FUNC_CameraComponent(DetachRenderTarget),
			REGISTER_MEMBER_FUNC_CameraComponent(GetRenderWindow),
			REGISTER_MEMBER_FUNC_CameraComponent(GetAttachedRenderTarget),
			REGISTER_MEMBER_FUNC_CameraComponent(Recalculate),
			"Activate", [](CameraComponent& camera) { camera.SetActive(true); },
			"Deactivate", [](CameraComponent& camera) { camera.SetActive(false); }
			);
	}
	{	// 由于这个绑定lua库不能自动识别基类的真正类型，需要以下强制类型转换
		ns_table.set_function("ToComTransform", [](Component* com)->TransformComponent* { return static_cast<TransformComponent*>(com); });
		ns_table.set_function("ToComLight",		[](Component* com)->LightComponent* { return static_cast<LightComponent*>(com); });
		ns_table.set_function("ToComCamera",	[](Component* com)->CameraComponent* { return static_cast<CameraComponent*>(com); });
		ns_table.set_function("ToComRender",	[](Component* com)->RenderComponent* { return static_cast<RenderComponent*>(com); });
		ns_table.set_function("ToScene",		[](Object* com)->Scene* { return static_cast<Scene*>(com); });
		ns_table.set_function("ToGameObject",	[](Object* com)->GObject* { return static_cast<GObject*>(com); });
	}

	{	// ProjectSetting
		ns_table.new_usertype<ProjectSetting>("ProjectSetting",
			"Instance", &ProjectSetting::Instance,
			"SetCurrentAdvancedShading", &ProjectSetting::SetCurrentAdvancedShading,
			"IsCurrentAdvancedShading", &ProjectSetting::IsCurrentAdvancedShading
			);
	}

	{	// Engine
		ns_table.new_usertype<Engine>("Engine",
			"Instance", &Engine::Instance,
			"AddSynchronizeUpdateCallback", &Engine::AddSynchronizeUpdateCallback,
			"RemoveSynchronizeUpdateCallback", &Engine::RemoveSynchronizeUpdateCallback,
			"AddTouchCallback", &Engine::AddTouchCallback,
			"RemoveTouchCallback", &Engine::RemoveTouchCallback,
			"AddKeyboardCallback", &Engine::AddKeyboardCallback,
			"RemoveKeyboardCallback", &Engine::RemoveKeyboardCallback
			);
	}
}


NS_JYE_END

