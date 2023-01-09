
#include "MaterialStreamData.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(MaterialStreamData, Object);
BEGIN_ADD_PROPERTY(MaterialStreamData, Object);
REGISTER_PROPERTY(m_shaderPath, m_shaderPath, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_pParameterArray, m_pParameterArray, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(MaterialStreamData)
IMPLEMENT_INITIAL_END

MaterialStreamData::MaterialStreamData()
	: m_pParameterArray(_NEW ParameterArray)
{

}

MaterialStreamData::MaterialStreamData(const String& path)
    : m_shaderPath(path)
	, m_pParameterArray(_NEW ParameterArray)
{
}

MaterialStreamData::~MaterialStreamData()
{
	SAFE_DELETE(m_pParameterArray);
}

NS_JYE_END