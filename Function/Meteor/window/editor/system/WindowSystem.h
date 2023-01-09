
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "System/Singleton.hpp"

NS_JYE_BEGIN

class MMemuWindowBase;
class MGame;

class WindowSystem
{
	SINGLETON_DECLARE_ONCREATE_ONDESTROY(WindowSystem);
public:
	WindowSystem();
	~WindowSystem();

	void _OnCreate();
	void _OnDestroy();

	void ClearScene();
	void Update(float dt);

	FORCEINLINE const Vector<MMemuWindowBase*>& GetMenuWindowList() { return m_memuWindowlist; }
	FORCEINLINE MGame* GetGameWindow() { return m_game; }
private:
	Vector<MMemuWindowBase*> m_memuWindowlist;
	MGame* m_game;
};

NS_JYE_END