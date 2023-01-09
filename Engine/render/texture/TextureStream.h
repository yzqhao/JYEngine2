#pragma once

#include "Engine/private/Define.h"
#include "RHI/RHIDefine.h"
#include "Math/IntVec2.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

NS_JYE_BEGIN

class ENGINE_API TextureStream 
{
private:
	typedef void(*TextureCompressFunction)(byte* buff, int u, int v, const Math::IntVec2& size, const void* data);
private:
	typedef std::vector<byte> Buffer;
private:
	Math::IntVec2 m_vSize;
	byte* m_Buffer;
	uint m_BufferSize;
	TextureCompressFunction m_pFunction;
	RHIDefine::PixelFormat m_ePixelFormat;
	RHIDefine::TextureType m_eTextureType;
	uint m_Levels;
	uint m_PixelSize;
public:
	TextureStream(void);
	TextureStream(const TextureStream& ts);
	TextureStream(TextureStream&& ts);
	~TextureStream(void);

	TextureStream& operator=(TextureStream&& ts);
private:
	void _FreeBuffer();
	void _ReserverBuffer(bool needMemInit = true);
	uint _CaculateBufferSize();
	void _SetFormat(RHIDefine::PixelFormat pf);
	const TextureStream& _Copy(const TextureStream& ts);
	void _AllocBuffer(bool needMemInit = true);
public:
	void Serialize();
	virtual void OnSerializeRecycle();
	virtual const byte* GetSerializeBuffer() const;
	virtual uint GetSerializeLength() const;
	virtual bool FromSerializeBuffer(const byte* buff, uint len);
public:
	const TextureStream& operator= (const TextureStream& ts);
	void SetStreamType(const Math::IntVec2& size, RHIDefine::PixelFormat pf, bool allocbuffer = true, bool needMemInit = true);
	void ConvertPixelFormat(RHIDefine::PixelFormat pf);
	void SetPixel(int u, int v, const void* data);
	void SetBuffer(const byte* buffer);
	void Resize(const Math::IntVec2& size);
	void Merge(const TextureStream& ts);
	bool isCompressed();
	Math::Vec4 ReadPixel(int u, int v);
private:
	FORCEINLINE uint _CaculatePosition(int x, int y) const;
public:
	FORCEINLINE void SetLevel(uint level);
	FORCEINLINE void SetTextureType(RHIDefine::TextureType tt);
	FORCEINLINE const Math::IntVec2& GetSize() const;
	FORCEINLINE RHIDefine::PixelFormat GetPixelFormat() const;
	FORCEINLINE RHIDefine::TextureType GetTextureType() const;
	FORCEINLINE byte* GetBuffer();
	FORCEINLINE byte* GetBuffer(int x, int y);
	FORCEINLINE const byte* GetBuffer() const;
	FORCEINLINE const byte* GetBuffer(int x, int y) const;
	FORCEINLINE uint GetPixelSize() const;
	FORCEINLINE uint GetBufferSize() const;
	FORCEINLINE uint GetLevels() const;
	FORCEINLINE void ClearBuffer();
};

FORCEINLINE uint TextureStream::_CaculatePosition(int x, int y) const
{
	if (RHIDefine::PF_YUV420P == m_ePixelFormat)
	{
		return (y * GetSize().x + x) * m_PixelSize * 3 / 2;
	}
	return  (y * GetSize().x + x) * m_PixelSize;
}

FORCEINLINE void TextureStream::SetLevel(uint level)
{
	m_Levels = level;
}

FORCEINLINE void TextureStream::SetTextureType(RHIDefine::TextureType tt)
{
	m_eTextureType = tt;
}

FORCEINLINE const Math::IntVec2& TextureStream::GetSize() const
{
	return m_vSize;
}

FORCEINLINE RHIDefine::PixelFormat TextureStream::GetPixelFormat() const
{
	return m_ePixelFormat;
}

FORCEINLINE RHIDefine::TextureType TextureStream::GetTextureType() const
{
	return m_eTextureType;
}

FORCEINLINE byte* TextureStream::GetBuffer()
{
	return m_Buffer;
}

FORCEINLINE byte* TextureStream::GetBuffer(int x, int y)
{
	return m_Buffer + _CaculatePosition(x, y);
}

FORCEINLINE const byte* TextureStream::GetBuffer() const
{
	return m_Buffer;
}

FORCEINLINE const byte* TextureStream::GetBuffer(int x, int y) const
{
	return m_Buffer + _CaculatePosition(x, y);
}

FORCEINLINE uint TextureStream::GetPixelSize() const
{
	return m_PixelSize;
}

FORCEINLINE uint TextureStream::GetBufferSize() const
{
	return m_BufferSize;
}

FORCEINLINE uint TextureStream::GetLevels() const
{
	return m_Levels;
}

FORCEINLINE void TextureStream::ClearBuffer()
{
	memset(m_Buffer, 0, m_BufferSize);
}

typedef Vector<TextureStream*>	TextureStreams;

NS_JYE_END