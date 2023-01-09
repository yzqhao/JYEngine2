#pragma once

#include "../../System/Delegate.h"
#include "../Core.h"

NS_JYE_BEGIN

namespace WindowPropery
{
	enum Attribute
	{
		WA_FOCUSED = 0,
		WA_RESIZABLE,
		WA_VISIBLE,
		WA_DECORATED,
		WA_AUTO_ICONIFY,
		WA_FLOATING,
	};

}

class CORE_API IWindow
{
private:
	DECLARE_MULTICAST_DELEGATE(WindowCloseCallbackList, void*);
	DECLARE_MULTICAST_DELEGATE(WindowMoveCallbackList, void*);
	DECLARE_MULTICAST_DELEGATE(ResizeviewCallbackList, int, int, int, int, int, int);
	WindowMoveCallbackList m_WindowMoveCallback;
	WindowCloseCallbackList m_WindowCloseCallback;
	ResizeviewCallbackList m_ResizeviewCallback;

	bool m_windowShoudClose { false };
	bool m_windowCloseNow { false };
public:
	enum DialogType
	{
		SingleFile = 0,
		MultiFiles,
		Folder,
		SaveFile,
	};

	IWindow() {}
	~IWindow() {}

	virtual void SetClipboardText(const char* text) = 0;
	virtual const char* GetClipboardText() = 0;
	virtual void GetWindowSize(int* w, int* h) = 0;
	virtual void SetWindowSize(int x, int y) = 0;
	virtual void GetWindowPosition(int* x, int* y) = 0;
	virtual void SetWindowPosition(int x, int y) = 0;
	virtual void DestroyWindow() = 0;
	virtual void HideWindow() = 0;
	virtual void MaximizeWindow() = 0;
	virtual void ShowWindow(bool taskbar) = 0;
	virtual void SetWindowTitle(const std::string& title) = 0;
	virtual bool GetWindowMinimized() = 0;
	virtual void SwapBuffers() = 0;
	virtual void SwapInterval(int i) = 0;
	virtual int GetContextVersion() = 0;
	virtual uint GetMonitorCount() = 0;
	virtual void GetMonitorInfomation(uint index, int* x, int* y, int* w, int* h) = 0;
	virtual IWindow* CreateSubWindows(int x, int y, int w, int h, bool contextonly, bool nodecoration, bool topmost) = 0;
	virtual void* GetWindowHanle() = 0;
	virtual void* GetNativeWindow() = 0;
	virtual bool IsMainWindow() = 0;
	virtual bool IsFull() = 0;
	virtual void SetWindowMode(bool bWindow) = 0;
	virtual void SetWindowFocus() = 0;
	virtual bool GetWindowFocus() = 0;
	virtual void SetWindowAttribute(WindowPropery::Attribute wa, int value) = 0;
	virtual int GetWindowAttribute(WindowPropery::Attribute wa) = 0;
	virtual void SetCursorPosition(double x, double y) = 0;
	virtual void GetCursorPosition(double* x, double* y) = 0;
	virtual int GetMouseButton(int index) = 0;
	virtual void OpenFileDialog(const std::string& filterList, const std::string& defaultPath, const std::string& defaultName, DialogType dialogType) = 0;
	virtual std::vector<std::string>& GetSelectedPath() = 0;
	virtual void FreshWindowFocus() = 0;
	virtual bool GetFocus() = 0;

	virtual void Update() {}
	virtual void OnBeginTick() = 0;
	virtual void OnEndTick() = 0;

	void Resizeview(int bx, int by, int w, int h, int vw, int vh);
	void OnWindowCloseCallback(void* window) { m_WindowCloseCallback.Broadcast(window); }
	void OnWindowMoveCallback(void* window) { m_WindowMoveCallback.Broadcast(window); }
	DelegateHandle SetResizeviewCallback(ResizeviewCallbackListDelegate& func) { return m_ResizeviewCallback += std::move(func); }
	void RemoveResizeviewCallback(DelegateHandle& func) { m_ResizeviewCallback -= (func); }
	void SetWindowCloseCallback(WindowCloseCallbackListDelegate& func) { m_WindowCloseCallback += std::move(func); }
	void SetWindowMoveCallback(WindowMoveCallbackListDelegate& func) { m_WindowMoveCallback += std::move(func); }

	FORCEINLINE void SetWindowShoudClose(bool _close)
	{
		m_windowShoudClose = _close;
	}
	FORCEINLINE bool GetWindowShoudClose() 
	{
		return m_windowShoudClose;
	}
	FORCEINLINE void SetWindowCloseNow(bool _close)
	{
		m_windowCloseNow = _close;
	}
	FORCEINLINE bool GetWindowCloseNow()
	{
		return m_windowCloseNow;
	}
};

NS_JYE_END