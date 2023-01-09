#ifndef __IMAGE_UTILS_H__
#define __IMAGE_UTILS_H__

#include <algorithm>
#include <string>

enum ImageOcclusion
{
	IMAGE_OPAQUE,
	IMAGE_TRANSPARENT
};

struct ImageProperties
{
	int            width;
	int            height;
	ImageOcclusion occlusion;
};

ImageProperties GetImageProperties(char const *filePath);

/**
* Very simple method for mapping filename suffix to mime type. The glTF 2.0 spec only accepts values
* "image/jpeg" and "image/png" so we don't need to get too fancy.
*/
inline std::string suffixToMimeType(std::string suffix) {
	std::transform(suffix.begin(), suffix.end(), suffix.begin(), ::tolower);

	if (suffix == "jpg" || suffix == "jpeg") {
		return "image/jpeg";
	}
	if (suffix == "png") {
		return "image/png";
	}
	return "image/unknown";
}

#endif // !__IMAGE_UTILS_H__#pragma once
