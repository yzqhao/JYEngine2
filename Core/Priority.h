
#pragma once

#include "Core.h"

NS_JYE_BEGIN

class CORE_API Priority
{
public:
	Priority();
	Priority(uint uiPriorityNum);
	~Priority();
	bool AddPriorityThan(Priority* pPriority);
	uint GetPriorityNum();

	Priority& operator = (Priority& rPriority);
	bool operator== (Priority& rPriority);
	bool operator!= (Priority& rPriority);


	bool operator >(Priority& rPriority);
	bool operator <(Priority& rPriority);

	bool operator >=(Priority& rPriority);
	bool operator <=(Priority& rPriority);

private:
	bool CheckIsCircle(const Priority* pChecked)const;
	uint CheckPriorityNum();
	//0既表示优先级别最大，也或者没有优先级。如果m_pPriorityThanList无成员，表示最大有限级别
	//否则需要计算优先级别
	uint m_uiPriorityNum = {};
	Vector<Priority*> m_pPriorityThanList;
};

#define DECLARE_PRIORITY \
public:\
	static  Priority ms_Priority;

#define IMPLEMENT_PRIORITY(classname)\
	Priority classname::ms_Priority;

#define ADD_PRIORITY(classname)\
	if(!ms_Priority.AddPriorityThan(&classname::ms_Priority))\
		return 0;

NS_JYE_END
