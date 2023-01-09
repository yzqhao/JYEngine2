
#include "ConfigSystem.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/IFile.h"


NS_JYE_BEGIN

ConfigSystem::ConfigSystem()
{
}

ConfigSystem::~ConfigSystem()
{
}

IConfig* ConfigSystem::ConfigureFactory(const String& path)
{
	JsonConfigure* res = NULL;
	IFile* file = IFileSystem::Instance()->FileFactory(path);
	if (file->OpenFile(IFile::AT_READ))
	{
		uint size = file->GetSize();
		char* content = _NEW char[size + 1];
		content[size] = 0;
		file->ReadFile(content, size);
		res = _NEW JsonConfigure;
		if (!res->Parse(content))
		{
			SAFE_DELETE(res)
		}
		SAFE_DELETE_ARRAY(content);

		file->CloseFile();
	}
	IFileSystem::Instance()->RecycleBin(file);
	return res;
}

void ConfigSystem::RecycleBin(IConfig* cf)
{
	SAFE_DELETE(cf);
}

void ConfigSystem::SetDefaultConfigure(const String& path)
{
	m_pDefaultConfigure = ConfigureFactory(path);
}

IConfig* ConfigSystem::GetDefaultConfigure()
{
	return m_pDefaultConfigure;
}

NS_JYE_END
