#include "BgfxRHI.h"
#include "bgfx/bgfx.h"

NS_JYE_BEGIN

bgfx::TextureFormat::Enum _BGFXPixelFormat(RHIDefine::PixelFormat f) 
{
	switch (f)
	{
		// Currently expand L8, A8 and L8A8 to RGBA8 to fit bgfx
	case RHIDefine::PF_A8: return bgfx::TextureFormat::A8;
	case RHIDefine::PF_L8: return bgfx::TextureFormat::RGBA8;
	case RHIDefine::PF_L8A8: return bgfx::TextureFormat::RGBA8;
	case RHIDefine::PF_R8G8B8: return bgfx::TextureFormat::RGB8;
	case RHIDefine::PF_R5G6B5: return bgfx::TextureFormat::R5G6B5;
	case RHIDefine::PF_R8G8B8A8: return bgfx::TextureFormat::RGBA8;
	case RHIDefine::PF_R4G4B4A4: return bgfx::TextureFormat::RGBA4;
	case RHIDefine::PF_DEPTH16: return bgfx::TextureFormat::D16;
	case RHIDefine::PF_DEPTH24_STENCIL8: return bgfx::TextureFormat::D24S8;
	case RHIDefine::PF_AUTO:  return bgfx::TextureFormat::RGBA8;
		// floating point texture format
	case RHIDefine::PF_RGBAFLOAT:  return bgfx::TextureFormat::RGBA32F;
	case RHIDefine::PF_RGBAHALF:	return bgfx::TextureFormat::RGBA16F;
	case RHIDefine::PF_RG11B10FLOAT:	return bgfx::TextureFormat::RG11B10F;
	case RHIDefine::PF_R16_FLOAT: return bgfx::TextureFormat::R16F;
	case  RHIDefine::PF_R32_UINT: return bgfx::TextureFormat::R32U;
	case RHIDefine::PF_R32_FLOAT: return bgfx::TextureFormat::R32F;
	default: JYERROR("BgfxTexture: unkown pixel format"); break;
	}
	return bgfx::TextureFormat::Unknown;
}

uint8_t _BGFXCubeSize(RHIDefine::TextureType texType)
{
	switch (texType)
	{
	case RHIDefine::TT_TEXTURECUBE_FRONT:
	{
		return BGFX_CUBE_MAP_POSITIVE_Z;
	}
	case RHIDefine::TT_TEXTURECUBE_BACK:
	{
		return BGFX_CUBE_MAP_NEGATIVE_Z;
	}
	case RHIDefine::TT_TEXTURECUBE_TOP:
	{
		return BGFX_CUBE_MAP_POSITIVE_Y;
	}
	case RHIDefine::TT_TEXTURECUBE_BOTTOM:
	{
		return BGFX_CUBE_MAP_NEGATIVE_Y;
	}
	case RHIDefine::TT_TEXTURECUBE_LEFT:
	{
		return BGFX_CUBE_MAP_NEGATIVE_X;
	}
	case RHIDefine::TT_TEXTURECUBE_RIGHT:
	{
		return BGFX_CUBE_MAP_POSITIVE_X;
	}
	default:JYERROR("BgfxTexture: Unsupported cube map target type!");
	}
	return BGFX_CUBE_MAP_POSITIVE_Z;
}

uint64 _BGFXMemUsage2Flags(RHIDefine::TextureUseage usage)
{
	switch (usage)
	{
	case RHIDefine::TU_READ:
		return BGFX_TEXTURE_READ_BACK;
	case RHIDefine::TU_RT_WRITE_ONLY:
		return BGFX_TEXTURE_RT_WRITE_ONLY;
	case RHIDefine::TU_COMPUTEWRITE:
		return BGFX_TEXTURE_COMPUTE_WRITE;
	default:
		break;
	}
	return 0ull;
}

void ExpandBuffer(byte* dest, const byte* src, uint cpysize, uint _format);

