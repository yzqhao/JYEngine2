#pragma once

#include "Engine/private/Define.h"
#include "Math/3DMath.h"

NS_JYE_BEGIN

namespace Configure
{
	//配置表相关
#ifdef _PLATFORM_WINDOWS
	static const char* CONFIG_FILE_PATH("root:venus_win.json");
#else
	static const char* CONFIG_FILE_PATH("root:venus_linux.json");
#endif
	static const char* WORK_THREAD_COUNT("Workthreadcount");
	static const char* COMMON_PATH("Commonpath");
	static const char* DOCUMENT_PATH("Documentspath");
	static const char* SCRIPT_PATH("Scriptpath");
#ifdef _EDITOR
	static const char* EDITOR_SCRIPT("Editorscriptpath");
	static const char* EDITOR_DOCUMENT_PATH("Editordocumentpath");
	static const char* EDITOR_SCRIPT_MAIN("Scriptmain");
#endif
	static const char* SCRIPT_MAIN("Scriptmain");
	static const char* LOG_FILE_NAME("Logfile");
	static const char* LOG_FILE_SIZE("Logsize");

	static const char* DEFAULT_SCRIPT_KEY("Default");

	//脚本相关
	static const char* INITALIZE_FUNCTION("Initialize");

	static const char* DEFAULT_SCENE_ROOT_NAME("Scene Root");

	static const char* ImageCopyMaterial("comm:/material/2d/imagecopy");

	//imgui new font resource
	static const char* ARIALUNI_FONT("eddc:fonts/arialuni.ttf");

	//方向
	static const Math::Vec3 Vector3_DefaultForward(0, 0, 1);
	static const Math::Vec3 Vector3_DefaultUp(0, 1, 0);
	static const Math::Vec3 Vector3_DefaultTangent(1, 0, 0);

	//压缩文件相关
	static const char* shaderCodeSuffix = ".code";
	static const char* archiveSuffix = ".pkg";
	static const char* compressionSuffix = ".lz4";
	static const char* dictCompressionSuffix = ".lzd";
	static const int compSuffixLen = 4;
	static const char* DICTIONARY_PATH("comm:dictionary/dictionary_89.dic");
	static const int AIresource = -1000;

	// shader 宏定义
	static const char* lightKeyWords[] =
	{
		"NoLight",
		"DirLight",
		"PointLight",
		"SpotLight",
	};
	static const char* gpuskinKeyWord = "GPUSKIN4";
	static const char* shadowOnKeyWord = "ShadowOn";
	static const char* shadowHardwareKeyWord = "SHADOWS_HARDWARE";
	static const char* shadowOffKeyWord = "ShadowOff";
	static const char* softShadowType[] =
	{
		"SHADOW_ATTEN_PCF3x3",
		"SHADOW_ATTEN_PCF5x5",
		"SHADOW_ATTEN_PCF7x7",
		"SHADOW_ATTEN_NOHARDWARE",
	};
};

NS_JYE_END