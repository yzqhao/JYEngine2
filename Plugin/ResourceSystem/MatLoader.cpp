#include "MatLoader.h"
#include "Core/Interface/ILogSystem.h"
#include "Core/Interface/IFile.h"
#include "Core/Interface/IFileSystem.h"
#include "Engine/resource/MaterialStreamData.h"

NS_JYE_BEGIN

void* MatLoader::_DoLoad(const String& path)
{
	Stream stream;
	stream.Load(path.c_str());
	m_pSourceData = static_cast<MaterialStreamData*>(stream.GetObjectByRtti(MaterialStreamData::RTTI()));
	
	return m_pSourceData;
}

void MatLoader::Release(void* data)
{
	if (data != NULL)
	{
		JY_ASSERT(data == m_pSourceData);
	}
	else
	{
		JYERROR("mat maybe delete while loading");
	}
	SAFE_DELETE(m_pSourceData);
}

ILoader* MatLoader::CreateCacheLoader() const
{
	return NULL;
}


NS_JYE_END