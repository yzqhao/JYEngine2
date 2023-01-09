#pragma once

#include "Engine/private/Define.h"
#include "Core/Object.h"
#include "ObjectIdAllocator.h"
#include "Engine/component/Component.h"
#include "System/Singleton.hpp"

#include <random>

NS_JYE_BEGIN

class Scene;

class ENGINE_API SceneManager : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL

	SINGLETON_DECLARE(SceneManager);

	friend Scene;
private:
	SceneManager();
	virtual ~SceneManager();

	void InsertScene(Scene* sce);
public:
	Scene* CreateScene(const String& sceneName);
	Scene* GetScene(const String& sceneName) const;
	void DeleteSceneByName(const String& sceneName);
	void DeleteScene(Scene* sce);
	Scene* GetScenebyId(uint64 staticId) const;
	Scene* GetUtilityScene() const;

	FORCEINLINE const std::map<uint64, Scene*>& GetAllScenes() const { return m_Scenes; }

protected:

	Map<uint64, Scene*> m_Scenes;
	Scene* m_UtilityScene; //ÒýÇæÄÚ²¿Scene(sgq)

	std::mt19937_64 m_RandomGenerator;
};

NS_JYE_END