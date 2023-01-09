
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "Engine/render/texture/TextureEntity.h"
#include "System/Singleton.hpp"
#include <variant>
#include "Engine/GraphicDefine.h"
#include "MDefined.h"

NS_JYE_BEGIN

class MAssets;
class MEditor;
class GObject;
class Scene;
class TextureEntity;

class EditorSystem
{
	SINGLETON_DECLARE_ONCREATE_ONDESTROY(EditorSystem);
public:
private:
	using AssetsPathChangeCaller = RawDelegate<false, MAssets, void(StaticDelegate<void()>&)>;
	using OpenSceneCaller = RawDelegate<false, MEditor, void()>;

	class EditorSystemaSelect
	{
	public:
		enum TSelected
		{
			TSelected_String,
			TSelected_GObject,
			TSelected_Scene,
		};
		EditorSystemaSelect();
		~EditorSystemaSelect();
		void Select(GObject* node);
		void Select(Scene* node);
		void Select(const String& node);
		GObject* GetSelectedGObject();
		Scene* GetSelectedScene();
		String* GetSelectedString();
	private:
		GObject* m_go;
		Scene* m_sce;
		String* m_str;
		TSelected m_selectType = TSelected_GObject;
	};
public:
	EditorSystem();
	~EditorSystem();

	void _OnCreate();
	void _OnDestroy();

	void Update(float dt);

	void OpenScene(const String& scenefile, OpenSceneCaller& openevent);
	void SetAssetsPath(const String& path);
	void CreateScene(const String& path, const String& name);

	void OnOpenSceneEvent();

	void CreateObjectFrombundle(const String& filename);

	void DefaultScene();
	void CreateCameraNode(bool flag);
	GObject* CreateEmptyNode(const String& nodename);
	GObject* CreateLight(GraphicDefine::LightType lt);
	TextureEntity* GetEditorCameraOutput();

	GObject* GetNodeByID(uint64 id);
	void Select(GObject* node);
	void Select(Scene* node);
	void Select(const String& node);
	GObject* GetSelectedGObject();
	Scene* GetSelectedScene();
	String* GetSelectedString();
	void Destroy(GObject* node);
	void DestroyNodeTree(GObject* node);
	void ClearScene();

	//void Play();
	//void Pause();
	//void Stop();

	FORCEINLINE const Map<String, TextureEntity*>& GetIcons() const { return m_icons;  }
	FORCEINLINE void RegisterAssetsPathChange(AssetsPathChangeCaller& call) { m_OnAssetsPathChangeCaller = std::move(call); }
	FORCEINLINE bool GetPrefabMode() const { return m_prefabMode; }
	FORCEINLINE void SetShowGrid(bool show) { m_bshowGrid = show; }
	FORCEINLINE bool GetShowGrid() const { return m_bshowGrid; }
	FORCEINLINE const String& GetSceneFile() const { return m_sceneFile; }
	FORCEINLINE bool ShowEverything() const { return m_showEverything; }
	FORCEINLINE void SetShowStaus(bool show) { m_showEverything = show; }
	//FORCEINLINE bool IsPlay() { return m_EditorStatus & MDefined::PLAY != 0; }
	//FORCEINLINE bool IsPause() { return (m_EditorStatus & MDefined::PLAY != 0) && (m_EditorStatus & MDefined::PAUSE != 0); }
	//FORCEINLINE bool IsStop() { return m_EditorStatus & MDefined::STOP; }

private:
	Map<String, TextureEntity*> m_icons;
	bool m_prefabMode;
	bool m_bshowGrid;
	bool m_showEverything;
	String m_sceneFile;
	AssetsPathChangeCaller m_OnAssetsPathChangeCaller;
	OpenSceneCaller m_openSceneCaller;
	EditorSystemaSelect m_select;
	//MDefined::EditorStatus m_EditorStatus;
};

NS_JYE_END