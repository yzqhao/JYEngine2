
#pragma once

#include "Main.h"
#include "Property.h"

NS_JYE_BEGIN

class Object;

class CORE_API Rtti
{
	DECLARE_PRIORITY
	DECLARE_INITIAL_ONLY
	
	using CreateObjectFun = Object*(*)(Object* host);
public:
	Rtti(const char * pcRttiName, Rtti *pBase, CreateObjectFun COF);
	~Rtti();

	inline const String &GetName()const {return m_cRttiName;}
	inline Rtti* GetBase()const { return m_pBase; }
	inline bool IsSameType(const Rtti &Type) const {return  (&Type == this);}

	bool IsDerived(const Rtti& Type) const;
	Property* GetProperty(unsigned int uiIndex) const;
	Property* GetProperty(const String& PropertyName) const;
	unsigned int GetPropertyNum()const;
	void AddProperty(Property* pProperty);
	void AddProperty(Rtti& Rtti);

	uint GetMemberNum()const;
	void AddMember(MemberRegister* member);
	MemberRegister* GetMember(uint uiIndex) const;

	friend class Object;
	void ClearProperty();

private:
	String	m_cRttiName;
	Rtti*	m_pBase;
	Vector<Property*> m_PropertyArray;
	Vector<MemberRegister*> m_MemberArray;
	CreateObjectFun m_CreateFun;
	static bool InitialDefaultState();
	static bool TerminalDefaultState();
};

struct CORE_API RttiInfoIn
{
	RttiInfoIn(Rtti* pcur, Rtti* pbase) : cur(pcur), baser(pbase) {}

	Rtti* cur;
	Rtti* baser;
	Vector<RttiInfoIn*> children;
};

#include "RttiMacro.h"

NS_JYE_END