uint BgfxRHI::GenTextureFlags(const RHIDefine::WarpType& warpType, const RHIDefine::FilterType& ft, const RHIDefine::TextureSamplerCompare& samplerCompare)
{
	uint uWarpType = 0;
	uint vWarpType = 0;
	uint migFilterType = 0;
	uint magFilterType = 0;
	uint samplerCmp = 0;

	switch (warpType.m_SWarp)
	{
	case RHIDefine::TW_REPEAT:
		uWarpType = BGFX_TEXTURE_NONE;
		break;
	case RHIDefine::TW_CLAMP_TO_EDGE:
		uWarpType = BGFX_SAMPLER_U_CLAMP;
		break;
	case RHIDefine::TW_MIRRORED_REPEAT:
		uWarpType = BGFX_SAMPLER_U_MIRROR;
		break;
	case RHIDefine::TW_CLAMP_TO_BORDER:
		uWarpType = BGFX_SAMPLER_U_BORDER;
		break;
	default:
		break;
	}

	switch (warpType.m_TWarp)
	{
	case RHIDefine::TW_REPEAT:
		vWarpType = BGFX_TEXTURE_NONE;
		break;
	case RHIDefine::TW_CLAMP_TO_EDGE:
		vWarpType = BGFX_SAMPLER_V_CLAMP;
		break;
	case RHIDefine::TW_MIRRORED_REPEAT:
		vWarpType = BGFX_SAMPLER_V_MIRROR;
		break;
	case RHIDefine::TW_CLAMP_TO_BORDER:
		vWarpType = BGFX_SAMPLER_V_BORDER;
		break;
	default:
		break;
	}

	switch (ft.m_MinFilter)
	{
	case RHIDefine::TF_NEAREST:
		migFilterType = BGFX_SAMPLER_MIN_POINT;
		break;
	case RHIDefine::TF_LINEAR:
		migFilterType = BGFX_SAMPLER_NONE;
		break;
	case RHIDefine::TF_NEAREST_MIPMAP_NEAREST:
		migFilterType = BGFX_SAMPLER_MIP_POINT;
		break;
	case RHIDefine::TF_LINEAR_MIPMAP_NEAREST:
		migFilterType = BGFX_SAMPLER_NONE;
		break;
	case RHIDefine::TF_NEAREST_MIPMAP_LINEAR:
		migFilterType = BGFX_SAMPLER_MIP_POINT;
		break;
	case RHIDefine::TF_LINEAR_MIPMAP_LINEAR:
		migFilterType = BGFX_SAMPLER_NONE;
		break;
	default:
		break;
	}

	switch (ft.m_MagFilter)
	{
	case RHIDefine::TF_NEAREST:
		magFilterType = BGFX_SAMPLER_MAG_POINT;
		break;
	case RHIDefine::TF_LINEAR:
		magFilterType = BGFX_SAMPLER_NONE;
		break;
	case RHIDefine::TF_NEAREST_MIPMAP_NEAREST:
		magFilterType = BGFX_SAMPLER_MAG_POINT;
		break;
	case RHIDefine::TF_LINEAR_MIPMAP_NEAREST:
		magFilterType = BGFX_SAMPLER_NONE;
		break;
	case RHIDefine::TF_NEAREST_MIPMAP_LINEAR:
		magFilterType = BGFX_SAMPLER_MAG_POINT;
		break;
	case RHIDefine::TF_LINEAR_MIPMAP_LINEAR:
		magFilterType = BGFX_SAMPLER_NONE;
		break;
	default:
		break;
	}

	switch (samplerCompare)
	{
	case RHIDefine::SAMPLER_COMPARE_LESS:
		samplerCmp = BGFX_SAMPLER_COMPARE_LESS;
		break;
	case RHIDefine::SAMPLER_COMPARE_LEQUAL:
		samplerCmp = BGFX_SAMPLER_COMPARE_LEQUAL;
		break;
	case RHIDefine::SAMPLER_COMPARE_EQUAL:
		samplerCmp = BGFX_SAMPLER_COMPARE_EQUAL;
		break;
	case RHIDefine::SAMPLER_COMPARE_GEQUAL:
		samplerCmp = BGFX_SAMPLER_COMPARE_GEQUAL;
		break;
	case RHIDefine::SAMPLER_COMPARE_GREATER:
		samplerCmp = BGFX_SAMPLER_COMPARE_GREATER;
		break;
	case RHIDefine::SAMPLER_COMPARE_NOTEQUAL:
		samplerCmp = BGFX_SAMPLER_COMPARE_NOTEQUAL;
		break;
	case RHIDefine::SAMPLER_COMPARE_NEVER:
		samplerCmp = BGFX_SAMPLER_COMPARE_NEVER;
		break;
	case RHIDefine::SAMPLER_COMPARE_ALWAYS:
		samplerCmp = BGFX_SAMPLER_COMPARE_ALWAYS;
		break;
	default:
		break;
	}

	return uWarpType | vWarpType | migFilterType | magFilterType | samplerCmp;
}

