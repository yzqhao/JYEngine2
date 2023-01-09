#pragma once

#include "Engine/private/Define.h"
#include "Engine/graphic/IAsyncResource.h"
#include "RHI/RHIDefine.h"
#include "Engine/resource/IAsyncEntity.h"
#include "Engine/graphic/VertexBufferResource.h"
#include "Engine/resource/ISharedSource.h"

NS_JYE_BEGIN

class VertexBufferSource : public ISharedSource
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	VertexBufferSource();
	virtual ~VertexBufferSource();

	FORCEINLINE RHIDefine::MemoryUseage GetMemoryUseage() { return m_eMemoryUseage; }
	FORCEINLINE void SetMemoryUseage(RHIDefine::MemoryUseage mu) { m_eMemoryUseage = mu; }
	FORCEINLINE void SetKeepSource(bool isKeepSource) { m_isKeepSource = isKeepSource; }
	FORCEINLINE bool isKeepSource() const { return m_isKeepSource; }

private:
	virtual IAsyncResource* _DoCreateResource() override;//将资源数据制作为实体资源
	virtual bool _DoSetupResource(IAsyncResource* res) override;//重新创建资源
	virtual bool _DoLoadResource() override;//加载资源

private:
	RHIDefine::MemoryUseage m_eMemoryUseage;
	bool m_isKeepSource;
};
DECLARE_Ptr(VertexBufferSource);
TYPE_MARCO(VertexBufferSource);

class ENGINE_API VertexBufferEntity : public IAsyncEntity<VertexBufferResource, VertexBufferSource>
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	VertexBufferEntity();
	virtual ~VertexBufferEntity();
	DECLARE_OBJECT_CTOR_FUNC(VertexBufferEntity);

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLoad(Stream* pStream) override;

	FORCEINLINE VertexStream* GetVertexStream()
	{
		if (_GetResourcePtr())
			return _GetResourcePtr()->GetVertexStream();
		return NULL;
	}

	FORCEINLINE RHIDefine::MemoryUseage GetMemoryUsage()
	{
		return _GetSourcePtr()->GetMemoryUseage();
	}
	FORCEINLINE bool FlushVertexBuffer(const VertexStream& buffer)
	{
		return _GetResourcePtr()->FlushVertexBuffer(buffer, isKeepSource());
	}
	FORCEINLINE bool ChangeVertexBuffer(const VertexStream& stream)
	{
		return _GetResourcePtr()->ChangeVertexBuffer(stream);
	}
	FORCEINLINE uint GetVertexSize() const//返回每一个顶点的大小
	{
		return _GetResourcePtr()->GetVertexSize();
	}
	FORCEINLINE uint GetVertexCount() const
	{
		return _GetResourcePtr()->GetVertexCount();
	}
	FORCEINLINE void SetKeepSource(bool isKeepSource)
	{
		_GetSourcePtr()->SetKeepSource(isKeepSource);
		if (isKeepSource == false && _GetResourcePtr())  //卸载缓存的原始数据
		{
			_GetResourcePtr()->UnloadSource();
		}
	}
	FORCEINLINE bool isKeepSource() const
	{
		return _GetSourcePtr()->isKeepSource();
	}
	FORCEINLINE handle GetGpuHandle() const
	{
		return _GetResourcePtr()->GetGpuHandle();
	}
private:
	virtual void _OnCreateResource() override;

};
DECLARE_Ptr(VertexBufferEntity);
TYPE_MARCO(VertexBufferEntity);

NS_JYE_END