
#include "private/Define.h"
#include "Core/Interface/IModuleInterface.h"
#include "Core/Interface/ILogSystem.h"
#include "Math/IntRect.h"
#include "Core/ThreadSafeCounter.h"
#include "Math/Box2d.h"
#include "Math/IntVec4.h"
#include "Math/Color.h"

NS_JYE_BEGIN

class VertexStreamLayout;
class RenderState;
struct InputAttributes;
struct InputUniformInfo;
struct OutputUniformInfo;

typedef void(*RHIReleaseFn)(void* _ptr, void* _userData);

/** The interface which is implemented by the dynamically bound RHI. */
class RHI_API FDynamicRHI
{
public:

	/** Declare a virtual destructor, so the dynamic RHI can be deleted without knowing its type. */
	virtual ~FDynamicRHI() {}

	/** Initializes the RHI; separate from IDynamicRHIModule::CreateRHI so that GDynamicRHI is set when it is called. */
	virtual void Init() = 0;

	/** Called after the RHI is initialized; before the render thread is started. */
	virtual void PostInit() {}

	/** Shutdown the RHI; handle shutdown and resource destruction before the RHI's actual destructor is called (so that all resources of the RHI are still available for shutdown). */
	virtual void Shutdown() = 0;

	virtual const char* GetName() = 0;

	virtual RHIDefine::GraphicRenderer GetRenderer() = 0;

	virtual RHIDefine::GraphicFeatureLevel GetCurrentFeatureLevel() const = 0;

	virtual void Frame() = 0;

	virtual uint64 GetRenderState(const RenderState* state) = 0;

	virtual handle CreateVertexBuffer(const void* buffer, uint size, const VertexStreamLayout& layout, RHIDefine::MemoryUseage usage, uint16 flags) = 0;
	virtual void DestoryVertexBuffer(handle handle) = 0;
	virtual void UpdateVertexBuffer(handle handle, uint startVertex, const void* buffer, uint size) = 0;

	virtual handle CreateIndiceBuffer(uint datasize, uint typesize, const void* dataptr, RHIDefine::MemoryUseage use) = 0;
	virtual void UpdateIndiceBuffer(handle ih, uint startIndex, uint datasize, uint typesize, const void* dataptr) = 0;
	virtual void DestroyIndiceBuffer(handle ih) = 0;

	virtual handle CreateShader(const std::string& code, RHIDefine::ShaderType shaderType,
		const std::vector<InputUniformInfo>& iuniformInfo,
		const std::vector<InputAttributes>& vsAttributes,
		unsigned short constBufferSize,
		std::vector<OutputUniformInfo>& oUniformInfo) = 0;
	virtual void SetShaderName(handle _handle, const std::string& shaderName) = 0;
	virtual void DestoryShader(handle _handle) = 0;

	virtual handle CreateProgram(handle vsShader, handle psShader) = 0;
	virtual handle CreateComputeProgram(handle csShader) = 0;
	virtual void DestoryProgram(handle _handle) = 0;

	virtual void DestoryUniform(handle _handle) = 0;

	virtual uint GenTextureFlags(const RHIDefine::WarpType& ft,
		const RHIDefine::FilterType& warpType,
		const RHIDefine::TextureSamplerCompare& samplerCompare) = 0;
	virtual handle CreateTexture2D(unsigned short _width, unsigned short _height,
		bool _hasMips, unsigned short _numLayers,
		RHIDefine::PixelFormat format,
		RHIDefine::TextureUseage usage,
		uint64 _flags = 0ULL,
		const void* buffer = nullptr, unsigned int size = 0,
		bool autoGenMipmap = false,
		bool isRT = false,
		bool isSRGB = false,
		int mipNums = 0,
		bool isMsaa = false) = 0;
	virtual void SetTextureName(handle _handle, const std::string& name) = 0;
	virtual void UpdateTexture2D(handle _handle, unsigned int _layer,
		unsigned char _mip, unsigned int x, unsigned int y,
		unsigned int width, unsigned int height,
		const void* buffer, unsigned int size,
		RHIReleaseFn _releasefunc = RHIReleaseFn(0),
		void* _releaseData = NULL,
		unsigned short _pitch = 0xffffU) = 0;
	virtual handle CreateTextureCube(unsigned short _size,
		bool _hasMips, unsigned short _numLayers,
		RHIDefine::PixelFormat format, uint64 _flags = 0ULL,
		const void* buffer = nullptr, unsigned int size = 0,
		bool autoGenMipmap = false, int refhandle = -1, int mipNums = 0) = 0;

