#include "FboTool.h"
#include "Core/Interface/Serialize/ISerializeSystem.h"
#include "Core/Interface/Serialize/ISerializer.h"
#include "Core/Interface/Serialize/IEncoder.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/IFile.h"
#include "util/MFileUtility.h"

NS_JYE_BEGIN

static const uint s_fbo_vertion = 1;

static const String s_TargetType = "TargetType";
static const String s_SwapTarget = "SwapTarget";
static const String s_Scale = "Scale";
static const String s_Size = "Size";
static const String s_Srgb = "Srgb";
static const String s_Attachment = "Attachment";
static const String s_TextureUseage = "TextureUseage";
static const String s_PixelFormat = "PixelFormat";
static const String s_TextureWarp = "TextureWarp";
static const String s_TextureFilter = "TextureFilter";

void FboTool::CreateFbo(const String& path)
{
	ISerializer* ser = ISerializeSystem::Instance()->SerializerFactory();
	IEncoder* encoder = ser->GetEncoder();
	encoder->SetField(0, s_fbo_vertion);
	encoder->SetField(s_TargetType.c_str(), s_TargetType.size(), 1);
	encoder->SetField(s_SwapTarget.c_str(), s_SwapTarget.size(), -1);
	IEncoder* scaleEncoder = encoder->GetChild();
	scaleEncoder->SetField(0, 1);
	scaleEncoder->SetField(1, 1);
	encoder->SetField(s_Scale.c_str(), s_Scale.size(), scaleEncoder);
	IEncoder* sizeEncoder = encoder->GetChild();
	sizeEncoder->SetField(0, 16);
	sizeEncoder->SetField(1, 16);
	encoder->SetField(s_Size.c_str(), s_Size.size(), sizeEncoder);
	encoder->SetField(s_Srgb.c_str(), s_Srgb.size(), false);

	uint pixformats[2] = { 6, 10 };
	int atts[2] = { 0, 11 };	// color0 depty_stencil
	for (int i = 0; i < 2; ++i)
	{
		IEncoder* attEncoder = encoder->GetChild();
		attEncoder->SetField(s_TargetType.c_str(), s_TargetType.size(), 2);
		attEncoder->SetField(s_TextureUseage.c_str(), s_TextureUseage.size(), 0);
		attEncoder->SetField(s_PixelFormat.c_str(), s_PixelFormat.size(), pixformats[i]);
		IEncoder* wrapEncoder = encoder->GetChild();
		wrapEncoder->SetField(0, 1);
		wrapEncoder->SetField(1, 1);
		attEncoder->SetField(s_TextureWarp.c_str(), s_TextureWarp.size(), wrapEncoder);
		IEncoder* filterEncoder = encoder->GetChild();
		filterEncoder->SetField(0, 1);
		filterEncoder->SetField(1, 1);
		attEncoder->SetField(s_TextureFilter.c_str(), s_TextureFilter.size(), filterEncoder);

		encoder->SetField(atts[i], attEncoder);
	}

	IFile* readfile = IFileSystem::Instance()->FileFactory(path);
	readfile->OpenFile(IFile::AT_WRITE);
	readfile->WriteFile(ser->GetBuffer(), ser->GetBufferSize());
	readfile->CloseFile();
	IFileSystem::Instance()->RecycleBin(readfile);

	ISerializeSystem::Instance()->RecycleBin(ser);

	MFileUtility::AppendToFileTree(path);
}

NS_JYE_END