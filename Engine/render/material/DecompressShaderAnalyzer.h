#pragma once

#include <vector>
#include <string>
#include <mutex>
#include <memory>
#include "Engine/private/Define.h"

NS_JYE_BEGIN

class IFile;

// DecompressShaderAnalyzer read the compressed shader file and decompress it.
class DecompressShaderAnalyzer
{
public:

	// params: blockIndex indicates the shader index among all, blockOffset locates position in the specified shader string.
	// return: uncompressed shader
	void DecompressShaderFromFile(handle filehandle, int blockIndex, int blockOffset, int readSize, std::string& filecontext);

	// open file and return file handle
	handle LoadFile(const std::string& filename);
	// unload memory by handle returned by LoadFile
	void UnloadFile(handle filehandle);

	static inline DecompressShaderAnalyzer* Instance()
	{
		return &instance;
	}

	~DecompressShaderAnalyzer();

private:
	IFile* ReadFile(const std::string& filename);

	DecompressShaderAnalyzer();

	static DecompressShaderAnalyzer instance;

	struct CompressFile
	{
		IFile* m_file;
		std::string m_name;
	};
	std::vector<CompressFile>    m_fileList;
	uint16                       m_handleCnt;
	std::mutex                   m_mutex;

	static const uint MAX_HANDLES = 65536;
};

NS_JYE_END