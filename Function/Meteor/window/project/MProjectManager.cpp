#include "MProjectManager.h"
#include "MProjectOptions.h"
#include "Core/Interface/IFileSystem.h"
#include "../editor/system/EditorSystem.h"
#include "util/MFileUtility.h"

NS_JYE_BEGIN

SINGLETON_IMPLEMENT(MProjectManager);

MProjectManager::MProjectManager()
	: m_options(_NEW MProjectOptions())
{
	m_options->Deserialize();

	_InitProjectPath(m_options->GetProjectPath());
}

MProjectManager::~MProjectManager()
{

}

void MProjectManager::_OnCreate()
{

}

void MProjectManager::_OnDestroy()
{

}

void MProjectManager::CreateProject(const String& projPath)
{
	m_options->Serialize(projPath);

	_InitProjectPath(m_options->GetProjectPath());
}

const String& MProjectManager::GetProjectPath() const
{
	return m_options->GetProjectPath();
}

void MProjectManager::SelectScene(const String& sce)
{
	if (!sce.empty())
	{
		m_sceneFile = sce;
	}
}

bool MProjectManager::CreateScene(const String& name, String& outErrorInfo)
{
	auto path = IFileSystem::Instance()->PathAssembly("proj:assets/") + name + '/';
	if (IFileSystem::Instance()->isFileExist(path))
	{
		outErrorInfo = "Scene already exists, please use another name.";	// "场景已存在，请使用其它名字。";
		return false;
	}

	EditorSystem::Instance()->CreateScene(path, name);
	m_sceneFile = path + name + ".scene";
	return true;
}

void MProjectManager::_InitProjectPath(const String& ppath)
{
	if (!ppath.empty())
	{
		IFileSystem::Instance()->SetProjPath(ppath + "/");
		MFileUtility::MCreateDirectoryDefined("proj:");	//创建工程默认文件夹
		IFileSystem::Instance()->SetAsstPath(IFileSystem::Instance()->PathAssembly("proj:assets/"));
	}
}

NS_JYE_END