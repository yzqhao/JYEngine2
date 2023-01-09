
FORCEINLINE const String& RenderTargetSource::GetName() const
{
	return m_Name;
}

FORCEINLINE bool RenderTargetSource::isRefrence()
{
	return nullhandle != m_RefrenceHandle;
}

FORCEINLINE void RenderTargetSource::SetWinHandle(void* whd)
{
	m_gWindow = (whd);
}

FORCEINLINE void RenderTargetSource::SetSize(const Math::IntVec2& size)
{
	m_Size = size;
}

FORCEINLINE const Math::IntVec2& RenderTargetSource::GetSize()
{
	return m_Size;
}

FORCEINLINE void RenderTargetSource::SetViewPort(const Math::IntVec4& rect)
{
	m_ViewPort = rect;
}

FORCEINLINE const Math::IntVec4& RenderTargetSource::GetViewPort()
{
	return m_ViewPort;
}

FORCEINLINE void RenderTargetSource::SetTargetType(RHIDefine::TargetType tt)
{
	m_eTargetType = tt;
}

//
// RenderTargetEntity
//

FORCEINLINE const std::string& RenderTargetEntity::GetName() const
{
	return _GetSourcePtr()->GetName();
}

FORCEINLINE bool RenderTargetEntity::IsSRGB() const
{
	if (_GetResourcePtr())
	{
		return _GetResourcePtr()->IsSRGB();
	}
	return false;
}

FORCEINLINE void RenderTargetEntity::MakeSureCreate()
{
	_GetResourcePtr()->MakeSureCreate();
}

FORCEINLINE TextureEntity* RenderTargetEntity::GetAttachment(RHIDefine::RenderTargetAttachment att)
{
	return _GetResourcePtr()->GetAttachment(att);
}

FORCEINLINE bool RenderTargetEntity::isMainRenderTarget() const
{
	return _GetResourcePtr()->isMainRenderTarget();
}

FORCEINLINE const Math::IntVec4& RenderTargetEntity::GetViewPort() const
{
	return _GetResourcePtr()->GetViewPort();
}

FORCEINLINE const Math::IntVec2& RenderTargetEntity::GetResolution() const
{
	return _GetResourcePtr()->GetResolution();
}

FORCEINLINE TextureStream* RenderTargetEntity::GetAttachmentStream(RHIDefine::RenderTargetAttachment rta)
{
	return _GetResourcePtr()->GetAttachmentStream(rta);
}

FORCEINLINE bool RenderTargetEntity::NeedRead()
{
	return _GetResourcePtr()->NeedRead();
}

FORCEINLINE void RenderTargetEntity::ReadBuffer()	//目前只读取COLOR0
{
	_GetResourcePtr()->ReadBuffer();
}

FORCEINLINE bool RenderTargetEntity::HasAttachment()
{
	return _GetResourcePtr()->HasAttachment();
}

FORCEINLINE handle RenderTargetEntity::GetGPUHandle() const
{
	return _GetResourcePtr()->GetGPUHandle();
}