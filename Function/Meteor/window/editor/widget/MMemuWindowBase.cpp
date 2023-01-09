#include "MMemuWindowBase.h"

NS_JYE_BEGIN

MMemuWindowBase::MMemuWindowBase(const String& name)
	: m_isOnGui(true)
	, m_filesearching(false)
	, m_titleName(name)
{

}

MMemuWindowBase::~MMemuWindowBase()
{

}
NS_JYE_END