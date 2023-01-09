
#pragma once

#include "Priority.h"

NS_JYE_BEGIN

class Rtti;

class CORE_API Main
{
	typedef bool (*Function)();
	typedef bool (*FunctionProperty)(Rtti*);
public:
	static void AddInitialPropertyFunction(FunctionProperty Func);
	static void AddTerminalPropertyFunction(Function Func);
	static void AddInitialFunction(Function Func);
	static void AddInitialFunction(Function Func, Priority* pPriority);
	static bool Initialize();
	static void AddTerminalFunction(Function Func);
	static void AddTerminalFunction(Function Func, Priority* pPriority);
	static bool Terminate();
private:
	Main();
	~Main();
	struct Element
	{
	public:
		Function Func {};
		Priority* pPriority {};
		bool operator > (const Element& e)const
		{
			static Priority Pr1;
			static Priority Pr2;

			Priority* p1 = NULL;
			Priority* p2 = NULL;
			if (pPriority)
				p1 = pPriority;
			else
				p1 = &Pr1;
			if (e.pPriority)
				p2 = e.pPriority;
			else
				p2 = &Pr2;
			return (*p1) > (*p2);
		}
		bool operator < (const Element& e)const
		{
			static Priority Pr1;
			static Priority Pr2;

			Priority* p1 = NULL;
			Priority* p2 = NULL;
			if (pPriority)
				p1 = pPriority;
			else
				p1 = &Pr1;
			if (e.pPriority)
				p2 = e.pPriority;
			else
				p2 = &Pr2;
			return (*p1) > (*p2);
		}
		bool operator == (const Element& e)const
		{
			static Priority Pr1;
			static Priority Pr2;

			Priority* p1 = NULL;
			Priority* p2 = NULL;
			if (pPriority)
				p1 = pPriority;
			else
				p1 = &Pr1;
			if (e.pPriority)
				p2 = e.pPriority;
			else
				p2 = &Pr2;
			return (*p1) == (*p2);
		}
	};
	static Vector<Element>* ms_pInitialArray;
	static Vector<FunctionProperty>* ms_pInitialPropertyArray;
	static Vector<Function>* ms_pTerminalPropertyArray;
	static Vector<Element>* ms_pTerminalArray;

	class PriorityCompare
	{
	public:
		inline bool operator()(Element& e1, Element& e2)
		{
			static Priority Pr1;
			static Priority Pr2;

			Priority* p1 = NULL;
			Priority* p2 = NULL;
			if (e1.pPriority)
				p1 = e1.pPriority;
			else
				p1 = &Pr1;
			if (e2.pPriority)
				p2 = e2.pPriority;
			else
				p2 = &Pr2;
			return (*p1) < (*p2);
		}
	};
};

#include "MainMacro.h"

NS_JYE_END
