#include "MeshLoader.h"
#include "Core/Interface/ILogSystem.h"
#include "Core/Interface/IFileSystem.h"
#include "Core/Interface/IFile.h"
#include "BundleReader.h"
#include "Engine/GraphicDefine.h"
#include "Math/3DMath.h"
#include "MeshCache.h"

#include "draco/compression/decode.h"

NS_JYE_BEGIN

#define READ_IF_RETURN_NULL(cmp) if (cmp) { return NULL;}

#define READ_IF_RETURN(cmp) if (cmp) { return;}

void* MeshLoader::_DoLoad(const String& path)
{
	JY_ASSERT(NULL == m_pMeshStreamData);
	IFile* readfile;
	GraphicDefine::MeshStreamData* result = 0;
	readfile = IFileSystem::Instance()->FileFactory(path);
	GraphicDefine::MeshStreamData* meshStreamData = _NEW GraphicDefine::MeshStreamData();
	if (readfile->OpenFile(IFile::AT_READ))
	{
		Header header;
		readfile->ReadFile(&header, sizeof(Header));

		switch (header.version)
		{
		case 1:
		{
			int chunkID;
			VertexChunk vertexChunk;
			readfile->ReadFile(&chunkID, sizeof(int));
			readfile->ReadFile(&vertexChunk, sizeof(VertexChunk));
			int currPos = readfile->Tell();
			vertexChunk.bufferOffset += header.binaryDataOffset;
			std::vector<std::vector<int>> dump;
			_LoadMesh(vertexChunk, readfile, header, dump, meshStreamData);
			break;
		}
		case 2:
		{
			int chunkID;
			VertexChunk2 vertexChunk2;
			readfile->ReadFile(&chunkID, sizeof(int));
			readfile->ReadFile(&vertexChunk2, sizeof(VertexChunk2));
			int currPos = readfile->Tell();
			vertexChunk2.bufferOffset += header.binaryDataOffset;
			vertexChunk2.jointsIDOffset += header.binaryDataOffset;
			_LoadMesh2(vertexChunk2, readfile, header, meshStreamData);
			break;
		}
		case 3:
		{
			int chunkID;
			VertexChunk3_vertex vertexChunk3_vertex;
			VertexChunk3_indices vertexChunk3_indices;
			readfile->ReadFile(&chunkID, sizeof(int));
			readfile->ReadFile(&vertexChunk3_vertex, sizeof(VertexChunk3_vertex));
			vertexChunk3_vertex.bufferOffset += header.binaryDataOffset;
			int currPos = readfile->Tell();
			_LoadMesh3(vertexChunk3_indices, vertexChunk3_vertex, readfile, header, meshStreamData);
			break;
		}
		case 4:
		{
			BindingBox bindBox;
			readfile->ReadFile(&bindBox, sizeof(BindingBox));
			int chunkID;
			VertexChunk3_vertex vertexChunk3_vertex;
			VertexChunk3_indices vertexChunk3_indices;
			readfile->ReadFile(&chunkID, sizeof(int));
			readfile->ReadFile(&vertexChunk3_vertex, sizeof(VertexChunk3_vertex));
			vertexChunk3_vertex.bufferOffset += header.binaryDataOffset;
			int currPos = readfile->Tell();
			_LoadMesh3(vertexChunk3_indices, vertexChunk3_vertex, readfile, header, meshStreamData);
			meshStreamData->m_BindingBox = Math::AABB(Math::Vec3(bindBox.bindingbox[0], bindBox.bindingbox[1], bindBox.bindingbox[2]),
				Math::Vec3(bindBox.bindingbox[3], bindBox.bindingbox[4], bindBox.bindingbox[5]));
			break;
		}
		case 5:
		{
			BindingBox bindBox;
			readfile->ReadFile(&bindBox, sizeof(BindingBox));
			int chunkID;
			VertexChunk4_vertex vertexChunk4_vertex;
			VertexChunk3_indices vertexChunk3_indices;
			readfile->ReadFile(&chunkID, sizeof(int));
			readfile->ReadFile(&vertexChunk4_vertex, sizeof(VertexChunk4_vertex));
			vertexChunk4_vertex.bufferOffset = header.binaryDataOffset;
			int currPos = readfile->Tell();
			_LoadMesh4(vertexChunk3_indices, vertexChunk4_vertex, readfile, header, meshStreamData);
			meshStreamData->m_BindingBox = Math::AABB(Math::Vec3(bindBox.bindingbox[0], bindBox.bindingbox[1], bindBox.bindingbox[2]),
				Math::Vec3(bindBox.bindingbox[3], bindBox.bindingbox[4], bindBox.bindingbox[5]));
			break;
		}
		default: {
			JYERROR("MeshLoader::_DoLoad unknown version %d ", header.version);
			break;
		}

		}

		m_pMeshStreamData = meshStreamData;

		readfile->CloseFile();
	}
	IFileSystem::Instance()->RecycleBin(readfile);
	return m_pMeshStreamData;
}

