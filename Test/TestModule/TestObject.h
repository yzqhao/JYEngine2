#pragma once

#include "Core/Main.h"
#include "Core/Object.h"

#include <iostream>

using namespace JYE;

class A : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	virtual ~A() {}
	A() {}
	static bool InitialDefaultState();
	static bool TerminalDefaultState();
public:
	bool m_b{true};
};
DECLARE_Ptr(A);		
TYPE_MARCO(A);	

class B : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	virtual ~B() {}
	B() {}
	static bool InitialDefaultState();
	static bool TerminalDefaultState();
};
DECLARE_Ptr(B);
TYPE_MARCO(B);

class C : public Object
{
	// DECLARE_RTTI
public:
	virtual Rtti& GetType()const { return ms_Type; }
	static Rtti ms_Type; 
public:
	static  Priority ms_Priority;
	
	// DECLARE_INITIAL
public: 
	static bool RegisterMainFactory(); 
public: 
	static bool InitialClassFactory(); 
	static Object* FactoryFunc(Object*);
	static bool ms_bRegisterMainFactory; 
	static bool InitialProperty(Rtti*); 
	static bool TerminalProperty();

public:
	virtual ~C() {}
	C() {}
	C(int i, float f) : m_i(i), m_f(f) {}
	static bool InitialDefaultState();
	static bool TerminalDefaultState();

public:
	int m_i{};
	float m_f{};
};
DECLARE_Ptr(C);
TYPE_MARCO(C);

struct TestStruct
{
	int m_sInt = 1;
	float m_sFloat = 4.0;
};

class D : public A
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	virtual ~D() 
	{ 
		SAFE_DELETE_ARRAY(m_FloatData);
		SAFE_DELETE_ARRAY(m_FixedFloatData);
	}
	D() 
	{
		m_FixedFloatData = _NEW float[4];
		for (unsigned int i = 0; i < 4; i++)
		{
			m_FixedFloatData[i] = 1.0f + i;
		}
	}
	D(const D& src)
	{
		m_vec = src.m_vec;
		m_FixedFloatData = _NEW float[4];
		for (unsigned int i = 0; i < 4; i++)
		{
			m_FixedFloatData[i] = src.m_FixedFloatData[i];
		}
		m_FloatData = src.m_FloatData;
		m_FloatDataNum = src.m_FloatDataNum;
		if (src.m_FloatData)
		{
			m_FloatData = _NEW float[m_FloatDataNum];
			for (unsigned int i = 0; i < m_FloatDataNum; i++)
			{
				m_FloatData[i] = src.m_FloatData[i];
			}
		}
	}
	static bool InitialDefaultState();
	static bool TerminalDefaultState();
public:
	std::vector<int> m_vec;
	float* m_FloatData{};
	uint m_FloatDataNum{};
	float* m_FixedFloatData;
	TestStruct m_testStruct;
};
DECLARE_Ptr(D);
TYPE_MARCO(D);

class E : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	virtual ~E() { m_pTestObjectClonePtr = {}; }
	E() {}
	static bool InitialDefaultState();
	static bool TerminalDefaultState();
public:
	enum TestEnum
	{
		TE_1,
		TE_2,
		TE_3,
		TE_MAX
	};
	TestEnum m_EnumTest = TE_3;
	Vector<C*> m_testVecObject;
	HashMap<String, C*> m_testHashObject;
	HashMap<int, int> m_testHash;
	D* m_pTestObjectCloneD{};
	C* m_pTestObjectClone{};
	C* m_pTestObjectCopy{};
	Pointer<C> m_pTestObjectClonePtr{};
	D m_objectD;
};
DECLARE_Ptr(E);
TYPE_MARCO(E);