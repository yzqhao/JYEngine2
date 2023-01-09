
FORCEINLINE void TextureSource::SetKeepSource(bool k)
{
	m_isKeepSource = k;
}

FORCEINLINE bool TextureSource::isKeepSource()
{
	return m_isKeepSource;
}

FORCEINLINE void TextureSource::SetTextureUsage(RHIDefine::TextureUseage tu)
{
	m_Usage = tu;
}

FORCEINLINE void TextureSource::SetTextureType(RHIDefine::TextureType tt)
{
	m_eTextureType = tt;
}

FORCEINLINE void TextureSource::SetWarp(RHIDefine::TextureWarp s, RHIDefine::TextureWarp t)
{
	m_Warp.m_SWarp = s;
	m_Warp.m_TWarp = t;
}

FORCEINLINE void TextureSource::SetMipMap(bool mip, uint level)
{
	m_isMipMap = mip;
	if (m_uBaseLevel > level)
		m_uBaseLevel = level;
	if (m_uTopLevel < level)
		m_uTopLevel = level;
}

FORCEINLINE void TextureSource::SetFilter(RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min)
{
	m_Filter.m_MagFilter = mag;
	m_Filter.m_MinFilter = min;
}

FORCEINLINE void TextureSource::SetPixelFormat(RHIDefine::PixelFormat pf)
{
	m_eFormat = pf;
}

FORCEINLINE void TextureSource::SetAnisotropic(uint x)
{
	m_Anisotropic = x;
}

FORCEINLINE void TextureSource::SetSamplerCompare(RHIDefine::TextureSamplerCompare compare)
{
	m_SamplerCompare = compare;
}

FORCEINLINE RHIDefine::PixelFormat TextureSource::GetPixelFormat() const
{
	return m_eFormat;
}

FORCEINLINE const Math::IntVec2& TextureSource::GetSize() const
{
	return m_Size;
}

FORCEINLINE RHIDefine::TextureType TextureSource::GetTextureType() const
{
	return m_eTextureType;
}

FORCEINLINE RHIDefine::WarpType TextureSource::GetWrap() const
{
	return m_Warp;
}

FORCEINLINE RHIDefine::TextureUseage TextureSource::GetMemoryUseage() const
{
	return m_Usage;
}

FORCEINLINE const std::string& TextureSource::GetName()
{
	return m_Name;
}

FORCEINLINE void TextureSource::SetRTAttachmentFlag(bool f)
{
	m_isRTAttachment = f;
}

FORCEINLINE void TextureSource::SetSRGB(bool _srgb)
{
	m_isSRGB = _srgb;
}

FORCEINLINE bool TextureSource::IsSRGB() const
{
	return m_isSRGB;
}

//
// TextureEntity
//

FORCEINLINE void TextureEntity::SetKeepSource(bool k)
{
	_GetSourcePtr()->SetKeepSource(k);
}

FORCEINLINE bool TextureEntity::isKeepSource()
{
	return _GetSourcePtr()->isKeepSource();
}

FORCEINLINE RHIDefine::PixelFormat TextureEntity::GetPixelFormat() const
{
	return _GetSourcePtr()->GetPixelFormat();
}

FORCEINLINE const Math::IntVec2& TextureEntity::GetSize() const
{
	return _GetSourcePtr()->GetSize();
}

FORCEINLINE RHIDefine::TextureType TextureEntity::GetTextureType() const
{
	return _GetSourcePtr()->GetTextureType();
}

FORCEINLINE RHIDefine::WarpType TextureEntity::GetWrap() const
{
	return _GetSourcePtr()->GetWrap();
}

FORCEINLINE RHIDefine::TextureUseage TextureEntity::GetMemoryUseage() const
{
	return _GetSourcePtr()->GetMemoryUseage();
}

FORCEINLINE const std::string& TextureEntity::GetName()
{
	return _GetSourcePtr()->GetName();
}

FORCEINLINE void TextureEntity::SetRTAttachmentFlag(bool f)
{
	_GetSourcePtr()->SetRTAttachmentFlag(f);
}

FORCEINLINE bool TextureEntity::IsSRGB() const
{
	return _GetSourcePtr()->IsSRGB();
}

FORCEINLINE handle TextureEntity::GetHandle() const
{
	return _GetResourcePtr()->GetHandle();
}

FORCEINLINE uint TextureEntity::GetFlags() const
{
	return _GetResourcePtr()->GetFlags();
}