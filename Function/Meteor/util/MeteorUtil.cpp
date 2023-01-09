#include "MeteorUtil.h"
#include "Core/Interface/IWindow.h"

#include <algorithm>

NS_JYE_BEGIN

void MeteorUtil::OpenSaveFileDialog(IWindow* pWnd, const String& filter, String& defaultPath, const String& filename)
{
	std::replace(defaultPath.begin(), defaultPath.end(), '/', '\\');
	pWnd->OpenFileDialog(filter.c_str(), defaultPath.c_str(), filename, IWindow::SaveFile);
}

void MeteorUtil::OpenFileDialog(IWindow* pWnd, const String& filter, String& defaultPath)
{
	std::replace(defaultPath.begin(), defaultPath.end(), '/', '\\');
	pWnd->OpenFileDialog(filter, defaultPath, "", IWindow::SingleFile);
}

void MeteorUtil::OpenMultiFileDialog(IWindow* pWnd, const String& filter, String& defaultPath)
{
	std::replace(defaultPath.begin(), defaultPath.end(), '/', '\\');
	pWnd->OpenFileDialog(filter, defaultPath, "", IWindow::MultiFiles);
}

void MeteorUtil::OpenFloderDialog(IWindow* pWnd, String& defaultPath)
{
	std::replace(defaultPath.begin(), defaultPath.end(), '/', '\\');
	pWnd->OpenFileDialog("", defaultPath, "", IWindow::Folder);

}

void MeteorUtil::GetSelectedPath(IWindow* pWnd, Vector<String>& outselect)
{
	outselect = pWnd->GetSelectedPath();
	for (std::string& path : outselect)
	{
		replace(path.begin(), path.end(), '\\', '/');
	}
}

NS_JYE_END