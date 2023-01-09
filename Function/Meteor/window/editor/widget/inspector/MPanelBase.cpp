#include "MPanelBase.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI_NoCreateFun(MPanelBase, Object);
BEGIN_ADD_PROPERTY(MPanelBase, Object);
END_ADD_PROPERTY

MPanelBase::MPanelBase(const String& name)
	: m_panelName(name)
{

}

MPanelBase::~MPanelBase()
{

}

NS_JYE_END