
#pragma once

#include "private/Define.h"
#include "Engine/object/Scene.h"

NS_JYE_BEGIN

namespace MSceneExt
{
	enum SceneSequence
	{
		HIGH = 0,
		MID = 10,
		LOW = 20,
	};

	void SceneInit(Scene* sce);
	GObject* SceneCreateGrid(Scene* sce);
	GObject* SceneCreateEditorCamera(Scene* sce);
	GObject* SceneCreateGenericNode(Scene* sce, const String& name);
	String GenerateNonRepeatName(GObject* node);

	// gameobjectœ‡πÿ
	bool CheckLegalName(GObject* node, const String& name);
	void SetPrefabPathInEdSc(GObject* node, const String& name);
	void SetHostPrefabPathInEdSc(GObject* node, const String& name);
	void GetNoClothDebugChildrens(GObject* node, Vector<GObject*>& outChildren);
}

NS_JYE_END
