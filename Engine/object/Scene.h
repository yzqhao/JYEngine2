#pragma once

#include "Engine/private/Define.h"
#include "GObject.h"
#include "Engine/component/Component.h"
#include "ObjectIdAllocator.h"

NS_JYE_BEGIN

class CameraComponent;
class IWindow;
class RenderContext;
class IRenderQueue;
class ILightQueue;
class RenderTargetEntity;

class ENGINE_API Scene : public Object
{
	friend class GObject;
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	using SceneObjectsMap = std::unordered_map<uint64, GObject*>;
	using CameraComponentMap = std::unordered_map<uint64, CameraComponent*>;
	using WindowCameraSequence = std::map<IWindow*, std::vector<CameraComponent*>>;
public:
	Scene();
	Scene(uint64 sid, const String& name);
	virtual ~Scene();

	void Tick(float dt);

	FORCEINLINE const SceneObjectsMap& GetAllGObjects() const { return m_gobjects; }
	FORCEINLINE const String& GetName() const { return m_name; }
	FORCEINLINE IRenderQueue* GetRenderQueue() { return m_renderQueue; }
	FORCEINLINE ILightQueue* GeLightQueue() { return m_lightQueue; }
	FORCEINLINE uint GenerateID32() { return ObjectIDAllocator::AllocID32(); }
	FORCEINLINE int GetSequence() const { return m_Sequence; }
	FORCEINLINE void SetSequence(int s) { m_Sequence = s; }
	FORCEINLINE uint64 GetStaticID() const { return m_StaticID; }
	FORCEINLINE GObject* GetRootNode() { return m_rootObject; }
	FORCEINLINE CameraComponent* GetMainCamera() { return m_MainCamera; }

	int Render(IWindow* win, RenderContext& context);

	GObject* GetGObjectByID(uint64 go_id) const;
	void ClearNodes();

	GObject* CreateObject(const String& objName);
	void DeleteGObject(GObject* node);
	void DeleteGObjectByID(uint64 go_id);

	void AttachCamera(CameraComponent* com);
	void DetachCamera(CameraComponent* com);
	void SetMainCamera(CameraComponent* mainCamera);
	void CloneEvent(uint64 goid, GObject* cloneNode);

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLoad(Stream* pStream) override;

#ifdef _EDITOR
	void SetEditorCamera(CameraComponent* mainCamera);
	RenderTargetEntity* CreateDefaultRenderTarget(const Math::IntVec2& resolution);
	RenderTargetEntity* GetDefaultRenderTarget() const;
	void ChangeDefaultResolution(const Math::IntVec2& resolution);
	void AddToScene(GObject* go);
#endif

protected:

	void _DoDeleteNode(GObject* node);
	void _OnStart(GObject* node);
	void _OnUpdate(GObject* node, float dt);
	void _OnLateUpdate(GObject* node, float dt);
	void _ChangeGObjectID(Animatable* node, uint64 oldid, uint64 newid);

	String m_name;
	SceneObjectsMap m_gobjects;
	CameraComponentMap m_pAttachedCameras;
	WindowCameraSequence m_pWindowCameraSequence;
	CameraComponent* m_MainCamera;
	GObject* m_rootObject;
	IRenderQueue* m_renderQueue;
	ILightQueue* m_lightQueue;
	int m_Sequence;
	uint64 m_StaticID;

#ifdef _EDITOR
	CameraComponent* m_editorCamera;
	RenderTargetEntity* m_pDefaultRenderTarget;
	Math::IntVec2 m_ChangeResolution;
#endif
};
DECLARE_Ptr(Scene);
TYPE_MARCO(Scene);

NS_JYE_END