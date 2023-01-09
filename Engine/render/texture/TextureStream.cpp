#include "TextureStream.h"
#include "Core/Interface/ILogSystem.h"


NS_JYE_BEGIN

namespace
{
	static void SignleDataCopy(byte* buff, int u, int v, const Math::IntVec2& size, const void* data)
	{
		const float* vd = static_cast<const float*>(data);
		int index = u + v * size.x;
		buff[index] = (byte)*vd;
	}
	
	static void DoubleDataCopy(byte* buff, int u, int v, const Math::IntVec2& size, const void* data)
	{
		const Math::Vec2* vd = static_cast<const Math::Vec2*>(data);
		int index = (u + v * size.x) * 2;
		buff[index] = (byte)vd->x;
		buff[index + 1] = (byte)vd->y;
	}
	
	static void TripleDataCopy(byte* buff, int u, int v, const Math::IntVec2& size, const void* data)
	{
		const Math::Vec3* vd = static_cast<const Math::Vec3*>(data);
		int index = (u + v * size.x) * 3;
		buff[index] = (byte)vd->x;
		buff[index + 1] = (byte)vd->y;
		buff[index + 2] = (byte)vd->z;
	}
	
	static void QuadDataCopy(byte* buff, int u, int v, const Math::IntVec2& size, const void* data)
	{
		const Math::Vec4* vd = static_cast<const Math::Vec4*>(data);
		int index = (u + v * size.x) * 4;
		buff[index] = (byte)vd->x;
		buff[index + 1] = (byte)vd->y;
		buff[index + 2] = (byte)vd->z;
		buff[index + 3] = (byte)vd->w;
	}
	
	static void HexDataCopy(byte* buff, int u, int v, const Math::IntVec2& size, const void* data)
	{
		const Math::Vec4* vd = static_cast<const Math::Vec4*>(data);
		int index = (u + v * size.x) * 4 * 4;
		float* at = (float*)(buff + index);
		*at = vd->x;
		*(at + 1) = vd->y;
		*(at + 2) = vd->z;
		*(at + 3) = vd->w;
	}
}

TextureStream::TextureStream(void)
	:m_ePixelFormat(RHIDefine::PF_AUTO)
	, m_eTextureType(RHIDefine::TEXTURE_2D)
	, m_Levels(0)
	, m_PixelSize(0)
	, m_pFunction(NULL)
	, m_Buffer(nullptr)
	, m_BufferSize(0)
{

}

TextureStream::TextureStream(const TextureStream& ts)
	:TextureStream()
{
	_Copy(ts);
}

TextureStream::TextureStream(TextureStream&& ts)
{
	m_vSize = ts.m_vSize;
	m_pFunction = ts.m_pFunction;
	m_ePixelFormat = ts.m_ePixelFormat;
	m_eTextureType = ts.m_eTextureType;
	m_PixelSize = ts.m_PixelSize;
	m_Levels = ts.m_Levels;
	m_Buffer = ts.m_Buffer;
	m_BufferSize = ts.m_BufferSize;

	ts.m_Buffer = nullptr;
	ts.m_BufferSize = 0;
}

TextureStream& TextureStream::operator=(TextureStream&& ts)
{
	if (this != &ts)
	{
		_FreeBuffer();
		m_vSize = ts.m_vSize;
		m_pFunction = ts.m_pFunction;
		m_ePixelFormat = ts.m_ePixelFormat;
		m_eTextureType = ts.m_eTextureType;
		m_PixelSize = ts.m_PixelSize;
		m_Levels = ts.m_Levels;
		m_Buffer = ts.m_Buffer;
		m_BufferSize = ts.m_BufferSize;

		ts.m_Buffer = nullptr;
		ts.m_BufferSize = 0;
	}
	return *this;
}

TextureStream::~TextureStream(void)
{
	_FreeBuffer();
}

const TextureStream& TextureStream::_Copy(const TextureStream& ts)
{
	_FreeBuffer();
	m_vSize = ts.m_vSize;
	m_pFunction = ts.m_pFunction;
	m_ePixelFormat = ts.m_ePixelFormat;
	m_eTextureType = ts.m_eTextureType;
	m_PixelSize = ts.m_PixelSize;
	m_Levels = ts.m_Levels;
	m_BufferSize = ts.m_BufferSize;
	m_Buffer = _NEW byte[m_BufferSize];
	memcpy(m_Buffer, ts.m_Buffer, m_BufferSize);

	return *this;
}

