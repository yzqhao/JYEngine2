
#include "IWindow.h"
#include "IApplication.h"

NS_JYE_BEGIN

void IWindow::Resizeview(int bx, int by, int w, int h, int vw, int vh)
{
	IApplication::Instance()->Pasue();
	SetWindowSize(w, h);
	m_ResizeviewCallback.Broadcast(bx, by, w, h, vw, vh);
	IApplication::Instance()->Resume();
}


NS_JYE_END