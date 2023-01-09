/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include <vector>
#include <string>
#include <unordered_map>
#include <cmath>
#include <map>
#include <set>
#include <algorithm>
#include <assert.h>

#if defined( __unix__ )
#include <algorithm>
#endif


#include "String_Utils.h"
#include "Image_Utils.h"
#include "RawModel.h"
#include "FbxMathUtil.h"

extern bool verboseOutput;

#define kPI 3.14159265358979323846264338327950288419716939937510f;
float Deg2Rad(float deg)
{
	// TODO : should be deg * kDeg2Rad, but can't be changed, 
	// because it changes the order of operations and that affects a replay in some RegressionTests
	return deg / 360.0f * 2.0f * kPI;
}

bool RawVertex::operator==(const RawVertex &other) const
{
	float splitAngle = Math::Cos<float>(Deg2Rad(1.0f));

	bool bSplitNormal   = normal.Dot(other.normal) < splitAngle;
	bool bSplitTangent  = tangent.Dot(other.tangent) < splitAngle;
	bool bSplitBinormal = binormal.Dot(other.binormal) < splitAngle;

    return (position == other.position) &&
		    !bSplitNormal &&
		    !bSplitTangent &&
		    !bSplitBinormal &&
           (color == other.color) &&
           (uv0 == other.uv0) &&
           (uv1 == other.uv1) &&
		   (uv2 == other.uv2) &&
		   (uv3 == other.uv3) &&
           (jointIndices == other.jointIndices) &&
           (jointWeights == other.jointWeights) &&
           (polarityUv0 == other.polarityUv0) &&
           (blendSurfaceIx == other.blendSurfaceIx) &&
           (blends == other.blends);
}

size_t RawVertex::Difference(const RawVertex &other) const
{
    size_t attributes = 0;
    if (position != other.position) { attributes |= RAW_VERTEX_ATTRIBUTE_POSITION; }
    if (normal != other.normal) { attributes |= RAW_VERTEX_ATTRIBUTE_NORMAL; }
    if (tangent != other.tangent) { attributes |= RAW_VERTEX_ATTRIBUTE_TANGENT; }
    if (binormal != other.binormal) { attributes |= RAW_VERTEX_ATTRIBUTE_BINORMAL; }
    if (color != other.color) { attributes |= RAW_VERTEX_ATTRIBUTE_COLOR; }
    if (uv0 != other.uv0) { attributes |= RAW_VERTEX_ATTRIBUTE_UV0; }
    if (uv1 != other.uv1) { attributes |= RAW_VERTEX_ATTRIBUTE_UV1; }
	if (uv2 != other.uv2) { attributes |= RAW_VERTEX_ATTRIBUTE_UV2; }
	if (uv3 != other.uv3) { attributes |= RAW_VERTEX_ATTRIBUTE_UV3; }
    // Always need both or neither.
    if (jointIndices != other.jointIndices) { attributes |= RAW_VERTEX_ATTRIBUTE_JOINT_INDICES | RAW_VERTEX_ATTRIBUTE_JOINT_WEIGHTS; }
    if (jointWeights != other.jointWeights) { attributes |= RAW_VERTEX_ATTRIBUTE_JOINT_INDICES | RAW_VERTEX_ATTRIBUTE_JOINT_WEIGHTS; }
    return attributes;
}

RawModel::RawModel()
    : vertexAttributes(0)
{	
}

void RawModel::AddVertexAttribute(const RawVertexAttribute attrib)
{
    vertexAttributes |= attrib;
}

void RawModel::InitVerticeArray(int count)
{
	vertices = std::vector<RawVertex>(count);
}

int RawModel::AddVertex(const RawVertex &vertex)
{
    auto it = vertexHash.find(vertex);
    if (it != vertexHash.end()) {
        return it->second;
    }
    vertexHash.emplace(vertex, (int) vertices.size());
    vertices.push_back(vertex);
    return (int) vertices.size() - 1;
}

