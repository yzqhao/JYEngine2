#pragma once

#include "RHI/RHI.h"
#include "Engine/private/Define.h"
#include "MaterialParameter.h"
#include "Math/Vec1.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Math/Mat4.h"
#include "Core/Object.h"

#include <bitset>

NS_JYE_BEGIN

class Texture;

#define MaxBuiltinCount 128
typedef std::bitset<MaxBuiltinCount> BuiltinFlag;

class ENGINE_API BuiltinParams : public Reference
{
public:
	struct BuilitParamVal
	{
		RHIDefine::VariablesType type;
		RHIDefine::ParameterSlot slot;
		short index;
		MaterialParameterPtr pObj{};
	};
private:
	/**
	* a private variable.
	* unsigned short array used for constBuffer
	*/
	Vector<Math::Vec2> m_vector2f;
	Vector<Math::Vec1> m_vector1f;
	Vector<Math::Vec3> m_vector3f;
	Vector<Math::Vec4> m_vector4f;
	Vector<Math::Mat4> m_matrix44f;
	Vector<Texture*> m_texture2D;
	Vector<BuiltinParams::BuilitParamVal> m_Params;
	BuiltinFlag m_Bits;
	bool m_FlushFlag;
public:
	//static BuiltinParams& Instance();
	BuiltinParams();
	~BuiltinParams();

	static int GetBindIndex(RHIDefine::ParameterSlot slot);
	static BuiltinFlag QueryFlag(Vector<uint16>& indexList);

	inline void BeginRecordFlag()
	{
		m_FlushFlag = true;
		m_Bits.reset();
	}

	inline BuiltinFlag& EndRecordFlag()
	{
		m_FlushFlag = false;
		return m_Bits;
	}

	inline BuilitParamVal& GetValue(int bindIndex)
	{
		return m_Params[bindIndex];
	}

	void SetValue(int bindIndex, MaterialParameter* pSrcObj);
private:
	void AddParam(RHIDefine::VariablesType type, RHIDefine::ParameterSlot slot);
};
DECLARE_Ptr(BuiltinParams);

#define SetBuiltParam(builtin, slot, param) {\
								static int paramIndex = builtin->GetBindIndex(slot);\
								builtin->SetValue(paramIndex, param);\
							}
NS_JYE_END