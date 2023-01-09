
#pragma once

#include "private/Define.h"
#include "Core/Interface/ILoader.h"
#include "Engine/render/texture/TextureStream.h"

NS_JYE_BEGIN

class RESOURCE_SYSTEM_API TextureLoader : public ILoader
{
public:
	TextureLoader(const String& path) : ILoader(ResourceProperty::SLT_TEXTURE, path) {}
	~TextureLoader(void) {}

	virtual void Release(void* data) override;
	virtual ILoader* CreateCacheLoader() const override;

private:
	enum TEXTURE_LIMIT
	{
		LIMIT_SIZE = 2048
	};
	virtual void* _DoLoad(const String& paths);
private:
	TextureStreams* _LoadTga(std::string filename);
	TextureStreams* _LoadPng(std::string filename);
	TextureStreams* _LoadBmp(std::string filename);
	TextureStreams* _LoadJpeg(std::string filename);
	TextureStreams* _Error();

	enum JPEGOrientation
	{
		/*
		0th Row      0th Column
		1   top          left side
		2   top          right side   左右镜像
		3   bottom       right side
		4   bottom       left side
		5   left side    top
		6   right side   top
		7   right side   bottom
		8   left side    bottom    90度CW  顺时针旋转了90度
		*/
		DEFAULT = 0,
		TOP_LEFT = 1,  // 0°
		TOP_RIGHT = 2,  // 0° 跟 TOP_LEFT 区别就是左右镜像了
		BOTTOM_RIGHT = 3,  // 180° 
		BOTTOM_LEFT = 4,  // 180° 顺时针旋转180度 并且再左右镜像
		LEFT_TOP = 5,  // 90°  CW  顺时针转了90度 再左右镜像
		RIGHT_TOP = 6,  // 90°  CCW
		RIGHT_BOTTOM = 7,  // 90°  CCW
		LEFT_BOTTOM = 8,  // 90°  CW  顺时针转了90度  
	};
	enum JPEGOrientation _GetOrientation(const std::string& filename);
	bool _IsJpgFile(const std::string& filename);
};

NS_JYE_END
