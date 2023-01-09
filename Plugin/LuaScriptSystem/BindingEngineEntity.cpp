
#include "BindingEngine.h"
#include "BindingCore.h"

#include "Engine/render/SubMesh.h"
#include "Engine/render/RenderObjectEntity.h"
#include "Engine/render/material/MaterialEntity.h"
#include "Engine/render/RenderTargetEntity.h"
#include "Engine/render/texture/TextureFormat.h"
#include "Engine/resource/MaterialMetadata.h"
#include "Engine/resource/RenderTargetMetadata.h"
#include "Engine/resource/ShapeBufferUtility.h"
#include "Engine/resource/TextureMetadata.h"
#include "Engine/resource/VertexMetadata.h"
#include "Engine/resource/IndicesMetadata.h"

NS_JYE_BEGIN

void Bind_Engine_Entity(sol::state& sol_state)
{
	auto ns_table = sol_state["Engine"].get_or_create<sol::table>();

#define REGISTER_ENTITY_FUNC(entity) \
	sol::base_classes, sol::bases<IResourceEntity, Object>(),	\
	REGISTER_OBJECT_FUNC(entity), \
	"CreateResource", & entity::CreateResource,	\
	"ClearMetadata", & entity::ClearMetadata,	\
	"GetSourceMetadata", & entity::GetSourceMetadata,	\
	"isSuccess", & entity::isSuccess,	\
	"PushMetadata", sol::overload(	\
		(void (entity::*)(const IMetadata&)) & entity::PushMetadata,	\
		(void (entity::*)(const ISourceMetadataList& metalist)) & entity::PushMetadata),

	{	// IResourceEntity
		ns_table.new_usertype<IResourceEntity>("IResourceEntity", 
			sol::base_classes, sol::bases<Object>(),
			REGISTER_OBJECT_FUNC(IResourceEntity),
			"isReady", &IResourceEntity::isReady,
			"isPushIntoQueue", &IResourceEntity::isPushIntoQueue,
			"GetHashCode", sol::overload(
				(size_t(IResourceEntity::*)()const) & IResourceEntity::GetHashCode,
				(size_t(IResourceEntity::*)()) & IResourceEntity::GetHashCode)
			);
	}

	{
		{	// MaterialParameter
			ns_table.new_usertype<MaterialParameter>("MaterialParameter", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter)
				);
		}

		{	// MaterialParameterMat4
			ns_table.new_usertype<MaterialParameterMat4>("MaterialParameterMat4", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<MaterialParameter, Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter),
				"m_mat4", &MaterialParameterMat4::m_mat4
				);
		}

		{	// MaterialParameterMat4Array
			ns_table.new_usertype<MaterialParameterMat4Array>("MaterialParameterMat4Array", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<MaterialParameter, Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter),
				"m_matArray", &MaterialParameterMat4Array::m_matArray
				);
		}

		{	// MaterialParameterVec4
			ns_table.new_usertype<MaterialParameterVec4>("MaterialParameterVec4", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<MaterialParameter, Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter),
				"m_vec4", &MaterialParameterVec4::m_vec4
				);
		}

		{	// MaterialParameterVec4Array
			ns_table.new_usertype<MaterialParameterVec4Array>("MaterialParameterVec4Array", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<MaterialParameter, Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter),
				"m_vecArray4", &MaterialParameterVec4Array::m_vecArray4
				);
		}

		{	// MaterialParameterVec3
			ns_table.new_usertype<MaterialParameterVec3>("MaterialParameterVec3", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<MaterialParameter, Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter),
				"m_vec3", &MaterialParameterVec3::m_vec3
				);
		}

		{	// MaterialParameterVec3Array
			ns_table.new_usertype<MaterialParameterVec3Array>("MaterialParameterVec3Array", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<MaterialParameter, Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter),
				"m_vecArray3", &MaterialParameterVec3Array::m_vecArray3
				);
		}

		{	// MaterialParameterVec2
			ns_table.new_usertype<MaterialParameterVec2>("MaterialParameterVec2", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<MaterialParameter, Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter),
				"m_vec2", &MaterialParameterVec2::m_vec2
				);
		}

		{	// MaterialParameterVec2Array
			ns_table.new_usertype<MaterialParameterVec2Array>("MaterialParameterVec2Array", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<MaterialParameter, Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter),
				"m_vecArray2", &MaterialParameterVec2Array::m_vecArray2
				);
		}

		{	// MaterialParameterVecArray
			ns_table.new_usertype<MaterialParameterVecArray>("MaterialParameterVecArray", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<MaterialParameter, Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter),
				"m_vecArray", &MaterialParameterVecArray::m_vecArray
				);
		}

		{	// MaterialParameterFloat
			ns_table.new_usertype<MaterialParameterFloat>("MaterialParameterFloat", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<MaterialParameter, Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter),
				"m_float", &MaterialParameterFloat::m_float
				);
		}

		{	// MaterialParameterColor
			ns_table.new_usertype<MaterialParameterColor>("MaterialParameterColor", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<MaterialParameter, Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter),
				"m_color", &MaterialParameterColor::m_color
				);
		}

		{	// MaterialParameterTex
			ns_table.new_usertype<MaterialParameterTex>("MaterialParameterTex", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<MaterialParameter, Object>(),
				REGISTER_OBJECT_FUNC(MaterialParameter),
				"GetTex", &MaterialParameterTex::GetTex
				);
		}
	}

	{
#define REG_MATERIAL_SET_PARAM_FUNC(valtype) \
	[](MaterialEntity& mat, RHIDefine::ParameterSlot slot, valtype val) { mat.SetParameter(slot, MakeMaterialParam(val)); },	\
	[](MaterialEntity& mat, const std::string& slot, valtype val) { mat.SetParameter(slot, MakeMaterialParam(val)); }

	// MaterialEntity
	ns_table.new_usertype<MaterialEntity>("MaterialEntity", sol::call_constructor, sol::constructors<
		sol::types<>>(),
		REGISTER_ENTITY_FUNC(MaterialEntity)
		"EnableKeyWord", &MaterialEntity::EnableKeyWord,
		"DisableKeyWord", &MaterialEntity::DisableKeyWord,
		"DisableAllKeyWord", &MaterialEntity::DisableAllKeyWord,
		"IsKeyWordEnable", &MaterialEntity::IsKeyWordEnable,
		"hasParameter", &MaterialEntity::hasParameter,
		"GetMaterialName", &MaterialEntity::GetMaterialName,
		"IsSupportPass", &MaterialEntity::IsSupportPass,
		"SetParameter", sol::overload(
			(bool (MaterialEntity::*)(RHIDefine::ParameterSlot, MaterialParameter*))& MaterialEntity::SetParameter,
			(bool (MaterialEntity::*)(const std::string&, MaterialParameter*))& MaterialEntity::SetParameter,
			REG_MATERIAL_SET_PARAM_FUNC(float),
			REG_MATERIAL_SET_PARAM_FUNC(Math::VectorArrayF),
			REG_MATERIAL_SET_PARAM_FUNC(Math::Vec2),
			REG_MATERIAL_SET_PARAM_FUNC(Math::Vector2ArrayF),
			REG_MATERIAL_SET_PARAM_FUNC(Math::Vec3),
			REG_MATERIAL_SET_PARAM_FUNC(Math::Vector3ArrayF),
			REG_MATERIAL_SET_PARAM_FUNC(Math::Vec4),
			REG_MATERIAL_SET_PARAM_FUNC(Math::Vector4ArrayF),
			REG_MATERIAL_SET_PARAM_FUNC(Math::FColor),
			REG_MATERIAL_SET_PARAM_FUNC(Math::Mat4),
			REG_MATERIAL_SET_PARAM_FUNC(TextureEntity*)
			),
			"GetParameter", sol::overload(
				(MaterialParameter* (MaterialEntity::*)(RHIDefine::ParameterSlot))& MaterialEntity::GetParameter,
				(MaterialParameter* (MaterialEntity::*)(const std::string&))& MaterialEntity::GetParameter),
			"GetShaderID", &MaterialEntity::GetShaderID
			);
#undef REG_MATERIAL_SET_PARAM_FUNC
	}

	{	// IndicesStream
		ns_table.new_usertype<IndicesStream>("IndicesStream", sol::call_constructor, sol::constructors<
			sol::types<>>(),
			sol::base_classes, sol::bases<Object>(),
			REGISTER_OBJECT_FUNC(IndicesStream),
			"SetIndicesType", &IndicesStream::SetIndicesType,
			"ReserveBuffer", &IndicesStream::ReserveBuffer,
			"GetIndicesData", &IndicesStream::GetIndicesData,
			"CopyIndicesBuffer", &IndicesStream::CopyIndicesBuffer,
			"PushIndicesData", &IndicesStream::PushIndicesData,
			"PushIndicesDataFast", &IndicesStream::PushIndicesDataFast,
			"GetIndicesType", &IndicesStream::GetIndicesType,
			"GetBuffer", &IndicesStream::GetBuffer,
			"GetIndicesCount", &IndicesStream::GetIndicesCount,
			"GetByteSize", &IndicesStream::GetByteSize,
			"GetIndicesStride", &IndicesStream::GetIndicesStride,
			"Clear", &IndicesStream::Clear
			);
	}

	{	// IndicesBufferEntity
		ns_table.new_usertype<IndicesBufferEntity>("IndicesBufferEntity", sol::call_constructor, sol::constructors<
			sol::types<>>(),
			REGISTER_ENTITY_FUNC(IndicesBufferEntity)
			"GetIndexStream", &IndicesBufferEntity::GetIndexStream,
			"GetIndicesCount", &IndicesBufferEntity::GetIndicesCount,
			"FlushIndiesBuffer", &IndicesBufferEntity::FlushIndiesBuffer,
			"ChangeIndiesBuffer", &IndicesBufferEntity::ChangeIndiesBuffer,
			"GetIndicesType", &IndicesBufferEntity::GetIndicesType,
			"SetKeepSource", &IndicesBufferEntity::SetKeepSource,
			"isKeepSource", &IndicesBufferEntity::isKeepSource,
			"GetGpuHandle", &IndicesBufferEntity::GetGpuHandle,
			"GetIndicesCount", &IndicesBufferEntity::GetIndicesCount,
			"GetIndexTypeSize", &IndicesBufferEntity::GetIndexTypeSize
			);
	}

	{	// VertexStreamLayout
		ns_table.new_usertype<VertexStreamLayout>("VertexStreamLayout", sol::call_constructor, sol::constructors<
			sol::types<>>(),
			"Clear", &VertexStreamLayout::Clear,
			"SetVertexLayout", &VertexStreamLayout::SetVertexLayout,
			"GetStride", &VertexStreamLayout::GetStride
			);
	}

	{	// VertexStream
		ns_table.new_usertype<VertexStream>("VertexStream", sol::call_constructor, sol::constructors<
			sol::types<>>(),
			sol::base_classes, sol::bases<Object>(),
			REGISTER_OBJECT_FUNC(VertexStream),
			"ReserveBuffer", &VertexStream::ReserveBuffer,
			"Clear", &VertexStream::Clear,
			"GetVerticesData", &VertexStream::GetVerticesData,
			"SetVertexType", sol::overload(
				(void (VertexStream::*)(RHIDefine::ShaderAttribute sa, int count))& VertexStream::SetVertexType,
				(void (VertexStream::*)(RHIDefine::ShaderAttribute sa, RHIDefine::DataType source, RHIDefine::DataType dest, int count))& VertexStream::SetVertexType),
			"PushVertexData", &VertexStream::PushVertexData,
			"PushVertexDataFast", &VertexStream::PushVertexDataFast,
			"ChangeVertexData", &VertexStream::ChangeVertexData,
			"ChangeVertexDataWithAttributeFast", &VertexStream::ChangeVertexDataWithAttributeFast,
			"GetAttributeIndex", &VertexStream::GetAttributeIndex,
			"ResetFlushInterval", &VertexStream::ResetFlushInterval,
			"GetVertexStride", &VertexStream::GetVertexStride,
			"GetVertexSize", &VertexStream::GetVertexSize,
			"GetVertexCount", &VertexStream::GetVertexCount,
			"GetByteSize", &VertexStream::GetByteSize,
			"hasVertex", &VertexStream::hasVertex,
			"SetReflushInterval", &VertexStream::SetReflushInterval,
			"GetReflushInterval", &VertexStream::GetReflushInterval,
			"isValid", &VertexStream::isValid,
			"GetFlag", &VertexStream::GetFlag,
			"SetFlag", &VertexStream::SetFlag
			);
	}

	{	// VertexBufferEntity
		ns_table.new_usertype<VertexBufferEntity>("VertexBufferEntity", sol::call_constructor, sol::constructors<
			sol::types<>>(),
			REGISTER_ENTITY_FUNC(VertexBufferEntity)
			"GetVertexStream", &VertexBufferEntity::GetVertexStream,
			"GetMemoryUsage", &VertexBufferEntity::GetMemoryUsage,
			"FlushVertexBuffer", &VertexBufferEntity::FlushVertexBuffer,
			"ChangeVertexBuffer", &VertexBufferEntity::ChangeVertexBuffer,
			"GetVertexSize", &VertexBufferEntity::GetVertexSize,
			"GetVertexCount", &VertexBufferEntity::GetVertexCount,
			"SetKeepSource", &VertexBufferEntity::SetKeepSource,
			"isKeepSource", &VertexBufferEntity::isKeepSource,
			"GetGpuHandle", &VertexBufferEntity::GetGpuHandle
			);
	}

	{	// SubMesh
		ns_table.new_usertype<SubMesh>("SubMesh", sol::call_constructor, sol::constructors<
			sol::types<>>(),
			sol::base_classes, sol::bases<Object>(),
			REGISTER_OBJECT_FUNC(SubMesh),
			"Enable", &SubMesh::Enable,
			"Disable", &SubMesh::Disable,
			"isEnable", &SubMesh::isEnable,
			"SetDrawRange", &SubMesh::SetDrawRange,
			"ReculateHash", &SubMesh::ReculateHash,
			"GetHash", &SubMesh::GetHash,
			"GetDrawRange", &SubMesh::GetDrawRange
			);
	}

	{	// RenderObjectEntity
		ns_table.new_usertype<RenderObjectEntity>("RenderObjectEntity", sol::call_constructor, sol::constructors<
			sol::types<>>(),
			REGISTER_ENTITY_FUNC(RenderObjectEntity)
			"GetVertexStream", &RenderObjectEntity::GetVertexStream,
			"GetIndexStream", &RenderObjectEntity::GetIndexStream,
			"GetAffectedIDs", &RenderObjectEntity::GetAffectedIDs,
			"CreateSubMesh", &RenderObjectEntity::CreateSubMesh,
			"DisableSubMeshs", &RenderObjectEntity::DisableSubMeshs,
			"GetDrawCount", &RenderObjectEntity::GetDrawCount,
			"ChangeVertexBuffer", &RenderObjectEntity::ChangeVertexBuffer,
			"ChangeIndexBuffer", &RenderObjectEntity::ChangeIndexBuffer,
			"FlushVertexBuffer", &RenderObjectEntity::FlushVertexBuffer,
			"FlushIndiesBuffer", &RenderObjectEntity::FlushIndiesBuffer,
			"GetRenderMode", &RenderObjectEntity::GetRenderMode,
			"SetKeepSource", &RenderObjectEntity::SetKeepSource,
			"SetDynamicBatch", &RenderObjectEntity::SetDynamicBatch,
			"isKeepSource", &RenderObjectEntity::isKeepSource,
			"GetMeshName", &RenderObjectEntity::GetMeshName,
			"GetBindBox", &RenderObjectEntity::GetBindBox,
			"GetDrawRange", &RenderObjectEntity::GetDrawRange,
			"GetSubMeshsSize", &RenderObjectEntity::GetSubMeshsSize,
			"GetVertexBufferHandle", &RenderObjectEntity::GetVertexBufferHandle,
			"GetVertexBufferEntity", &RenderObjectEntity::GetVertexBufferEntity,
			"GetIndexBufferEntity", &RenderObjectEntity::GetIndexBufferEntity,
			"GetSubMeshByIndex", &RenderObjectEntity::GetSubMeshByIndex,
			"GetSubMeshHashByIndex", &RenderObjectEntity::GetSubMeshHashByIndex
			);
	}

	{	// RenderTargetEntity
		ns_table.new_usertype<RenderTargetEntity>("RenderTargetEntity", sol::call_constructor, sol::constructors<
			sol::types<>>(),
			REGISTER_ENTITY_FUNC(RenderTargetEntity)
			"MakeTextureAttachment", &RenderTargetEntity::MakeTextureAttachment,
			"Resizeview", &RenderTargetEntity::Resizeview,
			"SetResizeviewFlag", &RenderTargetEntity::SetResizeviewFlag,
			"GetName", &RenderTargetEntity::GetName,
			"IsSRGB", &RenderTargetEntity::IsSRGB,
			"GetAttachment", &RenderTargetEntity::GetAttachment,
			"isMainRenderTarget", &RenderTargetEntity::isMainRenderTarget,
			"GetResolution", &RenderTargetEntity::GetResolution,
			"GetViewPort", &RenderTargetEntity::GetViewPort,
			"GetAttachmentStream", &RenderTargetEntity::GetAttachmentStream,
			"NeedRead", &RenderTargetEntity::NeedRead,
			"ReadBuffer", &RenderTargetEntity::ReadBuffer,
			"HasAttachment", &RenderTargetEntity::HasAttachment,
			"GetGPUHandle", &RenderTargetEntity::GetGPUHandle
			);
	}

	{	// TextureStream
		ns_table.new_usertype<TextureStream>("TextureStream", sol::call_constructor, sol::constructors<
			sol::types<>>(),
			"SetStreamType", &TextureStream::SetStreamType,
			"ConvertPixelFormat", &TextureStream::ConvertPixelFormat,
			"SetPixel", &TextureStream::SetPixel,
			"SetBuffer", &TextureStream::SetBuffer,
			"Resize", &TextureStream::Resize,
			"Merge", &TextureStream::Merge,
			"isCompressed", &TextureStream::isCompressed,
			"ReadPixel", &TextureStream::ReadPixel,
			"SetLevel", &TextureStream::SetLevel,
			"SetTextureType", &TextureStream::SetTextureType,
			"GetSize", &TextureStream::GetSize,
			"GetPixelFormat", &TextureStream::GetPixelFormat,
			"GetTextureType", &TextureStream::GetTextureType,
			"GetPixelSize", &TextureStream::GetPixelSize,
			"GetBufferSize", &TextureStream::GetBufferSize,
			"GetLevels", &TextureStream::GetLevels,
			"ClearBuffer", &TextureStream::ClearBuffer
			);
	}

	{	// TextureFormat
		auto global_ns_table = sol_state["TextureFormat"].get_or_create<sol::table>();
		global_ns_table.set_function("FormatMapping", &TextureFormat::FormatMapping);
		global_ns_table.set_function("GetMemSize", &TextureFormat::GetMemSize);
	}

	{	// TextureEntity
		ns_table.new_usertype<TextureEntity>("TextureEntity", 
			sol::call_constructor, sol::constructors<
			sol::types<>>(),
			REGISTER_ENTITY_FUNC(TextureEntity)
			"Resize", &TextureEntity::Resize,
			"SetKeepSource", &TextureEntity::SetKeepSource,
			"isKeepSource", &TextureEntity::isKeepSource,
			"GetPixelFormat", &TextureEntity::GetPixelFormat,
			"GetSize", &TextureEntity::GetSize,
			"GetTextureType", &TextureEntity::GetTextureType,
			"GetWrap", &TextureEntity::GetWrap,
			"GetMemoryUseage", &TextureEntity::GetMemoryUseage,
			"GetName", &TextureEntity::GetName,
			"SetRTAttachmentFlag", &TextureEntity::SetRTAttachmentFlag,
			"IsSRGB", &TextureEntity::IsSRGB,
			"GetHandle", &TextureEntity::GetHandle,
			"GetFlags", &TextureEntity::GetFlags
			);
	}

	{
		{	// IMetadata
			ns_table.new_usertype<IMetadata>("IMetadata",
				sol::base_classes, sol::bases<Object>(),
				REGISTER_OBJECT_FUNC(IMetadata),
				"GetIdentifier", &IMetadata::GetIdentifier,
				"ReleaseMetadate", &IMetadata::ReleaseMetadate
				);
		}

		{	// PathMetadata
			ns_table.new_usertype<PathMetadata>("PathMetadata", sol::call_constructor, sol::constructors<
				sol::types<>>(),
				sol::base_classes, sol::bases<IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(PathMetadata),
				"GetPath", &PathMetadata::GetPath,
				"SetPath", &PathMetadata::SetPath,
				"AddDependencePath", &PathMetadata::AddDependencePath,
				"SetDependencePathList", &PathMetadata::SetDependencePathList,
				"GetDependencePathList", &PathMetadata::GetDependencePathList
				);
		}

		{	// MaterialMetadata
			ns_table.new_usertype<MaterialMetadata>("MaterialMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<const std::string&>>(),
				sol::base_classes, sol::bases<PathMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(MaterialMetadata)
				);
		}

		{	// RenderObjectMeshMetadate
			ns_table.new_usertype<RenderObjectMeshMetadate>("RenderObjectMeshMetadate", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<RHIDefine::RenderMode, const IMetadata&>,
				sol::types<RHIDefine::RenderMode, const IMetadata&, const IMetadata&>>(),
				sol::base_classes, sol::bases<IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(RenderObjectMeshMetadate)
				);
		}

		{	// RenderObjectMeshFileMetadate
			ns_table.new_usertype<RenderObjectMeshFileMetadate>("RenderObjectMeshFileMetadate", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<RHIDefine::MemoryUseage, const String&, const bool>>(),
				sol::base_classes, sol::bases<IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(RenderObjectMeshFileMetadate),
				"GetMemoryUseage", &RenderObjectMeshFileMetadate::GetMemoryUseage,
				"SetMemoryUseage", &RenderObjectMeshFileMetadate::SetMemoryUseage
				);
		}

		{	// RenderTargetMetadata
			ns_table.new_usertype<RenderTargetMetadata>("RenderTargetMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<RHIDefine::TargetType, const Math::IntVec4&, const Math::IntVec2&>,
				sol::types<RHIDefine::TargetType, const Math::IntVec4&, const Math::IntVec2&, const Math::Vec2&>,
				sol::types<RHIDefine::TargetType, const Math::IntVec4&, const Math::IntVec2&, const Math::Vec2&, const bool>>(),
				sol::base_classes, sol::bases<PathMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(RenderTargetMetadata)
				);
		}

		{	// QuadIndicesMetadata
			ns_table.new_usertype<QuadIndicesMetadata>("QuadIndicesMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<RHIDefine::MemoryUseage>>(),
				sol::base_classes, sol::bases<IndicesMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(QuadIndicesMetadata)
				);
		}

		{	// QuadVertexMetadata
			ns_table.new_usertype<QuadVertexMetadata>("QuadVertexMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<RHIDefine::MemoryUseage, bool, bool>,
				sol::types<RHIDefine::MemoryUseage, bool>>(),
				sol::base_classes, sol::bases<VertexMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(QuadVertexMetadata)
				);
		}

		{	// TextureMetadata
			ns_table.new_usertype<TextureMetadata>("TextureMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<                      RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, bool, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter>,
				sol::types<                      RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, bool, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter, bool>,
				sol::types<const Math::IntVec2&, RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, bool, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter>,
				sol::types<const Math::IntVec2&, RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, bool, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter, bool>>(),
				sol::base_classes, sol::bases<PathMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(TextureMetadata)
				);
		}

		{	// TextureFileMetadata
			ns_table.new_usertype<TextureFileMetadata>("TextureFileMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, bool, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter, const std::string&, bool, bool>,
				sol::types<RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, bool, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter, const std::string&>>(),
				sol::base_classes, sol::bases<TextureMetadata, PathMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(TextureFileMetadata)
				);
		}

		{	// TextureRenderMetadata
			ns_table.new_usertype<TextureRenderMetadata>("TextureRenderMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<                    const Math::IntVec2&, RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter>,
				sol::types<                    const Math::IntVec2&, RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter, bool>,
				sol::types<                    const Math::IntVec2&, RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter, bool, bool>,
				sol::types<const std::string&, const Math::IntVec2&, RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter>,
				sol::types<const std::string&, const Math::IntVec2&, RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter, bool>,
				sol::types<const std::string&, const Math::IntVec2&, RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter, bool, bool>>(),
				sol::base_classes, sol::bases<TextureMetadata, PathMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(TextureRenderMetadata)
				);
		}

		{	// DepthRenderBufferMetadata
			ns_table.new_usertype<DepthRenderBufferMetadata>("DepthRenderBufferMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<const Math::IntVec2&, RHIDefine::PixelFormat>,
				sol::types<const Math::IntVec2&, RHIDefine::PixelFormat, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter>>(),
				sol::base_classes, sol::bases<TextureMetadata, PathMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(DepthRenderBufferMetadata)
				);
		}

		{	// TextureBufferMetadata
			ns_table.new_usertype<TextureBufferMetadata>("TextureBufferMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<const Math::IntVec2&>,
				sol::types<const Math::IntVec2&, RHIDefine::TextureType, RHIDefine::TextureUseage, RHIDefine::PixelFormat, uint, bool, uint, RHIDefine::TextureWarp, RHIDefine::TextureWarp, RHIDefine::TextureFilter, RHIDefine::TextureFilter, byte*>>(),
				sol::base_classes, sol::bases<TextureMetadata, PathMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(TextureBufferMetadata)
				);
		}

		{	// VertexMetadata
			ns_table.new_usertype<VertexMetadata>("VertexMetadata",
				sol::base_classes, sol::bases<IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(VertexMetadata),
				"GetMemoryUseage", &VertexMetadata::GetMemoryUseage
				);
		}

		{	// ReferenceVertexMetadata
			ns_table.new_usertype<ReferenceVertexMetadata>("ReferenceVertexMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<RHIDefine::MemoryUseage, VertexStream*>>(),
				sol::base_classes, sol::bases<VertexMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(ReferenceVertexMetadata)
				);
		}

		{	// FileVertexMetadata
			ns_table.new_usertype<FileVertexMetadata>("FileVertexMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<RHIDefine::MemoryUseage, const std::string&>>(),
				sol::base_classes, sol::bases<VertexMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(FileVertexMetadata)
				);
		}

		{	// IndicesMetadata
			ns_table.new_usertype<IndicesMetadata>("IndicesMetadata",
				sol::base_classes, sol::bases<IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(IndicesMetadata),
				"GetMemoryUseage", &IndicesMetadata::GetMemoryUseage
				);
		}

		{	// ReferenceIndicesMetadata
			ns_table.new_usertype<ReferenceIndicesMetadata>("ReferenceIndicesMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<RHIDefine::MemoryUseage, IndicesStream*>>(),
				sol::base_classes, sol::bases<IndicesMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(ReferenceIndicesMetadata)
				);
		}

		{	// FileIndicesMetadata
			ns_table.new_usertype<FileIndicesMetadata>("FileIndicesMetadata", sol::call_constructor, sol::constructors<
				sol::types<>,
				sol::types<RHIDefine::MemoryUseage, const std::string&>>(),
				sol::base_classes, sol::bases<IndicesMetadata, IMetadata, Object>(),
				REGISTER_OBJECT_FUNC(FileIndicesMetadata)
				);
		}
	}

}


NS_JYE_END

