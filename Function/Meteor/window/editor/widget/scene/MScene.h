
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"
#include "../MMemuWindowBase.h"

NS_JYE_BEGIN

class GObject;
class CameraComponent;

class MScene : public MMemuWindowBase
{
public:
	MScene();
	~MScene();

	virtual bool OnGui(float dt);

	void SetGridShow(GObject* grid, bool show);
	void ProcessInput();

private:
	void _UpdateGrid(GObject* grid, CameraComponent* cam);

	float m_mousespeed;
	float m_wheelspeed;
	float m_keyspeed;
	float m_dragthreshold;
	float m_keypressrate;
	float m_keypressdelay;
	bool m_isGridShow;
};

NS_JYE_END