int RawModel::AddVertex(const RawVertex &vertex, int index)
{
    auto it = vertexHash.find(vertex);
    if (it != vertexHash.end()) {
        return it->second;
    }
    vertexHash.emplace(vertex, index);
	vertices[index] = vertex;
    return index;
}

void RawModel::AddVertexInSplitMesh(const RawVertex &vertex)
{
	vertices.push_back(vertex);
}

int RawModel::AddTriangle(const int v0, const int v1, const int v2, const int materialIndex, const int surfaceIndex)
{
    const RawTriangle triangle = {{v0, v1, v2}, materialIndex, surfaceIndex};
    triangles.push_back(triangle);

	

    return (int) triangles.size() - 1;
}

int RawModel::AddTexture(const std::string &name, const std::string &fileName, const std::string &fileLocation, RawTextureUsage usage)
{
	
    if (name.empty()) {
        return -1;
    }
    for (size_t i = 0; i < textures.size(); i++) {
        if (StringUtils::CompareNoCase(textures[i].name, name) == 0 && textures[i].usage == usage) {
            return (int) i;
        }
    }

    const ImageProperties properties = GetImageProperties(!fileLocation.empty() ? fileLocation.c_str() : fileName.c_str());

    RawTexture texture;
    texture.name         = name;
    texture.width        = properties.width;
    texture.height       = properties.height;
    texture.mipLevels    = (int) ceilf(log2f(std::max((float) properties.width, (float) properties.height)));
    texture.usage        = usage;
    texture.occlusion    = (properties.occlusion == IMAGE_TRANSPARENT) ?
                           RAW_TEXTURE_OCCLUSION_TRANSPARENT : RAW_TEXTURE_OCCLUSION_OPAQUE;
    texture.fileName     = fileName;
    texture.fileLocation = fileLocation;
    textures.emplace_back(texture);
    return (int) textures.size() - 1;
	
	return -1;
}

int RawModel::AddMaterial(const RawMaterial &material)
{
    return AddMaterial(material.name.c_str(), material.type, material.textures, material.info);
}

int RawModel::AddMaterial(
    const char *name,
    const RawMaterialType materialType,
    const int textures[RAW_TEXTURE_USAGE_MAX],
    std::shared_ptr<RawMatProps> materialInfo)
{
    for (size_t i = 0; i < materials.size(); i++) {
        if (materials[i].name != name) {
            continue;
        }
        if (materials[i].type != materialType) {
            continue;
        }
        if (*(materials[i].info) != *materialInfo) {
            continue;
        }
        bool match = true;
        for (int j = 0; match && j < RAW_TEXTURE_USAGE_MAX; j++) {
            match = match && (materials[i].textures[j] == textures[j]);
        }
        if (match) {
            return (int) i;
        }
    }

    RawMaterial material;
    material.name = name;
    material.type = materialType;
    material.info = materialInfo;

    for (int i = 0; i < RAW_TEXTURE_USAGE_MAX; i++) {
        material.textures[i] = textures[i];
    }

    materials.emplace_back(material);

    return (int) materials.size() - 1;
}

int RawModel::AddSurface(const RawSurface &surface)
{
    for (size_t i = 0; i < surfaces.size(); i++) {
        if (StringUtils::CompareNoCase(surfaces[i].name, surface.name) == 0) {
            return (int) i;
        }
    }

    surfaces.emplace_back(surface);
    return (int) (surfaces.size() - 1);
}

int RawModel::AddSurface(const char *name, const long surfaceId)
{
    assert(name[0] != '\0');

    for (size_t i = 0; i < surfaces.size(); i++) {
        if (surfaces[i].id == surfaceId) {
            return (int) i;
        }
    }
    RawSurface  surface;
    surface.id = surfaceId;
    surface.name     = name;
	surface.bounds.Clear();
    surface.discrete  = false;
	surface.maxBone = 0;

    surfaces.emplace_back(surface);
    return (int) (surfaces.size() - 1);
}

