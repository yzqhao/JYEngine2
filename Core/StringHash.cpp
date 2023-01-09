
#include "StringHash.h"

NS_JYE_BEGIN

//
// StringManager
//
StringManager::StringManager()
{

}

StringManager::~StringManager()
{
	for (auto& it : m_strings)
	{
		SAFE_DELETE(it.second);
	}
}

StringHash* StringManager::GetOrCreateName(const char* pChar)
{
	if (!pChar)
	{
		return nullptr;
	}
	// thread lock
	unsigned int uiCRCCode = Math::MathInstance::GetMathInstance().CRC32Compute(pChar, (unsigned int)strlen(pChar));
	StringHash* pName = nullptr;
	pName = CheckIsHave(uiCRCCode);
	if (!pName)
	{
		pName = _NEW StringHash(uiCRCCode, pChar);
		if (!pName)
		{
			return nullptr;
		}

		m_strings.insert({ uiCRCCode, pName });
	}
	return pName;
}

StringHash* StringManager::GetOrCreateName(const String& str)
{
	return GetOrCreateName(str.c_str());
}

StringHash* StringManager::CheckIsHave(unsigned uid)
{
	return m_strings.find(uid) != m_strings.end() ? m_strings[uid] : nullptr;
}

NS_JYE_END
