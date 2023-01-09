
#pragma once

#include "System/Singleton.hpp"
#include "private/Define.h"
#include "Core/Interface/IFile.h"
#include "Core/Interface/IFileSystem.h"

NS_JYE_BEGIN

class FileSystem : public IFileSystem
{
public:

    FileSystem();
    virtual ~FileSystem();

	virtual void RecycleBin(IFile* file) override;
	virtual std::string GetTimestamp(const std::string& path) override;

private:

	virtual IFile* _DoFileFactory(const std::string& path) override;
	virtual bool _DoIsFileExist(const std::string& filename) override;
	virtual FileType _DoGetFileType(const std::string& path) override;
	virtual void _DoMakeFolder(const std::string& path) override;
	virtual bool _DoDeleteFile(const std::string& strFileName) override;
};

NS_JYE_END