int RawModel::AddAnimation(const RawAnimation &animation)
{
    animations.emplace_back(animation);
    return (int) (animations.size() - 1);
}

int RawModel::AddNode(const RawNode &node)
{
    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i].id == node.id) {
            return (int)i;
        }
    }

    nodes.emplace_back(node);
    return (int) nodes.size() - 1;
}

int RawModel::AddCameraPerspective(
    const char *name, const long nodeId, const float aspectRatio, const float fovDegreesX, const float fovDegreesY, const float nearZ,
    const float farZ)
{
    RawCamera camera;
    camera.name                    = name;
    camera.nodeId                  = nodeId;
    camera.mode                    = RawCamera::CAMERA_MODE_PERSPECTIVE;
    camera.perspective.aspectRatio = aspectRatio;
    camera.perspective.fovDegreesX = fovDegreesX;
    camera.perspective.fovDegreesY = fovDegreesY;
    camera.perspective.nearZ       = nearZ;
    camera.perspective.farZ        = farZ;
    cameras.emplace_back(camera);
    return (int) cameras.size() - 1;
}

int RawModel::AddCameraOrthographic(
    const char *name, const long nodeId, const float magX, const float magY, const float nearZ, const float farZ)
{
    RawCamera camera;
    camera.name               = name;
    camera.nodeId             = nodeId;
    camera.mode               = RawCamera::CAMERA_MODE_ORTHOGRAPHIC;
    camera.orthographic.magX  = magX;
    camera.orthographic.magY  = magY;
    camera.orthographic.nearZ = nearZ;
    camera.orthographic.farZ  = farZ;
    cameras.emplace_back(camera);
    return (int) cameras.size() - 1;
}

int RawModel::AddNode(const long id, const char *name, const long parentId)
{
    assert(name[0] != '\0');

    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i].id == id ) {
            return (int) i;
        }
    }

    RawNode joint;
	joint.isSkeleton  = false;
    joint.isJoint     = false;
    joint.id          = id;
    joint.name        = name;
    joint.parentId    = parentId;
    joint.surfaceId   = 0;
	joint.translation = Math::Vec3(0.0f, 0.0f, 0.0f);
	joint.rotation = Math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	joint.scale = Math::Vec3(0.0f, 0.0f, 0.0f);
    //joint.translation = Vec3f(0, 0, 0);
    //joint.rotation    = Quatf(0, 0, 0, 1);
    //joint.scale       = Vec3f(1, 1, 1);

    nodes.emplace_back(joint);
    return (int) nodes.size() - 1;
}

void RawModel::Condense()
{
    // Only keep surfaces that are referenced by one or more triangles.
    {
        std::vector<RawSurface> oldSurfaces = surfaces;

        surfaces.clear();

        for (auto &triangle : triangles) {
            const RawSurface &surface = oldSurfaces[triangle.surfaceIndex];
            const int surfaceIndex = AddSurface(surface.name.c_str(), surface.id);
            surfaces[surfaceIndex] = surface;
            triangle.surfaceIndex = surfaceIndex;
        }
    }

    // Only keep materials that are referenced by one or more triangles.
    {
        std::vector<RawMaterial> oldMaterials = materials;

        materials.clear();

        for (auto &triangle : triangles) {
            const RawMaterial &material = oldMaterials[triangle.materialIndex];
            const int materialIndex = AddMaterial(material);
            materials[materialIndex] = material;
            triangle.materialIndex = materialIndex;
        }
    }

    // Only keep textures that are referenced by one or more materials.
    {
        std::vector<RawTexture> oldTextures = textures;

        textures.clear();

        for (auto &material : materials) {
            for (int j = 0; j < RAW_TEXTURE_USAGE_MAX; j++) {
                if (material.textures[j] >= 0) {
                    const RawTexture &texture = oldTextures[material.textures[j]];
                    const int textureIndex = AddTexture(texture.name, texture.fileName, texture.fileLocation, texture.usage);
                    textures[textureIndex] = texture;
                    material.textures[j] = textureIndex;
                }
            }
        }
    }

    // Only keep vertices that are referenced by one or more triangles.
    {
        std::vector<RawVertex> oldVertices = vertices;

        vertexHash.clear();
        vertices.clear();

        for (auto &triangle : triangles) {
            for (int j = 0; j < 3; j++) {
                triangle.verts[j] = AddVertex(oldVertices[triangle.verts[j]]);
            }
        }
    }
}

