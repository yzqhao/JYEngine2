
#pragma once

#include "private/Define.h"
#include "Core/Object.h"

NS_JYE_BEGIN

class Scene;
class GObject;
class Component;

class MCommandBase : public Object
{
public:
	MCommandBase();
	virtual ~MCommandBase();

	Scene* GetScene();
	GObject* GetGameObject(uint64 id);
	Component* GetComponent(uint64 id);

	Object* DoIt();
	void Undo();
	void Redo();

	FORCEINLINE void SetSceneID(uint64 sceneID) { m_sceneID = sceneID; }
	FORCEINLINE uint64 GetSceneID() { return m_sceneID; }
	FORCEINLINE void SetSyncType() { m_IsSyncCommand = true; }
	FORCEINLINE bool IsSyncType() { return m_IsSyncCommand; }
	FORCEINLINE void BeginCmd() { m_IsExcuting = true; }
	FORCEINLINE void EndCmd() { m_IsExcuting = false; }
	FORCEINLINE bool IsFinished() { return !m_IsExcuting; }
	FORCEINLINE bool IsExcutOnlyInGame() { return m_IsExcutOnlyInGame; }

protected:
	virtual Object* _DoIt() = 0;
	virtual void _Undo() = 0;
	void _CheckEditorSelectNode(uint64 gid);
		 
private:
	bool m_IsExcuting;		// 命令是否处于正在执行过程中
	bool m_IsSyncCommand;	// 是否是同步类型的命令
	bool m_IsExcutOnlyInGame;
	uint64 m_sceneID;
};
DECLARE_Ptr(MCommandBase);

template<class T>
struct TMCmdReturnTypeTraits
{
};

#define DEFINE_MCMD_RETURN_TYPE(MCMD, baseclassname)	\
	template<>	\
	struct TMCmdReturnTypeTraits< MCMD > { typedef baseclassname TReturnType; };
	
template<typename TType, typename ...Args>
static inline TType* MakeMCommand(Args&&... args)
{
	return _NEW TType(std::forward<Args>(args)...);
}

NS_JYE_END