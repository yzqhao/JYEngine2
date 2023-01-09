#include "IAsyncEntity.h"

NS_JYE_BEGIN

//BEGIN_ADD_PROPERTY(IAsyncEntity, IResourceEntity);
//REGISTER_PROPERTY(m_CameraResolution, m_CameraResolution, Property::F_SAVE_LOAD_CLONE)
//END_ADD_PROPERTY

/*
template<typename RES, typename SOU>
bool IAsyncEntity<RES, SOU>::TerminalProperty()
{ 
	ms_Type.ClearProperty();
	return true;
} 

template<typename RES, typename SOU>
bool IAsyncEntity<RES, SOU>::InitialProperty(Rtti* pRtti)
{ 
	IAsyncEntity<RES, SOU>* dummyPtr = NULL;
	Property* activeProperty = NULL;
	Rtti* pRttiTemp = pRtti;
	if (!pRtti)
	{
		pRtti = &ms_Type;
	}

	IResourceEntity::InitialProperty(pRtti);
	return true;
}
*/

NS_JYE_END