handle BgfxRHI::CreateTexture2D(unsigned short _width, unsigned short _height, bool _hasMips, unsigned short _numLayers, RHIDefine::PixelFormat format, RHIDefine::TextureUseage usage, uint64 _flags /*= 0ULL*/, const void* buffer /*= nullptr*/, unsigned int size /*= 0*/, bool autoGenMipmap /*= false*/, bool isRT /*= false*/, bool isSRGB /*= false*/, int mipNums /*= 0*/, bool isMsaa /*= false*/)
{
	const bgfx::Memory* _mem = nullptr;
	bgfx::TextureFormat::Enum bgfxFormat = _BGFXPixelFormat(format);

	// Handle special format used in GL, convert to rgba8
	uint specialFormatBit = 0u;
	if (format == RHIDefine::PF_L8)
	{
		specialFormatBit = TEXTURE_L8_FORMAT_MASK_BIT;
	}
	else if (format == RHIDefine::PF_L8A8)
	{
		specialFormatBit = TEXTURE_L8A8_FORMAT_MASK_BIT;
	}

	const void* bgfxbuffer = buffer;
	uint bgfxSize = size;
	byte* expandBuffer = NULL;
	if (specialFormatBit && size > 0)
	{
		uint expandSize = _width * _height * 4;
		expandBuffer = static_cast<byte*>(malloc(expandSize));
		ExpandBuffer(expandBuffer, static_cast<const byte*>(buffer), size, specialFormatBit);
		bgfxbuffer = expandBuffer;
		bgfxSize = expandSize;
	}

	if (bgfxbuffer != nullptr && size > 0)
	{
		_mem = bgfx::copy(bgfxbuffer, bgfxSize);
	}

	uint64 bgfxflags = _flags;
	if (isRT)
	{
		bgfxflags = bgfxflags | BGFX_TEXTURE_RT;
	}
	if (isSRGB)
	{
		bgfxflags = bgfxflags | BGFX_TEXTURE_SRGB;
	}
	if (isMsaa)
	{
		bgfxflags = bgfxflags | BGFX_TEXTURE_RT_MSAA_X4;
	}
	bgfxflags |= _BGFXMemUsage2Flags(usage);

	bgfx::TextureHandle textureHandle = bgfx::createTexture2D(_width, _height, _hasMips, _numLayers, bgfxFormat, bgfxflags, _mem, autoGenMipmap, -1, mipNums);

	if (expandBuffer)
	{
		free(expandBuffer);
	}
	return (textureHandle.idx | specialFormatBit);
}

void BgfxRHI::SetTextureName(handle _handle, const std::string& name)
{
	bgfx::TextureHandle textureHandle;
	textureHandle.idx = _handle & HANDLE_MASK;
	bgfx::setName(textureHandle, name.c_str());
}

