#pragma once

#include "Engine/private/Define.h"
#include "Engine/graphic/IAsyncResource.h"
#include "RHI/RHIDefine.h"
#include "Engine/resource/IAsyncEntity.h"
#include "Engine/graphic/MaterialResource.h"
#include "Engine/resource/ISharedSource.h"

NS_JYE_BEGIN

class MaterialStreamData;

class MaterialSource : public ISharedSource
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialSource();
	virtual ~MaterialSource();

private:
	virtual IAsyncResource* _DoCreateResource() override;//将资源数据制作为实体资源
	virtual bool _DoSetupResource(IAsyncResource* res) override;//重新创建资源
	virtual bool _DoLoadResource() override;//加载资源

private:
	ShaderState* m_shader;
	MaterialStreamData* m_pSourceData;
};
DECLARE_Ptr(MaterialSource);
TYPE_MARCO(MaterialSource);

class ENGINE_API MaterialEntity : public IAsyncEntity<MaterialResource, MaterialSource>
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialEntity();
	virtual ~MaterialEntity();
	DECLARE_OBJECT_CTOR_FUNC(MaterialEntity);

	MaterialEntity* Clone();

	virtual void BeforeSave(Stream* pStream) override;
	virtual void PostSave(Stream* pStream) override;
	virtual void PostLoad(Stream* pStream) override;

	const String& GetMatPath() const;
	Map<String, MaterialParameter*> GetParameters();

	FORCEINLINE void EnableKeyWord(const String& key);
	FORCEINLINE void DisableKeyWord(const String& key);
	FORCEINLINE void DisableAllKeyWord();
	FORCEINLINE bool IsKeyWordEnable(const String& key);
	FORCEINLINE ShaderKeyWords GetKeyWords();
	FORCEINLINE bool hasParameter(RHIDefine::ParameterSlot att);
	FORCEINLINE const String& GetMaterialName() const;
	FORCEINLINE const String& GetShaderPath() const;
	FORCEINLINE uint GetRenderQueue() const;
	FORCEINLINE bool IsSupportPass(RHIDefine::PassType passType) const;
	FORCEINLINE ParameterArray* GetParmArray();
	FORCEINLINE const Vector<ShaderPass>& GetPasses() const;
	FORCEINLINE void SetStateEntity(ShaderState* pShaderData);
	FORCEINLINE bool SetParameter(RHIDefine::ParameterSlot att, MaterialParameter* obj);
	FORCEINLINE bool SetParameter(const String& paramName, MaterialParameter* obj);
	FORCEINLINE MaterialParameter* GetParameter(RHIDefine::ParameterSlot att);
	FORCEINLINE MaterialParameter* GetParameter(const String& paramName);
	FORCEINLINE bool GetRenderPass(int passIndex, ShaderKeyWords& keyWords, PassInfo& passInfo);
	FORCEINLINE uint64 GetShaderID() const;
	FORCEINLINE uint GetBaseLightMask() const;
	FORCEINLINE bool HasDefProperty(RHIDefine::ParameterSlot att) const;
	FORCEINLINE const DefProperty& GetProperty(RHIDefine::ParameterSlot slot);
private:
	virtual void _OnCreateResource() override;

};
DECLARE_Ptr(MaterialEntity);
TYPE_MARCO(MaterialEntity);

FORCEINLINE void MaterialEntity::EnableKeyWord(const String& key)
{
	return _GetResourcePtr()->EnableKeyWord(key);
}
FORCEINLINE void MaterialEntity::DisableKeyWord(const String& key)
{
	return _GetResourcePtr()->DisableKeyWord(key);
}
FORCEINLINE void MaterialEntity::DisableAllKeyWord()
{
	return _GetResourcePtr()->DisableAllKeyWord();
}
FORCEINLINE bool MaterialEntity::IsKeyWordEnable(const String& key)
{
	return _GetResourcePtr()->IsKeyWordEnable(key);
}
FORCEINLINE ShaderKeyWords MaterialEntity::GetKeyWords()
{
	return _GetResourcePtr()->GetKeyWords();
}
FORCEINLINE bool MaterialEntity::hasParameter(RHIDefine::ParameterSlot att)
{
	return _GetResourcePtr()->hasParameter(att);
}
FORCEINLINE const String& MaterialEntity::GetMaterialName() const
{
	return _GetResourcePtr()->GetMaterialName();
}
FORCEINLINE const String& MaterialEntity::GetShaderPath() const
{
	return _GetResourcePtr()->GetShaderPath();
}
FORCEINLINE uint MaterialEntity::GetRenderQueue() const
{
	return _GetResourcePtr()->GetRenderQueue();
}
FORCEINLINE bool MaterialEntity::IsSupportPass(RHIDefine::PassType passType) const
{
	return _GetResourcePtr()->IsSupportPass(passType);
}
FORCEINLINE ParameterArray* MaterialEntity::GetParmArray()
{
	return _GetResourcePtr()->GetParmArray();
}
FORCEINLINE const Vector<ShaderPass>& MaterialEntity::GetPasses() const
{
	return _GetResourcePtr()->GetPasses();
}
FORCEINLINE bool MaterialEntity::SetParameter(RHIDefine::ParameterSlot att, MaterialParameter* obj)
{
	return _GetResourcePtr()->SetParameter(att, obj);
}
FORCEINLINE bool MaterialEntity::SetParameter(const String& paramName, MaterialParameter* obj)
{
	return _GetResourcePtr()->SetParameter(paramName, obj);
}
FORCEINLINE MaterialParameter* MaterialEntity::GetParameter(RHIDefine::ParameterSlot att)
{
	return _GetResourcePtr()->GetParameter(att);
}
FORCEINLINE MaterialParameter* MaterialEntity::GetParameter(const String& paramName)
{
	return _GetResourcePtr()->GetParameter(paramName);
}
FORCEINLINE bool MaterialEntity::GetRenderPass(int passIndex, ShaderKeyWords& keyWords, PassInfo& passInfo)
{
	return _GetResourcePtr()->GetRenderPass(passIndex, keyWords, passInfo);
}
FORCEINLINE uint64 MaterialEntity::GetShaderID() const
{
	return _GetResourcePtr()->GetShaderID();
}
FORCEINLINE uint MaterialEntity::GetBaseLightMask() const
{
	return _GetResourcePtr()->GetBaseLightMask();
}
FORCEINLINE bool MaterialEntity::HasDefProperty(RHIDefine::ParameterSlot att) const
{
	return _GetResourcePtr()->HasDefProperty(att);
}
FORCEINLINE const DefProperty& MaterialEntity::GetProperty(RHIDefine::ParameterSlot slot)
{
	return _GetResourcePtr()->GetProperty(slot);
}

NS_JYE_END