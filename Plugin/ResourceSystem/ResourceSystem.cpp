
#include "ResourceSystem.h"
#include "Core/Interface/ILogSystem.h"
#include "TextureLoader.h"
#include "MeshLoader.h"
#include "MatLoader.h"
#include "TexMetaLoader.h"
#include "Engine/resource/IResourceEntity.h"
#include "Engine/resource/IAsyncSource.h"
#include "Engine/graphic/IAsyncResource.h"
#include "Engine/resource/ISharedSource.h"

NS_JYE_BEGIN

ResourceSystem::ResourceSystem()
{

}

ResourceSystem::~ResourceSystem()
{

}

void ResourceSystem::PreLoadResource(IResourceEntity* host, IAsyncSource* source)
{
	m_RecursiveMutex.lock();

	if (source->GetPreLoaded() == false)
	{
		if (source->GetSourceType() == ResourceProperty::CST_SHARED)
		{
			ISharedSource* shareSource = static_cast<ISharedSource*>(source);
			IAsyncResource* resourcePtr = shareSource->PreCreateResource();
			shareSource->ResourceChange(resourcePtr);
		}

		source->SetPreLoaded(true);
	}

	m_RecursiveMutex.unlock();
}

void ResourceSystem::LoadResource(IResourceEntity* host, IAsyncSource* source)
{
	m_RecursiveMutex.lock();
	// 由于copy出来的资源共享了source，这里可能存在多个entity对应一个source的情况
	uint64 objID = source->GetObjectID();
	if (m_EntityMap.find(objID) == m_EntityMap.end())
	{
		m_EntityMap.insert({ objID, host });
	}
	m_RecursiveMutex.unlock();

	PreLoadResource(host, source);
	_LoadSource(source);
}

void ResourceSystem::UnloadSource(IAsyncSource* source)
{
	if (source->GetResource() == nullptr)
	{
		return;
	}
	if (source->GetSourceType() == ResourceProperty::SourceType::CST_MONOPOLY)
	{
		_ReleaseResource(source->GetResource());
		return;
	}
}

void ResourceSystem::TryCacheLoader(ILoader* loader)
{
	ILoader* cache = loader->CreateCacheLoader();
	if (cache)
	{
		std::lock_guard<std::recursive_mutex> hlock(m_RecursiveMutex);
		const std::string& path = loader->GetPath();
		ResourceProperty::SourceLoaderType slt = loader->GetSourceLoaderType();
		CacheList& list = m_CacheList[slt];
		CacheLoaderMap& map = m_CacheLoaderMap[slt];
		CacheLoaderMap::const_iterator it = map.find(path);
		if (it != map.end())
		{
			RecycleBin(cache);
		}
		else
		{
			map.insert({ path, cache });
			list.push_back(path);
			if (list.size() > ResourceSystem::RSC_MAX_CACHE_COUNT)
			{
				std::string dh = list.front();
				list.pop_front();
				CacheLoaderMap::iterator todelete = map.find(dh);
				todelete->second->Release(NULL);
				RecycleBin(todelete->second);
				map.erase(todelete);
			}
		}
	}
}

ILoader* ResourceSystem::LoaderFactory(ResourceProperty::SourceLoaderType slt, ResourceProperty::ResourceType rt, size_t source_hash, const String& path)
{
	std::lock_guard<std::recursive_mutex> hlock(m_RecursiveMutex);
	_InsertFileSource(slt, rt, source_hash, path);
	ILoader* res = _TryFindCache(slt, path);
	if (!res)
	{
		switch (slt)
		{
		case ResourceProperty::SLT_TEXTURE: return _NEW TextureLoader(path);
		case ResourceProperty::SLT_MESH: return _NEW MeshLoader(path);
		case ResourceProperty::SLT_MAT: return _NEW MatLoader(path);
		case ResourceProperty::SLT_METADATA: return _NEW TexMetaLoader(path);
		default: JYERROR("LoaderFactory failed: unkown resource property");
		}
	}
	return res;
}

void ResourceSystem::ResourceChanged(const String& path)
{
	
}

void ResourceSystem::RecycleBin(ILoader* ptr)
{
	SAFE_DELETE(ptr);
}

void ResourceSystem::_SharedSourceLoadedCallback(ISharedSource* source, ISharedResource* res)
{
	m_RecursiveMutex.lock();//TODO 需要调整这个锁，锁的范围太大了，效率不高
	_ResourceCallback(source, res);
	m_RecursiveMutex.unlock();
}

void ResourceSystem::_MonopolySourceLoadedCallback(IAsyncSource* source, IAsyncResource* res)
{
	_ResourceCallback(source, res);
}

void ResourceSystem::_ResourceCallback(IAsyncSource* source, IAsyncResource* res)
{
	std::lock_guard<std::recursive_mutex> hlock(m_RecursiveMutex);
	EntityMap::iterator it = m_EntityMap.find(source->GetObjectID());
	if (m_EntityMap.end() != it)
	{
		IResourceEntity* ire = it->second;
		ire->ResourceCallBack(res, source->GetHashCode());
		m_EntityMap.erase(it);
	}
}

void ResourceSystem::OnSourceLoadedCallBack(IAsyncSource* source, IAsyncResource* res)
{
	switch (source->GetSourceType())
	{
	case ResourceProperty::CST_SHARED:
		_SharedSourceLoadedCallback(
			static_cast<ISharedSource*>(source),
			static_cast<ISharedResource*>(res));
		break;
	case ResourceProperty::CST_MONOPOLY:
		_MonopolySourceLoadedCallback(source, res);
		break;
	default: JYERROR("OnSourceLoadedCallBack : unkown source type");
	}
}

void ResourceSystem::_LoadSource(IAsyncSource* source)
{
	source->SetLoadedCallBacke(MulticastDelegate<IAsyncSource*, IAsyncResource*>::DelegateT::CreateRaw(this, &ResourceSystem::OnSourceLoadedCallBack));
	source->DoJob(source);
}

ILoader* ResourceSystem::_TryFindCache(ResourceProperty::SourceLoaderType slt, const String& path)
{
	ILoader* res = NULL;
	const CacheLoaderMap& map = m_CacheLoaderMap[slt];
	CacheLoaderMap::const_iterator it = map.find(path);
	if (it != map.end())
	{
		res = it->second->CreateCacheLoader();
		//命中后提高优先级
		CacheList& list = m_CacheList[slt];
		list.remove(path);
		list.push_back(path);
	}
	return res;
}

void ResourceSystem::_InsertFileSource(ResourceProperty::SourceLoaderType slt, ResourceProperty::ResourceType rt, size_t hash, const String& path)
{
	//插入数据
	FileSourceMap::iterator it = m_FileSourceMap.find(path);
	if (it != m_FileSourceMap.end())
	{
		for (TypeHashPair& data : it->second.second)
		{
			if (data.first == rt
				&& data.second == hash)
			{
				return;
			}
		}
		it->second.second.push_back({ rt, hash });
	}
	else
	{
		TypeHashPairList list;
		list.push_back({ rt, hash });
		m_FileSourceMap.insert({ path, { slt, list } });
	}
}

void ResourceSystem::_ReleaseResource(IAsyncResource* res)
{
	res->ReleaseResource();
	SAFE_DELETE(res);
}

NS_JYE_END