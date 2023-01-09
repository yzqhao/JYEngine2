
#pragma once

#include "private/Define.h"
#include "Core/Propery/TouchInput.h"
#include "Core/Propery/InputPropery.h"
#include "System/Singleton.hpp"

#include <list>

NS_JYE_BEGIN

class AdapterWindow;

class AdapterInput 
{
	SINGLETON_DECLARE_ONCREATE_ONDESTROY(AdapterInput);
private:
	typedef std::list<bool>		MouseActions;
private:
	enum Constant
	{
		AIC_MOUSE_COUNT = 5,
	};
private:
	MouseActions	m_MouseJustPressed[AIC_MOUSE_COUNT];
	bool			m_MouseActionStatus[AIC_MOUSE_COUNT];
public:
	AdapterInput();
	~AdapterInput();
private:
	void _OnCreate();
	void _OnDestroy();
public:
	void OnChar(uint c);
	void OnScroll(float x, float y);
	void OnTouch(const TouchInputs& touch);
	void OnKeyboard(InputPropery::Keyboard key, InputPropery::KeyStatus status);
	void EarlyUpdate();
	void LateUpdate();
};

NS_JYE_END