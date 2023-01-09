#include "ZippedFile.h"
#include "Core/Interface/ILogSystem.h"
#include "CDirEntry.h"
#include "Core/Configure.h"
#include "FileSystem.h"

NS_JYE_BEGIN

#define IN_CHUNK_SIZE  (16*1024)
#define RETURN_SUCCESS 0
#define RETURN_FAILURE 1

ZippedFile::ZippedFile(const std::string& parent, const std::string& entry)
	:BaseFile(parent + "/" + entry)
	, m_pZipFile(NULL)
	, m_FileIndex(-1)
	, m_DecompressBufferSize(0)
	, m_uDecompressPos(0)
	, m_ZipUrl(parent)
	, m_EntryName(entry)
	, m_bHoldPackage(true)
{
	bool res = _DoOpenZipFromUrl();
	if (res)
	{
		_GetFileIndexFromEntry();
	}
}

ZippedFile::ZippedFile(zip_t* zipFile, uint index, const std::string& parent, const std::string& entry)
	:BaseFile(parent + "/" + entry)
	, m_pZipFile(zipFile)
	, m_FileIndex(index)
	, m_DecompressBufferSize(0)
	, m_uDecompressPos(0)
	, m_ZipUrl(parent)
	, m_EntryName(entry)
	, m_bHoldPackage(false)
{
	_ParseEntryName();
}

ZippedFile::~ZippedFile(void)
{
	CloseFile();
	if (m_bHoldPackage)
	{
		zip_close(m_pZipFile);
	}
}

int ZippedFile::_DoSeekFile(int to, int whence)
{
	if (m_bCompressed)
	{
		switch (whence)
		{
		case SEEK_SET:
		{
			if (to < 0 || to >= m_pDecompressBuffer.size())
				return -1;
			m_uDecompressPos = to;
		}break;
		case SEEK_CUR:
		{
			int pos = m_uDecompressPos + to;
			if (pos < 0 || pos >= m_pDecompressBuffer.size())
				return -1;
			m_uDecompressPos = pos;
		}break;
		case SEEK_END:
		{
			int pos = m_pDecompressBuffer.size() - 1 + to;
			if (pos < 0 || pos >= m_pDecompressBuffer.size())
				return -1;
			m_uDecompressPos = pos;
		}break;
		default:
			JYERROR("File seeking error!Invalid location.");
			return -1;
		}
		return 0;
	}
	else
	{
		//If successful, zip_fseek returns 0. Otherwise, -1 is returned.
		//Only works on uncompressed (stored) data. When called on compressed data it will return an error.
		return zip_fseek(m_pFile, to, whence);
	}

}

int ZippedFile::_DoTell()
{
	if (m_bCompressed)
	{
		return m_uDecompressPos;
	}
	else
	{
		return zip_ftell(m_pFile);
	}
}

bool ZippedFile::_DoFlush()
{
	JYERROR("Unsuported");
	return 0;
}

bool ZippedFile::_DoOpenZipFromUrl()
{
	int errorp = ZIP_ER_OK;
	m_pZipFile = zip_open(m_ZipUrl.c_str(), ZIP_RDONLY, &errorp);
	if (errorp != ZIP_ER_OK)
	{
		m_pZipFile = NULL;
		JYERROR("Can not Open Zip File %s! Error Code = %d \n", GetFilePath().c_str(), errorp);
		return false;
	}
	return true;
}

bool ZippedFile::_GetFileIndexFromEntry()
{
	int fileIndex = -1;
	const char* suffixes[3] = { "", Configure::compressionSuffix, Configure::dictCompressionSuffix };
	for (int i = 0; i < 3; ++i)
	{
		fileIndex = zip_name_locate(m_pZipFile, (m_EntryName + suffixes[i]).c_str(), ZIP_FL_ENC_RAW);
		if (fileIndex != -1)
		{
			m_EntryName += suffixes[i];
			m_FileIndex = fileIndex;
			_ParseEntryName();
			break;
		}
	}

	if (fileIndex == -1)
	{
		JYERROR("Can not Open Zip File %s! Invalid Entry. \n", GetFilePath().c_str());
		return false;
	}

	return true;
}

int ZippedFile::_DoReadFile(void* data, uint byteSize)
{
	if (m_bCompressed)
	{
		if (m_uDecompressPos >= m_pDecompressBuffer.size())
			return 0;

		int readSize = std::min(/*m_FileSize*/(uint)m_pDecompressBuffer.size() - m_uDecompressPos, byteSize);
		memcpy(data, m_pDecompressBuffer.data() + m_uDecompressPos, readSize);
		m_uDecompressPos += readSize;
		return readSize;
	}
	else
	{
		return zip_fread(m_pFile, data, byteSize);
	}
}

