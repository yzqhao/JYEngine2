
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "../MMemuWindowBase.h"
#include "MHierarchyBaseView.h"

NS_JYE_BEGIN

class MHierarchy : public MMemuWindowBase
{
public:
	MHierarchy();
	~MHierarchy();

	virtual void ClearScene();
	virtual bool OnGui(float dt);

private:

	bool m_bshowPrefabDialog;
	bool m_bshownContextMenu;
	Vector<MHierarchyBaseView*> m_views;
	MHierarchyBaseView::TMHierarchyView m_curViewType;
};

NS_JYE_END