void RawModel::TransformGeometry(ComputeNormalsOption normals)
{
    switch(normals) {
        case ComputeNormalsOption::NEVER:
            break;
        case ComputeNormalsOption::MISSING:
            if ((vertexAttributes & RAW_VERTEX_ATTRIBUTE_NORMAL) != 0) {
                break;
            }
            // otherwise fall through
        case ComputeNormalsOption::BROKEN:
        case ComputeNormalsOption::ALWAYS:
            size_t computedNormalsCount = this->CalculateNormals(normals == ComputeNormalsOption::BROKEN);
            vertexAttributes |= RAW_VERTEX_ATTRIBUTE_NORMAL;

            if (verboseOutput) {
                if (normals == ComputeNormalsOption::BROKEN) {
                    printf("Repaired %lu empty normals.\n", computedNormalsCount);
                } else {
                    printf("Computed %lu normals.\n", computedNormalsCount);
                }
            }
            break;
    }
}

void RawModel::TransformTextures(const std::vector<std::function<Math::Vec2(Math::Vec2)>> &transforms)
{
    for (auto &vertice : vertices) {
        if ((vertexAttributes & RAW_VERTEX_ATTRIBUTE_UV0) != 0) {
            for (const auto &fun : transforms) {
                vertice.uv0 = fun(vertice.uv0);
            }
        }
        if ((vertexAttributes & RAW_VERTEX_ATTRIBUTE_UV1) != 0) {
            for (const auto &fun : transforms) {
                vertice.uv1 = fun(vertice.uv1);
            }
        }
		if ((vertexAttributes & RAW_VERTEX_ATTRIBUTE_UV2) != 0)
		{
			for (const auto &fun : transforms) 
			{
				vertice.uv2 = fun(vertice.uv2);
			}
		}
		if ((vertexAttributes & RAW_VERTEX_ATTRIBUTE_UV3) != 0)
		{
			for (const auto &fun : transforms)
			{
				vertice.uv3 = fun(vertice.uv3);
			}
		}
    }
}

struct TriangleModelSortPos
{
    static bool Compare(const RawTriangle &a, const RawTriangle &b)
    {
        if (a.materialIndex != b.materialIndex) {
            return a.materialIndex < b.materialIndex;
        }
        if (a.surfaceIndex != b.surfaceIndex) {
            return a.surfaceIndex < b.surfaceIndex;
        }
        return a.verts[0] < b.verts[0];
    }
};

struct TriangleModelSortNeg
{
    static bool Compare(const RawTriangle &a, const RawTriangle &b)
    {
        if (a.materialIndex != b.materialIndex) {
            return a.materialIndex < b.materialIndex;
        }
        if (a.surfaceIndex != b.surfaceIndex) {
            return a.surfaceIndex < b.surfaceIndex;
        }
        return a.verts[0] > b.verts[0];
    }
};

