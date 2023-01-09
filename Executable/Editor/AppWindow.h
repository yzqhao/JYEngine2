#pragma once

#include "private/Define.h"
#include "Core/Interface/IWindow.h"

struct GLFWwindow;

NS_JYE_BEGIN

class AppWindow : public IWindow
{
public:
	AppWindow(GLFWwindow* ud, bool own, uint w, uint h);
	virtual ~AppWindow();

	virtual void SetClipboardText(const char* text) override;
	virtual const char* GetClipboardText() override;
	virtual void GetWindowSize(int* w, int* h) override;
	virtual void SetWindowSize(int x, int y) override;
	virtual void GetWindowPosition(int* x, int* y) override;
	virtual void SetWindowPosition(int x, int y) override;
	virtual void DestroyWindow() override;
	virtual void HideWindow() override;
	virtual void MaximizeWindow() override;
	virtual void ShowWindow(bool taskbar) override;
	virtual void SetWindowTitle(const std::string& title) override;
	virtual bool GetWindowMinimized() override;
	virtual void SwapBuffers() override;
	virtual void SwapInterval(int i) override;
	virtual int GetContextVersion() override;
	virtual uint GetMonitorCount() override;
	virtual void GetMonitorInfomation(uint index, int* x, int* y, int* w, int* h) override;
	virtual IWindow* CreateSubWindows(int x, int y, int w, int h, bool contextonly, bool nodecoration, bool topmost) override;
	virtual void* GetWindowHanle() override;
	virtual void* GetNativeWindow() override;
	virtual bool IsMainWindow() override;
	virtual bool IsFull() override;
	virtual void SetWindowMode(bool bWindow) override;
	virtual void SetWindowFocus() override;
	virtual bool GetWindowFocus() override;
	virtual void SetWindowAttribute(WindowPropery::Attribute wa, int value) override;
	virtual int GetWindowAttribute(WindowPropery::Attribute wa) override;
	virtual void SetCursorPosition(double x, double y) override;
	virtual void GetCursorPosition(double* x, double* y) override;
	virtual int GetMouseButton(int index) override;
	virtual void OpenFileDialog(const std::string& filterList, const std::string& defaultPath, const std::string& defaultName, DialogType dialogType) override;
	virtual std::vector<std::string>& GetSelectedPath() override;
	virtual void FreshWindowFocus() override;
	virtual bool GetFocus() override;

	virtual void OnBeginTick() override;
	virtual void OnEndTick() override;

	static IWindow* TryFindWindow(GLFWwindow* h);

protected:

	void _SetWindowAttribute(WindowPropery::Attribute wa, int value);
	bool _OpenFileDialog(const std::string& filterList, const std::string& defaultPath, const std::string& defaultName, DialogType dialogType);

	GLFWwindow* m_pWindow{};
	void* m_nativeWindow{}; 
	bool m_isMainWindow{ false };
	bool m_bWindow{ true };
	bool m_isOwned{ false };
	bool m_isOpenDialog{ false };
	bool m_isFocuse { false };

	std::string						m_defaultPath;
	std::string						m_filter;
	std::string						m_defaultName;
	std::vector<std::string>		m_selectedPath;
	DialogType						m_dialogType;

	int m_x{};
	int m_y{};
	int m_w{};
	int m_h{};

	static std::map<uint64, IWindow*> s_WindowMap;
	static bool s_MainWindowCreated;
};

NS_JYE_END
