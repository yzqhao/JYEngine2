#include "TestObject.h"
#include "Core/Property.h"
#include "Core/EnumTrans.h"

// 
// A
// 
IMPLEMENT_RTTI(A, Object);
BEGIN_ADD_PROPERTY(A, Object);
REGISTER_PROPERTY(m_b, m_b, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(A)
ADD_PRIORITY(B)
ADD_INITIAL_FUNCTION_WITH_PRIORITY(InitialDefaultState);
ADD_TERMINAL_FUNCTION_WITH_PRIORITY(TerminalDefaultState);
IMPLEMENT_INITIAL_END

bool A::InitialDefaultState()
{
	//std::cout << "This is A Initial" << std::endl;
	return true;
}
bool A::TerminalDefaultState()
{
	//std::cout << "This is A Terminal" << std::endl;
	return true;
}

//
// B
//

IMPLEMENT_RTTI(B, Object);
NO_PROPERTY(B);

IMPLEMENT_INITIAL_BEGIN(B)
ADD_INITIAL_FUNCTION_WITH_PRIORITY(InitialDefaultState);
ADD_TERMINAL_FUNCTION_WITH_PRIORITY(TerminalDefaultState);
IMPLEMENT_INITIAL_END

bool B::InitialDefaultState()
{
	//std::cout << "This is B Initial" << std::endl;
	return true;
}
bool B::TerminalDefaultState()
{
	//std::cout << "This is B Terminal" << std::endl;
	return true;
}

//
// C
//

// IMPLEMENT_RTTI
Rtti C::ms_Type("C", &Object::ms_Type, C::FactoryFunc); \
Priority C::ms_Priority;

//BEGIN_ADD_PROPERTY(C, Object);
bool C::TerminalProperty() 
{ 
	ms_Type.ClearProperty();
	return true;
} 
bool C::InitialProperty(Rtti* pRtti) 
{ 
	C* dummyPtr = NULL;
	Property* activeProperty = NULL;
	Rtti* pRttiTemp = pRtti;
	if (!pRtti)
	{
		pRtti = &ms_Type;
	}
	Object::InitialProperty(pRtti);
	{	// REGISTER_PROPERTY(m_i, m_i, Property::F_SAVE_LOAD_CLONE)
		activeProperty = PropertyCreator::GetAutoPropertyCreator(dummyPtr->m_i).CreateProperty(
			("m_i"), *pRtti, (unsigned int)((char*)&(dummyPtr->m_i) - (char*)dummyPtr), Property::F_SAVE_LOAD_CLONE); 
		pRtti->AddProperty(activeProperty); 
	}
	{	// REGISTER_PROPERTY(m_f, m_f, Property::F_SAVE_LOAD_CLONE)
		activeProperty = PropertyCreator::GetAutoPropertyCreator(dummyPtr->m_f).CreateProperty(
			("m_f"), *pRtti, (unsigned int)((char*)&(dummyPtr->m_f) - (char*)dummyPtr), Property::F_SAVE_LOAD_CLONE);
		pRtti->AddProperty(activeProperty);
	}

	// END_ADD_PROPERTY
	return true; 
}

// NO_PROPERTY
/*
bool C::InitialProperty(Rtti*)
{
	return true;
}
bool C::TerminalProperty()
{
	ms_Type.ClearProperty();
	return true;
}
*/

static bool gs_bStreamRegistered_C = C::RegisterMainFactory(); 
bool C::ms_bRegisterMainFactory = false; 
bool C::InitialClassFactory() 
{ 
	ms_ClassFactory.insert({ ms_Type.GetName(), FactoryFunc });
	return 1;
} 
Object* C::FactoryFunc(Object*)
{ 
	return _NEW C;
} 
bool C::RegisterMainFactory() 
{ 
	if (!ms_bRegisterMainFactory)
	{
		Main::AddInitialFunction(InitialDefaultState, &ms_Priority);
		Main::AddTerminalFunction(TerminalDefaultState, &ms_Priority);
		Main::AddInitialFunction(InitialClassFactory); 
		Main::AddInitialPropertyFunction(InitialProperty);
		Main::AddTerminalPropertyFunction(TerminalProperty);
		ms_bRegisterMainFactory = true;
	} 
	return ms_bRegisterMainFactory; 
}

bool C::InitialDefaultState()
{
	//std::cout << "This is C Initial" << std::endl;
	return true;
}
bool C::TerminalDefaultState()
{
	//std::cout << "This is C Terminal" << std::endl;
	return true;
}

//
// D
//
IMPLEMENT_RTTI(D, A);
BEGIN_ADD_PROPERTY(D, A);
REGISTER_PROPERTY(m_vec, m_vec, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY_DATA(m_FloatData, m_FloatDataNum, TestDataBuffer)
REGISTER_PROPERTY_FIXED_DATA(m_FixedFloatData, 4, FixedFloatData, false)
REGISTER_PROPERTY(m_testStruct, m_testStruct, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(D)
ADD_INITIAL_FUNCTION_WITH_PRIORITY(InitialDefaultState);
ADD_TERMINAL_FUNCTION_WITH_PRIORITY(TerminalDefaultState);
IMPLEMENT_INITIAL_END

bool D::InitialDefaultState()
{
	//std::cout << "This is B Initial" << std::endl;
	return true;
}
bool D::TerminalDefaultState()
{
	//std::cout << "This is B Terminal" << std::endl;
	return true;
}

//
// E
//
IMPLEMENT_RTTI(E, Object);
BEGIN_ADD_PROPERTY(E, Object);
REGISTER_PROPERTY(m_testVecObject, m_testVecObject, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_testHashObject, m_testHashObject, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_testHash, m_testHash, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_pTestObjectCloneD, TestObjectCloneD, Property::F_SAVE_LOAD_CLONE)
//REGISTER_PROPERTY(m_objectD, m_objectD, Property::F_SAVE_LOAD_CLONE)	// 暂时不支持Object对象
REGISTER_PROPERTY(m_pTestObjectClone, TestObjectClone, Property::F_SAVE_LOAD_CLONE)
REGISTER_PROPERTY(m_pTestObjectCopy, TestObjectCopy, Property::F_SAVE_LOAD_COPY)
REGISTER_PROPERTY(m_pTestObjectClonePtr, TestObjectClonePtr, Property::F_SAVE_LOAD_CLONE)
REGISTER_ENUM_PROPERTY(m_EnumTest, EnumTest, TestEnum, Property::F_SAVE_LOAD_CLONE)
BEGIN_ADD_ENUM
ADD_ENUM(TestEnum, TE_1)
ADD_ENUM(TestEnum, TE_2)
ADD_ENUM(TestEnum, TE_3)
END_ADD_ENUM
END_ADD_PROPERTY
IMPLEMENT_INITIAL_BEGIN(E)
ADD_INITIAL_FUNCTION_WITH_PRIORITY(InitialDefaultState);
ADD_TERMINAL_FUNCTION_WITH_PRIORITY(TerminalDefaultState);
IMPLEMENT_INITIAL_END

bool E::InitialDefaultState()
{
	//std::cout << "This is E Initial" << std::endl;
	return true;
}
bool E::TerminalDefaultState()
{
	//std::cout << "This is E Terminal" << std::endl;
	return true;
}
