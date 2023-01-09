#include "private/Define.h"
#include "Core/Interface/IApplication.h"
#include "Engine/component/render/RenderComponent.h"
#include "Engine/component/camera/CameraComponent.h"
#include "Engine/object/GObject.h"

#include "imgui.h"

NS_JYE_BEGIN

class AdapterRender
{
private:
	typedef std::vector< std::pair<GObject*, RenderComponent*> >	RenderNodeArray;
private:
	static uint				s_CameraSequence;
private:
	RenderNodeArray			m_RenderNodeArray;
	GObject* m_RootNode;
	CameraComponent* m_rpCameraComponent;
	uint					m_RenderIndex;
	MaterialEntity* m_pUIMat;
public:
	AdapterRender(IWindow* win);
	~AdapterRender();
public:
	static void SetupRender();
private:
	void _BeginRender();
public:
	RenderComponent* GetRenderComponent(int subCount);
	void Render(ImDrawData* draw_data);
	void OnWindowDestroy();
public:
	FORCEINLINE CameraComponent* GetCameraComponent();
};



FORCEINLINE CameraComponent* AdapterRender::GetCameraComponent()
{
	return m_rpCameraComponent;
}

NS_JYE_END