	virtual void UpdateTextureCube(handle _handle, unsigned int _layer,
		RHIDefine::TextureType _side,
		unsigned char _mip, unsigned int x, unsigned int y,
		unsigned int width, unsigned int height,
		const void* buffer, unsigned int size,
		RHIReleaseFn _releasefunc = RHIReleaseFn(0),
		void* _releaseData = NULL,
		unsigned short _pitch = 0xffffU) = 0;
	virtual void DestoryTexture(handle _handle) = 0;
	virtual void ResizeTexture(handle _handle, unsigned short _width, unsigned short _height,
		unsigned char _numMips, unsigned short _numLayers,
		bool autoGenMipmap) = 0;

	virtual handle CreateRenderTarget(void* h, uint width, uint height,
		RHIDefine::PixelFormat format = RHIDefine::PF_R8G8B8A8,
		RHIDefine::PixelFormat depth_format = RHIDefine::PF_DEPTH24_STENCIL8) = 0;
	virtual handle CreateRenderTarget(int32_t refHandle, uint width, uint height,
		RHIDefine::PixelFormat format = RHIDefine::PF_R8G8B8A8,
		RHIDefine::PixelFormat depth_format = RHIDefine::PF_DEPTH24_STENCIL8) = 0;
	virtual handle CreateRenderTarget(uint count, RHIDefine::RenderTargetAttachment* att, handle* texHandles) = 0;
	virtual handle CreateRenderTarget(uint width, uint height, RHIDefine::PixelFormat format = RHIDefine::PF_R8G8B8A8) = 0;
	virtual void DestoryRenderTarget(handle _rt) = 0;

	virtual void ResetViewID() = 0;
	virtual void SetCurrentViewID(uint16 id) = 0;
	virtual uint16 GetUniformViewID() = 0;

	virtual float GetDeviceCoordinateYFlip(handle rt) = 0;

	


	virtual void SetViewPort(const Math::IntVec4& vp) = 0;
	virtual void SetRenderTargetSize(const Math::IntVec2& rtSize) = 0;
	virtual void SetScissor(const Math::IntVec4& scissor) = 0;
	virtual void SetRenderTarget(handle rthandle) = 0;
	virtual void SetClear(uint clearFlags, const Math::FColor& color, float depth = 1.0f, byte stencil = 0) = 0;
	virtual void SetIndiceBuffer(handle ih) = 0;
	virtual void SetRenderState(const uint64 state) = 0;
	virtual void SetStencilState(const uint64 state) = 0;
	virtual void SetUniform(handle handle, const void* value, uint16 num = 1) = 0;
	virtual void SetTexture(RHIDefine::ShaderType shaderType, byte stage, byte bindPoint, handle uniHandle, handle texHandle, unsigned int flags = 0xffffffffU) = 0;
	virtual void SetConstBuffer(RHIDefine::ShaderType shaderType, byte stage, byte bindingPoint, handle cbHandle) = 0;

	virtual void SubmitIndexedDraw(
		RHIDefine::RenderMode renderMode,
		handle _vertexHandle,
		handle _indexHandle,
		handle _programHandle,
		uint _firstIndex,
		uint _indexCount) = 0;
	virtual void SubmitIndexedDrawInstanced(
		RHIDefine::RenderMode renderMode,
		handle _vertexHandle,
		handle _indexHandle,
		handle _instanceHandle,
		handle _programHandle,
		uint _firstIndex,
		uint _indexCount,
		uint _instanceCount) = 0;

	virtual bool IsTextureFormatSupportSRGB(RHIDefine::PixelFormat pf) = 0;
	virtual bool IsSupportsSRGB() = 0;
	virtual uint GetFrameBufferAttachments() = 0;
	virtual bool SupportDepthTexture() = 0;
	virtual bool SupportShadowSampler() = 0;
	virtual uint GetMaxRTAttachments() = 0;
	virtual bool SupportHalfFrameBuffer() = 0;
	virtual bool SupportFloatFrameBuffer() = 0;
	virtual bool SupportDeferredShading() = 0;
	virtual bool IsSupportCompute() = 0;
	virtual bool IsSupportGPUInstancing() = 0;
};

