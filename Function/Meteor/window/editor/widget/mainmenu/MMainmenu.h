
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "../MMemuWindowBase.h"

NS_JYE_BEGIN

class MSelectScene;
class MInputBox;

enum M_OPERATE_STATE
{
	DoNothing = 0,
	NewSceneOpenDiolog = 3,
	NewSceneSaveScene = 5,
	NewSceneDo = 6,
	OpenSceneOpenChoseWindow = 7,
	OpenSceneGetPath = 8,
	OpenSceneOpenDiolog = 9,
	OpenSceneLoadScene = 10,
	OpenSceneDoSave = 11,
	OpenSceneOpenSaveWindow = 12,
	ExportSceneOpenWindow = 13,
	ExportGiftSceneOpenWindow = 14,
	ExportSceneDo = 15,
	ExportSceneOpenDiolog = 16,
	ExportSceneOpenChineseDiolog = 17,
	ExportSceneOpenInvalidCameraDialog = 18,
};

enum M_CLOSEAPP_STATE
{
	CLOSEAPP_DoNothing = 0,
	CLOSEAPP_OpenDiolog = 1,
	CLOSEAPP_OpenSaveWindow = 2,
	CLOSEAPP_DoSave = 3,
	CLOSEAPP_DO = 4
};

class MMainmenu 
{
public:
	MMainmenu();
	~MMainmenu();

	void NewScene();
	void OpenScene();
	void SaveScene();
	void SaveSceneAs();
	void ExportScene();
	void ShowFileMenu();
	void ShowEditMenu();
	void ShowDebugMenu();
	void ShowWindowsMenu();
	void ShowExtraToolsMenu();
	void ShowCloseAppMenu();

	virtual bool OnGui(float dt);

private:
	void _OpenSaveDiolog();
	void _LoadScene();
	void _OnSelectScene(const String& name);
	bool _OnNewSceneName(const String& name, String& outError);

	MSelectScene* m_selectSceneDialog;
	MInputBox* m_createSceneInpuBox;
	M_OPERATE_STATE m_curState;
	M_CLOSEAPP_STATE m_closeAppState;
	uint64 m_windowFlags;
	String m_selectSceneFile;
	bool m_initDiologPos = false;
	bool m_selectedPathNULL = false;
	bool m_exportSceneSuc = false;
	bool m_exportPathHaveChinese = false;
	bool m_focused = false;
	bool m_showSelectSceneDialog = false;
	bool m_showCreateSceneInputBox = false;
};

NS_JYE_END