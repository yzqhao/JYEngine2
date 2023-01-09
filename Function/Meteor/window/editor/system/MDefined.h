
#pragma once

#include "private/Define.h"
#include "imgui.h"
#include "Engine/component/Animatable.h"
#include "RHI/RHIDefine.h"

NS_JYE_BEGIN

namespace MDefined
{
	static const String EditorSceneName = "Scene";

	static const String ProjectPath = "proj:";
	static const Vector<String> Internal = { "eddc:internal_fodder" };
	static const String ShaderPath = "comm:material/";

	extern Map<String, String> SystemDirectories;
	static const Map<String, String> FirstDirectories = {
		{ "scene_asset", "assets/" },
		{ "assets", "assets/" },
		{ "library", "library/" },
		{ "temp", "temp/" },
	};
	static const Map<String, String> SecondDirectories = {
		{ "plugin", "assets/plugin/" },
		{ "script", "assets/script/" },
		{ "resource", "assets/resource/" },
	};

	static const Map<Animatable::EditorUIType, String> mapEditorUIType = {
		{ Animatable::VISUAL, "VISUAL" },
		{ Animatable::UNVISUAL, "UNVISUAL" },
	};

	static const Map<String, String> Icons =
	{
		{ "Translate", "comm:documents/icons/translate.png" },
		{ "Rotation", "comm:documents/icons/rotate.png" },
		{ "Scale", "comm:documents/icons/scale.png" },
		{ "Play", "comm:documents/icons/play.png" },
		{ "Pause", "comm:documents/icons/pause.png" },
		{ "Stop", "comm:documents/icons/stop.png" },
		{ "Grid", "comm:documents/icons/grid.png" },
		//{ "WireFrame", "comm:documents/icons/wireframe.png" },
		{ "Hierarchy_View", "comm:documents/icons/hierarchy_view.png" },
		{ "MaterialDefault", "comm:documents/icons/default_material.png" },
		{ "ChangePic", "comm:documents/icons/add_pic.png" },
	};

	static const Map<String, RHIDefine::TextureType> TextureTypeMap =
	{
		{ "TT_TEXTURE1D", RHIDefine::TEXTURE_1D } ,
		{ "TT_TEXTURE2D", RHIDefine::TEXTURE_2D } ,
		{ "TT_TEXTURE3D", RHIDefine::TEXTURE_3D } ,
		{ "TT_TEXTURECUBE", RHIDefine::TEXTURE_CUBE_MAP } ,
		{ "TT_TEXTURECUBE_FRONT", RHIDefine::TT_TEXTURECUBE_FRONT } ,
		{ "TT_TEXTURECUBE_BACK", RHIDefine::TT_TEXTURECUBE_BACK } ,
		{ "TT_TEXTURECUBE_TOP", RHIDefine::TT_TEXTURECUBE_TOP } ,
		{ "TT_TEXTURECUBE_BOTTOM", RHIDefine::TT_TEXTURECUBE_BOTTOM } ,
		{ "TT_TEXTURECUBE_LEFT", RHIDefine::TT_TEXTURECUBE_LEFT } ,
		{ "TT_TEXTURECUBE_RIGHT", RHIDefine::TT_TEXTURECUBE_RIGHT } ,
	};
	static const Vector<String> TextureWrapName =
	{
		"TW_REPEAT",
		"TW_CLAMP_TO_EDGE",
		"TW_MIRRORED_REPEAT",
		"TW_CLAMP_TO_BORDER",
	};
	static const Vector<String> TextureFilterName =
	{
		 "TF_NEAREST",
		 "TF_LINEAR",
		 "TF_NEAREST_MIPMAP_NEAREST",
		 "TF_LINEAR_MIPMAP_NEAREST",
		 "TF_NEAREST_MIPMAP_LINEAR",
		 "TF_LINEAR_MIPMAP_LINEAR",
	};
	static const Vector<RHIDefine::TextureWarp> TextureWrap =
	{
		  RHIDefine::TW_REPEAT,
		  RHIDefine::TW_CLAMP_TO_EDGE,
		  RHIDefine::TW_MIRRORED_REPEAT,
		  RHIDefine::TW_CLAMP_TO_BORDER,
	};
	static const Vector<RHIDefine::TextureFilter> TextureFilter =
	{
		 RHIDefine::TF_NEAREST,
		 RHIDefine::TF_LINEAR,
		 RHIDefine::TF_NEAREST_MIPMAP_NEAREST,
		 RHIDefine::TF_LINEAR_MIPMAP_NEAREST,
		 RHIDefine::TF_NEAREST_MIPMAP_LINEAR,
		 RHIDefine::TF_LINEAR_MIPMAP_LINEAR,
	};
	static const Vector<RHIDefine::TextureUseage> TextureUseage =
	{
		RHIDefine::TU_STATIC,
		RHIDefine::TU_READ,
		RHIDefine::TU_WRITE,
	};
	static const Vector<String> TextureUseageName =
	{
		"TU_STATIC",
		"TU_READ",
		"TU_WRITE",
	};

	enum MFileType
	{
		Fbx = 0,
		Fbo,
		Hdr,
		Mesh,
		DynamicMesh,
		Shader,
		Prefab,
		Mat,
		FrameAnimation,
		Animation,
		Metadata,
		Texture,
	};
	static const Vector<Vector<String>> FileTypeList =
	{
		{ "fbx" },
		{ "fbo" },
		{ "hdr" },
		{ "mesh" },
		{ "dynamicmesh" },
		{ "shader" },
		{ "prefab" },
		{ "mat" },
		{ "frameani" },
		{ "ani" },
		{ "meta" },
		{"jpg", "png", "fbo", "bmp", "tga"},
	};

	static const uint TreeNodeFlag = ImGuiTreeNodeFlags_AllowItemOverlap + ImGuiTreeNodeFlags_OpenOnArrow + ImGuiTreeNodeFlags_SpanAllAvailWidth;

	static const float MouseDragThreshold = 1;
	static const float MouseSpeed = 0.05f;
	static const float WheelSpeed = 0.1f;
	static const float KeyBoardSpeed = 0.01f;
	static const float KeyBoardPressRate = 0.01f;
	static const float KeyBoardPressDelay = 0.01f;

	enum Mouse
	{
		Left = 0,
		Right = 1,
		Mid = 2,
	};

	enum EditorStatus
	{
		STOP = 0,
		PLAY = 1,
		PAUSE = 2,
	};
}

NS_JYE_END