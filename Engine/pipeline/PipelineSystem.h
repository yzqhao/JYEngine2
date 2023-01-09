#pragma once

#include "IRenderLoop.h"
#include "../GraphicDefine.h"

NS_JYE_BEGIN

class IRenderLoop;
class IWindow;

class ENGINE_API PipelineSystem
{
	SYSTEM_SINGLETON_DECLEAR(PipelineSystem);
public:

	PipelineSystem();
	virtual ~PipelineSystem();

	virtual IRenderLoop* CreateRenderLoop(GraphicDefine::PiplineType passType) = 0;
	virtual void DeleteRenderLoop(IRenderLoop* pRenderLoop) = 0;

	void Update(float dt);

	uint RenderWindow(IWindow* win, RenderContext& context);

	void AttachScene(Scene* sce);
	void DetachScene(Scene* sce);

private:
	std::map<uint64, Scene*> m_AttachedScenes;
	std::multimap<int, Scene*> m_RenderScene;
};

NS_JYE_END