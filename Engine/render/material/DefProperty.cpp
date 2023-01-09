#include "DefProperty.h"
#include "IMaterialSystem.h"
#include "System/Utility.hpp"

NS_JYE_BEGIN
	
DefProperty::DefProperty(void)
	: m_varRenderType(RHIDefine::VT_ERRORCODE)
	, m_parserType(ParserMaterial::VT_ERRORCODE)
	, m_pRenderValue(nullptr)
	, m_pDefValue(nullptr)
	, m_slot(RHIDefine::PS_ERRORCODE)
{

}

void DefProperty::FreeRenderValue()
{
	//OF_SAFE_DELETE(m_pDefValue);
	//OF_SAFE_DELETE(m_pRenderValue);
}

DefProperty::~DefProperty(void)
{
	FreeRenderValue();
}

DefProperty::DefProperty(const DefProperty& property)
{
	m_varRenderType = property.m_varRenderType;
	m_parserType = property.m_parserType;
	m_slot = property.m_slot;
	m_name = property.m_name;
	m_desc = property.m_desc;
	m_attribute = property.m_attribute;
	m_attHader = property.m_attHader;
	m_attContent = property.m_attContent;

	if (property.m_pRenderValue != nullptr)
	{
		m_pRenderValue = property.m_pRenderValue;
	}
	else
	{
		m_pRenderValue = nullptr;
	}

	if (property.m_pDefValue != nullptr)
	{
		m_pDefValue = property.m_pDefValue;
	}
	else
	{
		m_pDefValue = nullptr;
	}
}

void DefProperty::SetFloat(float x)
{
	FreeRenderValue();
	m_parserType = ParserMaterial::VT_FLOAT;
	m_varRenderType = RHIDefine::VT_FLOAT;
	m_pDefValue = MakeMaterialParam(x);
	m_pRenderValue = MakeMaterialParam(x);
}

void DefProperty::SetFloat2(float x, float y)
{
	FreeRenderValue();
	m_parserType = ParserMaterial::VT_VEC2;
	m_varRenderType = RHIDefine::VT_VEC2;
	m_pDefValue = MakeMaterialParam(Math::Vec2(x, y));
	m_pRenderValue = MakeMaterialParam(Math::Vec2(x, y));
}

void DefProperty::SetFloatRange(float x, float minVal, float maxVal)
{
	FreeRenderValue();
	m_parserType = ParserMaterial::VT_FLOATRANGE;
	m_varRenderType = RHIDefine::VT_VEC2;
	m_pDefValue = MakeMaterialParam(x);  // new Range1f(x, maxVal, minVal);
	m_pRenderValue = MakeMaterialParam(x);
}

void DefProperty::SetFloat3(float x, float y, float z)
{
	FreeRenderValue();
	m_parserType = ParserMaterial::VT_VEC3;
	m_varRenderType = RHIDefine::VT_VEC3;
	m_pDefValue = MakeMaterialParam(Math::Vec3(x, y, z));
	m_pRenderValue = MakeMaterialParam(Math::Vec3(x, y, z));
}

void DefProperty::SetFloat4(float x, float y, float z, float w)
{
	FreeRenderValue();
	m_parserType = ParserMaterial::VT_VEC4;
	m_varRenderType = RHIDefine::VT_VEC4;
	m_pDefValue = MakeMaterialParam(Math::Vec4(x, y, z, w));
	m_pRenderValue = MakeMaterialParam(Math::Vec4(x, y, z, w));
}

void DefProperty::SetColor(float r, float g, float b, float a)
{
	FreeRenderValue();
	m_parserType = ParserMaterial::VT_COLOR;
	m_varRenderType = RHIDefine::VT_VEC4;
	m_pDefValue = MakeMaterialParam(Math::FColor(r, g, b, a));
	m_pRenderValue = MakeMaterialParam(Math::FColor(r, g, b, a));
}

void DefProperty::SetTexture1D(TextureEntity* pTex)
{
	FreeRenderValue();
	m_parserType = ParserMaterial::VT_TEXTURE1D;
	m_varRenderType = RHIDefine::VT_TEXTURE1D;
	m_pDefValue = MakeMaterialParam(pTex);
	m_pRenderValue = MakeMaterialParam(pTex);
}

void DefProperty::SetTexture2D(TextureEntity* pTex)
{
	FreeRenderValue();
	m_parserType = ParserMaterial::VT_TEXTURE2D;
	m_varRenderType = RHIDefine::VT_TEXTURE2D;
	m_pDefValue = MakeMaterialParam(pTex);
	m_pRenderValue = MakeMaterialParam(pTex);
}

void DefProperty::SetTexture3D(TextureEntity* pTex)
{
	FreeRenderValue();
	m_parserType = ParserMaterial::VT_TEXTURE3D;
	m_varRenderType = RHIDefine::VT_TEXTURE3D;
	m_pDefValue = MakeMaterialParam(pTex);
	m_pRenderValue = MakeMaterialParam(pTex);
}

void DefProperty::SetTextureCube(TextureEntity* pTex)
{
	FreeRenderValue();
	m_parserType = ParserMaterial::VT_TEXTURECUBE;
	m_varRenderType = RHIDefine::VT_TEXTURECUBE;
	m_pDefValue = MakeMaterialParam(pTex);
	m_pRenderValue = MakeMaterialParam(pTex);
}

void DefProperty::SetAttribute(const String& attribute)
{
	m_attribute = attribute;

	if (!m_attribute.empty())
	{
		// 进行属性分析

		size_t startHeader = m_attribute.find_first_not_of('[');
		size_t endHeader = m_attribute.find_first_of('(') - 1;
		size_t startContentPos = m_attribute.find_first_of('(') + 1;
		size_t endContentPos = m_attribute.find_last_not_of(')');
		String attContent = m_attribute.substr(startContentPos, endContentPos - startContentPos + 1);

		m_attHader = m_attribute.substr(startHeader, endHeader - startHeader + 1);
		Utility::trim(m_attHader);

		m_attContent = Utility::split(attContent, ",");
		for (int i = 0; i < m_attContent.size(); i++)
		{
			String& curStr = m_attContent.at(i);
			Utility::trim(curStr);
		}
	}
}

void DefProperty::SetName(const String& name)
{
	IMaterialSystem* pInstance = IMaterialSystem::Instance();
	bool exist = pInstance->ParameterSlotExist(name);
	m_name = name;

	if (!exist)
	{
		m_slot = pInstance->NewParameterSlot(RHIDefine::SU_UNIFORM, name);
	}
	else
	{
		m_slot = pInstance->GetParameterSlot(name);
	}
}

NS_JYE_END
