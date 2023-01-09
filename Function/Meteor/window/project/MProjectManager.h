
#pragma once

#include "private/Define.h"
#include "System/Singleton.hpp"

NS_JYE_BEGIN

class MProjectOptions;

class MProjectManager
{
	SINGLETON_DECLARE_ONCREATE_ONDESTROY(MProjectManager);
public:
	MProjectManager();
	~MProjectManager();

	void _OnCreate();
	void _OnDestroy();

	void CreateProject(const String& projPath);

	const String& GetProjectPath() const;

	void SelectScene(const String& sce);
	bool CreateScene(const String& sce, String& outErrorInfo);

	FORCEINLINE const String& GetSceneFullName() const { return m_sceneFile; }

private:

	void _InitProjectPath(const String& ppath);

	MProjectOptions* m_options;
	String m_sceneFile;
};

NS_JYE_END