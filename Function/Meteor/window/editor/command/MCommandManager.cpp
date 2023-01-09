#include "MCommandManager.h"
#include "MCommandBase.h"
#include "util/MSceneManagerExt.h"
#include "Engine/object/Scene.h"

NS_JYE_BEGIN

SINGLETON_IMPLEMENT(MCommandManager)

MCommandManager::MCommandManager()
	: m_triggerSceneID(0)
	, m_calCmdCnt(0)
	, m_isExecIng(false)
{
	Reset();
}

MCommandManager::~MCommandManager()
{

}

void MCommandManager::_OnCreate()
{

}

void MCommandManager::_OnDestroy()
{

}

void MCommandManager::Reset()
{
	m_undoCmdStack.swap(Stack<MCommandBasePtr>());
	m_redoCmdStack.swap(Stack<MCommandBasePtr>());
	m_undoCntStack.swap(Stack<uint>());
	m_redoCntStack.swap(Stack<uint>());
	m_triggerSceneID = 0;
}

void MCommandManager::BeginExecute()
{
	JY_ASSERT(m_isExecIng == false);
	m_isExecIng = true;
	m_calCmdCnt = 0;
}

void MCommandManager::EndExecute()
{
	JY_ASSERT(m_isExecIng == true);
	m_isExecIng = false;
	
	m_undoCntStack.push(m_calCmdCnt);
}

void MCommandManager::Undo()
{
	if (m_undoCmdStack.empty())
	{
		JYWARNING("=== Nothing to undo! ===");
		return;
	}

	uint cnt = m_undoCntStack.top();
	m_undoCntStack.pop();
	m_redoCntStack.push(cnt);

	for (int i = 0 ; i < cnt; ++i)
	{
		_Undo();
	}
}

void MCommandManager::Redo()
{
	if (m_redoCntStack.empty())
	{
		JYWARNING("=== Nothing to redo! ===");
		return;
	}

	uint cnt = m_redoCntStack.top();
	m_redoCntStack.pop();
	m_undoCntStack.push(cnt);

	for (int i = 0; i < cnt; ++i)
	{
		_Redo();
	}
}

void MCommandManager::_Undo()
{
	if (m_undoCmdStack.size() > 0)
	{
		MCommandBasePtr cmd = m_undoCmdStack.top();
		m_undoCmdStack.pop();
		m_redoCmdStack.push(cmd);

		m_triggerSceneID = MSceneManagerExt::Instance()->GetEditSceneID();
		cmd->SetSceneID(m_triggerSceneID);

		if (!cmd->IsExcutOnlyInGame())
		{
			cmd->Undo();
		}

		auto& scenelist = MSceneManagerExt::Instance()->GetSyncSceneList();
		for (auto it : scenelist)
		{
			cmd->SetSceneID(it->GetStaticID());
			cmd->Undo();
		}
	}
	else
	{
		JYWARNING("Nothing to undo!");
	}
}

void MCommandManager::_Redo()
{
	if (m_redoCmdStack.size() > 0)
	{
		MCommandBasePtr cmd = m_redoCmdStack.top();
		m_redoCmdStack.pop();
		m_undoCmdStack.push(cmd);

		m_triggerSceneID = MSceneManagerExt::Instance()->GetEditSceneID();
		cmd->SetSceneID(m_triggerSceneID);

		if (!cmd->IsExcutOnlyInGame())
		{
			cmd->Redo();
		}

		auto& scenelist = MSceneManagerExt::Instance()->GetSyncSceneList();
		for (auto it : scenelist)
		{
			cmd->SetSceneID(it->GetStaticID());
			cmd->Redo();
		}
	}
	else
	{
		JYWARNING("Nothing to redo!");
	}
}

Object* MCommandManager::_DoExecute(MCommandBase* cmd)
{
	Object* res = nullptr;

	m_triggerSceneID = MSceneManagerExt::Instance()->GetEditSceneID();
	cmd->SetSceneID(m_triggerSceneID);

	m_undoCmdStack.push(cmd);

	if (!cmd->IsExcutOnlyInGame())
	{
		res = cmd->DoIt();
	}

	auto& scenelist = MSceneManagerExt::Instance()->GetSyncSceneList();
	for (auto it : scenelist)
	{
		cmd->SetSceneID(it->GetStaticID());
		res = cmd->DoIt();
	}

	return res;
}


NS_JYE_END