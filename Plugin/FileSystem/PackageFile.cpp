#include "PackageFile.h"
#include "Core/Interface/ILogSystem.h"
#include "ZippedFile.h"
#include "Core/Configure.h"

NS_JYE_BEGIN

PackageFile::PackageFile(const std::string& url)
	: m_pZipFile(NULL)
	, m_ZipUrl(url)
{
}

PackageFile::~PackageFile(void)
{
	JY_ASSERT(m_bIsOpen == false);
}

int PackageFile::Seek(int to, int whence)
{
	JYERROR("File Seeking Not Supported! %s", GetFilePath().c_str());
	return 0;
}

int PackageFile::Tell()
{
	JYERROR("File Tell Not Supported! %s", GetFilePath().c_str());
	return 0;
}

bool PackageFile::Flush()
{
	JYERROR("File Flush Not Supported! %s", GetFilePath().c_str());
	return 0;
}

int PackageFile::ReadFile(void* data, int byteSize)
{
	JYERROR("File Read Not Supported! %s", GetFilePath().c_str());
	return 0;
}

int PackageFile::WriteFile(const void* data, int bitesize)
{
	JYERROR("Packege File Writing Unsupported! %s", GetFilePath().c_str());
	return 0;
}

const std::string& PackageFile::GetFilePath()
{
	return m_ZipUrl;
}

uint PackageFile::GetSize()
{
	JYERROR("Packege File GetSize Unsupported! %s", GetFilePath().c_str());
	return 0;
}

bool PackageFile::OpenFile(IFile::ActionType at)
{
	switch (at)
	{
	case IFile::AT_READ:
	{
		int errorp = ZIP_ER_OK;
		m_pZipFile = zip_open(m_ZipUrl.c_str(), ZIP_RDONLY, &errorp);
		if (errorp != ZIP_ER_OK)
		{
			m_pZipFile = NULL;
			JYERROR("Can not Open Package File %s! Error Code = %d \n", GetFilePath().c_str(), errorp);
			return false;
		}
		m_bIsOpen = true;
		auto numEntries = zip_get_num_entries(m_pZipFile, ZIP_FL_UNCHANGED);
		for (int i = 0; i < numEntries; ++i)
		{
			std::string name = zip_get_name(m_pZipFile, i, ZIP_FL_UNCHANGED);
			m_EntriesIndex[name] = i;
		}

	}break;
	case IFile::AT_WRITE:
	default:
		JYERROR("Can not Open Zip File %s. invalid action type %d", GetFilePath().c_str(), at);
	}

	return (m_pZipFile != NULL) ? true : false;
}

bool PackageFile::isOpen()
{
	return m_bIsOpen;
}

bool PackageFile::CloseFile()
{
	if (m_bIsOpen)
	{
		if (!m_EntryFileMap.empty())
		{
			JYWARNING("You'd Better Close And Recycle All Entry-Files Before Closing This Package!");
			for (auto ptr : m_EntryFileMap)
			{
				IFile* file = ptr;
				file->CloseFile();
				SAFE_DELETE(file);
			}
			m_EntryFileMap.clear();
		}
		auto res = zip_close(m_pZipFile);
		if (res != ZIP_ER_OK)
		{
			JYWARNING("Error Occured While Closing PackageFile %s, ErrorCode = %d", m_ZipUrl.c_str(), res);
			return false;
		}
		m_bIsOpen = false;
	}
	return true;
}

IFile* PackageFile::GetFileFromPackage(const std::string& entry)
{
	int fileIndex = -1;
	std::string fileName;
	const char* suffixes[3] = { "", Configure::compressionSuffix, Configure::dictCompressionSuffix };
	for (int i = 0; i < 3; ++i)
	{
		auto ptr = m_EntriesIndex.find(entry + suffixes[i]);
		if (ptr != m_EntriesIndex.end())
		{
			fileIndex = ptr->second;
			fileName = ptr->first;
			break;
		}
	}

	if (fileIndex == -1)
	{
		JYERROR("Failed to Open Entry %s! \n", entry.c_str());
		return NULL;
	}
	ZippedFile* file = _NEW ZippedFile(m_pZipFile, fileIndex, m_ZipUrl, fileName);
	m_EntryFileMap.insert(file);

	return file;
}

void PackageFile::RecycleFileInPackage(IFile* file)
{
	if (m_EntryFileMap.find(file) != m_EntryFileMap.end())
	{
		return;
	}
	m_EntryFileMap.erase(file);

	SAFE_DELETE(file);
}

NS_JYE_END
