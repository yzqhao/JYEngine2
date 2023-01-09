
#pragma once

#include "private/Define.h"
#include "System/Delegate.h"

NS_JYE_BEGIN

class MMemuWindowBase
{
public:
	MMemuWindowBase(const String& name);
	virtual ~MMemuWindowBase();

	FORCEINLINE bool IsOnGui() const { return m_isOnGui; }
	FORCEINLINE void SetOnGui(bool flag) { m_isOnGui = flag; }
	FORCEINLINE void FileSearchingFinish() { m_filesearching = false; }
	FORCEINLINE const String GetTitleName() { return m_titleName; }

	virtual void ClearScene() {}
	virtual bool OnGui(float dt) = 0;

protected:

	bool m_isOnGui;
	bool m_filesearching;
	String m_titleName;
};

NS_JYE_END