void MeshLoader::_LoadMesh(const VertexChunk& chunkDesc, IFile* readfile, Header& header, std::vector<std::vector<int>>& jointsIndex, GraphicDefine::MeshStreamData* meshStreamData)
{
	char* vertexDataBuf = (char*)malloc(chunkDesc.bufferLength);
	readfile->Seek(chunkDesc.bufferOffset, SEEK_SET);
	readfile->ReadFile(vertexDataBuf, chunkDesc.bufferLength);

	meshStreamData->m_eMode = RHIDefine::RM_TRIANGLES;

	//解压
	draco::DecoderBuffer decoderBuffer;
	decoderBuffer.Init(vertexDataBuf, chunkDesc.bufferLength);
	const  auto geom_type = draco::Decoder::GetEncodedGeometryType(&decoderBuffer);
	if (geom_type.value() == draco::TRIANGULAR_MESH)
	{
		draco::Decoder decoder;
		auto statusor = decoder.DecodeMeshFromBuffer(&decoderBuffer);
		if (!statusor.ok())
		{
			JYERROR("faile to load mesh");
		}
		else
		{
			std::unique_ptr<draco::Mesh> in_mesh = std::move(statusor).value();
			if (in_mesh)
			{
				//索引数据
				meshStreamData->m_Indices.SetIndicesType(RHIDefine::IT_UINT16);
				meshStreamData->m_Indices.ReserveBuffer(in_mesh->num_faces() * 3);
				for (int i = 0; i < in_mesh->num_faces(); ++i)
				{
					draco::FaceIndex faceIndex(i);
					draco::Mesh::Face face = in_mesh->face(faceIndex);
					draco::PointIndex pIndex = face[0];
					int meshIndex = pIndex.value();
					meshStreamData->m_Indices.PushIndicesDataFast(meshIndex);
					pIndex = face[1];
					meshIndex = pIndex.value();
					meshStreamData->m_Indices.PushIndicesDataFast(meshIndex);
					pIndex = face[2];
					meshIndex = pIndex.value();
					meshStreamData->m_Indices.PushIndicesDataFast(meshIndex);
				}

				std::vector<RHIDefine::ShaderAttribute> paramSlots;
				std::vector<draco::PointAttribute*> paramAtt;
				int attributeCnt = in_mesh->num_attributes();
				paramSlots.reserve(attributeCnt);
				paramAtt.reserve(attributeCnt);

				int texCoordIndex = 0;
				int normalIndex = 0;  //normal ,tangent,binormal
				int vertexCnt = 0;
				for (int i = 0; i < attributeCnt; i++)
				{
					draco::PointAttribute* ptAtt = in_mesh->attribute(i);
					uint8_t* rawData = 0;
					size_t rawDataSize = 0;
					switch (ptAtt->attribute_type())
					{
					case draco::GeometryAttribute::POSITION:
						meshStreamData->m_Vertex.SetVertexType<Math::Vec4>(RHIDefine::PS_ATTRIBUTE_POSITION, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
						rawData = ptAtt->buffer()->data();
						rawDataSize = ptAtt->buffer()->data_size();
						ptAtt->buffer()->data_size();
						vertexCnt = rawDataSize / (sizeof(float) * 3);
						paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_POSITION);
						paramAtt.push_back(ptAtt);
						break;
					case draco::GeometryAttribute::NORMAL:
						if (normalIndex == 0)
						{
							meshStreamData->m_Vertex.SetVertexType<Math::Vec3>(RHIDefine::PS_ATTRIBUTE_NORMAL, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
							rawData = ptAtt->buffer()->data();
							paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_NORMAL);
							paramAtt.push_back(ptAtt);
						}
						else if (normalIndex == 1)
						{
							meshStreamData->m_Vertex.SetVertexType<Math::Vec3>(RHIDefine::PS_ATTRIBUTE_TANGENT, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
							rawData = ptAtt->buffer()->data();
							paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_TANGENT);
							paramAtt.push_back(ptAtt);
						}
						else if (normalIndex == 2)
						{
							meshStreamData->m_Vertex.SetVertexType<Math::Vec3>(RHIDefine::PS_ATTRIBUTE_BINORMAL, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
							rawData = ptAtt->buffer()->data();
							paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_BINORMAL);
							paramAtt.push_back(ptAtt);
						}
						normalIndex++;

						break;
					case draco::GeometryAttribute::TEX_COORD:

						if (texCoordIndex == 0)
						{
							meshStreamData->m_Vertex.SetVertexType<Math::Vec2>(RHIDefine::PS_ATTRIBUTE_COORDNATE0, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
							rawData = ptAtt->buffer()->data();
							paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_COORDNATE0);
							paramAtt.push_back(ptAtt);
						}
						else if (texCoordIndex == 1)
						{
							meshStreamData->m_Vertex.SetVertexType<Math::Vec2>(RHIDefine::PS_ATTRIBUTE_COORDNATE1, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
							rawData = ptAtt->buffer()->data();
							paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_COORDNATE1);
							paramAtt.push_back(ptAtt);
						}
						else if (texCoordIndex == 2)
						{
							meshStreamData->m_Vertex.SetVertexType<Math::Vec2>(RHIDefine::PS_ATTRIBUTE_COORDNATE2, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
							rawData = ptAtt->buffer()->data();
							paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_COORDNATE2);
							paramAtt.push_back(ptAtt);
						}
						else if (texCoordIndex == 3)
						{
							meshStreamData->m_Vertex.SetVertexType<Math::Vec2>(RHIDefine::PS_ATTRIBUTE_COORDNATE3, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
							rawData = ptAtt->buffer()->data();
							paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_COORDNATE3);
							paramAtt.push_back(ptAtt);
						}
						texCoordIndex++;
						break;
					case draco::GeometryAttribute::GENERIC: {
						uint32_t uid = ptAtt->unique_id();

						/*
						 *  如果一个draco所有attribute都没有设置attribute的uid的话，
						 *  这个uid按照AddAttribute的顺序递增
						 *  但是只要要设置uid的话，就是自定义的的uid，其他没有设置的都是为0
						 *
						 *  旧的素材导出都没有设置这个uid,所以按照AddAtribute的顺序,有默认的递增id
						 *  新的素材如果有GENERIC顶点属性的,就会设置这个ID
						 *
						 * */
						JYLOG("mesh GENERIC uid = %u version %d \n", uid, header.version);
						if (header.version < 4)
						{
							if (ptAtt->data_type() == draco::DT_UINT16)
							{

								meshStreamData->m_Vertex.SetVertexType<Math::Vec4>(RHIDefine::PS_ATTRIBUTE_BONE_INEX, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
								rawData = ptAtt->buffer()->data();
								paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_BONE_INEX);
								paramAtt.push_back(ptAtt);
							}
							else if (ptAtt->data_type() == draco::DT_FLOAT32)
							{

								meshStreamData->m_Vertex.SetVertexType<Math::Vec4>(RHIDefine::PS_ATTRIBUTE_BONE_WEIGHT, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
								rawData = ptAtt->buffer()->data();
								paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_BONE_WEIGHT);
								paramAtt.push_back(ptAtt);
							}
						}
						else
						{
							switch (uid)
							{
							case BOND_INDEX:
							{
								meshStreamData->m_Vertex.SetVertexType<Math::Vec4>(RHIDefine::PS_ATTRIBUTE_BONE_INEX, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
								rawData = ptAtt->buffer()->data();
								paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_BONE_INEX);
								paramAtt.push_back(ptAtt);
							}
							break;
							case BOND_WEIGHTS:
							{
								meshStreamData->m_Vertex.SetVertexType<Math::Vec4>(RHIDefine::PS_ATTRIBUTE_BONE_WEIGHT, RHIDefine::DT_FLOAT, RHIDefine::DT_HALF_FLOAT);
								rawData = ptAtt->buffer()->data();
								paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_BONE_WEIGHT);
								paramAtt.push_back(ptAtt);
							}
							break;
							case MORPH_TARGETS_VERTEX:
							case MORPH_TARGETS_NORMAL:
							{ // moprh target

								rawData = ptAtt->buffer()->data();
								rawDataSize = ptAtt->buffer()->data_size();

								float* moprhTargetVertexAttr = (float*)rawData;
								int vertexCnt = rawDataSize / (sizeof(float) * 3);

								int index = meshStreamData->m_pTargets.size();
								meshStreamData->m_pTargets.emplace_back(std::make_shared<std::vector<Math::Vec3>>());

								std::shared_ptr<std::vector<Math::Vec3>>& vs = (meshStreamData->m_pTargets)[index];

								for (int idx = 0; idx < vertexCnt; idx++)
								{
									vs->emplace_back(
										moprhTargetVertexAttr[idx * 3],
										moprhTargetVertexAttr[idx * 3 + 1],
										moprhTargetVertexAttr[idx * 3 + 2]
									);
								}

								// 这个是attr的id 跟动画文件的id对应/drc/mesh文件中的索引
								meshStreamData->m_pTargetIds.push_back(i);
							}
							break;

							default:
								break;
							}
						}

					}break;
					case draco::GeometryAttribute::COLOR:
						meshStreamData->m_Vertex.SetVertexType<Math::Vec4>(RHIDefine::PS_ATTRIBUTE_COLOR0, RHIDefine::DT_FLOAT, RHIDefine::DT_UINT_8_8_8_8);
						rawData = ptAtt->buffer()->data();
						paramSlots.push_back(RHIDefine::PS_ATTRIBUTE_COLOR0);
						paramAtt.push_back(ptAtt);
						break;
					default:
						break;
					}
				}

				int numPoint = in_mesh->num_points();
				meshStreamData->m_Vertex.ReserveBuffer(vertexCnt);
				std::vector<float> tempCacheF; //临时保存下数据
				tempCacheF.resize(4);
				std::vector<uint16_t> tempCacheU16;
				tempCacheU16.resize(4);
				draco::AttributeValueIndex val_index;
				draco::PointAttribute* ptAtt;
				for (int i = 0; i < vertexCnt; i++)
				{
					int rawDatIndex = 0;
					for (auto iter : paramSlots)
					{
						switch (iter)
						{
						case RHIDefine::PS_ATTRIBUTE_POSITION:
							ptAtt = paramAtt[rawDatIndex];
							val_index = ptAtt->mapped_index(draco::PointIndex(i));
							ptAtt->ConvertValue<float, 3>(val_index, tempCacheF.data());
							meshStreamData->m_Vertex.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_POSITION, Math::Vec4(tempCacheF[0], tempCacheF[1], tempCacheF[2], 1).GetPtr());
							break;
						case RHIDefine::PS_ATTRIBUTE_NORMAL:
							ptAtt = paramAtt[rawDatIndex];
							val_index = ptAtt->mapped_index(draco::PointIndex(i));
							ptAtt->ConvertValue<float, 3>(val_index, tempCacheF.data());
							meshStreamData->m_Vertex.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_NORMAL, Math::Vec3(tempCacheF[0], tempCacheF[1], tempCacheF[2]).GetPtr());
							break;
						case RHIDefine::PS_ATTRIBUTE_TANGENT:
							ptAtt = paramAtt[rawDatIndex];
							val_index = ptAtt->mapped_index(draco::PointIndex(i));
							ptAtt->ConvertValue<float, 3>(val_index, tempCacheF.data());
							meshStreamData->m_Vertex.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_TANGENT, Math::Vec3(tempCacheF[0], tempCacheF[1], tempCacheF[2]).GetPtr());
							break;
						case RHIDefine::PS_ATTRIBUTE_BINORMAL:
							ptAtt = paramAtt[rawDatIndex];
							val_index = ptAtt->mapped_index(draco::PointIndex(i));
							ptAtt->ConvertValue<float, 3>(val_index, tempCacheF.data());
							meshStreamData->m_Vertex.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_BINORMAL, Math::Vec3(tempCacheF[0], tempCacheF[1], tempCacheF[2]).GetPtr());
							break;;
						case RHIDefine::PS_ATTRIBUTE_COORDNATE0:
							ptAtt = paramAtt[rawDatIndex];
							val_index = ptAtt->mapped_index(draco::PointIndex(i));
							ptAtt->ConvertValue<float, 2>(val_index, tempCacheF.data());
							meshStreamData->m_Vertex.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE0, Math::Vec2(tempCacheF[0], 1.0 - tempCacheF[1]).GetPtr());
							break;
						case RHIDefine::PS_ATTRIBUTE_COORDNATE1:
							ptAtt = paramAtt[rawDatIndex];
							val_index = ptAtt->mapped_index(draco::PointIndex(i));
							ptAtt->ConvertValue<float, 2>(val_index, tempCacheF.data());
							meshStreamData->m_Vertex.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE1, Math::Vec2(tempCacheF[0], 1.0 - tempCacheF[1]).GetPtr());
							break;
						case RHIDefine::PS_ATTRIBUTE_COORDNATE2:
							ptAtt = paramAtt[rawDatIndex];
							val_index = ptAtt->mapped_index(draco::PointIndex(i));
							ptAtt->ConvertValue<float, 2>(val_index, tempCacheF.data());
							meshStreamData->m_Vertex.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE2, Math::Vec2(tempCacheF[0], 1.0 - tempCacheF[1]).GetPtr());
							break;
						case RHIDefine::PS_ATTRIBUTE_COORDNATE3:
							ptAtt = paramAtt[rawDatIndex];
							val_index = ptAtt->mapped_index(draco::PointIndex(i));
							ptAtt->ConvertValue<float, 2>(val_index, tempCacheF.data());
							meshStreamData->m_Vertex.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COORDNATE3, Math::Vec2(tempCacheF[0], 1.0 - tempCacheF[1]).GetPtr());
							break;
						case RHIDefine::PS_ATTRIBUTE_COLOR0:
							ptAtt = paramAtt[rawDatIndex];
							val_index = ptAtt->mapped_index(draco::PointIndex(i));
							ptAtt->ConvertValue<float, 4>(val_index, tempCacheF.data());
							meshStreamData->m_Vertex.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_COLOR0, Math::Vec4(tempCacheF[0], tempCacheF[1], tempCacheF[2], tempCacheF[3]).GetPtr());
							break;
						case RHIDefine::PS_ATTRIBUTE_BONE_INEX:
							ptAtt = paramAtt[rawDatIndex];
							val_index = ptAtt->mapped_index(draco::PointIndex(i));
							ptAtt->ConvertValue<uint16_t, 4>(val_index, tempCacheU16.data());
							meshStreamData->m_Vertex.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_BONE_INEX, Math::Vec4(tempCacheU16[0], tempCacheU16[1], tempCacheU16[2], tempCacheU16[3]).GetPtr());
							break;
						case RHIDefine::PS_ATTRIBUTE_BONE_WEIGHT:
							ptAtt = paramAtt[rawDatIndex];
							val_index = ptAtt->mapped_index(draco::PointIndex(i));
							ptAtt->ConvertValue<float, 4>(val_index, tempCacheF.data());
							meshStreamData->m_Vertex.PushVertexDataFast(RHIDefine::PS_ATTRIBUTE_BONE_WEIGHT, Math::Vec4(tempCacheF[0], tempCacheF[1], tempCacheF[2], tempCacheF[3]).GetPtr());
							break;

						}
						rawDatIndex++;
					}
				}
			}
		}
	}
	free(vertexDataBuf);
}

