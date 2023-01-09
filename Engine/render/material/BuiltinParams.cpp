#include "BuiltinParams.h"
#include "Core/Interface/ILogSystem.h"

NS_JYE_BEGIN

struct BuiltinType
{
	RHIDefine::VariablesType varType;
	RHIDefine::ParameterSlot shaderSlot;
};

static BuiltinType s_BuiltinParams[] =
{
	{RHIDefine::VT_CONSTBUFFER, RHIDefine::PS_INSTANCE_CBUFFER0},
	{RHIDefine::VT_CONSTBUFFER, RHIDefine::PS_INSTANCE_CBUFFER1},

	{RHIDefine::VT_FLOAT, RHIDefine::PS_SYSTEM_TIME},
	{RHIDefine::VT_VEC4_ARRAY, RHIDefine::PS_ANIMATION_REAL_ARRAY},
	{RHIDefine::VT_VEC4_ARRAY, RHIDefine::PS_ANIMATION_DUAL_ARRAY},
	{RHIDefine::VT_VEC3_ARRAY, RHIDefine::PS_ANIMATION_SCALE_ARRAY},
	{RHIDefine::VT_VEC4_ARRAY, RHIDefine::PS_ANIMATION_MATRIX_ARRAY},
	{ RHIDefine::VT_FLOAT, RHIDefine::PS_SYSTEM_TIME },

	{RHIDefine::VT_MAT4, RHIDefine::PS_LOCALWORLD_TRANSFORM},
	{RHIDefine::VT_MAT4, RHIDefine::PS_WORLDLOCAL_TRANSFROM},
	{RHIDefine::VT_MAT3, RHIDefine::PS_LOCALWORLD_ROTATION},
	{RHIDefine::VT_VEC3, RHIDefine::PS_WORLD_POSITION},
	{RHIDefine::VT_VEC3, RHIDefine::PS_WORLD_SCALE},
	{RHIDefine::VT_MAT4, RHIDefine::PS_LOCALSCREEN_TRANSVIEWPROJ},

	{ RHIDefine::VT_MAT3, RHIDefine::PS_CAMERA_WORLDROTATION },
	{ RHIDefine::VT_VEC3, RHIDefine::PS_CAMERA_WORLDPOSITION },
	{ RHIDefine::VT_VEC3, RHIDefine::PS_CAMERA_WORLDDIRECTION },
	{ RHIDefine::VT_MAT4, RHIDefine::PS_CAMERA_VIEW },
	{ RHIDefine::VT_MAT4, RHIDefine::PS_CAMERA_PROJECTION },
	{ RHIDefine::VT_MAT4, RHIDefine::PS_CAMERA_VIEWPROJ },
	{ RHIDefine::VT_MAT4, RHIDefine::PS_CAMERA_VIEWPROJ_INV },
	{ RHIDefine::VT_VEC3, RHIDefine::PS_CAMERA_LINERPARAM },
	{ RHIDefine::VT_VEC3, RHIDefine::PS_CAMERA_LINEARPARAMBIAS },
	{ RHIDefine::VT_VEC2, RHIDefine::PS_CAMERA_RESOLUTION },
	{ RHIDefine::VT_VEC2, RHIDefine::PS_CAMERA_RESOLUTION_INV },
	{ RHIDefine::VT_VEC2, RHIDefine::PS_CAMERA_RESOLUTION_HALF_INV },
	{ RHIDefine::VT_VEC3, RHIDefine::PS_CAMERA_SHADOWRANGE },

	{ RHIDefine::VT_VEC3, RHIDefine::PS_AMBIENT_COLOR },

	{ RHIDefine::VT_VEC3, RHIDefine::PS_LIGHT_POSITION },
	{ RHIDefine::VT_VEC3, RHIDefine::PS_LIGHT_GIVEN_DIRECTION },
	{ RHIDefine::VT_VEC3, RHIDefine::PS_LIGHT_COLOR },
	{ RHIDefine::VT_VEC2, RHIDefine::PS_LIGHT_ANGLE },
	{ RHIDefine::VT_VEC2, RHIDefine::PS_LIGHT_INNER_DIFF_INV },
	{ RHIDefine::VT_FLOAT, RHIDefine::PS_LIGHT_RANGE_INV },
	{ RHIDefine::VT_VEC4, RHIDefine::PS_LIGHT_ATTENUATION },
	{ RHIDefine::VT_VEC4, RHIDefine::PS_LIGHT_PARAM },
	{ RHIDefine::VT_MAT4, RHIDefine::PS_LIGHT_CAMERA_VIEW },
	{ RHIDefine::VT_MAT4, RHIDefine::PS_LIGHT_CAMERA_PROJECTION },
	{ RHIDefine::VT_VEC3, RHIDefine::PS_LIGHT_CAMERA_LINEARPARAM },
	{ RHIDefine::VT_VEC3, RHIDefine::PS_LIGHT_CAMERA_POSITION },
	{ RHIDefine::VT_FLOAT, RHIDefine::PS_DEVICE_COORDINATE_Y_FLIP },

	{ RHIDefine::VT_TEXTURE2D, RHIDefine::PS_TEXTURE_SHADOW_DEPTH },
	{ RHIDefine::VT_TEXTURE2D, RHIDefine::PS_TEXTURE_HAIR_SHADOW_DEPTH },
	{ RHIDefine::VT_TEXTURE2D, RHIDefine::PS_TEXTURE_HAIR_SHADOW_MASK},
	{ RHIDefine::VT_TEXTURE2D, RHIDefine::PS_TEXTURE_GRAP },
	{ RHIDefine::VT_TEXTURE2D, RHIDefine::PS_TEXTURE_GBUFFER_A },
	{ RHIDefine::VT_TEXTURE2D, RHIDefine::PS_TEXTURE_GBUFFER_B },
	{ RHIDefine::VT_TEXTURE2D, RHIDefine::PS_TEXTURE_GBUFFER_C },
	{ RHIDefine::VT_TEXTURE2D, RHIDefine::PS_TEXTURE_GBUFFER_DEPTH },
	{ RHIDefine::VT_TEXTURE2D, RHIDefine::PS_TEXTURE_SCENE_DEPTH },

	{ RHIDefine::VT_TEXTURE2D, RHIDefine::PS_HAIR_FIRSTLAYER_DEPTH },
	{ RHIDefine::VT_TEXTURE2D, RHIDefine::PS_HAIR_SECONDLAYER_DEPTH },
	{ RHIDefine::VT_TEXTURE2D, RHIDefine::PS_HAIR_THIRDLAYER_DEPTH },
};

