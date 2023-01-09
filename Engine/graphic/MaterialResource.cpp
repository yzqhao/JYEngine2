#include "MaterialResource.h"
#include "Engine/render/material/IMaterialSystem.h"
#include "Engine/render/material/ApplyKeyWord.h"

NS_JYE_BEGIN

MaterialResource::MaterialResource()
	: ISharedResource(ResourceProperty::SRT_MATERIALENTITY)
	, m_pParameterArray(_NEW ParameterArray)
{

}

MaterialResource::~MaterialResource()
{
	SAFE_DELETE(m_pParameterArray);
	SAFE_DELETE(m_pShaderState);
}

void MaterialResource::_DoCopy(const MaterialResource& rhs)
{
	m_pParameterArray = _NEW ParameterArray(*rhs.m_pParameterArray);
	m_KeyWords = rhs.m_KeyWords;
	m_KeyWordStrs = rhs.m_KeyWordStrs;
	m_pShaderState = new ShaderState(*rhs.m_pShaderState);
}

const MaterialResource& MaterialResource::operator = (const MaterialResource& rhs)
{
	_DoCopy(rhs);
	return *this;
}

void MaterialResource::ReleaseResource()
{

}

bool MaterialResource::_DeprecatedFilter(bool isd)
{
	return false;
}

bool MaterialResource::SetParameter(RHIDefine::ParameterSlot att, MaterialParameter* obj)
{
	if (obj == nullptr)
	{
		JYLOGTAG(LogEngineRHI, "Material::SetParameter param is null, check it!");
		return false;
	}

	ShaderState* pEntity = const_cast<ShaderState*>(m_pShaderState);
	const DefProperty& property = pEntity->GetProperty(att);

	if (property.IsValid())
	{
		bool isKeyEnum = property.IsKeyEnum();

		if (isKeyEnum)
		{
			MaterialParameter* preObj = GetParameter(att);
			JY_ASSERT(obj->IsSameType(preObj));
			const MaterialParameterFloat* pPreFloat = static_cast<const MaterialParameterFloat*>(preObj);
			const MaterialParameterFloat* pCurFloat = static_cast<const MaterialParameterFloat*>(obj);

			int preIndex = static_cast<int>(pPreFloat->m_float);
			int curIndex = static_cast<int>(pCurFloat->m_float);

			if (preIndex != curIndex)
			{
				const String& preKeyWord = property.GetAttributeContent(preIndex);
				const String& curKeyWord = property.GetAttributeContent(curIndex);
				DisableKeyWord(preKeyWord);
				EnableKeyWord(curKeyWord);
			}
		}
	}

	m_pParameterArray->SetParameter(att, obj);
	return true;
}

bool MaterialResource::SetParameter(const String& paramName, MaterialParameter* obj)
{
	IMaterialSystem* pInstance = IMaterialSystem::Instance();
	bool hasSlot = pInstance->ParameterSlotExist(paramName);
	RHIDefine::ParameterSlot slot;
	if (!hasSlot)
	{
		slot = pInstance->NewParameterSlot(RHIDefine::ParameterUsage::SU_UNIFORM, paramName);
	}
	else
	{
		slot = pInstance->GetParameterSlot(paramName);
	}

	return SetParameter(slot, obj);
}

MaterialParameter* MaterialResource::GetParameter(RHIDefine::ParameterSlot att)
{
	return m_pParameterArray->GetParameter(att);
}

MaterialParameter* MaterialResource::GetParameter(const String& paramName)
{
	IMaterialSystem* pInstance = IMaterialSystem::Instance();
	bool hasSlot = pInstance->ParameterSlotExist(paramName);

	if (hasSlot)
	{
		RHIDefine::ParameterSlot slot = pInstance->GetParameterSlot(paramName);
		return GetParameter(slot);
	}
	else
	{
		return NULL;
	}
}

bool MaterialResource::GetRenderPass(int passIndex, ShaderKeyWords& keyWords, PassInfo& passInfo)
{
	passInfo.pParams = GetParmArray();
	ShaderState* pEntity = const_cast<ShaderState*>(m_pShaderState);
	ApplyKeyWord apply(&keyWords, m_KeyWords);
	return pEntity->GetRenderPass(passIndex, keyWords, passInfo);
}

Map<String, MaterialParameter*> MaterialResource::GetParameters()
{
	Map<String, MaterialParameter*> res;
	Vector<RHIDefine::ParameterSlot> slots = m_pParameterArray->GetAllSlots();
	size_t count = slots.size();

	for (int i = 0; i < count; i++)
	{
		const IMaterialSystem::InputParameter& info = IMaterialSystem::Instance()->GetParameterInfomation(slots[i]);

		if (info.Usage == RHIDefine::SU_UNIFORM)
		{
			MaterialParameter* temp = m_pParameterArray->GetParameter(slots[i]);
			if (temp)
			{
				res.insert({ info.Name, temp });
			}
		}
	}

	return std::move(res);
}

void MaterialResource::SyncDefProperty(const HashMap<String, MaterialParameter*>* params)
{
	const Vector<DefProperty>& defValue = m_pShaderState->GetDefPropertys();
	size_t size = defValue.size();

	for (int i = 0; i < size; i++)
	{
		const DefProperty& defProperty = defValue[i];
		const String& name = defProperty.GetName();

		if (!hasParameter(defProperty.GetSlot()))
		{
			MaterialParameter* valueObj = nullptr;
			if (params && params->find(name) != params->end())
			{
				valueObj = params->find(name)->second;
			}
			else
			{
				valueObj = defProperty.GetRenderValue();
			}

			m_pParameterArray->SetParameter(defProperty.GetSlot(), valueObj);

			// set key word
			if (defProperty.IsKeyEnum())
			{
				MaterialParameter* pDefObj = valueObj;
				JY_ASSERT(pDefObj != nullptr && pDefObj->IsSameType(MaterialParameterFloat::ms_Type));
				MaterialParameterFloat* pCurFloat = static_cast<MaterialParameterFloat*>(pDefObj);
				int curIndex = static_cast<int>(pCurFloat->m_float);
				String keyWord = defProperty.GetAttributeContent(curIndex);

				EnableKeyWord(keyWord);
			}
		}
		else
		{
			if (defProperty.IsKeyEnum())
			{
				MaterialParameter* obj = GetParameter(defProperty.GetSlot());
				JY_ASSERT(obj != nullptr && obj->IsSameType(MaterialParameterFloat::ms_Type));
				MaterialParameterFloat* pCurFloat = static_cast<MaterialParameterFloat*>(obj);
				int curIndex = static_cast<int>(pCurFloat->m_float);
				const String& curKeyWord = defProperty.GetAttributeContent(curIndex);
				EnableKeyWord(curKeyWord);
			}
		}
	}
}

NS_JYE_END