/** A global pointer to the dynamically bound RHI implementation. */
extern RHI_API FDynamicRHI* GDynamicRHI;

class IDynamicRHIModule : public IModuleInterface
{
public:

	/** Checks whether the RHI is supported by the current system. */
	virtual bool IsSupported() = 0;

	/** Creates a new instance of the dynamic RHI implemented by the module. */
	virtual FDynamicRHI* CreateRHI() = 0;
};

FORCEINLINE RHIDefine::GraphicRenderer RHIGetRenderer()
{
	return GDynamicRHI->GetRenderer();
}

FORCEINLINE RHIDefine::GraphicFeatureLevel RHIGetCurrentFeatureLevel()
{
	return GDynamicRHI->GetCurrentFeatureLevel();
}

FORCEINLINE void RHIFrame()
{
	return GDynamicRHI->Frame();
}

FORCEINLINE uint64 RHIGetRenderState(const RenderState* state)
{
	return GDynamicRHI->GetRenderState(state);
}

FORCEINLINE handle RHICreateVertexBuffer(const void* buffer, uint size, const VertexStreamLayout& layout, RHIDefine::MemoryUseage usage, uint16 flags)
{
	return GDynamicRHI->CreateVertexBuffer(buffer, size, layout, usage, flags);
}

FORCEINLINE void RHIDestoryVertexBuffer(handle h)
{
	return GDynamicRHI->DestoryVertexBuffer(h);
}

FORCEINLINE void RHIUpdateVertexBuffer(handle h, uint startVertex, const void* buffer, uint size)
{
	return GDynamicRHI->UpdateVertexBuffer(h, startVertex, buffer, size);
}

FORCEINLINE handle RHICreateIndiceBuffer(uint datasize, uint typesize, const void* dataptr, RHIDefine::MemoryUseage use)
{
	return GDynamicRHI->CreateIndiceBuffer(datasize, typesize, dataptr, use);
}

FORCEINLINE void RHIUpdateIndiceBuffer(handle ih, uint startIndex, uint datasize, uint typesize, const void* dataptr)
{
	return GDynamicRHI->UpdateIndiceBuffer(ih, startIndex, datasize, typesize, dataptr);
}

FORCEINLINE void RHIDestroyIndiceBuffer(handle h)
{
	return GDynamicRHI->DestroyIndiceBuffer(h);
}

FORCEINLINE handle RHICreateShader(const String& code, RHIDefine::ShaderType shaderType,
	const std::vector<InputUniformInfo>& iuniformInfo,
	const std::vector<InputAttributes>& vsAttributes,
	unsigned short constBufferSize,
	std::vector<OutputUniformInfo>& oUniformInfo)
{
	return GDynamicRHI->CreateShader(code, shaderType, iuniformInfo, vsAttributes, constBufferSize, oUniformInfo);
}

FORCEINLINE void RHISetShaderName(handle _handle, const String& shaderName)
{
	return GDynamicRHI->SetShaderName(_handle, shaderName);
}

FORCEINLINE void RHIDestoryShader(handle _handle)
{
	return GDynamicRHI->DestoryShader(_handle);
}

FORCEINLINE handle RHICreateProgram(handle vsShader, handle psShader)
{
	return GDynamicRHI->CreateProgram(vsShader, psShader);
}

FORCEINLINE handle RHICreateComputeProgram(handle csShader)
{
	return GDynamicRHI->CreateComputeProgram(csShader);
}

FORCEINLINE void RHIDestoryProgram(handle _handle)
{
	return GDynamicRHI->DestoryProgram(_handle);
}

FORCEINLINE void RHIDestoryUniform(handle _handle)
{
	return GDynamicRHI->DestoryUniform(_handle);
}

FORCEINLINE uint RHIGenTextureFlags(const RHIDefine::WarpType& ft,
	const RHIDefine::FilterType& warpType,
	const RHIDefine::TextureSamplerCompare& samplerCompare)
{
	return GDynamicRHI->GenTextureFlags(ft, warpType, samplerCompare);
}

