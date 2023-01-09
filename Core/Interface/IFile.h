#pragma once

#include "../Core.h"

NS_JYE_BEGIN

class CORE_API IFile
{
public:
	enum ActionType
	{
		AT_NONE = 0, //没有初始化的
		AT_READ,
		AT_WRITE,
	};

	IFile(void) {};
	virtual ~IFile(void) {};

	virtual bool OpenFile(ActionType at) = 0;
	virtual bool isOpen() = 0;
	virtual bool CloseFile() = 0;
	virtual int Seek(int to, int whence) = 0;
	virtual int Tell() = 0;
	virtual bool Flush() = 0;
	virtual int ReadFile(void* data, int bitesize) = 0;
	virtual int WriteFile(const void* data, int bitesize) = 0;
	virtual const std::string& GetFilePath() = 0;
	virtual uint GetSize() = 0;

	virtual IFile* GetFileFromPackage(const std::string& entry) { return NULL; }
	virtual void RecycleFileInPackage(IFile* file) { }
};

NS_JYE_END