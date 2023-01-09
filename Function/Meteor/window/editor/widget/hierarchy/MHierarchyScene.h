
#pragma once

#include "private/Define.h"
#include "MHierarchyBaseView.h"

NS_JYE_BEGIN

class Scene;
class GObject;

class MHierarchyScene : public MHierarchyBaseView
{
public:
	MHierarchyScene();
	~MHierarchyScene();

	FORCEINLINE void SetScene(Scene* sce) { m_scene = sce; }

	void DrawNode(GObject* node);
	void Render(GObject* node);

	virtual bool OnGui(float dt);

	void CheckDrag(GObject* node);

private:
	bool _ProcessDragDropSource(GObject* node);
	void _ProcessDragDropTarget(GObject* node);

	bool m_bbodyfail;
	bool m_bedit_lable;
};


NS_JYE_END