
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "../MMemuWindowBase.h"
#include "Math/3DMath.h"
#include "Core/Rtti.h"

NS_JYE_BEGIN

class MFileTree;
class MFileItem;

class MSelectWindow : public MMemuWindowBase
{
public:
	DECLARE_DELEGATE_RET(SelectDelegate, void, const String&, Object*);	// path, entity
	DECLARE_DELEGATE_RET(CloseDelegate, void, const String&);
public:
	MSelectWindow();
	~MSelectWindow();

	void Open(const Vector<String>& typefilter, SelectDelegate& selectFunc, bool showFolder, const Vector<String>& extraPath);
	void Open(const Vector<String>& typefilter, SelectDelegate& selectFunc, bool showFolder, const Vector<String>& extraPath, CloseDelegate& closeFunc, const String& titleName);
	void Close();

	void ShowFilesByFilter(MFileItem* filetrees, const Vector<String>& typefilter, const String& namefilter, SelectDelegate& callback);
	void ShowFolderFilesByFilter(Vector<MFileTree*>& filetrees, const Vector<String>& typefilter, const String& namefilter, SelectDelegate& callback);

	void showdirectory(Vector<MFileTree*>& filetrees, SelectDelegate& callback, const String& foldername = "");
	void showSerchFoders(Vector<MFileTree*>& filetrees, SelectDelegate& callback);

	virtual bool OnGui(float dt);

	FORCEINLINE void SetRttiFilter(Rtti* rtti) { m_type_rtti = rtti; }

private:
	void _AddExtrafileList(Vector<MFileTree*>& filetree);
	void _DoShowdirectory(MFileItem* fileitem, SelectDelegate& callback, const String& foldername = "");
	Object* CreateEntity(const String& targetpath, const String& ext);

	bool m_open;
	String m_namefilter;
	SelectDelegate m_selectCallBack;
	CloseDelegate m_closeCallBack;
	Vector<String> m_typefilter;	// 选取的后缀名
	bool m_br;
	int m_window_flags;
	bool m_showFolder;
	String m_targetPath;
	Rtti* m_type_rtti;

	//self.serchFolder = {};
	String m_warningText;
	//self.extraFileTree = {};
	//self.specialExtraFile = {};
	Vector<MFileTree*> m_filetree;
	Vector<String> m_extraPath;
	bool m_hasInit;
};

extern MSelectWindow* g_SeclectWindow;

NS_JYE_END