void MeshLoader::_LoadMesh2(const VertexChunk2& chunkDesc2, IFile* file, Header& header, GraphicDefine::MeshStreamData* meshStreamData)
{
	VertexChunk chunkdesc1;
	chunkdesc1.bufferLength = chunkDesc2.bufferLength;
	chunkdesc1.bufferOffset = chunkDesc2.bufferOffset;
	chunkdesc1.vertexLayout = chunkDesc2.vertexLayout;
	chunkdesc1.skeletonIndex = chunkDesc2.skeletonIndex;

	std::vector<std::vector<int>> dump;
	_LoadMesh(chunkdesc1, file, header, dump, meshStreamData);

	if (chunkDesc2.jointsIDLength > 0)
	{
		std::vector<int> temp;
		int vectorlength = chunkDesc2.jointsIDLength / 4;
		temp.resize(vectorlength, 100);
		void* pStart = &temp[0];
		file->Seek(chunkDesc2.jointsIDOffset, SEEK_SET);
		file->ReadFile(pStart, chunkDesc2.jointsIDLength);
		meshStreamData->m_SplitedJointsIDs.push_back(temp);
	}
}

void MeshLoader::_LoadMesh3(VertexChunk3_indices& chunkDesc3_indices, const VertexChunk3_vertex& chunkDesc3_vertex, IFile* file, Header& header, GraphicDefine::MeshStreamData* meshStreamData)
{
	VertexChunk chunkdesc1;
	chunkdesc1.bufferLength = chunkDesc3_vertex.bufferLength;
	chunkdesc1.bufferOffset = chunkDesc3_vertex.bufferOffset;
	chunkdesc1.vertexLayout = chunkDesc3_vertex.vertexLayout;
	chunkdesc1.skeletonIndex = chunkDesc3_vertex.skeletonIndex;

	file->ReadFile(&chunkDesc3_indices.triangleCntOffset, sizeof(int));
	file->ReadFile(&chunkDesc3_indices.triangleCntLength, sizeof(int));
	chunkDesc3_indices.jointsIDOffsetAndLength.resize(chunkDesc3_vertex.subMeshCnt * 2);
	file->ReadFile(&chunkDesc3_indices.jointsIDOffsetAndLength[0], chunkDesc3_vertex.subMeshCnt * 2 * sizeof(int));

	file->Seek(chunkDesc3_indices.jointsIDOffsetAndLength[0] + header.binaryDataOffset, SEEK_SET);
	for (int i = 1; i < chunkDesc3_vertex.subMeshCnt * 2; i += 2)
	{
		if (chunkDesc3_indices.jointsIDOffsetAndLength[i] > 0)
		{
			std::vector<int> temp(chunkDesc3_indices.jointsIDOffsetAndLength[i] / 4);
			file->ReadFile(&temp[0], chunkDesc3_indices.jointsIDOffsetAndLength[i]);
			meshStreamData->m_SplitedJointsIDs.push_back(temp);
		}
	}
	meshStreamData->m_TrianglesCnt.resize(chunkDesc3_vertex.subMeshCnt);
	file->Seek(chunkDesc3_indices.triangleCntOffset + header.binaryDataOffset, SEEK_SET);
	file->ReadFile(&meshStreamData->m_TrianglesCnt[0], chunkDesc3_indices.triangleCntLength);

	std::vector<std::vector<int>> dump;
	_LoadMesh(chunkdesc1, file, header, dump, meshStreamData);
}

