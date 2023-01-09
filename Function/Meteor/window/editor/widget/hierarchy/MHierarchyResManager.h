
#pragma once

#include "private/Define.h"
#include "System/Singleton.hpp"

NS_JYE_BEGIN

class TextureEntity;
class GObject;

enum MTHierarchyRes
{
	M_Hierarchy_New,
	M_Hierarchy_Layer,
	M_Hierarchy_Quad,
	M_Hierarchy_Light,
	M_Hierarchy_Camera,
	M_Hierarchy_Sphere,
	M_Hierarchy_Box,
	M_Hierarchy_Particular,
	M_Hierarchy_Effect,
	M_Hierarchy_Face,
	M_Hierarchy_Count,
};

struct MHierarchyResInfo
{
	String m_path;
	String m_tips;
	TextureEntity* m_tex;
};

class MHierarchyResManager
{
	SINGLETON_DECLARE_ONCREATE_ONDESTROY(MHierarchyResManager);
public:
	MHierarchyResManager();
	~MHierarchyResManager();

	void _OnCreate();
	void _OnDestroy();

	void LoadResources();
	struct MHierarchyResInfo& GetIconRes(MTHierarchyRes tres) { return m_res[tres];  }
	TextureEntity* GetNodeIcon(GObject* node);

private:
	Vector<MHierarchyResInfo> m_res;
};

NS_JYE_END