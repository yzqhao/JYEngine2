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
	virtual void LoadResource(IResourceEntity* host, IAsyncSource* source) = 0;//������Դ
	virtual void UnloadSource(IAsyncSource* source) = 0;
	virtual void TryCacheLoader(ILoader* loader) = 0;
	virtual ILoader* LoaderFactory(ResourceProperty::SourceLoaderType slt, ResourceProperty::ResourceType rt, size_t source_hash, const String& path) = 0;	//������Դ������
	virtual void ResourceChanged(const String& path) = 0;	//�༭������Դ�����ʱ���ҵ���Ӧ����ԴȻ�����´���
	virtual void RecycleBin(ILoader* ptr) = 0;	//������Դ

};

NS_JYE_END