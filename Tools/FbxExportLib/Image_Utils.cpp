#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB/stb_image_write.h"

#include "Image_Utils.h"

static bool imageHasTransparentPixels(FILE *f) {
	int width, height, channels;
	// RGBA: we have to load the pixels to figure out if the image is fully opaque
	uint8_t *pixels = stbi_load_from_file(f, &width, &height, &channels, 0);
	if (pixels != nullptr) {
		int pixelCount = width * height;
		for (int ix = 0; ix < pixelCount; ix++) {
			// test fourth byte (alpha); 255 is 1.0
			if (pixels[4 * ix + 3] != 255) {
				return true;
			}
		}
	}
	return false;
}

ImageProperties GetImageProperties(char const *filePath)
{
	ImageProperties result = {
		1,
		1,
		IMAGE_OPAQUE,
	};

	FILE *f = fopen(filePath, "rb");
	if (f == nullptr) {
		return result;
	}

	int channels;
	int success = stbi_info_from_file(f, &result.width, &result.height, &channels);

	if (success && channels == 4 && imageHasTransparentPixels(f)) {
		result.occlusion = IMAGE_TRANSPARENT;
	}
	return result;
}