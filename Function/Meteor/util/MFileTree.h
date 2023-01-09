
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "System/Singleton.hpp"

NS_JYE_BEGIN

class MFileItem
{
public:
	MFileItem(const String& name, bool isDir);
	~MFileItem();

	FORCEINLINE bool IsDirectory() const { return m_isDirectory; }
	FORCEINLINE bool IsFile() const { return !m_isDirectory; }
	FORCEINLINE uint GetUUID() const { return m_uuid; }
	FORCEINLINE const String& GetName() const { return m_name; }
	FORCEINLINE MFileItem* GetParent() const { return m_RootNode; }
	FORCEINLINE const Vector<MFileItem*>& GetChildren() const { return m_children; }

	bool AddItem(MFileItem* node);
	bool RemoveItem(MFileItem* actor);

private:
	void _SetFather(MFileItem* root);
	bool _DetachNode(MFileItem* actor);
	bool m_isDirectory;
	bool m_visible;
	uint m_uuid;
	String m_name;
	Vector<MFileItem*> m_children;
	MFileItem* m_RootNode;
};

class MFileTree
{
	using FileTreeMap = std::unordered_map<uint, MFileItem*>;
public:
	MFileTree(const String& rootpath);
	~MFileTree();

	FORCEINLINE MFileItem* GetRootNode() { return m_rootObject; }

	MFileItem* CreateFileItem(const String& name, bool bdir);
	MFileItem* GetNodeByID(uint id);
	void DeleteNode(uint id);
	void DeleteNode(MFileItem* item);
private:
	void _DoDeleteNode(MFileItem* item);

	MFileItem* m_rootObject;
	FileTreeMap m_allFiles;
};

extern MFileTree* g_FileTree;

NS_JYE_END