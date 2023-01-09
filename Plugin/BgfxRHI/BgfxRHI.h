
#pragma once

#include "RHI/RHI.h"
#include "private/Define.h"
#include "Math/Math.h"
#include "System/DataStruct.h"
#include "Core/EnableIf.h"

NS_JYE_BEGIN

#define TEXTURE_A8_FORMAT_MASK_BIT (1 << 27)
#define TEXTURE_L8_FORMAT_MASK_BIT (1 << 28)
#define TEXTURE_L8A8_FORMAT_MASK_BIT (1 << 29)

#define MAIN_RT_MASK_BIT (1 << 30)
#define DYNAMIC_BUFFER_MASK_BIT (1 << 31)

#define HANDLE_MASK ((1 << 16) - 1)

class BGFXRHI_API BgfxRHI : public FDynamicRHI
{
public:

	/** Initialization constructor. */
	BgfxRHI();

	/** Destructor */
	~BgfxRHI() {}

	// FDynamicRHI interface.
	virtual void Init();
	virtual void PostInit();

	virtual void Shutdown();
	virtual const char* GetName() override { return ("Bgfx RHI"); }

	virtual RHIDefine::GraphicRenderer GetRenderer() override final;

	virtual RHIDefine::GraphicFeatureLevel GetCurrentFeatureLevel() const override final;

	virtual void Frame() override final;

	virtual uint64 GetRenderState(const RenderState* state) override final;

	virtual handle CreateVertexBuffer(const void* buffer, uint size, const VertexStreamLayout& layout, RHIDefine::MemoryUseage usage, uint16 flags) override final;
	virtual void DestoryVertexBuffer(handle handle) override final;
	virtual void UpdateVertexBuffer(handle handle, uint startVertex, const void* buffer, uint size) override final;

	virtual handle CreateIndiceBuffer(uint datasize, uint typesize, const void* dataptr, RHIDefine::MemoryUseage use) override final;
	virtual void UpdateIndiceBuffer(handle ih, uint startIndex, uint datasize, uint typesize, const void* dataptr) override final;
	virtual void DestroyIndiceBuffer(handle ih) override final;

	virtual handle CreateShader(const std::string& code, RHIDefine::ShaderType shaderType,
		const std::vector<InputUniformInfo>& iuniformInfo,
		const std::vector<InputAttributes>& vsAttributes,
		unsigned short constBufferSize,
		std::vector<OutputUniformInfo>& oUniformInfo) override final;
	virtual void SetShaderName(handle _handle, const std::string& shaderName) override final;
	virtual void DestoryShader(handle _handle) override final;

	virtual handle CreateProgram(handle vsShader, handle psShader) override final;
	virtual handle CreateComputeProgram(handle csShader) override final;
	virtual void DestoryProgram(handle _handle) override final;

	virtual void DestoryUniform(handle _handle) override final;

