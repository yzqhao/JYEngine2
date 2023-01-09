#include "MWindow.h"
#include "project/MProject.h"
#include "editor/MEditor.h"
#include "editor/MEditorService.h"

NS_JYE_BEGIN

MWindow::MWindow()
	: m_idx(MWI_Project)
{
	m_windows.push_back(_NEW MProject());
	m_windows.push_back(_NEW MEditor());

	m_windows[m_idx]->Show();
}

MWindow::~MWindow()
{
	for (auto& it : m_windows)
	{
		SAFE_DELETE(it);
	}
}

void MWindow::Update(float dt)
{
	MWindowIndex idx = m_windows[m_idx]->Update(dt);

	if (idx != m_idx)
	{
		if (idx == MWI_Editor)
		{
			InitService(static_cast<MEditor*>(m_windows[m_idx]));
		}

		m_windows[m_idx]->Close();
		m_windows[idx]->Show();
		m_idx = idx;
	}
}

NS_JYE_END