void BgfxRHI::UpdateTexture2D(handle _handle, unsigned int _layer, unsigned char _mip, unsigned int x, unsigned int y, unsigned int width, unsigned int height, const void* buffer, unsigned int size,
	RHIReleaseFn _releasefunc, void* _releaseData, unsigned short _pitch)
{
	const bgfx::Memory* _mem = nullptr;
	bgfx::TextureHandle textureHandle;
	textureHandle.idx = _handle & HANDLE_MASK;

	uint specialFormatBit = 0u;
	if (_handle & TEXTURE_L8_FORMAT_MASK_BIT)
	{
		specialFormatBit = TEXTURE_L8_FORMAT_MASK_BIT;
	}
	else if (_handle & TEXTURE_L8A8_FORMAT_MASK_BIT)
	{
		specialFormatBit = TEXTURE_L8A8_FORMAT_MASK_BIT;
	}

	bool manageMem = (_releasefunc != RHIReleaseFn(0));
	bgfx::ReleaseFn memRelease = _releasefunc;

	if (buffer != nullptr && size > 0)
	{
		// Valid memory
		const void* bgfxbuffer = buffer;
		uint bgfxSize = size;

		if (specialFormatBit && size > 0)
		{
			uint expandSize = width * height * 4;
			byte* expandBuffer = static_cast<byte*>(malloc(expandSize));
			if (expandBuffer == NULL)
			{
				JYERROR("BgfxTexture2D: alloc expandBuffer failed!");
				return;
			}
			ExpandBuffer(expandBuffer, static_cast<const byte*>(buffer), size, specialFormatBit);
			bgfxbuffer = expandBuffer;
			bgfxSize = expandSize;
			memRelease = [](void* _ptr, void* _userData)
			{
				free((byte*)_ptr);
			};
			if (manageMem)
			{
				_releasefunc(NULL, _releaseData);
				_releasefunc = RHIReleaseFn(0);
				_releaseData = NULL;
			}
		}
		// if has valid mem release function
		if (memRelease)
		{
			_mem = bgfx::makeRef(bgfxbuffer, bgfxSize, memRelease, _releaseData);
		}
		else
		{
			_mem = bgfx::copy(bgfxbuffer, bgfxSize);
			if (nullptr == _mem)
			{
				JYERROR("BgfxTexture2D: bgfx::copy failed, out of memory!");
				return;
			}
		}
	}
	else
	{
		// Invalid passed memory, release them
		if (_releasefunc != RHIReleaseFn(0))
		{
			_releasefunc(NULL, _releaseData);
		}
		JYERROR("BgfxTexture2D: passed-in data is null!");
	}

	// Make sure the _mem is not empty.
	if (nullptr != _mem)
	{
		bgfx::updateTexture2D(textureHandle, _layer, _mip, x, y, width, height, _mem, _pitch);
	}
}

handle BgfxRHI::CreateTextureCube(unsigned short _size, bool _hasMips, unsigned short _numLayers, RHIDefine::PixelFormat format, uint64 _flags /*= 0ULL*/, const void* buffer /*= nullptr*/, unsigned int size /*= 0*/, bool autoGenMipmap /*= false*/, int refhandle /*= -1*/, int mipNums /*= 0*/)
{
	const bgfx::Memory* _mem = nullptr;
	bgfx::TextureFormat::Enum bgfxFormat = _BGFXPixelFormat(format);

	if (buffer != nullptr && size > 0)
	{
		_mem = bgfx::copy(buffer, size);
	}

	bgfx::TextureHandle textureHandle = bgfx::createTextureCube(_size, _hasMips, _numLayers, bgfxFormat, _flags, _mem, autoGenMipmap, -1, mipNums);
	return textureHandle.idx;
}

