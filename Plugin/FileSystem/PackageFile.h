#pragma once

#include "BaseFile.h"
#include "zip.h"
#include "lz4frame.h"

NS_JYE_BEGIN

class PackageFile : public IFile
{
private:
	typedef std::map<std::string, uint> EntryIndexMap;
	typedef std::set<IFile*> EntryFileMap;

private:
	zip_t* m_pZipFile;
	bool                      m_bIsOpen;
	std::string               m_ZipUrl;
	EntryIndexMap             m_EntriesIndex;
	EntryFileMap              m_EntryFileMap;
public:
	PackageFile(const std::string& url);
	virtual ~PackageFile();
public:
	virtual bool OpenFile(IFile::ActionType at);
	virtual bool isOpen();
	virtual bool CloseFile();
	virtual int Seek(int to, int whence);
	virtual int Tell();
	virtual bool Flush();
	virtual int ReadFile(void* data, int bitesize);
	virtual int WriteFile(const void* data, int bitesize);
	virtual const std::string& GetFilePath();
	virtual uint GetSize();
public:
	IFile* GetFileFromPackage(const std::string& entry) override;
	void RecycleFileInPackage(IFile* file) override;
};

NS_JYE_END