int ZippedFile::_DoWriteFile(const void* data, uint bitesize)
{
	JYERROR("Zipped File Writing Unsupported!");
	return 0;
}

bool ZippedFile::_DoOpenFile(IFile::ActionType at)
{
	switch (at)
	{
	case IFile::AT_READ:
	{
		if (m_pZipFile == NULL || m_FileIndex == -1)
		{
			return false;
		}

		m_pFile = zip_fopen_index(m_pZipFile, m_FileIndex, 0);
		if (m_pFile == NULL)
		{
			JYERROR("Failed to Open Entry %s! \n", GetFilePath().c_str());
			return false;
		}

		struct zip_stat st;
		zip_stat_init(&st);
		zip_stat_index(m_pZipFile, m_FileIndex, 0, &st);
		JY_ASSERT(st.comp_method == ZIP_CM_STORE);
		JY_ASSERT(st.size > LZ4F_HEADER_SIZE_MIN);
		if (!m_bCompressed)
		{
			_SetFileSize(st.size);
		}
		else
		{
			if (_Decompress_File() != RETURN_SUCCESS)
			{
				return false;
			}
			return true;
		}

	}break;
	case IFile::AT_WRITE:
	default:
		JYERROR("Can not Open Zip File %s. invalid action type %d", GetFilePath().c_str(), at);
	}

	return (m_pFile != NULL) ? true : false;
}

bool ZippedFile::_DoCloseFile()
{
	bool res = true;
	res &= (zip_fclose(m_pFile) == ZIP_ER_OK);
	m_pFile = NULL;
	_ClearBuffer();
	return res;
}

void ZippedFile::_ClearBuffer()
{
	m_pDecompressBuffer.clear();
	m_DecompressBufferSize = 0;
	m_uDecompressPos = 0;
}

void ZippedFile::_ParseEntryName()
{
	if (m_EntryName.length() > Configure::compSuffixLen)
	{
		auto sfx = CDirEntry::suffix(m_EntryName);
		if (sfx == Configure::compressionSuffix)
		{
			m_bCompressed = true;
			m_bUseDict = false;
		}
		else if (sfx == Configure::dictCompressionSuffix)
		{
			m_bCompressed = true;
			m_bUseDict = true;
		}
		else
		{
			m_bCompressed = false;
			m_bUseDict = false;
		}
	}
	else
	{
		m_bCompressed = false;
		m_bUseDict = false;
	}
}

static size_t get_block_size(const LZ4F_frameInfo_t* info) {
	switch (info->blockSizeID) {
	case LZ4F_default:
	case LZ4F_max64KB:  return 1 << 16;
	case LZ4F_max256KB: return 1 << 18;
	case LZ4F_max1MB:   return 1 << 20;
	case LZ4F_max4MB:   return 1 << 22;
	default:
		JYERROR("Impossible with expected frame specification (<=v1.6.1)\n");
	}
}

int	ZippedFile::_Decompress_Blocks(
	LZ4F_dctx* dctx, const char* dict, size_t dictSize,
	void* srcBuffer, const size_t srcCapacity, const size_t filled, size_t alreadyConsumed,
	void* dstBuffer, const size_t dstCapacity)
{
	int firstChunk = 1;
	size_t ret = 1;

	JY_ASSERT(alreadyConsumed <= filled);

	/* Decompression */
	while (ret != 0)
	{
		/* Load more input */
		size_t readSize = firstChunk ? filled : zip_fread(m_pFile, srcBuffer, srcCapacity);
		firstChunk = 0;
		const void* srcPtr = (const char*)srcBuffer + alreadyConsumed;
		alreadyConsumed = 0;
		const void* const srcEnd = (const char*)srcPtr + readSize;
		if (readSize == 0)
		{
			JYERROR("Decompress: not enough data in blocks\n");
			return RETURN_FAILURE;
		}

		/* Decompress:
		 * Continue while there is more input to read (srcPtr != srcEnd)
		 * and the frame isn't over (ret != 0)
		 */
		while (srcPtr < srcEnd && ret != 0)
		{
			/* Any data within dstBuffer has been saved at this stage */
			size_t dstSize = dstCapacity;
			size_t srcSize = (const char*)srcEnd - (const char*)srcPtr;
			ret = LZ4F_decompress_usingDict(dctx, dstBuffer, &dstSize, srcPtr, &srcSize, dict, dictSize,/* LZ4F_decompressOptions_t */ NULL);
			if (LZ4F_isError(ret))
			{
				JYERROR("Decompression error: %s %s\n", LZ4F_getErrorName(ret), m_EntryName.c_str());
				return RETURN_FAILURE;
			}
			/* Save output */
			if (dstSize != 0)
			{
				m_pDecompressBuffer.resize(m_DecompressBufferSize + dstSize);
				memcpy(m_pDecompressBuffer.data() + m_DecompressBufferSize, dstBuffer, dstSize);
				m_DecompressBufferSize += dstSize;
			}
			/* Update input */
			srcPtr = (const char*)srcPtr + srcSize;
		}

		JY_ASSERT(srcPtr <= srcEnd);

		/* Ensure all input data has been consumed.
		 * It is valid to have multiple frames in the same file,
		 * but this example only supports one frame.
		 */
		if (srcPtr < srcEnd)
		{
			JYERROR("Decompress: Trailing data left in file after frame\n");
			return RETURN_FAILURE;
		}
	}

	return RETURN_SUCCESS;
}


