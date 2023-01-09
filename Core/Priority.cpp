#include "Priority.h"

NS_JYE_BEGIN

bool Priority::AddPriorityThan(Priority* pPriority)
{
	if (!pPriority)
		return 0;
	if (pPriority->CheckIsCircle(this))
		return 0;

	m_uiPriorityNum = 0;

	m_pPriorityThanList.push_back(pPriority);

	return true;
}

uint Priority::GetPriorityNum()
{
	CheckPriorityNum();
	return m_uiPriorityNum;
}

Priority::Priority()
{
}

Priority::Priority(unsigned int uiPriorityNum)
{
	m_uiPriorityNum = uiPriorityNum;
}

Priority::~Priority()
{
	m_pPriorityThanList.clear();
}

Priority& Priority::operator= (Priority& rPriority)
{
	rPriority.CheckPriorityNum();
	m_uiPriorityNum = rPriority.m_uiPriorityNum;
	m_pPriorityThanList = rPriority.m_pPriorityThanList;
	return *this;
}

bool Priority::operator== (Priority& rPriority)
{
	rPriority.CheckPriorityNum();
	CheckPriorityNum();
	return (m_uiPriorityNum == rPriority.m_uiPriorityNum);
}

bool Priority::operator!= (Priority& rPriority)
{
	rPriority.CheckPriorityNum();
	CheckPriorityNum();
	return (m_uiPriorityNum != rPriority.m_uiPriorityNum);
}

bool Priority::operator >(Priority& rPriority)
{
	rPriority.CheckPriorityNum();
	CheckPriorityNum();
	return (m_uiPriorityNum > rPriority.m_uiPriorityNum);
}

bool Priority::operator <(Priority& rPriority)
{
	rPriority.CheckPriorityNum();
	CheckPriorityNum();
	return (m_uiPriorityNum < rPriority.m_uiPriorityNum);
}

bool Priority::operator >=(Priority& rPriority)
{
	rPriority.CheckPriorityNum();
	CheckPriorityNum();
	return (m_uiPriorityNum >= rPriority.m_uiPriorityNum);
}

bool Priority::operator <=(Priority& rPriority)
{
	rPriority.CheckPriorityNum();
	CheckPriorityNum();
	return (m_uiPriorityNum <= rPriority.m_uiPriorityNum);

}

uint Priority::CheckPriorityNum()
{
	if (m_uiPriorityNum || !m_pPriorityThanList.size())
		return m_uiPriorityNum;
	else
	{
		unsigned int uiMax = 0;
		for (unsigned int i = 0; i < m_pPriorityThanList.size(); i++)
		{
			if (uiMax < m_pPriorityThanList[i]->CheckPriorityNum())
				uiMax = m_pPriorityThanList[i]->m_uiPriorityNum;

		}
		m_uiPriorityNum = uiMax + 1;
		return m_uiPriorityNum;
	}
}

bool Priority::CheckIsCircle(const Priority* pChecked)const
{
	if (!pChecked)
		return false;

	for (unsigned int i = 0; i < m_pPriorityThanList.size(); i++)
	{
		if (pChecked == m_pPriorityThanList[i])
			return true;
		else
			return m_pPriorityThanList[i]->CheckIsCircle(pChecked);

	}
	return false;
}

NS_JYE_END
