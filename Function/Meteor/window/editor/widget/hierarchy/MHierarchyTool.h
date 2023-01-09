
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "Engine/object/GObject.h"

NS_JYE_BEGIN

namespace MHierarchyTool
{
	bool NewNodeContextMenu();
	bool IsNodeVisible(GObject* node);
	Math::AABB GetBindBox(GObject* node);

	template <typename TCom>
	void SearchComponent(Vector<TCom*>& outComs, GObject* node)
	{
		TCom* pcom = node->TryGetComponent<TCom>();
		if (pcom)
		{
			outComs.push_back(pcom);
		}

		auto& childs = node->GetChildren();
		for (auto& child : childs)
		{
			SearchComponent(outComs, chilld);
		}
	}
};

NS_JYE_END