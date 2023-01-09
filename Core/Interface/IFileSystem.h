#pragma once

#include "../Core.h"
#include "../../System/Singleton.hpp"

NS_JYE_BEGIN

class IFile;

class CORE_API IFileSystem
{
	SYSTEM_SINGLETON_DECLEAR(IFileSystem);
protected:
	IFileSystem();
	virtual ~IFileSystem();
public:
	enum Constant
	{
		IO_IDENTIFIERS_LENGTH = 4,
		IO_SUBPATH_POS = 5,
		IO_MAX_LITE_PATH_SIZE = 1024,
	};
	enum FileType
	{
		FT_NO_EXIST,
		FT_FILE,
		FT_DIR,
		FT_ZIP,
		FT_STH_ELSE,
	};

	IFile* FileFactory(const String& path);

	bool isFileExist(const String& filePath);
	FileType GetFileType(const std::string& path);
	void MakeFolder(const std::string& path);
	bool DeleteFile(const std::string& path);

	void SetRootPath(const String& path);
	void SetSavePath(const String& path);
	void SetDocmPath(const String& path);
	void SetScrsPath(const String& path);
	void SetCommPath(const String& path);
#ifdef _EDITOR
	void SetEdscPath(const String& path);
	void SetEddcPath(const String& path);
	void SetAsstPath(const String& path);
	void SetProjPath(const String& path);
#endif

	const String& GetRootPath() const;
	const String& GetSavePath() const;
	const String& GetDocmPath() const;
	const String& GetScrsPath() const;
	const String& GetCommPath() const;
#ifdef _EDITOR
	const String& GetEdscPath() const;
	const String& GetEddcPath() const;
	const String& GetAsstPath() const;
	const String& GetProjPath() const;
#endif

	String PathAssembly(const String& path);	//Â·¾¶×é×°

	const std::vector<char>& GetDictionaryFile();

	virtual void RecycleBin(IFile* file) = 0;
	virtual std::string GetTimestamp(const std::string& path) = 0;

private:

	virtual IFile* _DoFileFactory(const std::string& path) = 0;
	virtual bool _DoIsFileExist(const std::string& filename) = 0;
	virtual FileType _DoGetFileType(const std::string& path) = 0;
	virtual void _DoMakeFolder(const std::string& path) = 0;
	virtual bool _DoDeleteFile(const std::string& strFileName) = 0;

	String _root;
	String _save;
	String _docm;
	String _scrs;
	String _comm;
#ifdef _EDITOR
	String m_edscPath;
	String m_eddcPath;
	String m_asstPath;
	String m_projPath;
#endif

	std::vector<char>   m_pDictionaryFile;
};

NS_JYE_END