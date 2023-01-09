#include "TexMetaLoader.h"
#include "Core/Interface/ILogSystem.h"
#include "Core/Interface/IFile.h"
#include "Core/Interface/IFileSystem.h"
#include "Engine/resource/TextureMetadata.h"

NS_JYE_BEGIN

void* TexMetaLoader::_DoLoad(const String& path)
{
	Stream stream;
	stream.Load(path.c_str());
	m_pSourceData = static_cast<TextureDescribeData*>(stream.GetObjectByRtti(TextureDescribeData::RTTI()));
	
	return m_pSourceData;
}

void TexMetaLoader::Release(void* data)
{
	if (data != NULL)
	{
		JY_ASSERT(data == m_pSourceData);
	}
	else
	{
		JYERROR("tex meta maybe delete while loading");
	}
	SAFE_DELETE(m_pSourceData);
}

ILoader* TexMetaLoader::CreateCacheLoader() const
{
	return NULL;
}


NS_JYE_END