#include "MatTool.h"
#include "Core/Interface/Serialize/ISerializeSystem.h"
#include "Core/Interface/Serialize/ISerializer.h"
#include "Core/Interface/Serialize/IEncoder.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/IFile.h"
#include "Engine/resource/MaterialStreamData.h"
#include "util/MFileUtility.h"
#include "Engine/render/material/MaterialEntity.h"
#include "Engine/resource/MaterialMetadata.h"
#include "Engine/render/material/IMaterialSystem.h"

NS_JYE_BEGIN

static const uint s_mat_vertion = 1;

static const String s_shaderPathName = "shaderPath";
static const String s_shaderPath = "shaderPath";
static const String s_shaderExt = ".shader";

void MatTool::CreateMat(const String& path)
{
	MaterialStreamDataPtr matDat = _NEW MaterialStreamData("comm:/material/3d/unlit"); 
	Stream stream ;
	stream.SetStreamFlag(Stream::AT_REGISTER);
	stream.ArchiveAll(matDat);
	stream.Save(path.c_str());
}

void MatTool::CreateMat(const String& path, const String& shaderpath)
{
	// 引擎能识别的是不带后缀的名字
	String fixpath = shaderpath.substr(0, shaderpath.size() - s_shaderExt.size());
	MaterialStreamDataPtr matDat = _NEW MaterialStreamData(fixpath);
	Stream stream;
	stream.SetStreamFlag(Stream::AT_REGISTER);
	stream.ArchiveAll(matDat);
	stream.Save(path.c_str());
}
	
void MatTool::SaveMat(MaterialEntity* mat)
{
	IMetadata* imeta = *mat->GetSourceMetadata().begin();
	if (imeta->IsSameType(MaterialMetadata::RTTI()))
	{
		String matpath = mat->GetMatPath();
		MaterialStreamDataPtr matDat = _NEW MaterialStreamData(mat->GetShaderPath());
		auto params = mat->GetParameters();
		for (auto& it : params)
		{
			auto slot = IMaterialSystem::Instance()->GetParameterSlot(it.first);
			matDat->SetParameter(slot, it.second);
		}

		Stream stream;
		stream.SetStreamFlag(Stream::AT_REGISTER);
		stream.ArchiveAll(matDat);
		stream.Save(matpath.c_str());
	}
}

NS_JYE_END