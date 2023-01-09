#pragma once

#include "RHI/RHIDefine.h"
#include "Engine/private/Define.h"
#include "ParserMaterial.h"
#include "MaterialParameter.h"

#include <string>
#include <map>

NS_JYE_BEGIN

class ENGINE_API DefProperty
{
public:
	struct TexEnv
	{
		float unused;
	};

	struct DefValue
	{
		const String* pName;
		const String* pDescription;
		const String* pAttribute;
		MaterialParameterPtr pValue;
	};
private:
	RHIDefine::VariablesType m_varRenderType;
	ParserMaterial::ParamsType m_parserType;
	RHIDefine::ParameterSlot m_slot;
	String m_desc;
	String m_name;
	String m_attribute;
	String m_attHader;
	Vector<String> m_attContent;
	MaterialParameterPtr m_pDefValue;
	MaterialParameterPtr m_pRenderValue;
private:
	void FreeRenderValue();

public:
	DefProperty();
	DefProperty(const DefProperty& property);

	virtual ~DefProperty();
	void SetFloat(float x);
	void SetFloatRange(float x, float minVal, float maxVal);
	void SetFloat2(float x, float y);
	void SetFloat3(float x, float y, float z);
	void SetFloat4(float x, float y, float z, float w);
	void SetColor(float r, float g, float b, float a);
	void SetTexture1D(TextureEntity* pTex);
	void SetTexture2D(TextureEntity* pTex);
	void SetTexture3D(TextureEntity* pTex);
	void SetTextureCube(TextureEntity* pTex);

	void SetName(const String& name);
	void SetAttribute(const String& attribute);

	inline void SetDesc(const String& desc);
	inline TextureEntity* GetTexEntity() const;
	inline const String GetName() const;
	inline RHIDefine::VariablesType GetVarRenderType() const;
	inline ParserMaterial::ParamsType GetParserType() const;
	inline MaterialParameterPtr GetRenderValue() const;
	inline RHIDefine::ParameterSlot GetSlot() const;

	inline bool IsKeyEnum() const;
	inline const Vector<String>& GetAttrbuteContents() const;
	inline const String& GetAttributeContent(int index) const;

	inline bool IsValid() const;
	inline bool IsTexture() const;
	inline DefProperty::DefValue GetDefVal() const;
	inline bool IsEnum() const;
};

inline void DefProperty::SetDesc(const String& desc)
{
	m_desc = desc;
}
inline TextureEntity* DefProperty::GetTexEntity() const
{
	return DynamicCast<MaterialParameterTex>(m_pRenderValue.GetEObject())->GetTex();
}
inline const String DefProperty::GetName() const
{
	return m_name;
}
inline RHIDefine::VariablesType DefProperty::GetVarRenderType() const
{
	return m_varRenderType;
}
inline MaterialParameterPtr DefProperty::GetRenderValue() const
{
	return m_pRenderValue;
}
inline RHIDefine::ParameterSlot DefProperty::GetSlot() const
{
	return m_slot;
}
inline ParserMaterial::ParamsType DefProperty::GetParserType() const
{
	return m_parserType;
}
inline const String& DefProperty::GetAttributeContent(int index) const
{
	static String nullString = "";
	if (index >= 0
		&& index < m_attContent.size())
	{
		return m_attContent[index];
	}
	return nullString;
}
inline bool DefProperty::IsKeyEnum() const
{
	return m_attHader == "Keywords";
}
inline const Vector<String>& DefProperty::GetAttrbuteContents() const
{
	return m_attContent;
}
inline bool DefProperty::IsValid() const
{
	return m_parserType != ParserMaterial::VT_ERRORCODE;
}
inline bool DefProperty::IsTexture() const
{
	return m_parserType == ParserMaterial::VT_TEXTURE1D
		|| m_parserType == ParserMaterial::VT_TEXTURE2D
		|| m_parserType == ParserMaterial::VT_TEXTURE3D
		|| m_parserType == ParserMaterial::VT_TEXTURECUBE;
}
inline DefProperty::DefValue DefProperty::GetDefVal() const
{
	DefProperty::DefValue defValue;
	defValue.pName = &m_name;
	defValue.pDescription = &m_desc;
	defValue.pAttribute = &m_attribute;
	defValue.pValue = m_pDefValue;

	return defValue;
}
inline bool DefProperty::IsEnum() const
{
	return m_attHader == "Enum";
}

NS_JYE_END