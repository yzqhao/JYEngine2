

#ifndef __FBX2RAW_H__
#define __FBX2RAW_H__

#include "RawModel.h"
struct AnimExportOption
{
	float PositionError;
	float ScaleError;
	float RotationError;
	bool  UseMatrixKeyFrame;
	bool  KeepIndex;
	bool  WriteObj;
};

bool LoadFBXFile(RawModel &raw, const char *fbxFileName, const char* textureSearchPath,const char *textureExtensions,const AnimExportOption& option);

#endif // !__FBX2RAW_H__
