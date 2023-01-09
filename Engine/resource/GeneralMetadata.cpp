#include "GeneralMetadata.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(PathMetadata, IMetadata);
BEGIN_ADD_PROPERTY(PathMetadata, IMetadata);
REGISTER_PROPERTY(m_Path, m_Path, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(PathMetadata)
IMPLEMENT_INITIAL_END

PathMetadata::PathMetadata()
{
}

PathMetadata::PathMetadata(const String& path)
	: m_Path(path)
{
}

void PathMetadata::GetIdentifier(String& hash) const
{
	hash.append(m_Path);
	hash.append(m_SceneIDString);
}

void PathMetadata::SetSceneID(const String& idstring)
{
	m_SceneIDString = idstring;
}

void* PathMetadata::_ReturnMetadata()
{
	return &m_Path;
}

void PathMetadata::ReleaseMetadate()
{
}

const String& PathMetadata::GetPath() const
{
	return m_Path;
}

void PathMetadata::SetPath(const String& path)
{
	m_Path = path;
}

void PathMetadata::SetDependencePathList(const Vector<String>& depPathList)
{
	m_DependencePath = depPathList;
}

void PathMetadata::AddDependencePath(const String& path)
{
	auto pos = std::find(m_DependencePath.begin(), m_DependencePath.end(), path);
	if (pos == m_DependencePath.end())
	{
		m_DependencePath.push_back(path);
	}
}

const Vector<String>& PathMetadata::GetDependencePathList() const
{
	return m_DependencePath;
}

void PathMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{

}

NS_JYE_END
