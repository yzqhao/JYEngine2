#include "Application.h"
#include "Engine/platform/PlatformEngine.h"
#include "Core/Configure.h"
#include "Core/Interface/ILogSystem.h"
#include "Core/Interface/IConfigSystem.h"
#include "Core/Interface/IFileSystem.h"
#include "Engine/Engine.h"
#include "Core/Interface/IScriptSystem.h"
#include "Function/Meteor/Meteor.h"
#include "System/Delegate.h"

NS_JYE_BEGIN

Application::Application(IWindow* wd)
	: IApplication(wd)
	, m_mainVM(NULL)
{

}

Application::~Application()
{
	
}

bool Application::PreInitial()
{
	if (!IApplication::PreInitial())
		return false;

	return true;
}

bool Application::OnInitial(const String& root, const String& save)
{
	if (!IApplication::OnInitial(root, save))
		return false;

	m_moduleManager = _NEW ModuleManager();
	m_moduleManager->AddLibraryPath(root);

	Engine::Create();
	
	IConfig* conf = IConfigSystem::Instance()->GetDefaultConfigure();
	String comm = conf->GetString(Configure::COMMON_PATH);
	String logfile = conf->GetString(Configure::LOG_FILE_NAME);
	uint filesize = conf->GetInt(Configure::LOG_FILE_SIZE);

	ILogSystem::Instance()->SetLogFile(filesize, logfile);
	String docs = conf->GetString(Configure::DOCUMENT_PATH);
	String scrs = conf->GetString(Configure::SCRIPT_PATH);
	IFileSystem::Instance()->SetDocmPath(docs);
	IFileSystem::Instance()->SetScrsPath(scrs);
	IFileSystem::Instance()->SetCommPath(comm);

#ifdef _EDITOR
	std::string editscrs = conf->GetString(Configure::EDITOR_SCRIPT);
	IFileSystem::Instance()->SetEdscPath(editscrs);

	std::string editdoc = conf->GetString(Configure::EDITOR_DOCUMENT_PATH);
	IFileSystem::Instance()->SetEddcPath(editdoc);

	String edscmain = conf->GetString(Configure::EDITOR_SCRIPT_MAIN);
#endif

	// std::string packPath = ";" + IFileSystem::Instance()->GetEdscPath() + "?.lua;" + IFileSystem::Instance()->GetEdscPath() + "?/init.lua;" +
	// 	IFileSystem::Instance()->GetScrsPath() + "?.lua;" + IFileSystem::Instance()->GetScrsPath() + "?/init.lua;";
	// IScriptSystem::Instance()->SetPackagePath(packPath);

#ifdef _EDITOR
	//m_mainVM = IScriptSystem::Instance()->VirtualMachineFactory();
	//m_mainVM->CallScript(edscmain);
	//m_mainVM->CallScriptFunc("Init");
	//Engine::Instance()->AddSynchronizeUpdateCallback(m_mainVM, "Update");
	Engine::Instance()->AddUpdateCallback(MulticastDelegate<>::DelegateT::CreateRaw(Meteor::Instance(), &Meteor::Update));
#endif

	Map<String, String> scripts;
	if (conf->isObjectString(Configure::SCRIPT_MAIN))
	{
		scripts = conf->GetObjectString(Configure::SCRIPT_MAIN);
	}
	else
	{
		String scriptmain = conf->GetString(Configure::SCRIPT_MAIN);
		scripts.insert({ Configure::DEFAULT_SCRIPT_KEY, scriptmain });
	}

	return true;
}

bool Application::OnTerminal()
{
	if (!IApplication::OnTerminal())
		return false;

	Engine::Destroy();

	SAFE_DELETE(m_moduleManager);

	return true;
}


NS_JYE_END