void RawModel::CreateMaterialModels(
	std::vector<RawModel> &materialModels, bool shortIndices, const int keepAttribs, const bool forceDiscrete, bool keepIndex) const
{
    // Sort all triangles based on material first, then surface, then first vertex index.
    std::vector<RawTriangle> sortedTriangles;

    bool invertedTransparencySort = true;
    if (invertedTransparencySort) {
        // Split the triangles into opaque and transparent triangles.
        std::vector<RawTriangle> opaqueTriangles;
        std::vector<RawTriangle> transparentTriangles;
        for (const auto &triangle : triangles) {
            const int materialIndex = triangle.materialIndex;
            if (materialIndex < 0) {
                opaqueTriangles.push_back(triangle);
                continue;
            }
            const int textureIndex = materials[materialIndex].textures[RAW_TEXTURE_USAGE_DIFFUSE];
            if (textureIndex < 0) {
         
                opaqueTriangles.push_back(triangle);
                continue;
            }
            if (textures[textureIndex].occlusion == RAW_TEXTURE_OCCLUSION_TRANSPARENT) {
                transparentTriangles.push_back(triangle);
            } else {
                opaqueTriangles.push_back(triangle);
            }
        }

        // Sort the opaque triangles.
        std::sort(opaqueTriangles.begin(), opaqueTriangles.end(), TriangleModelSortPos::Compare);

        // Sort the transparent triangles in the reverse direction.
        std::sort(transparentTriangles.begin(), transparentTriangles.end(), TriangleModelSortNeg::Compare);

        // Add the triangles to the sorted list.
        for (const auto &opaqueTriangle : opaqueTriangles) {
            sortedTriangles.push_back(opaqueTriangle);
        }
        for (const auto &transparentTriangle : transparentTriangles) {
            sortedTriangles.push_back(transparentTriangle);
        }
    } else {
        sortedTriangles = triangles;
        std::sort(sortedTriangles.begin(), sortedTriangles.end(), TriangleModelSortPos::Compare);
    }

    // Overestimate the number of models that will be created to avoid massive reallocation.
    int discreteCount = 0;
    for (const auto &surface : surfaces) {
        discreteCount += (surface.discrete != false);
    }

    materialModels.clear();
    materialModels.reserve(materials.size() + discreteCount);

    const RawVertex defaultVertex;

    // Create a separate model for each material.
    RawModel *model = nullptr;
    for (size_t i = 0; i < sortedTriangles.size(); i++) {
		bool shouldChangeName = false;
        if (sortedTriangles[i].materialIndex < 0 || sortedTriangles[i].surfaceIndex < 0) {
            continue;
        }

        if (i == 0 ||
            (shortIndices && model->GetVertexCount() >= 0xFFFE) ||
            /*sortedTriangles[i].materialIndex != sortedTriangles[i - 1].materialIndex ||*/
            (sortedTriangles[i].surfaceIndex != sortedTriangles[i - 1].surfaceIndex &&
                (forceDiscrete || surfaces[sortedTriangles[i].surfaceIndex].discrete ||
                    surfaces[sortedTriangles[i - 1].surfaceIndex].discrete)))
		{
			if (i != 0 && model->GetVertexCount() >= 0xFFFE)
			{
				shouldChangeName = true;
			}
            materialModels.resize(materialModels.size() + 1);
            model = &materialModels[materialModels.size() - 1];
			if (keepIndex)
			{
				model->InitVerticeArray(vertices.size());
			}
        }

        // FIXME: will have to unlink from the nodes, transform both surfaces into a
        // common space, and reparent to a new node with appropriate transform.

        const int prevSurfaceCount = model->GetSurfaceCount();
        const int materialIndex    = model->AddMaterial(materials[sortedTriangles[i].materialIndex]);
        const int surfaceIndex     = model->AddSurface(surfaces[sortedTriangles[i].surfaceIndex]);


        RawSurface &rawSurface = model->GetSurface(surfaceIndex);

		if (model->meshName.empty())
		{
			model->meshName = rawSurface.name;
			if (shouldChangeName)
				model->meshName = model->meshName + std::to_string(materialModels.size());
		}
	

        if (model->GetSurfaceCount() > prevSurfaceCount) {
            const std::vector<long> &jointIdArray = surfaces[sortedTriangles[i].surfaceIndex].jointIds;
            for (const auto &jointId : jointIdArray) {
				const int nodeIndex = GetNodeById(jointId);
				assert(nodeIndex != -1);
				model->AddNode(GetNode(nodeIndex)); 
            }
			rawSurface.bounds.Clear();
        }

        int verts[3];
        for (int j = 0; j < 3; j++) {
			int vertexIndex = sortedTriangles[i].verts[j];
            RawVertex vertex = vertices[vertexIndex];
			
            if (keepAttribs != -1) {
                int keep = keepAttribs;
                if ((keepAttribs & RAW_VERTEX_ATTRIBUTE_POSITION) != 0) {
                    keep |= RAW_VERTEX_ATTRIBUTE_JOINT_INDICES | RAW_VERTEX_ATTRIBUTE_JOINT_WEIGHTS;
                }
                if ((keepAttribs & RAW_VERTEX_ATTRIBUTE_AUTO) != 0) {
                    keep |= RAW_VERTEX_ATTRIBUTE_POSITION;

                    const RawMaterial &mat = model->GetMaterial(materialIndex);
                    if (mat.textures[RAW_TEXTURE_USAGE_DIFFUSE] != -1) {
                        keep |= RAW_VERTEX_ATTRIBUTE_UV0;
                    }
                    if (mat.textures[RAW_TEXTURE_USAGE_NORMAL] != -1) {
                        keep |= RAW_VERTEX_ATTRIBUTE_NORMAL |
                                RAW_VERTEX_ATTRIBUTE_TANGENT |
                                RAW_VERTEX_ATTRIBUTE_BINORMAL |
                                RAW_VERTEX_ATTRIBUTE_UV0;
                    }
                    if (mat.textures[RAW_TEXTURE_USAGE_SPECULAR] != -1) {
                        keep |= RAW_VERTEX_ATTRIBUTE_NORMAL |
                                RAW_VERTEX_ATTRIBUTE_UV0;
                    }
                    if (mat.textures[RAW_TEXTURE_USAGE_EMISSIVE] != -1) {
                        keep |= RAW_VERTEX_ATTRIBUTE_UV1;
                    }
                }
                if ((keep & RAW_VERTEX_ATTRIBUTE_POSITION) == 0) { vertex.position = defaultVertex.position; }
                if ((keep & RAW_VERTEX_ATTRIBUTE_NORMAL) == 0) { vertex.normal = defaultVertex.normal; }
                if ((keep & RAW_VERTEX_ATTRIBUTE_TANGENT) == 0) { vertex.tangent = defaultVertex.tangent; }
                if ((keep & RAW_VERTEX_ATTRIBUTE_BINORMAL) == 0) { vertex.binormal = defaultVertex.binormal; }
                if ((keep & RAW_VERTEX_ATTRIBUTE_COLOR) == 0) { vertex.color = defaultVertex.color; }
                if ((keep & RAW_VERTEX_ATTRIBUTE_UV0) == 0) { vertex.uv0 = defaultVertex.uv0; }
                if ((keep & RAW_VERTEX_ATTRIBUTE_UV1) == 0) { vertex.uv1 = defaultVertex.uv1; }
				if ((keep & RAW_VERTEX_ATTRIBUTE_UV2) == 0) { vertex.uv2 = defaultVertex.uv2; }
				if ((keep & RAW_VERTEX_ATTRIBUTE_UV3) == 0) { vertex.uv3 = defaultVertex.uv3; }
                if ((keep & RAW_VERTEX_ATTRIBUTE_JOINT_INDICES) == 0) { vertex.jointIndices = defaultVertex.jointIndices; }
                if ((keep & RAW_VERTEX_ATTRIBUTE_JOINT_WEIGHTS) == 0) { vertex.jointWeights = defaultVertex.jointWeights; }
            }
			if (keepIndex)
			{
				verts[j] = model->AddVertex(vertex, vertexIndex);
			}
			else
			{
				verts[j] = model->AddVertex(vertex);
			}

            model->vertexAttributes |= vertex.Difference(defaultVertex);

			//rawSurface.meshNode->EvaluateGlobalTransform();
            //rawSurface.bounds.AddPoint(vertex.position * toMat4f(rawSurface.meshGlobalInverseMatrix.Inverse()));
			rawSurface.bounds.AddPoint(vertex.position);
        }

        model->AddTriangle(verts[0], verts[1], verts[2], materialIndex, surfaceIndex);
    }

}

