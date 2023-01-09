#include "private/Define.h"

NS_JYE_BEGIN

class IWindow;

namespace MeteorUtil
{
	void OpenSaveFileDialog(IWindow* pWnd, const String& filter, String& defaultPath, const String& filename);
	void OpenFileDialog(IWindow* pWnd, const String& filter , String& defaultPath);
	void OpenMultiFileDialog(IWindow* pWnd, const String& filter, String& defaultPath);
	void OpenFloderDialog(IWindow* pWnd, String& defaultPath);
	void GetSelectedPath(IWindow* pWnd, Vector<String>& outselect);
}

NS_JYE_END