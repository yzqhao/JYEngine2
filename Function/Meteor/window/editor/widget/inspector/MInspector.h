
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "../MMemuWindowBase.h"
#include "MPanelBase.h"

NS_JYE_BEGIN

class GObject;
class Scene;

class MInspector : public MMemuWindowBase
{
public:
	MInspector();
	~MInspector();

	virtual bool OnGui(float dt);

private:

	void _OnNode(GObject* node);
	void _OnScene(Scene* sce);
	void _OnComponents(GObject* node);
	void _AddComponent(GObject* node);
	void _AddComponentPopup(GObject* node);

	HashMap<String, MPanelBasePtr> m_panels;
};

NS_JYE_END