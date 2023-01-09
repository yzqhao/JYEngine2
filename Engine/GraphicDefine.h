
#pragma once

#include "private/Define.h"
#include "Core/EnumClassFlags.h"
#include "RHI/RHIDefine.h"
#include "render/IndicesStream.h"
#include "render/VertexStream.h"
#include "Math/AABB.h"

NS_JYE_BEGIN

class IMetadata;
class RenderComponent;
class LightComponent;

namespace GraphicDefine {

	static const char* NORMAL_MESH = "mesh";
	static const char* SELF_DEFINED_MESH = "dynamicmesh";

	enum RenderProperty
	{
		RP_SHOW = 1 << 0UL,
		RP_CULL = 1 << 1UL,
		RP_LIGHT = 1 << 2UL,
		RP_SHADOW_CASTER = 1 << 3UL,
		RP_SHADOW_RECEIVER = 1 << 4UL,
		RP_IGNORE_PICK = 1 << 5UL,
		RP_SKINANI = 1 << 6UL,

		RP_DEFAULT = RP_SHOW | RP_CULL | RP_LIGHT | RP_SHADOW_CASTER | RP_SHADOW_RECEIVER,
	};

	enum PiplineType
	{
		PP_NONE = -1, /**< none pipeline */
		PP_FORWARD_LIGHTING, /**< forward pipeline */
		PP_UI,   /**< ui pipeline */
		PP_DEFERRED_LIGHTING, /**< deferred pipeline */
		PP_DEPTH, /**< depth pipeline or shadow pipeline */
	};

	enum CameraProjectionType
	{
		CPT_PERSPECTIVE = 0,
		CPT_ORTHOGRAPHI,
		CPT_UICAMERA,
	};

	enum LightType
	{
		LT_AMBIENT = 0,
		LT_DIRECTIONAL,
		LT_POINT,
		LT_SPOT,

		LI_COUNT,
	};

	enum ShadowType
	{
		NO_SHADOW = 0,
		OPAQUE_SHADOW,
		ALL_SHADOW,
	};
	enum SoftShadowType
	{
		SST_PCF3X3 = 1,
		SST_PCF5X5,
		SST_PCF7X7,
		SST_NoHardware,
	};

	enum GetAttribute
	{
		GA_WORLD_TRANSFORM = 0, //缩放，旋转，平移的一个矩阵
		GA_WORLD_TRANSFORM_INVERS, //缩放，旋转，平移的一个矩阵的逆矩阵
		GA_WORLD_POSITION,
		GA_WORLD_SCALE,
		GA_WORLD_ROTATION,
		GA_NORMAL_TRANSFORM,//法线的矩阵，只有旋转

		GA_WORLD_VELOCITY,//速度
		GA_WORLD_FORWARLD,//朝向
		GA_WORLD_UP,//方向

		GA_ANIMATION_REAL,
		GA_ANIMATION_DUAL,
		GA_ANIMATION_SCALE,
		GA_ANIMATION_MATRIX,

		GA_COUNT,
	};

	struct VertexAttribute
	{
		RHIDefine::ShaderAttribute	slot;
		int size;
	};

	struct MeshStreamData
	{
		IndicesStream m_Indices;
		std::vector<int> m_TrianglesCnt;
		VertexStream m_Vertex;
		RHIDefine::RenderMode m_eMode;
		std::vector<Vector<int>> m_SplitedJointsIDs;
		std::vector<std::shared_ptr<Vector<Math::Vec3>>> m_pTargets;
		std::vector<int32_t> m_pTargetIds;
		Math::AABB m_BindingBox;
	};

	typedef Vector< IMetadata* > RenderMetadataVector;
	typedef Vector< RenderComponent* >	VisableRender;
	typedef Vector< LightComponent* >	VisableLight;

}

NS_JYE_END