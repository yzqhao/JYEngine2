
#include "Rtti.h"
#include "Property.h"
#include "Stream.h"
#include "RttiManager.h"

NS_JYE_BEGIN

bool Rtti::TerminalDefaultState()
{
	return true;
}

bool Rtti::InitialDefaultState()
{
	return true;
}

Rtti::Rtti(const char* pcRttiName, Rtti* pBase, CreateObjectFun COF) : m_cRttiName(pcRttiName)
{
	m_pBase = pBase;
	m_CreateFun = COF;
	RttiManager::Instance()->RegisterClass(m_cRttiName, this);
}

Rtti::~Rtti()
{
	m_pBase = 0;
	for(uint i = 0 ; i < m_PropertyArray.size() ;i++) 
	{ 
		if(m_PropertyArray[i]) 
		{ 
			SAFE_DELETE(m_PropertyArray[i]) 
		} 
	} 
}

bool Rtti::IsDerived(const Rtti &Type) const
{
	const Rtti * pTemp = this;
	while(!pTemp->IsSameType(Type))
	{
		if(pTemp->m_pBase)
		{
			pTemp = pTemp->m_pBase;
		}
		else
		{
			return false;
		}
	}
	return true;
}

Property* Rtti::GetProperty(uint uiIndex)const
{
	if (uiIndex >= m_PropertyArray.size())
	{
		return nullptr;
	}
	return m_PropertyArray[uiIndex];
}

Property* Rtti::GetProperty(const String & PropertyName)const
{
	for (uint i = 0; i < m_PropertyArray.size(); i++)
	{
		if (m_PropertyArray[i]->GetName() == PropertyName)
		{
			return m_PropertyArray[i];
		}
	}
	JY_ASSERT(0);
	return nullptr;
}

uint Rtti::GetPropertyNum()const
{
	return m_PropertyArray.size();
}

void Rtti::AddProperty(Property * pProperty)
{	
	if (pProperty)
	{
		for (uint i = 0 ; i < m_PropertyArray.size() ; i++)
		{
			JY_ASSERT(m_PropertyArray[i]->GetName() != pProperty->GetName());
		}
		m_PropertyArray.push_back(pProperty);
	}
}

void Rtti::AddProperty(Rtti & Rtti)
{
	for (uint i = 0 ; i < Rtti.m_PropertyArray.size() ; i++)
	{
		Property * pProperty = Rtti.m_PropertyArray[i];
		Property *pNewProperty = pProperty->GetInstance();
		pNewProperty->Clone(pProperty);
		pNewProperty->SetOwner(*this);
		m_PropertyArray.push_back(pNewProperty);
	}
}

uint Rtti::GetMemberNum() const
{
	return m_MemberArray.size();
}

void Rtti::AddMember(MemberRegister* member)
{
	if (member)
	{
		m_MemberArray.push_back(member);
	}
}

MemberRegister* Rtti::GetMember(uint uiIndex) const
{
	return m_MemberArray[uiIndex];
}

void Rtti::ClearProperty()
{
	for(uint i = 0 ; i < m_PropertyArray.size() ;i++) 
	{ 
		if(m_PropertyArray[i]) 
		{ 
			SAFE_DELETE(m_PropertyArray[i]) 
		} 
	} 
	m_PropertyArray.clear();	
}

NS_JYE_END
