#define TINYOBJLOADER_IMPLEMENTATION
#include "ObjExporter.h"
#include "Raw2Apollo.h"
#include "tiny_obj_loader.h"

extern bool ExportMeshDataWithDracoObj(ApolloBinData& apolloData, RawModel& surfaceModel, RawModel& rawModel, int maxBones, bool bCompress);

typedef struct {
	std::vector<Math::Vec3> vertices;
	std::vector<Math::Vec3> normals;
	std::vector<Math::Vec2> texcoords;
	std::vector<tinyobj::index_t> v_indices;
} MyMesh;

void vertex_cb(void *user_data, float x, float y, float z, float w) {
	MyMesh *mesh = reinterpret_cast<MyMesh *>(user_data);
	//printf("v[%ld] = %f, %f, %f (w %f)\n", mesh->vertices.size() / 3, x, y, z, w);
	mesh->vertices.push_back(Math::Vec3(x,y,z));
	// Discard w
}

void normal_cb(void *user_data, float x, float y, float z) {
	MyMesh *mesh = reinterpret_cast<MyMesh *>(user_data);
	//printf("vn[%ld] = %f, %f, %f\n", mesh->normals.size() / 3, x, y, z);
	mesh->normals.push_back(Math::Vec3(x, y, z));
}

void texcoord_cb(void *user_data, float x, float y, float z) {
	MyMesh *mesh = reinterpret_cast<MyMesh *>(user_data);
	//printf("vt[%ld] = %f, %f, %f\n", mesh->texcoords.size() / 3, x, y, z);
	mesh->texcoords.push_back(Math::Vec2(x, y));
}

void index_cb(void *user_data, tinyobj::index_t *indices, int num_indices) {
	// NOTE: the value of each index is raw value.
	// For example, the application must manually adjust the index with offset
	// (e.g. v_indices.size()) when the value is negative(whic means relative
	// index).
	// Also, the first index starts with 1, not 0.
	// See fixIndex() function in tiny_obj_loader.h for details.
	// Also, 0 is set for the index value which
	// does not exist in .obj
	MyMesh *mesh = reinterpret_cast<MyMesh *>(user_data);
	for (int i = 0; i < num_indices; i++) {
		tinyobj::index_t idx = indices[i];
		//printf("idx[%ld] = %d, %d, %d\n", mesh->v_indices.size(), idx.vertex_index,idx.normal_index, idx.texcoord_index);
		mesh->v_indices.push_back(idx);
	}
}

bool LoadObj(RawModel &raw, std::string& fbxFileName, std::string& outputPath)
{
	tinyobj::callback_t cb;
	cb.vertex_cb = vertex_cb;
	cb.normal_cb = normal_cb;
	cb.texcoord_cb = texcoord_cb;
	cb.index_cb = index_cb;
	raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_POSITION);
	raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_NORMAL);
	raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_UV0);
	MyMesh mesh;
	std::string warn;
	std::string err;
	std::ifstream ifs(fbxFileName);
	bool ret = tinyobj::LoadObjWithCallback(ifs, cb, &mesh, NULL, &warn, &err);
	if (ret)
	{
		const int rawSurfaceIndex = raw.AddSurface("default", 0);
		RawSurface &rawSurface = raw.GetSurface(rawSurfaceIndex);
		int textures[RAW_TEXTURE_USAGE_MAX];
		std::shared_ptr<RawMatProps> rawMatProps;
		RawMaterialType materialType = RawMaterialType::RAW_MATERIAL_TYPE_OPAQUE;
		const int rawMaterialIndex = raw.AddMaterial("Default", materialType, textures, rawMatProps);
		raw.InitVerticeArray(mesh.vertices.size());
		for (int i = 0; i < mesh.v_indices.size(); i+=3)
		{
			int rawVertexIndices[3];
			RawVertex rawVertices[3];
			for (int j = 0; j < 3; j++)
			{
				int n = i + j;
				tinyobj::index_t idx = mesh.v_indices[n];
				if (idx.vertex_index > 0 && idx.vertex_index <= mesh.vertices.size())
				{
					rawVertices[j].position = mesh.vertices[idx.vertex_index - 1];
				}
				else
				{
					printf("vertex out of range!.\n");
					return false;
				}
				if (idx.normal_index > 0 && idx.normal_index <= mesh.normals.size())
				{
					rawVertices[j].normal = mesh.normals[idx.normal_index - 1];
				}
				if (idx.texcoord_index > 0 && idx.texcoord_index <= mesh.texcoords.size())
				{
					rawVertices[j].uv0 = mesh.texcoords[idx.texcoord_index - 1];
				}
				rawVertexIndices[j] = raw.AddVertex(rawVertices[j], idx.vertex_index - 1);
				rawSurface.bounds.AddPoint(rawVertices[j].position);
			}
			raw.AddTriangle(rawVertexIndices[0], rawVertexIndices[1], rawVertexIndices[2], rawMaterialIndex, rawSurfaceIndex);
		}
		std::unique_ptr<ApolloBinData> apolloData(new ApolloBinData());
		ExportMeshDataWithDracoObj(*apolloData, raw, raw, 30, false);
		apolloData->Serialze(outputPath);  //输出到文件
	}
	else
	{
		return false;
	}
	return true;
}
