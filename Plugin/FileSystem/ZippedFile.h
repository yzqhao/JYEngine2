#pragma once

#include "BaseFile.h"
#include "zip.h"
#include "lz4frame.h"

NS_JYE_BEGIN

class ZippedFile : public BaseFile
{
private:
	zip_t* m_pZipFile;
	zip_file_t* m_pFile;
	int						  m_FileIndex;

	std::vector<byte>         m_pDecompressBuffer;
	uint                      m_DecompressBufferSize;
	uint                      m_uDecompressPos;

	std::string               m_ZipUrl;
	std::string               m_EntryName;

	bool                      m_bCompressed;
	bool                      m_bHoldPackage;
	bool                      m_bUseDict;
public:
	ZippedFile(const std::string& parent, const std::string& entry);
	ZippedFile(zip_t* zipFile, uint index, const std::string& parent, const std::string& entry);
	virtual ~ZippedFile();
protected:
	virtual bool _DoOpenFile(IFile::ActionType at);
	virtual int _DoReadFile(void* data, uint bitesize);
	virtual int _DoWriteFile(const void* data, uint bitesize);
	virtual bool _DoCloseFile();
	virtual int _DoSeekFile(int to, int whence);
	virtual int _DoTell();
	virtual bool _DoFlush();
private:
	bool _DoOpenZipFromUrl();
	bool _GetFileIndexFromEntry();
	int _Decompress_File();
	int _Decompress_Blocks(LZ4F_dctx* dctx, const char* dict, size_t dictSize, void* srcBuff, const size_t srcCapacity, const size_t filled, size_t alreadyConsumed,
		void* dstBuff, const size_t dstCapacity);
	void _ClearBuffer();
	void _ParseEntryName();
};

NS_JYE_END