void TextureStream::_FreeBuffer()
{
	SAFE_DELETE_ARRAY(m_Buffer);
	m_BufferSize = 0;
	//m_Buffer.clear();
	//m_Buffer.shrink_to_fit();
	//m_pFunction = NULL;
}

uint TextureStream::_CaculateBufferSize()
{
	return m_vSize.x * m_vSize.y * m_PixelSize;
}

void TextureStream::_AllocBuffer(bool needMemInit)
{
	uint buffersize = _CaculateBufferSize();
	//m_Buffer.resize(buffersize, 0);
	SAFE_DELETE_ARRAY(m_Buffer);
	m_Buffer = _NEW byte[buffersize];
	m_BufferSize = buffersize;

	if (needMemInit)
	{
		memset(m_Buffer, 0, m_BufferSize);
	}
}

void TextureStream::_ReserverBuffer(bool needMemInit)
{
	uint newsize = _CaculateBufferSize() + sizeof(TextureStream);
	if (newsize != m_BufferSize)
	{
		SAFE_DELETE_ARRAY(m_Buffer);
		m_Buffer = _NEW byte[newsize];
		m_BufferSize = newsize;

		if (needMemInit)
		{
			memset(m_Buffer, 0, m_BufferSize);
		}
	}
}

void TextureStream::_SetFormat(RHIDefine::PixelFormat pf)
{
	m_ePixelFormat = pf;
	switch (m_ePixelFormat)
	{
	case RHIDefine::PF_A8:
	case RHIDefine::PF_L8:
	{
		m_PixelSize = 1;
		m_pFunction = &SignleDataCopy;
		break;
	}
	case RHIDefine::PF_L8A8:
	{
		m_PixelSize = 2;
		m_pFunction = &DoubleDataCopy;
		break;
	}
	case RHIDefine::PF_R5G6B5:
	case RHIDefine::PF_R4G4B4A4:
	{
		m_PixelSize = 2;
		break;
	}
	case RHIDefine::PF_R8G8B8:
	{
		m_PixelSize = 3;
		m_pFunction = &TripleDataCopy;
		break;
	}
	case RHIDefine::PF_R8G8B8A8:
	{
		m_PixelSize = 4;
		m_pFunction = &QuadDataCopy;
		break;
	}
	case RHIDefine::PF_RG11B10FLOAT:
	{
		m_PixelSize = 4;
		break;
	}
	case RHIDefine::PF_RGBAFLOAT:
	{
		m_PixelSize = 16;
		m_pFunction = &HexDataCopy;
		break;
	}
	case RHIDefine::PF_RGBAHALF:
	{
		m_PixelSize = 8;
		break;
	}
	case RHIDefine::PF_YUV420P:
	{
		// 如果使用YUV会分配稍大的 960x540 yuv420=777600 x2=1036800 多30%内存
		m_PixelSize = 2; // 1.5 ?
		break;
	}
	case RHIDefine::PF_DEPTH16:
	{
		m_PixelSize = 2;
		break;
	}
	case RHIDefine::PF_DEPTH32:
	{
		m_PixelSize = 4;
		break;
	}
	case RHIDefine::PF_DEPTH24_STENCIL8:
	{
		m_PixelSize = 4;
		break;
	}
	case RHIDefine::PF_R16_FLOAT:
	{
		m_PixelSize = 2;
		break;
	}
	case RHIDefine::PF_R32_UINT:
	{
		m_PixelSize = 4;
		break;
	}
	default:JYERROR("unknown pixel format");
	}
}

const TextureStream& TextureStream::operator=(const TextureStream& ts)
{
	return _Copy(ts);
}

void TextureStream::Serialize()
{
	if (m_BufferSize > 0)
	{
		byte* dst = &m_Buffer[0] + _CaculateBufferSize();
		memcpy(dst, this, sizeof(TextureStream));
	}
}

void TextureStream::OnSerializeRecycle()
{
	_FreeBuffer();
}

const byte* TextureStream::GetSerializeBuffer() const
{
	return m_Buffer;
}

uint TextureStream::GetSerializeLength() const
{
	return GetBufferSize();
}

