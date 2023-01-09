
#pragma once

#include "private/Define.h"
#include "System/Singleton.hpp"
#include "MCommandBase.h"

NS_JYE_BEGIN

#define M_CMD_EXEC_BEGIN		MCommandManager::Instance()->BeginExecute()
#define M_CMD_EXEC_END		MCommandManager::Instance()->EndExecute()

class MCommandManager
{
	SINGLETON_DECLARE_ONCREATE_ONDESTROY(MCommandManager)
public:
	MCommandManager();
	~MCommandManager();

	void Reset();

	void _OnCreate();
	void _OnDestroy();

	void BeginExecute();
	void EndExecute();

	template<typename TType, typename ...Args>
	inline typename TMCmdReturnTypeTraits<TType>::TReturnType Execute(Args&&... args)
	{
		m_calCmdCnt++;
		TType* cmd = MakeMCommand<TType>(std::forward<Args>(args)...);
		Object* res = _DoExecute(cmd);
		return static_cast<typename TMCmdReturnTypeTraits<TType>::TReturnType>(res);
	}

	void Undo();
	void Redo();

private:

	Object* _DoExecute(MCommandBase* cmd);
	void _Undo();
	void _Redo();

	Stack<MCommandBasePtr> m_undoCmdStack;
	Stack<MCommandBasePtr> m_redoCmdStack;
	Stack<uint> m_undoCntStack;
	Stack<uint> m_redoCntStack;
	uint m_calCmdCnt;	// 统计用户每步操作需要几个cmd
	uint64 m_triggerSceneID;
	bool m_isExecIng;
};

NS_JYE_END