void BgfxRHI::UpdateTextureCube(handle _handle, unsigned int _layer, RHIDefine::TextureType _side, unsigned char _mip, unsigned int x, unsigned int y, unsigned int width, unsigned int height, 
	const void* buffer, unsigned int size, RHIReleaseFn _releasefunc, void* _releaseData, unsigned short _pitch)
{
	const bgfx::Memory* _mem = nullptr;
	bgfx::TextureHandle textureHandle;
	textureHandle.idx = _handle & HANDLE_MASK;

	bool manageMem = (_releasefunc != RHIReleaseFn(0) && _releaseData != NULL);

	if (buffer != nullptr && size > 0)
	{
		if (manageMem)
		{
			_mem = bgfx::makeRef(buffer, size, _releasefunc, _releaseData);
		}
		else
		{
			_mem = bgfx::copy(buffer, size);
			if (_mem->data == NULL)
			{
				JYERROR("BgfxTextureCube: bgfx::copy failed, out of memory!");
			}
		}
	}
	else
	{
		if (_releasefunc != RHIReleaseFn(0))
		{
			_releasefunc(NULL, _releaseData);
		}
		JYERROR("BgfxTextureCube: passed-in data is null!");
	}

	uint8_t cubeSide = _BGFXCubeSize(_side);
	bgfx::updateTextureCube(textureHandle, _layer, cubeSide, _mip, x, y, width, height, _mem, _pitch);
}

void BgfxRHI::DestoryTexture(handle _handle)
{
	bgfx::TextureHandle textureHandle;
	textureHandle.idx = _handle & HANDLE_MASK;
	bgfx::destroy(textureHandle);
}

void BgfxRHI::ResizeTexture(handle _handle, unsigned short _width, unsigned short _height, unsigned char _numMips, unsigned short _numLayers, bool autoGenMipmap)
{

}

//------------------------------------------------------------------------------------------------------
static void MemCpyA8Buffer(byte* dest, const byte* src, uint cpysize)
{
	uint srcbpp = 1;
	uint dstbpp = 4;
	uint pixelLength = cpysize; // = cpysize / srcbpp
	for (int i = 0; i < pixelLength; ++i)
	{
		uint dstPos = dstbpp * i;
		uint srcPos = srcbpp * i;
		dest[dstPos] = 0;
		dest[dstPos + 1] = 0;
		dest[dstPos + 2] = 0;
		dest[dstPos + 3] = src[srcPos];
	}
}

static void MemCpyL8Buffer(byte* dest, const byte* src, uint cpysize)
{
	uint srcbpp = 1;
	uint dstbpp = 4;
	uint pixelLength = cpysize; // = cpysize / srcbpp
	for (int i = 0; i < pixelLength; ++i)
	{
		uint dstPos = dstbpp * i;
		uint srcPos = srcbpp * i;
		dest[dstPos] = src[srcPos];
		dest[dstPos + 1] = src[srcPos];
		dest[dstPos + 2] = src[srcPos];
		dest[dstPos + 3] = (byte)(255);
	}
}

static void MemCpyL8A8Buffer(byte* dest, const byte* src, uint cpysize)
{
	// currently expand it to (LLLA)
	uint srcbpp = 2;
	uint dstbpp = 4;
	uint pixelLength = cpysize >> 1; // = cpysize / dstbpp
	for (int i = 0; i < pixelLength; ++i)
	{
		uint dstPos = dstbpp * i;
		uint srcPos = srcbpp * i;
		dest[dstPos] = src[srcPos];
		dest[dstPos + 1] = src[srcPos];
		dest[dstPos + 2] = src[srcPos];
		dest[dstPos + 3] = src[srcPos + 1];
	}
}

void ExpandBuffer(byte* dest, const byte* src, uint cpysize, uint _format)
{
	if (src && dest)
	{
		if (_format == TEXTURE_L8_FORMAT_MASK_BIT)
		{
			MemCpyL8Buffer(dest, src, cpysize);
		}
		else if (_format == TEXTURE_L8A8_FORMAT_MASK_BIT)
		{
			MemCpyL8A8Buffer(dest, src, cpysize);
		}
	}
}

NS_JYE_END