int RawModel::GetNodeById(const long nodeId) const
{
    for (size_t i = 0; i < nodes.size(); i++) {
        if (nodes[i].id == nodeId) {
            return (int) i;
        }
    }
    return -1;
}

int RawModel::GetSurfaceById(const long surfaceId) const
{
    for (size_t i = 0; i < surfaces.size(); i++) {
        if (surfaces[i].id == surfaceId) {
            return (int)i;
        }
    }
    return -1;
}

Math::Vec3 RawModel::getFaceNormal(int verts[3]) const
{
    const float l0 = (vertices[verts[1]].position - vertices[verts[0]].position ).LengthSquared();
    const float l1 = (vertices[verts[2]].position - vertices[verts[1]].position ).LengthSquared();
    const float l2 = (vertices[verts[0]].position - vertices[verts[2]].position ).LengthSquared();
    const int index = ( l0 > l1 ) ? ( l0 > l2 ? 2 : 1 ) : ( l1 > l2 ? 0 : 1 );

    const Math::Vec3 e0 = vertices[verts[(index + 1) % 3]].position - vertices[verts[index]].position;
    const Math::Vec3 e1 = vertices[verts[(index + 2) % 3]].position - vertices[verts[index]].position;
    if (e0.LengthSquared() < FLT_MIN || e1.LengthSquared() < FLT_MIN) {
		return Math::Vec3(0.0f, 0.0f, 0.0f);
    }
    //auto result = Math::Vec3::Cross()::CrossProduct(e0, e1);
	auto result = e0.Cross(e1);
    auto resultLengthSquared = result.LengthSquared();
    if (resultLengthSquared < FLT_MIN) {
		return Math::Vec3(0.0f, 0.0f, 0.0f);
    }
	float edgeDot = std::max(-1.0f, std::min(1.0f, e0.Dot(e1)));
    //float edgeDot = std::max(-1.0f, std::min(1.0f, Vec3f::DotProduct(e0, e1)));
    float angle = acos(edgeDot);
    float area = resultLengthSquared / 2.0f;
	return result.GetNormalized() * angle * area;
    //return result.Normalized() * angle * area;
}