void MeshLoader::_LoadMesh4(VertexChunk3_indices& chunkDesc3_indices, const VertexChunk4_vertex& chunkDesc4_vertex, IFile* file, Header& header, GraphicDefine::MeshStreamData* meshStreamData)
{
	VertexChunk chunkdesc1;
	chunkdesc1.bufferLength = chunkDesc4_vertex.bufferLength;
	chunkdesc1.bufferOffset = chunkDesc4_vertex.bufferOffset;
	chunkdesc1.vertexLayout = chunkDesc4_vertex.vertexLayout;

	file->ReadFile(&chunkDesc3_indices.triangleCntOffset, sizeof(int));
	file->ReadFile(&chunkDesc3_indices.triangleCntLength, sizeof(int));

	meshStreamData->m_TrianglesCnt.resize(chunkDesc4_vertex.subMeshCnt);

	std::vector<std::vector<int>> dump;
	_LoadMesh(chunkdesc1, file, header, dump, meshStreamData);
	file->ReadFile(&meshStreamData->m_TrianglesCnt[0], chunkDesc3_indices.triangleCntLength);
}

void MeshLoader::Release(void* data)
{
	if (data != NULL)
	{
		JY_ASSERT(data == m_pMeshStreamData);
	}
	else
	{
		JYERROR("mesh maybe delete while loading");
	}
	SAFE_DELETE(m_pMeshStreamData);
}

ILoader* MeshLoader::CreateCacheLoader() const
{
	if (m_pMeshStreamData)
	{
		return _NEW MeshCache(m_pMeshStreamData, GetPath());
	}
	return NULL;
}

NS_JYE_END