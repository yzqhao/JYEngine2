#include "RenderTargetMetadata.h"
#include "Core/Interface/IFileSystem.h"
#include "System/Utility.hpp"
#include "Core/Interface/IResourceSystem.h"
#include "Engine/render/texture/TextureStream.h"
#include "Core/Interface/ILoader.h"

#include <sstream>

NS_JYE_BEGIN

IMPLEMENT_RTTI(RenderTargetMetadata, PathMetadata);
BEGIN_ADD_PROPERTY(RenderTargetMetadata, PathMetadata);
//REGISTER_PROPERTY(m_Metadata.m_Name, m_eRenderMode, Property::F_SAVE_LOAD_CLONE)
//REGISTER_PROPERTY(m_Metadata.m_Color, m_eRenderMode, Property::F_SAVE_LOAD_CLONE)
//REGISTER_PROPERTY(m_Metadata.m_ViewPort, m_eRenderMode, Property::F_SAVE_LOAD_CLONE)
//REGISTER_PROPERTY(m_Metadata.m_Size, m_eRenderMode, Property::F_SAVE_LOAD_CLONE)
//REGISTER_PROPERTY(m_Metadata.m_Scale, m_eRenderMode, Property::F_SAVE_LOAD_CLONE)
//REGISTER_PROPERTY(m_Metadata.m_eTargetType, m_eRenderMode, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

IMPLEMENT_INITIAL_BEGIN(RenderTargetMetadata)
IMPLEMENT_INITIAL_END

RenderTargetMetadata::RenderTargetMetadata(
	RHIDefine::TargetType tt,
	const Math::IntVec4& vp,
	const Math::IntVec2& size,
	const Math::Vec2& scale,
	const bool           issRGB,
	void* _winhd)
{
	m_Metadata.m_ViewPort = vp;
	m_Metadata.m_Size = size;
	m_Metadata.m_eTargetType = tt;
	m_Metadata.m_Scale = scale;
	m_Metadata.m_gWindow = _winhd;
	m_Metadata.m_isSrgb = issRGB;
}

RenderTargetMetadata::RenderTargetMetadata(
	RHIDefine::TargetType tt,
	const Math::IntVec4& vp,
	const Math::IntVec2& size,
	const bool           issRGB,
	void* _winhd)
{
	m_Metadata.m_ViewPort = vp;
	m_Metadata.m_Size = size;
	m_Metadata.m_eTargetType = tt;
	m_Metadata.m_Scale.set(1, 1);
	m_Metadata.m_gWindow = _winhd;
	m_Metadata.m_isSrgb = issRGB;
}

RenderTargetMetadata::~RenderTargetMetadata(void)
{
}

void* RenderTargetMetadata::_ReturnMetadata()
{
	return &m_Metadata;
}

void RenderTargetMetadata::GetIdentifier(std::string& hash) const
{
	std::stringstream ss;
	ss.precision(2);
	ss << m_Metadata.m_ViewPort.x
		<< m_Metadata.m_ViewPort.y
		<< m_Metadata.m_ViewPort.z
		<< m_Metadata.m_ViewPort.w
		<< m_Metadata.m_Size.x
		<< m_Metadata.m_Size.y
		<< m_Metadata.m_Scale.x
		<< m_Metadata.m_Scale.y
		<< m_Metadata.m_eTargetType
		<< m_Metadata.m_isSrgb;
	hash = ss.str().c_str();
}

void RenderTargetMetadata::ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash)
{
}

void RenderTargetMetadata::ReleaseMetadate()
{

}

void RenderTargetMetadata::Resizeview(const ViewResolution& vr)
{
	m_Metadata.m_ViewPort = vr.m_ViewSize;
	m_Metadata.m_Size = vr.m_Resolution;
}

NS_JYE_END