size_t RawModel::CalculateNormals(bool onlyBroken)
{
	Math::Vec3 averagePos = Math::Vec3(0.0f,0.0f, 0.0f);
    std::set<int> brokenVerts;
    for (int vertIx = 0; vertIx < vertices.size(); vertIx ++) {
        RawVertex &vertex = vertices[vertIx];
        averagePos += (vertex.position / (float)vertices.size());
        if (onlyBroken && (vertex.normal.LengthSquared() >= FLT_MIN)) {
            continue;
        }
		vertex.normal = Math::Vec3(0.0f, 0.0f,0.0f);
        if (onlyBroken) {
            brokenVerts.emplace(vertIx);
        }
    }

	for (auto &triangle : triangles) {
        bool relevant = false;
        for (int vertIx : triangle.verts) {
            relevant |= (brokenVerts.count(vertIx) > 0);
		}
        if (!relevant) {
            continue;
        }
        Math::Vec3 faceNormal = this->getFaceNormal(triangle.verts);
        for (int vertIx : triangle.verts) {
            if (!onlyBroken || brokenVerts.count(vertIx) > 0) {
                vertices[vertIx].normal += faceNormal;
            }
		}
	}

    for (int vertIx = 0; vertIx < vertices.size(); vertIx ++) {
        if (onlyBroken && brokenVerts.count(vertIx) == 0) {
            continue;
        }
        RawVertex &vertex = vertices[vertIx];
        if (vertex.normal.LengthSquared() < FLT_MIN) {
            vertex.normal = vertex.position - averagePos;
            if (vertex.normal.LengthSquared() < FLT_MIN) {
				vertex.normal = Math::Vec3(0.0f, 1.0f, 0.0f);// { 0.0f, 1.0f, 0.0f };
                continue;
            }
        }
        vertex.normal.Normalize();
	}
    return onlyBroken ? brokenVerts.size() : vertices.size();
}