int ZippedFile::_Decompress_File()
{
	_ClearBuffer();

	/* Ressource allocation */
	/* ensure LZ4F_getFrameInfo() can read enough data */
	size_t srcCapacity = IN_CHUNK_SIZE;
	void* const srcBuffer = malloc(srcCapacity);

	if (!srcBuffer)
	{
		JYERROR("decompress_file(srcBuffer = NULL)");
		return RETURN_FAILURE;
	}

	LZ4F_dctx* dctx;
	{
		size_t const dctxStatus = LZ4F_createDecompressionContext(&dctx, LZ4F_VERSION);
		if (LZ4F_isError(dctxStatus))
		{
			JYERROR("LZ4F_dctx creation error: %s\n", LZ4F_getErrorName(dctxStatus));
			free(srcBuffer);
			return RETURN_FAILURE;
		}
	}
	JY_ASSERT(dctx != NULL);

	/* Read Frame header */
	size_t const readSize = zip_fread(m_pFile, srcBuffer, srcCapacity);
	if (readSize == 0)
	{
		JYERROR("Decompress: not enough input or error reading file\n");
		free(srcBuffer);
		LZ4F_freeDecompressionContext(dctx);   /* note : free works on NULL */
		return RETURN_FAILURE;
	}

	LZ4F_frameInfo_t info;
	size_t consumedSize = readSize;
	{
		size_t const fires = LZ4F_getFrameInfo(dctx, &info, srcBuffer, &consumedSize);
		if (LZ4F_isError(fires))
		{
			JYERROR("LZ4F_getFrameInfo error: %s\n", LZ4F_getErrorName(fires));
			free(srcBuffer);
			LZ4F_freeDecompressionContext(dctx);   /* note : free works on NULL */
			return RETURN_FAILURE;
		}
		if (info.contentSize != 0)
			_SetFileSize(info.contentSize);
	}

	/* Allocating enough space for an entire block isn't necessary for
	 * correctness, but it allows some memcpy's to be elided.
	 */
	size_t const dstCapacity = get_block_size(&info);
	void* const dstBuffer = malloc(dstCapacity);
	if (!dstBuffer)
	{
		JYERROR("decompress_file(dst)");
		free(srcBuffer);
		LZ4F_freeDecompressionContext(dctx);   /* note : free works on NULL */
		return RETURN_FAILURE;
	}

	int decompressionResult = RETURN_FAILURE;
	if (!m_bUseDict)
	{
		decompressionResult = _Decompress_Blocks(dctx,
			NULL, 0,
			srcBuffer, srcCapacity,
			readSize - consumedSize, consumedSize,
			dstBuffer, dstCapacity);
	}
	else
	{
		const std::vector<char>& dic = IFileSystem::Instance()->GetDictionaryFile();
		decompressionResult = _Decompress_Blocks(dctx,
			dic.data(), dic.size(),
			srcBuffer, srcCapacity,
			readSize - consumedSize, consumedSize,
			dstBuffer, dstCapacity);
	}

	if (decompressionResult == RETURN_SUCCESS)
	{
		if (info.contentSize == 0)
		{
			//m_FileSize = m_pDecompressBuffer.size();
			_SetFileSize(m_pDecompressBuffer.size());
		}
		else
		{
			JY_ASSERT(info.contentSize == m_pDecompressBuffer.size());
		}
	}
	free(srcBuffer);
	free(dstBuffer);
	LZ4F_freeDecompressionContext(dctx);   /* note : free works on NULL */
	return decompressionResult;
}

NS_JYE_END