bool TextureStream::FromSerializeBuffer(const byte* buff, uint len)
{
	TextureStream* pTargetStream = (TextureStream*)(buff + len - sizeof(TextureStream));
	m_vSize = pTargetStream->m_vSize;
	m_pFunction = pTargetStream->m_pFunction;
	m_ePixelFormat = pTargetStream->m_ePixelFormat;
	m_eTextureType = pTargetStream->m_eTextureType;
	m_PixelSize = pTargetStream->m_PixelSize;
	m_Levels = pTargetStream->m_Levels;

	int size = _CaculateBufferSize();
	SAFE_DELETE_ARRAY(m_Buffer);
	m_BufferSize = size + sizeof(TextureStream);
	m_Buffer = _NEW byte[m_BufferSize];
	memcpy(m_Buffer, buff, size);

	return true;
}

void TextureStream::SetStreamType(const Math::IntVec2& size, RHIDefine::PixelFormat pf, bool allocbuffer, bool needMemInit)
{
	m_vSize = size;
	_SetFormat(pf);
	if (allocbuffer)
	{
		_ReserverBuffer(needMemInit);
	}
	else
	{
		_FreeBuffer();
		_AllocBuffer(needMemInit);
	}
}

void TextureStream::Resize(const Math::IntVec2& size)
{
	m_vSize = size;
	_FreeBuffer();
	_AllocBuffer();
}

void TextureStream::SetPixel(int u, int v, const void* data) {
	if (u < GetSize().x
		&& v < GetSize().y
		&& m_pFunction)
	{
		m_pFunction(m_Buffer, u, v, GetSize(), data);
	}
	else
	{
		JYERROR("texture stream set pixel out of range or texture stream NOT support SetPixel %d", m_ePixelFormat);
	}
}

void TextureStream::SetBuffer(const byte* buffer)
{
	if (buffer)
	{
		if (m_ePixelFormat == RHIDefine::PixelFormat::PF_YUV420P)
		{
			memcpy(m_Buffer, buffer, (uint)((m_BufferSize - sizeof(TextureStream)) * 0.75));
		}
		else
		{
			memcpy(m_Buffer, buffer, m_BufferSize - sizeof(TextureStream));
		}

	}
	else
	{
		JYERROR("try copy null buffer");
	}
}

void TextureStream::Merge(const TextureStream& ts)
{
	SetStreamType(ts.GetSize(), ts.GetPixelFormat());
	m_BufferSize = ts.m_BufferSize;
	SAFE_DELETE_ARRAY(m_Buffer);
	m_Buffer = _NEW byte[m_BufferSize];
	memcpy(m_Buffer, ts.m_Buffer, m_BufferSize);
}

bool TextureStream::isCompressed()
{
	switch (m_ePixelFormat)
	{
	case RHIDefine::PF_A8:
	case RHIDefine::PF_L8:
	case RHIDefine::PF_L8A8:
	case RHIDefine::PF_R8G8B8:
	case RHIDefine::PF_R5G6B5:
	case RHIDefine::PF_R8G8B8A8:
	case RHIDefine::PF_R4G4B4A4:
	case RHIDefine::PF_AUTO:
	case RHIDefine::PF_DEPTH16:
	case RHIDefine::PF_DEPTH32:
	case RHIDefine::PF_DEPTH24_STENCIL8:
	case RHIDefine::PF_RGBAFLOAT:
	case RHIDefine::PF_RGBAHALF:
	case RHIDefine::PF_RG11B10FLOAT:

		return false;
	case RHIDefine::PF_YUV420P:
		return true;
	}
	JYERROR("unkonw pixel format");
	return true;
}

void TextureStream::ConvertPixelFormat(RHIDefine::PixelFormat pf)
{
	JYERROR("pixel format converter is not support yet");
}

Math::Vec4 TextureStream::ReadPixel(int u, int v)
{
	int index = (u + v * m_vSize.x);
	byte* tmpBuf = m_Buffer + index * m_PixelSize;
	unsigned char* texBuf = (unsigned char*)tmpBuf;
	Math::Vec4 color;
	color.x = (float)*texBuf / 255.0f;
	texBuf++;
	if (m_PixelSize > 1)
	{
		color.y = (float)*texBuf / 255.0f;
		texBuf++;
		color.z = (float)*texBuf / 255.0f;
		texBuf++;
		if (m_PixelSize > 3)
		{
			color.w = (float)*texBuf / 255.0f;
			texBuf++;
		}

	}
	return color;
}

NS_JYE_END