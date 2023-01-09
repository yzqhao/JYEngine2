
#pragma once

#include "private/Define.h"
#include "Engine/object/SceneManager.h"

NS_JYE_BEGIN

void SceneExt_Init(Scene* sce);
void SceneExt_CreateGrid(Scene* sce);
void SceneExt_CreateEditorCamera(Scene* sce);
void SceneExt_CreateGenericNode(Scene* sce);

class MSceneManagerExt
{
	SINGLETON_DECLARE_ONCREATE_ONDESTROY(MSceneManagerExt);
public:
	MSceneManagerExt();
	~MSceneManagerExt();

	void _OnCreate();
	void _OnDestroy();

	void InitEditScene(Scene* sce);
	void InitPreviewScene(Scene* sce);

	Scene* GetEditScene();
	Scene* GetPreviewScene();
	void RegisterSyncScene(Scene* sce);
	void ClearScene();

	GObject* GetEditCamera() { return m_EditCamera; }
	GObject* GetGrid() { return m_Grid; }
	uint64 GetEditSceneID() { return m_EditSceneID; }

	const Vector<Scene*> GetSyncSceneList() const { return m_sceneList; }

private:
	uint64 m_PreviewSceneID;
	uint64 m_EditSceneID;
	GObject* m_EditCamera;
	GObject* m_Grid;
	Vector<Scene*> m_sceneList;
};

NS_JYE_END
