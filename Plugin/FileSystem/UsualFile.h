#pragma once

#include "BaseFile.h"

NS_JYE_BEGIN

class UsualFile : public BaseFile
{
private:
	FILE* m_pFile;
public:
	UsualFile(const std::string& url);
	virtual ~UsualFile();
protected:
	virtual bool _DoOpenFile(IFile::ActionType at);
	virtual int _DoReadFile(void* data, uint bitesize);
	virtual int _DoWriteFile(const void* data, uint bitesize);
	virtual bool _DoCloseFile();
	virtual int _DoSeekFile(int to, int whence);
	virtual int _DoTell();
	virtual bool _DoFlush();
};

NS_JYE_END