void RawModel::ComputeTangentBasis()
{
	if ((GetVertexAttributes() & RAW_VERTEX_ATTRIBUTE_TANGENT) == 0)
	{
		if ((GetVertexAttributes() & RAW_VERTEX_ATTRIBUTE_UV0) != 0)
		{
			for (int i = 0; i < triangles.size(); i++)
			{
				int index1 = triangles[i].verts[0];
				int index2 = triangles[i].verts[1];
				int index3 = triangles[i].verts[2];

				if (index1 == 2197 || index2 == 2197 || index3 == 2197)
					int kkk = 3;

				RawVertex vertex1 = vertices[index1];
				RawVertex vertex2 = vertices[index2];
				RawVertex vertex3 = vertices[index3];

				Math::Vec3 pos0 = vertex1.position;
				Math::Vec3 pos1 = vertex2.position;
				Math::Vec3 pos2 = vertex3.position;

				Math::Vec2 uv0 = vertex1.uv0;
				Math::Vec2 uv1 = vertex2.uv0;
				Math::Vec2 uv2 = vertex3.uv0;

				if (uv1 == uv2)
					uv2 = uv2 + Math::M_EPSILON;  //±ÜÃâ³ý0

				if (uv1 == uv0)
					uv1 = uv1 + Math::M_EPSILON;

				if (uv2 == uv0)
					uv2 = uv2 + Math::M_EPSILON;

				Math::Vec3 deltaPos1 = pos1 - pos0;
				Math::Vec3 deltaPos2 = pos2 - pos0;

			
				Math::Vec2 deltaUV1 = uv1 - uv0;
				Math::Vec2 deltaUV2 = uv2 - uv0;
				float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

				if (!std::isfinite(r))
				{
					continue;
				}

				if (abs(deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x) < 0.0000000000001)
					int kkkkk = 3;
				Math::Vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
				Math::Vec3 binormal = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

				vertices[index1].tangent = tangent;
				vertices[index2].tangent = tangent;
				vertices[index3].tangent = tangent;

				vertices[index1].binormal = binormal;
				vertices[index2].binormal = binormal;
				vertices[index3].binormal = binormal;
			}

			//Õý½»»¯
			for (unsigned int i = 0; i < vertices.size(); i += 1)
			{
				RawVertex& vertex = vertices[i];
				Math::Vec3& n = vertex.normal;
				Math::Vec3& t = vertex.tangent;
				Math::Vec3& b = vertex.binormal;
		
				t = t - n * n.Dot(t);
				t.Normalize();
				if (n.Cross(t).Dot(b) < 0.0f)
					t = t * - 1.0f;
				b.Normalize();

				//test
				int result = t.Dot(b);
				int kkk = 3;

			}
		}
	}
}

void RawModel::CheckDuplicatedNames(RawNode& rootNode)
{
	 std::set<std::string> namesSet;

	 for (size_t i = 0; i < rootNode.childIds.size(); ++i)
	 {
		 long childId = rootNode.childIds.at(i);
		 int childInd = GetNodeById(childId);
		 RawNode& childNode = GetNode(childInd);
		 
		 auto name_itr = namesSet.find(childNode.name);
		 if (name_itr == namesSet.cend())
		 {
			 namesSet.insert(childNode.name);
		 }
		 else
		 {
			 childNode.name += std::to_string(i);
		 }
	 }

	 for (size_t i = 0; i < rootNode.childIds.size(); ++i)
	 {
		 long childId = rootNode.childIds.at(i);
		 int childInd = GetNodeById(childId);
		 RawNode& childNode = GetNode(childInd);
		 CheckDuplicatedNames(childNode);
	 }
}

void RawModel::ClearMorphInfo()
{
	for (RawAnimation& ra : animations)
	{
		for (RawChannel& rc : ra.channels)
		{
			rc.DestroyMorph();
		}
	}
}