FORCEINLINE handle RHICreateTexture2D(unsigned short _width, unsigned short _height,
	bool _hasMips, unsigned short _numLayers,
	RHIDefine::PixelFormat format,
	RHIDefine::TextureUseage usage,
	uint64 _flags = 0ULL,
	const void* buffer = nullptr, unsigned int size = 0,
	bool autoGenMipmap = false,
	bool isRT = false,
	bool isSRGB = false,
	int mipNums = 0,
	bool isMsaa = false)
{
	return GDynamicRHI->CreateTexture2D(_width, _height, _hasMips, _numLayers, format, usage, _flags, buffer, size, autoGenMipmap, isRT, isSRGB, mipNums, isMsaa);
}

FORCEINLINE void RHISetTextureName(handle _handle, const std::string& name)
{
	return GDynamicRHI->SetTextureName(_handle, name);
}

FORCEINLINE void RHIUpdateTexture2D(handle _handle, unsigned int _layer,
	unsigned char _mip, unsigned int x, unsigned int y,
	unsigned int width, unsigned int height,
	const void* buffer, unsigned int size,
	RHIReleaseFn _releasefunc = RHIReleaseFn(0),
	void* _releaseData = NULL,
	unsigned short _pitch = 0xffffU)
{
	return GDynamicRHI->UpdateTexture2D(_handle, _layer, _mip, x, y, width, height, buffer, size, _releasefunc, _releaseData, _pitch);
}

FORCEINLINE handle RHICreateTextureCube(unsigned short _size,
	bool _hasMips, unsigned short _numLayers,
	RHIDefine::PixelFormat format, uint64 _flags = 0ULL,
	const void* buffer = nullptr, unsigned int size = 0,
	bool autoGenMipmap = false, int refhandle = -1, int mipNums = 0)
{
	return GDynamicRHI->CreateTextureCube(_size, _hasMips, _numLayers, format, _flags, buffer, size, autoGenMipmap, refhandle, mipNums);
}

FORCEINLINE void RHIUpdateTextureCube(handle _handle, unsigned int _layer,
	RHIDefine::TextureType _side,
	unsigned char _mip, unsigned int x, unsigned int y,
	unsigned int width, unsigned int height,
	const void* buffer, unsigned int size,
	RHIReleaseFn _releasefunc = RHIReleaseFn(0),
	void* _releaseData = NULL,
	unsigned short _pitch = 0xffffU)
{
	return GDynamicRHI->UpdateTextureCube(_handle, _layer, _side, _mip, x, y, width, height, buffer, size, _releasefunc, _releaseData, _pitch);
}

FORCEINLINE void RHIDestoryTexture(handle _handle)
{
	return GDynamicRHI->DestoryTexture(_handle);
}

FORCEINLINE handle RHICreateRenderTarget(void* h, uint width, uint height,
	RHIDefine::PixelFormat format = RHIDefine::PF_R8G8B8A8,
	RHIDefine::PixelFormat depth_format = RHIDefine::PF_DEPTH24_STENCIL8)
{
	return GDynamicRHI->CreateRenderTarget(h, width, height, format, depth_format);
}

FORCEINLINE handle RHICreateRenderTarget(int32_t refHandle, uint width, uint height,
	RHIDefine::PixelFormat format = RHIDefine::PF_R8G8B8A8,
	RHIDefine::PixelFormat depth_format = RHIDefine::PF_DEPTH24_STENCIL8)
{
	return GDynamicRHI->CreateRenderTarget(refHandle, width, height, format, depth_format);
}

FORCEINLINE handle RHICreateRenderTarget(uint count, RHIDefine::RenderTargetAttachment* att, handle* texHandles)
{
	return GDynamicRHI->CreateRenderTarget(count, att, texHandles);
}

FORCEINLINE handle RHICreateRenderTarget(uint width, uint height, RHIDefine::PixelFormat format = RHIDefine::PF_R8G8B8A8)
{
	return GDynamicRHI->CreateRenderTarget(width, height, format);
}

FORCEINLINE void RHIDestoryRenderTarget(handle _rt)
{
	return GDynamicRHI->DestoryRenderTarget(_rt);
}

