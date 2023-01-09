#pragma once

#include "Core/Core.h"
#include "System/Singleton.hpp"
#include "Engine/private/Define.h"
#include "System/Delegate.h"
#include "Core/Interface/IWindow.h"

#include <mutex>

NS_JYE_BEGIN

class IWindow;
class RenderContext;

class ENGINE_API IGraphicSystem
{
	SINGLETON_DECLARE(IGraphicSystem);
private:
	DECLARE_MULTICAST_DELEGATE(RenderThreadExitCallback);
public:
	IGraphicSystem();
	virtual ~IGraphicSystem();

	void Initialize();

	void Update();
	void Render();

	RenderContext* CreateRenderContex();
	void ReleaseRenderContex(RenderContext* pRenderContex);


	FORCEINLINE DelegateHandle SetRenderThreadExitCallback(RenderThreadExitCallbackDelegate& func);
	FORCEINLINE void RemoveRenderThreadExitCallback(DelegateHandle& handle);
	FORCEINLINE void InsertRenderWindow(IWindow* con);
	FORCEINLINE void RemoveRenderWindow(IWindow* con);


private:

	void _RenderCheck();

	std::mutex m_Mutex;
	IWindow* m_pMainWindow;
	Map<void*, IWindow*> m_RenderContextList;
	RenderContext* m_pMainRenderContex; 
	Vector<RenderContext*> m_renderContexs; 
	RenderThreadExitCallback m_RenderThreadExitCallback;
};

FORCEINLINE DelegateHandle IGraphicSystem::SetRenderThreadExitCallback(RenderThreadExitCallbackDelegate& func)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_RenderThreadExitCallback.Add(std::move(func));
}

FORCEINLINE void IGraphicSystem::RemoveRenderThreadExitCallback(DelegateHandle& handle)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	m_RenderThreadExitCallback.Remove(handle);
}

FORCEINLINE void IGraphicSystem::InsertRenderWindow(IWindow* con)
{
	m_RenderContextList.insert({ con->GetNativeWindow(), con });
}

FORCEINLINE void IGraphicSystem::RemoveRenderWindow(IWindow* con)
{
	m_RenderContextList.erase(con->GetNativeWindow());
}

NS_JYE_END