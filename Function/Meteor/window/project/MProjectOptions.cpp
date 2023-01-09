#include "MProjectOptions.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/IFile.h"
#include "Core/Interface/ILogSystem.h"
#include "Core/Interface/Serialize/IDecoder.h"
#include "Core/Interface/Serialize/ISerializeSystem.h"
#include "Core/Interface/Serialize/ISerializer.h"
#include "Core/Interface/Serialize/IEncoder.h"

NS_JYE_BEGIN

const static int s_vertion = 1;

MProjectOptions::MProjectOptions()
{

}

MProjectOptions::~MProjectOptions()
{

}

void MProjectOptions::Deserialize()
{
	String path = "root:config.dat";
	if (!IFileSystem::Instance()->isFileExist(path))
	{
		//JYERROR("MProjectOptions_Serialize file %s is not exist", path.c_str());
		return;
	}
	IFile* readfile = IFileSystem::Instance()->FileFactory(path);
	readfile->OpenFile(IFile::AT_READ);
	byte* buffer = _NEW byte[readfile->GetSize()];
	readfile->ReadFile(buffer, readfile->GetSize());
	readfile->CloseFile();
	IFileSystem::Instance()->RecycleBin(readfile);

	ISerializer* ser = ISerializeSystem::Instance()->SerializerFactory();
	IDecoder* originDecoder = ser->GetDecoderFromBuffer(buffer);
	int version = originDecoder->AsInt(0);
	const IDecoder* decoder = originDecoder->AsClass(1);
	int length = decoder->GetSerializeLength();
	uint strlen = 0;
	m_ProjectPath = decoder->AsString(0, &strlen);
	ISerializeSystem::Instance()->RecycleBin(ser);
}

void MProjectOptions::Serialize(const String& projPath)
{
	m_ProjectPath = projPath;

	ISerializer* ser = ISerializeSystem::Instance()->SerializerFactory();
	IEncoder* encoder = ser->GetEncoder();
	IEncoder* childencoder = encoder->GetChild();
	childencoder->SetField(0, m_ProjectPath.c_str(), m_ProjectPath.length());
	encoder->SetField(0, s_vertion);
	encoder->SetField(1, childencoder);

	String path = "root:config.dat";
	IFile* readfile = IFileSystem::Instance()->FileFactory(path);
	readfile->OpenFile(IFile::AT_WRITE);
	readfile->WriteFile(ser->GetBuffer(), ser->GetBufferSize());
	readfile->CloseFile();
	IFileSystem::Instance()->RecycleBin(readfile);

	ISerializeSystem::Instance()->RecycleBin(ser);
}

NS_JYE_END