BuiltinParams::BuiltinParams()
{
	m_matrix44f.reserve(100);
	m_vector3f.reserve(100);
	m_vector4f.reserve(100);
	m_vector2f.reserve(10);
	m_vector1f.reserve(10);
	m_Bits.reset();
	m_FlushFlag = false;

	for (int i = 0; i < sizeof(s_BuiltinParams) / sizeof(s_BuiltinParams[0]); i++)
	{
		BuiltinType& curValue = s_BuiltinParams[i];
		AddParam(curValue.varType, curValue.shaderSlot);
	}
}

BuiltinParams::~BuiltinParams()
{
	m_Params.clear();
}

void BuiltinParams::AddParam(RHIDefine::VariablesType type, RHIDefine::ParameterSlot slot)
{
	BuilitParamVal paramVal;
	paramVal.type = type;
	paramVal.slot = slot;
	paramVal.index = m_Params.size();

	m_Params.push_back(paramVal);
}

int BuiltinParams::GetBindIndex(RHIDefine::ParameterSlot slot)
{
	int bindIndex = -1;
	for (int i = 0; i < sizeof(s_BuiltinParams) / sizeof(s_BuiltinParams[0]); i++)
	{
		BuiltinType& curValue = s_BuiltinParams[i];
		if (curValue.shaderSlot == slot)
		{
			bindIndex = i;
			break;
		}
	}

	return bindIndex;
}

BuiltinFlag BuiltinParams::QueryFlag(Vector<uint16>& indexList)
{
	BuiltinFlag flag;
	for (int i = 0; i < indexList.size(); i++)
	{
		flag.set(indexList[i]);
	}

	return flag;
}

void BuiltinParams::SetValue(int bindIndex, MaterialParameter* pSrcObj)
{
	if (bindIndex < 0 || bindIndex >= m_Params.size())
	{
		JYLOGTAG(LogEngineRHI, "BuiltinParams::SetValue invalid bindIndex %d", bindIndex);
		return;
	}

	BuilitParamVal& paramV = GetValue(bindIndex);
	if (m_FlushFlag)
	{
		m_Bits.set(bindIndex, 1);
	}

	paramV.pObj = pSrcObj;
}

NS_JYE_END