FORCEINLINE void RHIResetViewID()
{
	return GDynamicRHI->ResetViewID();
}

FORCEINLINE void RHISetCurrentViewID(uint16 id)
{
	return GDynamicRHI->SetCurrentViewID(id);
}

FORCEINLINE uint16 RHIGetUniformViewID()
{
	return GDynamicRHI->GetUniformViewID();
}

FORCEINLINE float RHIGetDeviceCoordinateYFlip(handle rt)
{
	return GDynamicRHI->GetDeviceCoordinateYFlip(rt);
}


FORCEINLINE void RHIResizeTexture(handle _handle, uint16 _width, uint16 _height, byte _numMips, uint16 _numLayers, bool autoGenMipmap)
{
	return GDynamicRHI->ResizeTexture(_handle, _width, _height, _numMips, _numLayers, autoGenMipmap);
}

FORCEINLINE void RHISubmitIndexedDraw(RHIDefine::RenderMode renderMode, handle vh, handle ih, handle ph, uint _firstIndex, uint _indexCount)
{
	return GDynamicRHI->SubmitIndexedDraw(renderMode, vh, ih, ph, _firstIndex, _indexCount);
}

FORCEINLINE void RHISetViewPort(const Math::IntVec4& vp)
{
	return GDynamicRHI->SetViewPort(vp);
}

FORCEINLINE void RHISetRenderTargetSize(const Math::IntVec2& s)
{
	return GDynamicRHI->SetRenderTargetSize(s);
}

FORCEINLINE void RHISetScissor(const Math::IntVec4& vp)
{
	return GDynamicRHI->SetScissor(vp);
}

FORCEINLINE void RHISetRenderTarget(handle rthandle)
{
	return GDynamicRHI->SetRenderTarget(rthandle);
}

FORCEINLINE void RHISetClear(uint clearFlags, const Math::FColor& color, float depth = 1.0f, byte stencil = 0)
{
	return GDynamicRHI->SetClear(clearFlags, color, depth, stencil);
}

FORCEINLINE void RHISetRenderState(const uint64 state)
{
	return GDynamicRHI->SetRenderState(state);
}

FORCEINLINE void RHISetUniform(handle handle, const void* value, uint16 num = 1)
{
	return GDynamicRHI->SetUniform(handle, value, num);
}

FORCEINLINE void RHISetTexture(RHIDefine::ShaderType shaderType, byte stage, byte bindPoint, handle uniHandle, handle texHandle, unsigned int flags = 0xffffffffU)
{
	return GDynamicRHI->SetTexture(shaderType, stage, bindPoint, uniHandle, texHandle, flags);
}


FORCEINLINE bool RHIIsTextureFormatSupportSRGB(RHIDefine::PixelFormat pf)
{
	return GDynamicRHI->IsTextureFormatSupportSRGB(pf);
}
FORCEINLINE bool RHIIsSupportsSRGB()
{
	return GDynamicRHI->IsSupportsSRGB();
}
FORCEINLINE uint RHIGetFrameBufferAttachments()
{
	return GDynamicRHI->GetFrameBufferAttachments();
}
FORCEINLINE bool RHISupportDepthTexture()
{
	return GDynamicRHI->SupportDepthTexture();
}
FORCEINLINE bool RHISupportShadowSampler()
{
	return GDynamicRHI->SupportShadowSampler();
}
FORCEINLINE uint RHIGetMaxRTAttachments()
{
	return GDynamicRHI->GetMaxRTAttachments();
}
FORCEINLINE bool RHISupportHalfFrameBuffer()
{
	return GDynamicRHI->SupportHalfFrameBuffer();
}
FORCEINLINE bool RHISupportFloatFrameBuffer()
{
	return GDynamicRHI->SupportFloatFrameBuffer();
}
FORCEINLINE bool RHISupportDeferredShading()
{
	return GDynamicRHI->SupportDeferredShading();
}
FORCEINLINE bool IsSupportCompute()
{
	return GDynamicRHI->IsSupportCompute();
}
FORCEINLINE bool IsSupportGPUInstancing()
{
	return GDynamicRHI->IsSupportGPUInstancing();
}

NS_JYE_END