	virtual uint GenTextureFlags(const RHIDefine::WarpType& ft,
		const RHIDefine::FilterType& warpType,
		const RHIDefine::TextureSamplerCompare& samplerCompare) override final;
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
		bool isMsaa = false) override final;
	virtual void SetTextureName(handle _handle, const std::string& name) override final;
	virtual void UpdateTexture2D(handle _handle, unsigned int _layer,
		unsigned char _mip, unsigned int x, unsigned int y,
		unsigned int width, unsigned int height,
		const void* buffer, unsigned int size,
		RHIReleaseFn _releasefunc = RHIReleaseFn(0),
		void* _releaseData = NULL,
		unsigned short _pitch = 0xffffU) override final;
	virtual handle CreateTextureCube(unsigned short _size,
		bool _hasMips, unsigned short _numLayers,
		RHIDefine::PixelFormat format, uint64 _flags = 0ULL,
		const void* buffer = nullptr, unsigned int size = 0,
		bool autoGenMipmap = false, int refhandle = -1, int mipNums = 0) override final;

	virtual void UpdateTextureCube(handle _handle, unsigned int _layer,
		RHIDefine::TextureType _side,
		unsigned char _mip, unsigned int x, unsigned int y,
		unsigned int width, unsigned int height,
		const void* buffer, unsigned int size,
		RHIReleaseFn _releasefunc = RHIReleaseFn(0),
		void* _releaseData = NULL,
		unsigned short _pitch = 0xffffU) override final;
	virtual void DestoryTexture(handle _handle) override final;
	virtual void ResizeTexture(handle _handle, unsigned short _width, unsigned short _height,
		unsigned char _numMips, unsigned short _numLayers,
		bool autoGenMipmap) override final;

	virtual handle CreateRenderTarget(void* h, uint width, uint height,
		RHIDefine::PixelFormat format = RHIDefine::PF_R8G8B8A8,
		RHIDefine::PixelFormat depth_format = RHIDefine::PF_DEPTH24_STENCIL8) override final;
	virtual handle CreateRenderTarget(int32_t refHandle, uint width, uint height,
		RHIDefine::PixelFormat format = RHIDefine::PF_R8G8B8A8,
		RHIDefine::PixelFormat depth_format = RHIDefine::PF_DEPTH24_STENCIL8) override final;
	virtual handle CreateRenderTarget(uint count, RHIDefine::RenderTargetAttachment* att, handle* texHandles) override final;
	virtual handle CreateRenderTarget(uint width, uint height, RHIDefine::PixelFormat format = RHIDefine::PF_R8G8B8A8) override final;
	virtual void DestoryRenderTarget(handle _rt) override final;

	virtual void ResetViewID() override final;
	virtual void SetCurrentViewID(uint16 id) override final;
	virtual uint16 GetUniformViewID() override final;

	virtual float GetDeviceCoordinateYFlip(handle rt)override final;




	virtual void SetViewPort(const Math::IntVec4& vp) override final;
	virtual void SetRenderTargetSize(const Math::IntVec2& rtSize) override final;
	virtual void SetScissor(const Math::IntVec4& scissor) override final;
	virtual void SetRenderTarget(handle rthandle) override final;
	virtual void SetClear(uint clearFlags, const Math::FColor& color, float depth = 1.0f, byte stencil = 0) override final;
	virtual void SetIndiceBuffer(handle ih) override final;
	virtual void SetRenderState(const uint64 state) override final;
	virtual void SetStencilState(const uint64 state) override final;
	virtual void SetUniform(handle handle, const void* value, uint16 num = 1) override final;
	virtual void SetTexture(RHIDefine::ShaderType shaderType, byte stage, byte bindPoint, handle uniHandle, handle texHandle, unsigned int flags = 0xffffffffU) override final;
	virtual void SetConstBuffer(RHIDefine::ShaderType shaderType, byte stage, byte bindingPoint, handle cbHandle) override final;

	virtual void SubmitIndexedDraw(
		RHIDefine::RenderMode renderMode,
		handle _vertexHandle,
		handle _indexHandle,
		handle _programHandle,
		uint _firstIndex,
		uint _indexCount) override final;
	virtual void SubmitIndexedDrawInstanced(
		RHIDefine::RenderMode renderMode,
		handle _vertexHandle,
		handle _indexHandle,
		handle _instanceHandle,
		handle _programHandle,
		uint _firstIndex,
		uint _indexCount,
		uint _instanceCount) override final;


	virtual bool IsTextureFormatSupportSRGB(RHIDefine::PixelFormat pf) override final;
	virtual bool IsSupportsSRGB() override final;
	virtual uint GetFrameBufferAttachments() override final;
	virtual bool SupportDepthTexture() override final;
	virtual bool SupportShadowSampler() override final;
	virtual uint GetMaxRTAttachments() override final;
	virtual bool SupportHalfFrameBuffer() override final;
	virtual bool SupportFloatFrameBuffer() override final;
	virtual bool SupportDeferredShading() override final;
	virtual bool IsSupportCompute() override final;
	virtual bool IsSupportGPUInstancing() override final;
private:
	void _SetCurrentViewID(uint16 id);
	uint16 _GetCurrentViewID();

	struct CacheVariables
	{
		uint64           cacheState;
		Math::IntVec2	 cacheRtSize;
		uint64			 cacheRTHandle;
	};

	CacheVariables		m_CacheInfo;

	uint m_ViewUniID;
};

class FBgfxDynamicRHIModule : public IDynamicRHIModule
{
public:

	// IDynamicRHIModule
	virtual bool IsSupported() override;
	virtual FDynamicRHI* CreateRHI() override;
};

extern BGFXRHI_API BgfxRHI* GOpenGLRHI;

NS_JYE_END