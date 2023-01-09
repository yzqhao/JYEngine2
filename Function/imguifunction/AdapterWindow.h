#include "private/Define.h"

#include "Core/Interface/IApplication.h"

#include "AdapterRender.h"

NS_JYE_BEGIN

class AdapterWindow 
{
private:
	AdapterRender* m_pAdapterRender;
	ImGuiViewport* m_rpImGuiViewport;
	IWindow* m_rpHostWindow;
public:
	AdapterWindow(IWindow* w, ImGuiViewport* vp);
	~AdapterWindow();
public:
	static void SetupWindow();
public:
	void OnWindowClose(void* window);
	void OnWindowMove(void* window);
	void OnWindowDestroy();
public:
	FORCEINLINE void Render(ImDrawData* draw_data);
};


FORCEINLINE void AdapterWindow::Render(ImDrawData* draw_data)
{
	m_pAdapterRender->Render(draw_data);
}

NS_JYE_END