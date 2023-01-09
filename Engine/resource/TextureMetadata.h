#pragma once

#include "GeneralMetadata.h"
#include "Math/IntVec2.h"
#include "RHI/RHI.h"
#include "Engine/render/texture/TextureStream.h"

NS_JYE_BEGIN

class ILoader;

class ENGINE_API TextureDescribeData : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	Math::IntVec2 m_Size;
	RHIDefine::TextureType m_eType;
	RHIDefine::TextureUseage m_eUsage;
	RHIDefine::PixelFormat m_ePixelFormat;
	RHIDefine::TextureWarp m_eSWarp;
	RHIDefine::TextureWarp m_eTWarp;
	RHIDefine::TextureSamplerCompare m_SamplerCompare;
	RHIDefine::TextureFilter m_eMagFilter;
	RHIDefine::TextureFilter m_eMinFilter;
	uint m_uAnisotropic;
	uint m_uMipMapLevel;
	bool m_isMipMap;
	bool m_isMsaa;
	bool m_sRGB;
	bool m_isKeepSource;
public:
	TextureDescribeData();
	TextureDescribeData(
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint x, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min);
	TextureDescribeData(
		const Math::IntVec2& size,
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint x, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min);
	TextureDescribeData(
		const Math::IntVec2& size,
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint x, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min, bool sRGB);
	TextureDescribeData(
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint x, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min, bool is_sRGB);
};
DECLARE_Ptr(TextureDescribeData);
TYPE_MARCO(TextureDescribeData);

class ENGINE_API TextureMetadata : public PathMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	TextureDescribeDataPtr m_texData;
	TextureStreams* m_pTextureStreams;
	std::string m_Name;
public:
	TextureMetadata();
	TextureMetadata(
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint x, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min);
	TextureMetadata(
		const Math::IntVec2& size,
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint x, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min);
	TextureMetadata(
		const Math::IntVec2& size,
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint x, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min, bool sRGB);
	TextureMetadata(
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint x, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min, bool is_sRGB);
private:
	virtual void* _ReturnMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;//��str�������ʾ����
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash) override;//����ԭʼ��Դ
	virtual void ReleaseMetadate();
};
DECLARE_Ptr(TextureMetadata);
TYPE_MARCO(TextureMetadata);

class ENGINE_API TextureFileMetadata : public TextureMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	ILoader* m_pILoader;
	bool m_enbaleCache;
public:
	TextureFileMetadata();
	TextureFileMetadata(
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint x, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
		const std::string& path);
	TextureFileMetadata(
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint x, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
		const std::string& path, bool cache, bool isSRGB);
	virtual ~TextureFileMetadata();
private:
	virtual void _Process(byte channel, const Math::IntVec2& size, byte* buff);
public:
	virtual void GetIdentifier(std::string& hash) const;//��str�������ʾ����
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash) override;//����ԭʼ��Դ
	virtual void ReleaseMetadate();
};

//��Ԫ����ͨ��һ��Json�����ļ�������������
class ENGINE_API TextureDescribeFileMetadata : public TextureMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	ILoader* m_pTextureLoader;
	ILoader* m_pMetadateLoader;
	TextureDescribeData* m_data;
public:
	TextureDescribeFileMetadata();
	TextureDescribeFileMetadata(const String& path);
private:
	virtual void* _ReturnMetadata();
	void _Process(byte channel, const Math::IntVec2& size, byte* buff);
public:
	virtual void GetIdentifier(String& hash) const;//��str�������ʾ����
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//����ԭʼ��Դ
	virtual void ReleaseMetadate();
};

class ENGINE_API TextureRenderMetadata : public TextureMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	TextureRenderMetadata() {}
	TextureRenderMetadata(
		const Math::IntVec2& size, RHIDefine::TextureType type,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint anisotropic,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
		bool isMsaa = false, bool isSRGB = false);
	TextureRenderMetadata(
		const std::string& name,
		const Math::IntVec2& size, RHIDefine::TextureType type,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint anisotropic,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
		bool isMsaa = false, bool isSRGB = false);

	TextureRenderMetadata(
		const Math::IntVec2& size,
		RHIDefine::TextureSamplerCompare compare,
		RHIDefine::TextureType type,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint anisotropic,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
		bool isMsaa = false, bool isSRGB = false);

	virtual ~TextureRenderMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;//��str�������ʾ����
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//����ԭʼ��Դ
	const Math::IntVec2& GetSize();
public:
	void Resize(const Math::IntVec2& size);
};

class ENGINE_API DepthRenderBufferMetadata : public TextureMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	DepthRenderBufferMetadata();
	DepthRenderBufferMetadata(
		const Math::IntVec2& size,
		RHIDefine::PixelFormat pf,
		RHIDefine::TextureWarp s = RHIDefine::TextureWarp::TW_REPEAT,
		RHIDefine::TextureWarp t = RHIDefine::TextureWarp::TW_REPEAT,
		RHIDefine::TextureFilter mag = RHIDefine::TextureFilter::TF_LINEAR,
		RHIDefine::TextureFilter min = RHIDefine::TextureFilter::TF_LINEAR_MIPMAP_LINEAR);
	virtual ~DepthRenderBufferMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;//��str�������ʾ����
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//����ԭʼ��Դ
};

class TextureStream;
//����������������
class ENGINE_API TextureBufferMetadata : public TextureMetadata
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	byte* m_rpBuffer;
public:
	TextureBufferMetadata();
	TextureBufferMetadata(const Math::IntVec2& size);
	TextureBufferMetadata(
		const Math::IntVec2& size,
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint anisotropic, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
		byte* buf);//���ݿ���Ϊ�գ�����������һ�Ű�ɫ����
	TextureBufferMetadata(
		const Math::IntVec2& size,
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint anisotropic, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
		byte* buf, bool is_sRGB);
	TextureBufferMetadata(
		const Math::IntVec2& size,
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint anisotropic, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
		TextureStream* ts);//���ݿ���Ϊ�գ�����������һ�Ű�ɫ����
	TextureBufferMetadata(
		const Math::IntVec2& size,
		RHIDefine::TextureType tt,
		RHIDefine::TextureUseage mu, RHIDefine::PixelFormat pf,
		uint anisotropic, bool mip, uint level,
		RHIDefine::TextureWarp s, RHIDefine::TextureWarp t,
		RHIDefine::TextureFilter mag, RHIDefine::TextureFilter min,
		TextureStream* ts, bool is_sRGB);
	virtual ~TextureBufferMetadata();
public:
	virtual void GetIdentifier(std::string& hash) const;//��str�������ʾ����
	virtual void ProcessMetadata(ResourceProperty::ResourceType rt, size_t source_hash);//����ԭʼ��Դ
};

NS_JYE_END