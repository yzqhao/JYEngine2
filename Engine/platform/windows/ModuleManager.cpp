#include "ModuleManager.h"

#if PLATFORM_WIN32

#include <windows.h>

NS_JYE_BEGIN

ModuleManager::ModuleManager()
{

}

ModuleManager::~ModuleManager()
{

}

void ModuleManager::AddLibraryPath(const String& path)
{
	// https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/fullpath-wfullpath?view=vs-2019
	// ���·��ת���ɾ���·��
	char* abspath = _fullpath(NULL, path.data(), path.size());
	String sAbsPath(abspath);
	free(abspath);

	// https://docs.microsoft.com/zh-cn/windows/win32/procthread/changing-environment-variables
	// ��ȡ��ǰ���̵Ļ�������PATH
	int currentSize = 1024 * 16;
	Vector<char> buf;
	buf.resize(currentSize);

	int dwRet = GetEnvironmentVariableA("PATH", buf.data(), buf.size());
	if (0 == dwRet)
	{
		// û�л�������PATH�Ļ�,ֱ������ 
	}
	else if (currentSize < dwRet)
	{
		// ���currentSize�������ɻ�������PATH�Ļ�,���·���
		buf.resize(dwRet);
		dwRet = GetEnvironmentVariableA("PATH", buf.data(), buf.size());
		printf("s");
	}

	String paths(buf.data(), dwRet);

	paths += ";" + sAbsPath;

	SetEnvironmentVariableA("PATH", paths.c_str());
}

IModuleInterface* ModuleManager::_DoLoadPlugin(const String& name)
{
	IModuleInterface* res = NULL;
	String fullname;
#ifdef _DEBUG
	fullname = name + "_d.dll";
#else
	fullname = name + ".dll";
#endif // _DEBUG		
	HMODULE hmodule = LoadLibraryA(fullname.c_str());
	JY_ASSERT(NULL != hmodule);

	// �õ������nameָ����dll�е�"Create_Plugin"�����ķ��ص�ַ��Ҳ���ǵõ������IPlugin* Create_Plugin()������ַ��
	EntryFuncPtr func = (EntryFuncPtr)GetProcAddress(hmodule, "CreateSystem");
	if (func)	// ��������ɹ�
	{
		PluginData data;
		data.m_Name = name;
		data.m_pPlugin = func();
		data.m_Handle = hmodule;
		m_LoadedPluginMap.insert({ (uint64)(data.m_pPlugin), data });
		res = data.m_pPlugin;
	}

	res->StartupModule();

	return res;
}

void ModuleManager::_DoReleasePlugin(IModuleInterface* plugin)
{
	plugin->ShutdownModule();

	auto key = (uint64)(plugin);
	auto pluginData = m_LoadedPluginMap.find(key);
	// �õ������nameָ����dll�е�"Destroy_Plugin"�����ķ��ص�ַ��Ҳ���ǵõ������void Destroy_Plugin()������ַ��
	ExitFuncPtr func = (ExitFuncPtr)GetProcAddress(pluginData->second.m_Handle, ("DestroySystem"));
	func(pluginData->second.m_pPlugin);								
	FreeLibrary(pluginData->second.m_Handle);
	m_LoadedPluginMap.erase(key);					
}

NS_JYE_END

#endif