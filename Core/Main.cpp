
#include "Main.h"
#include "RttiManager.h"

NS_JYE_BEGIN

Vector<Main::Element>* Main::ms_pInitialArray = NULL;
Vector<Main::Element>* Main::ms_pTerminalArray = NULL;
Vector<Main::FunctionProperty>* Main::ms_pInitialPropertyArray = NULL;
Vector<Main::Function>* Main::ms_pTerminalPropertyArray = NULL;

void Main::AddInitialPropertyFunction(FunctionProperty Func)
{
	if (!Func)
		return;
	if (!ms_pInitialPropertyArray)
	{
		ms_pInitialPropertyArray = _NEW Vector<FunctionProperty>;
	}
	ms_pInitialPropertyArray->push_back(Func);
}

void Main::AddTerminalPropertyFunction(Function Func)
{
	if (!Func)
		return;
	if (!ms_pTerminalPropertyArray)
	{
		ms_pTerminalPropertyArray = _NEW Vector<Function>;
	}
	ms_pTerminalPropertyArray->push_back(Func);
}

void Main::AddInitialFunction(Function Func)
{
	AddInitialFunction(Func, NULL);
}

void Main::AddInitialFunction(Function Func, Priority* pPriority)
{
	if (!Func)
		return;
	if (!ms_pInitialArray)
	{
		ms_pInitialArray = _NEW Vector<Element>;
	}
	Element e;
	e.Func = Func;
	e.pPriority = pPriority;
	ms_pInitialArray->push_back(e);
}

void Main::AddTerminalFunction(Function Func)
{
	AddTerminalFunction(Func, NULL);
}

void Main::AddTerminalFunction(Function Func, Priority* pPriority)
{
	if (!Func)
		return;
	if (!ms_pTerminalArray)
	{
		ms_pTerminalArray = _NEW Vector<Element>;
	}
	Element e;
	e.Func = Func;
	e.pPriority = pPriority;
	ms_pTerminalArray->push_back(e);
}

bool Main::Initialize()
{
	RttiManager::Instance()->Initlize();

	for (unsigned int i = 0; i < ms_pInitialPropertyArray->size(); i++)
	{
		if (!(*((*ms_pInitialPropertyArray)[i]))(NULL))
		{
			JY_ASSERT(0);
			return false;
		}
	}
	std::sort(ms_pInitialArray->begin(), ms_pInitialArray->end(), PriorityCompare());
	for (unsigned int i = 0; i < ms_pInitialArray->size(); i++)
	{
		if (!(*((*ms_pInitialArray)[i].Func))())
		{
			JY_ASSERT(0);
			return false;
		}
	}

	ms_pInitialArray->clear();
	SAFE_DELETE(ms_pInitialArray);
	ms_pInitialPropertyArray->clear();
	SAFE_DELETE(ms_pInitialPropertyArray);
	return true;
}

bool Main::Terminate()
{
	std::sort(ms_pTerminalArray->begin(), ms_pTerminalArray->end(), PriorityCompare());

	for (int i = ms_pTerminalArray->size() - 1; i >= 0; i--)
	{
		if (!(*((*ms_pTerminalArray)[i].Func))())
		{
			JY_ASSERT(0);
			return false;
		}
	}

	ms_pTerminalArray->clear();
	SAFE_DELETE(ms_pTerminalArray);

	for (unsigned int i = 0; i < ms_pTerminalPropertyArray->size(); i++)
	{
		if (!(*((*ms_pTerminalPropertyArray)[i]))())
		{
			JY_ASSERT(0);
			return false;
		}
	}
	ms_pTerminalPropertyArray->clear();
	SAFE_DELETE(ms_pTerminalPropertyArray);

	RttiManager::Instance()->Destory();

	return true;
}


NS_JYE_END

