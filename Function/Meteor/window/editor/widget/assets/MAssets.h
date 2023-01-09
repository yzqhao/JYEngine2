
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "../MMemuWindowBase.h"

NS_JYE_BEGIN

class MFileItem;

class MAssets : public MMemuWindowBase
{
public:
	MAssets();
	~MAssets();

	void OnAssetsPathChange(StaticDelegate<void()>& event);
	void GetFileSystemTree(StaticDelegate<void()>& event);

	virtual bool OnGui(float dt);

	void CheckDrag(const String& path);

private:

	void _CheckContextWindow(const String* filename);
	void _Showdirectory(MFileItem* fi);
	bool _ProcessDragDropSource(const String& name);
	void _ProcessDragDropTarget(const String& name);

	String m_projectpath;
	bool m_hasdropdown;
	bool fbxdialog;
	bool objDialog;
	bool fbodialog;
	bool meshdialog;
	bool matdialog;
	bool frameanidialog;
	bool showmetafile;
	uint currentID;
	String importfbxpath;
	String importObjPath;
};

NS_JYE_END