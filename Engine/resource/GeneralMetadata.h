#pragma once

#include "IMetadata.h"

NS_JYE_BEGIN

class ENGINE_API PathMetadata : public IMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
protected:
	String m_Path;
	Vector<String> m_DependencePath;
	String m_SceneIDString;
public:
	PathMetadata();
	PathMetadata(const String& path);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(String& hash) const;//在str中推入表示符号
	void SetSceneID(const String& idstring);
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash) override;//处理原始资源
	virtual void ReleaseMetadate();
	const String& GetPath() const;
	void SetPath(const String& path);
	void SetDependencePathList(const Vector<String>& depPathList);
	void AddDependencePath(const String& path);
	const Vector<String>& GetDependencePathList() const;
};
DECLARE_Ptr(PathMetadata);
TYPE_MARCO(PathMetadata);

NS_JYE_END