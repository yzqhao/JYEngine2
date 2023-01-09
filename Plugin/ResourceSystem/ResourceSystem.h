
#pragma once

#include "private/Define.h"
#include "Core/Interface/IResourceSystem.h"
#include "Core/Interface/ILoader.h"

#include <mutex>

NS_JYE_BEGIN

class IAsyncSource;
class IAsyncResource;
class ISharedSource;
class ISharedResource;

class RESOURCE_SYSTEM_API ResourceSystem : public IResourceSystem
{
	enum Constant
	{
		RSC_MAX_CACHE_COUNT = 10,
	};
private:
	typedef	std::list<std::string> CacheList;
	typedef std::map<std::string, ILoader*> CacheLoaderMap;

	typedef std::map<uint64, IResourceEntity*> EntityMap;
	typedef std::map<uint64, EntityMap> RedirectEntityMap;

	typedef std::pair<ResourceProperty::ResourceType, size_t> TypeHashPair;//针对每个source保存资源类型以及source的hash
	typedef std::list<TypeHashPair> TypeHashPairList;//保存当前文件路径所对应的资源的链表
	typedef std::pair<ResourceProperty::SourceLoaderType, TypeHashPairList> LoaderSourcePair;//保存loder类型对应资源链表，用于无效化cache
	typedef std::map<std::string, LoaderSourcePair> FileSourceMap;//保存文件路径与资源对的对于关系
public:
	ResourceSystem();
	virtual ~ResourceSystem();

	virtual void PreLoadResource(IResourceEntity* host, IAsyncSource* source) override final;
	virtual void LoadResource(IResourceEntity* host, IAsyncSource* source) override final;
	virtual void UnloadSource(IAsyncSource* source) override final;
	virtual void TryCacheLoader(ILoader* loader) override final;
	virtual ILoader* LoaderFactory(ResourceProperty::SourceLoaderType slt, ResourceProperty::ResourceType rt, size_t source_hash, const String& path) override final;
	virtual void ResourceChanged(const String& path) override final;
	virtual void RecycleBin(ILoader* ptr) override final;

	void OnSourceLoadedCallBack(IAsyncSource* source, IAsyncResource* res);	//当数据在异步线程加载完毕后，会调用这个函数将数据push到队列中，等待在主线程制作成资源

private:
	void _LoadSource(IAsyncSource* source);
	void _InsertFileSource(ResourceProperty::SourceLoaderType slt, ResourceProperty::ResourceType rt, size_t hash, const String& path);
	ILoader* _TryFindCache(ResourceProperty::SourceLoaderType slt, const String& path);
	void _ReleaseResource(IAsyncResource* res);
	void _MonopolySourceLoadedCallback(IAsyncSource* source, IAsyncResource* res);
	void _SharedSourceLoadedCallback(ISharedSource* source, ISharedResource* res);
	void _ResourceCallback(IAsyncSource* source, IAsyncResource* res);

	std::recursive_mutex m_RecursiveMutex;
	CacheLoaderMap m_CacheLoaderMap[ResourceProperty::SLT_COUNT];
	CacheList m_CacheList[ResourceProperty::SLT_COUNT];
	FileSourceMap m_FileSourceMap;	//文件对资源的缓存,这个表会一直增长，还没做删除机制，配合LoaderInfo可以删除，但是暂时先不做了
	EntityMap m_EntityMap;
	RedirectEntityMap m_RedirectEntityMap;
};

NS_JYE_END
