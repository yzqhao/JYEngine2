#pragma once

#include "Engine/private/Define.h"
#include "RHI/RHIDefine.h"

NS_JYE_BEGIN

namespace TextureFormat
{
	static RHIDefine::PixelFormat FormatMapping(uint channel)
	{
		switch (channel)
		{
		case 1: return RHIDefine::PF_L8;
		case 2: return RHIDefine::PF_L8A8;
		case 3: return RHIDefine::PF_R8G8B8;
		case 4: return RHIDefine::PF_R8G8B8A8;
		}
		JY_ASSERT("TextureProperty: Unknown channel!");
		return RHIDefine::PF_L8;
	}

	static unsigned int GetMemSize(int width, int height, int depth, RHIDefine::PixelFormat pf)
	{
		switch (pf)
		{
		case RHIDefine::PF_A8:
		case RHIDefine::PF_L8:
			return width * height;
		case RHIDefine::PF_L8A8:
			return width * height * 2;
		case RHIDefine::PF_R8G8B8:
			return width * height * 3;
		case RHIDefine::PF_DEPTH16:
		case RHIDefine::PF_R5G6B5:
		case RHIDefine::PF_R4G4B4A4:
		case RHIDefine::PF_R16_FLOAT:
			return width * height * 2;
		case RHIDefine::PF_DEPTH32:
		case RHIDefine::PF_R8G8B8A8:
		case RHIDefine::PF_DEPTH24_STENCIL8:
		case RHIDefine::PF_RG11B10FLOAT:
		case RHIDefine::PF_R32_FLOAT:
			return width * height * 4;
		case RHIDefine::PF_RGBAFLOAT:
			return width * height * 16;
		case RHIDefine::PF_RGBAHALF:
			return width * height * 8;
			return (Math::Max<int>((int)width, 8) * Math::Max<int>((int)height, 8) * 4 + 7) / 8;
		default:
			//JYERROR("Unknown pixel format!");
			return width * height;
		}
	}
}

NS_JYE_END