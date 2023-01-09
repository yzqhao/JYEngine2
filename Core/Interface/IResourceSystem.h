#pragma once

#include "../Core.h"
#include "../Propery/ResourceProperty.h"
#include "../../System/Singleton.hpp"

NS_JYE_BEGIN

class ILoader;
class IResourceEntity;
class IAsyncSource;

class CORE_API IResourceSystem
{
	SYSTEM_SINGLETON_DECLEAR(IResourceSystem);
public:
	IResourceSystem();
	virtual ~IResourceSystem();

	virtual void PreLoadResource(IResourceEntity* host, IAsyncSource* source) = 0;
	virtual void LoadResource(IResourceEntity* host, IAsyncSource* source) = 0;//加载资源
	virtual void UnloadSource(IAsyncSource* source) = 0;
	virtual void TryCacheLoader(ILoader* loader) = 0;
	virtual ILoader* LoaderFactory(ResourceProperty::SourceLoaderType slt, ResourceProperty::ResourceType rt, size_t source_hash, const String& path) = 0;	//生成资源加载器
	virtual void ResourceChanged(const String& path) = 0;	//编辑器当资源变更的时候找到对应的资源然后重新创建
	virtual void RecycleBin(ILoader* ptr) = 0;	//销毁资源

};

NS_JYE_END