
#ifndef __RAWMODEL_H__
#define __RAWMODEL_H__

#include <array>
#include <unordered_map>
#include <functional>
#include <set>
#include <memory>
#include <algorithm>

#include "System/Delegate.h"
#include "Math/3DMath.h"
#include <fbxsdk.h>

#include "RawMorph.h"

USING_JYE_CC

/**
 * The variuos situations in which the user may wish for us to (re-)compute normals for our vertices.
 */
enum class ComputeNormalsOption {
    NEVER,      // do not ever compute any normals (results in broken glTF for some sources)
    BROKEN,     // replace zero-length normals in any mesh that has a normal layer
    MISSING,    // if a mesh lacks normals, compute them all
    ALWAYS      // compute a new normal for every vertex, obliterating whatever may have been there before
};

enum class UseLongIndicesOptions {
    NEVER,      // only ever use 16-bit indices
    AUTO,       // use shorts or longs depending on vertex count
    ALWAYS,     // only ever use 32-bit indices
};

struct ConvertOptions
{
	bool useDarco;
	bool useCorto;
	bool bCompress;
	bool bKeepIndex;
};

struct Bounds3
{
	Math::Vec3 bmin;
	Math::Vec3 bmax;
	bool initialized = false;

	void Clear() {
		bmin = Math::Vec3();
		bmax = Math::Vec3();
		initialized = false;
	}

	void AddPoint(const Math::Vec3 &p) {
		if (initialized) {
			for (int ii = 0; ii < 3; ii++) {
				bmin[ii] = std::min(bmin[ii], p[ii]);
				bmax[ii] = std::max(bmax[ii], p[ii]);
			}
		}
		else {
			bmin = p;
			bmax = p;
			initialized = true;
		}
	}
};

struct Bounds4
{
	Math::Vec4 min;
	Math::Vec4 max;
	bool initialized = false;

	void Clear() {
		min = Math::Vec4();
		max = Math::Vec4();
		initialized = false;
	}

	void AddPoint(const Math::Vec4 &p) {
		if (initialized) {
			for (int ii = 0; ii < 4; ii++) {
				min[ii] = std::min(min[ii], p[ii]);
				max[ii] = std::max(max[ii], p[ii]);
			}
		}
		else {
			min = p;
			max = p;
			initialized = true;
		}
	}
};

enum RawVertexAttribute
{
    RAW_VERTEX_ATTRIBUTE_POSITION      = 1 << 0,
    RAW_VERTEX_ATTRIBUTE_NORMAL        = 1 << 1,
    RAW_VERTEX_ATTRIBUTE_TANGENT       = 1 << 2,
    RAW_VERTEX_ATTRIBUTE_BINORMAL      = 1 << 3,
    RAW_VERTEX_ATTRIBUTE_COLOR         = 1 << 4,
    RAW_VERTEX_ATTRIBUTE_UV0           = 1 << 5,
    RAW_VERTEX_ATTRIBUTE_UV1           = 1 << 6,
	RAW_VERTEX_ATTRIBUTE_UV2           = 1 << 7,
	RAW_VERTEX_ATTRIBUTE_UV3           = 1 << 8,
    RAW_VERTEX_ATTRIBUTE_JOINT_INDICES = 1 << 9,
    RAW_VERTEX_ATTRIBUTE_JOINT_WEIGHTS = 1 << 10,

    RAW_VERTEX_ATTRIBUTE_AUTO = 1 << 31
};

struct RawBlendVertex
{
    Math::Vec3 position;
    Math::Vec3 normal;
    Math::Vec4 tangent;
	uint64_t targetId; // 标记这个Vertex是哪个BlendTarget的

    bool operator==(const RawBlendVertex &other) const {
		return position == other.position &&
			normal == other.normal &&
			tangent == other.tangent &&
			targetId == other.targetId;
    }
};

struct RawVertex
{
    RawVertex() :
        polarityUv0(false)
        ,pad1(false)
        ,pad2(false)
        ,pad3(false)
		,position(0.0f,0.0f,0.0f){}

	Math::Vec3 position;
    //Vec3f position { 0.0f };
	Math::Vec3 normal;
	Math::Vec3 binormal;
	Math::Vec3 tangent;
	Math::Vec4 color;
	Math::Vec2 uv0;
	Math::Vec2 uv1;
	Math::Vec2 uv2;
	Math::Vec2 uv3;
	Math::IntVec4 jointIndices;
	Math::Vec4 jointWeights;

    //Vec3f normal { 0.0f };
    //Vec3f binormal { 0.0f };
    //Vec4f tangent { 0.0f };
    //Vec4f color { 0.0f };
    //Vec2f uv0 { 0.0f };
    //Vec2f uv1 { 0.0f };
    //Vec4i jointIndices { 0, 0, 0, 0 };
    //Vec4f jointWeights { 0.0f };
    // end of members that directly correspond to vertex attributes

    // if this vertex participates in a blend shape setup, the surfaceIx of its dedicated mesh; otherwise, -1
    int blendSurfaceIx = -1;
    // the size of this vector is always identical to the size of the corresponding RawSurface.blendChannels
    std::vector<RawBlendVertex> blends { }; // 这里存放了Blend Target的顶点属性 

    bool polarityUv0;
    bool pad1;
    bool pad2;
    bool pad3;

    bool operator==(const RawVertex &other) const;
    size_t Difference(const RawVertex &other) const;
};

class VertexHasher
{
public:
    size_t operator()(const RawVertex &v) const
    {
        size_t seed = 5381;
        const auto hasher = std::hash<float>{};
        seed ^= hasher(v.position[0]) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= hasher(v.position[1]) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        seed ^= hasher(v.position[2]) + 0x9e3779b9 + (seed<<6) + (seed>>2);

		//printf("seed is %d, x is %f, y is %f, z is %f\n", seed, v.position[0], v.position[1], v.position[2]);
        return seed;
    }
};

class VertexLess
{
public:
	bool operator()(const RawVertex &lhs, const RawVertex &rhs) const {
		VertexHasher lh;
		size_t lhash = lh.operator()(lhs);
		VertexHasher rh;
		size_t rhash = rh.operator()(rhs);
		return lhash < rhash;
	}
};

struct RawTriangle
{
    int verts[3];
    int materialIndex;
    int surfaceIndex;
};

enum RawShadingModel
{
    RAW_SHADING_MODEL_UNKNOWN = -1,
    RAW_SHADING_MODEL_CONSTANT,
    RAW_SHADING_MODEL_LAMBERT,
    RAW_SHADING_MODEL_BLINN,
    RAW_SHADING_MODEL_PHONG,
    RAW_SHADING_MODEL_PBR_MET_ROUGH,
    RAW_SHADING_MODEL_MAX
};

static inline std::string Describe(RawShadingModel model) {
    switch(model) {
        case RAW_SHADING_MODEL_UNKNOWN:         return "<unknown>";
        case RAW_SHADING_MODEL_CONSTANT:        return "Constant";
        case RAW_SHADING_MODEL_LAMBERT:         return "Lambert";
        case RAW_SHADING_MODEL_BLINN:           return "Blinn";
        case RAW_SHADING_MODEL_PHONG:           return "Phong";
        case RAW_SHADING_MODEL_PBR_MET_ROUGH:   return "Metallic/Roughness";
        case RAW_SHADING_MODEL_MAX: default:    return "<unknown>";
    }
}

enum RawTextureUsage
{
    RAW_TEXTURE_USAGE_NONE = -1,
    RAW_TEXTURE_USAGE_AMBIENT,
    RAW_TEXTURE_USAGE_DIFFUSE,
    RAW_TEXTURE_USAGE_NORMAL,
    RAW_TEXTURE_USAGE_SPECULAR,
    RAW_TEXTURE_USAGE_SHININESS,
    RAW_TEXTURE_USAGE_EMISSIVE,
    RAW_TEXTURE_USAGE_REFLECTION,
    RAW_TEXTURE_USAGE_ALBEDO,
    RAW_TEXTURE_USAGE_OCCLUSION,
    RAW_TEXTURE_USAGE_ROUGHNESS,
    RAW_TEXTURE_USAGE_METALLIC,
    RAW_TEXTURE_USAGE_MAX
};

static inline std::string Describe(RawTextureUsage usage)
{
    switch (usage) {
        case RAW_TEXTURE_USAGE_NONE:        return "<none>";
        case RAW_TEXTURE_USAGE_AMBIENT:     return "ambient";
        case RAW_TEXTURE_USAGE_DIFFUSE:     return "diffuse";
        case RAW_TEXTURE_USAGE_NORMAL:      return "normal";
        case RAW_TEXTURE_USAGE_SPECULAR:    return "specuar";
        case RAW_TEXTURE_USAGE_SHININESS:   return "shininess";
        case RAW_TEXTURE_USAGE_EMISSIVE:    return "emissive";
        case RAW_TEXTURE_USAGE_REFLECTION:  return "reflection";
        case RAW_TEXTURE_USAGE_OCCLUSION:   return "occlusion";
        case RAW_TEXTURE_USAGE_ROUGHNESS:   return "roughness";
        case RAW_TEXTURE_USAGE_METALLIC:    return "metallic";
        case RAW_TEXTURE_USAGE_MAX:default: return "unknown";
    }
};

enum RawTextureOcclusion
{
    RAW_TEXTURE_OCCLUSION_OPAQUE,
    RAW_TEXTURE_OCCLUSION_TRANSPARENT
};

struct RawTexture
{
    std::string         name;           // logical name in FBX file
    int                 width;
    int                 height;
    int                 mipLevels;
    RawTextureUsage     usage;
    RawTextureOcclusion occlusion;
    std::string         fileName;       // original filename in FBX file
    std::string         fileLocation;   // inferred path in local filesystem, or ""
};

enum RawMaterialType
{
    RAW_MATERIAL_TYPE_OPAQUE,
    RAW_MATERIAL_TYPE_TRANSPARENT,
    RAW_MATERIAL_TYPE_SKINNED_OPAQUE,
    RAW_MATERIAL_TYPE_SKINNED_TRANSPARENT,
};

struct RawMatProps {
    explicit RawMatProps(RawShadingModel shadingModel)
        : shadingModel(shadingModel)
    {}
    const RawShadingModel shadingModel;

    virtual bool operator!=(const RawMatProps &other) const { return !(*this == other); }
    virtual bool operator==(const RawMatProps &other) const { return shadingModel == other.shadingModel; };
};

struct RawTraditionalMatProps : RawMatProps {
    RawTraditionalMatProps(
        RawShadingModel shadingModel,
        const Math::Vec3 &&ambientFactor,
        const Math::Vec4 &&diffuseFactor,
        const Math::Vec3 &&emissiveFactor,
        const Math::Vec3 &&specularFactor,
        const float shininess
    ) : RawMatProps(shadingModel),
          ambientFactor(ambientFactor),
          diffuseFactor(diffuseFactor),
          emissiveFactor(emissiveFactor),
          specularFactor(specularFactor),
          shininess(shininess)
    {}

    const Math::Vec3 ambientFactor;
    const Math::Vec4 diffuseFactor;
    const Math::Vec3 emissiveFactor;
    const Math::Vec3 specularFactor;
    const float shininess;

    bool operator==(const RawMatProps &other) const override {
        if (RawMatProps::operator==(other)) {
            const auto &typed = (RawTraditionalMatProps &) other;
            return ambientFactor == typed.ambientFactor &&
                diffuseFactor == typed.diffuseFactor &&
                specularFactor == typed.specularFactor &&
                emissiveFactor == typed.emissiveFactor &&
                shininess == typed.shininess;
        }
        return false;
    }
};

struct RawMetRoughMatProps : RawMatProps {
    RawMetRoughMatProps(
        RawShadingModel shadingModel,
        const Math::Vec4 &&diffuseFactor,
        const Math::Vec3 &&emissiveFactor,
        float emissiveIntensity,
        float metallic,
        float roughness
    ) : RawMatProps(shadingModel),
      diffuseFactor(diffuseFactor),
      emissiveFactor(emissiveFactor),
      emissiveIntensity(emissiveIntensity),
      metallic(metallic),
      roughness(roughness)
    {}
    const Math::Vec4 diffuseFactor;
    const Math::Vec3 emissiveFactor;
    const float emissiveIntensity;
    const float metallic;
    const float roughness;

    bool operator==(const RawMatProps &other) const override {
        if (RawMatProps::operator==(other)) {
            const auto &typed = (RawMetRoughMatProps &) other;
            return diffuseFactor == typed.diffuseFactor &&
            emissiveFactor == typed.emissiveFactor &&
            emissiveIntensity == typed.emissiveIntensity &&
            metallic == typed.metallic &&
            roughness == typed.roughness;
        }
        return false;
    }
};

struct RawMaterial
{
    std::string                  name;
    RawMaterialType              type;
    std::shared_ptr<RawMatProps> info;
    int                          textures[RAW_TEXTURE_USAGE_MAX];
};

struct RawBlendChannel
{
    float defaultDeform;
    bool hasNormals;
    bool hasTangents;
	std::string name;
};

struct RawSurface
{
	RawSurface() {}
    long                         id;
    std::string                  name;                            // The name of this surface
    long                         skeletonRootId;  // The id of the root node of the skeleton.
	Math::Vec3                minP;
	Math::Vec3                maxP;
	Bounds3                      bounds;
    std::vector<long>            jointIds;
    std::vector<Math::Vec3>           jointGeometryMins;
    std::vector<Math::Vec3>           jointGeometryMaxs;
    std::vector<Math::Mat4>           inverseBindMatrices;
	FbxAMatrix                        meshGlobalInverseMatrix;
	FbxNode*                          meshNode;
    std::vector<RawBlendChannel> blendChannels;
    bool                         discrete;
	int maxBone;    //实际最大关联骨骼数
	Math::Vec3 translation;
	Math::Quaternion rotation;
	Math::Vec3 scale;
	std::vector<std::vector<int>>	splitedjointsAffectIDs;
	uint							subMeshCount;
	std::vector<Math::AABB>		bonesBoundingBox;
};

template<typename T>
struct animationKey
{
	T value;
	float time;
	uint8_t tangentMode;

	float leftTan;
	float rightTan;
};

typedef std::vector<animationKey<float>> Vec1fKeyFrames;
typedef std::vector<animationKey<Math::Quaternion>> QuatfKeyFrames;

struct RawMorphChannel;

struct MorphInfo
{
	RawMorphChannel* pRawMorphChannel;
	Vec1fKeyFrames	 floatKeys;
	int				 ind;

	MorphInfo()
		: pRawMorphChannel(NULL),
		  ind(-1)
	{

	};

};

struct RawChannel
{
	int                parentIndex;
    int                nodeIndex;
	std::vector<float> times;
    std::vector<Math::Vec3> translations;
    std::vector<Math::Quaternion> rotations;
	std::vector<Math::Vec3> eulers;
    std::vector<Math::Vec3> scales;
    std::vector<float> weights;
	std::vector<Math::Mat4> transforms; //直接保存矩阵

	Vec1fKeyFrames		translationKeys[3];
	Vec1fKeyFrames		scaleKeys[3];
	Vec1fKeyFrames		eulerKeys[3];
	QuatfKeyFrames		quatKeys;
	
	std::vector<MorphInfo>	morphInfoVec;

	void DestroyMorph()
	{
		for (MorphInfo& mi : morphInfoVec)
		{
			if (mi.pRawMorphChannel != NULL)
			{
				delete mi.pRawMorphChannel;
				mi.pRawMorphChannel = NULL;
			}
		}
		morphInfoVec.clear();
	}

};

struct RawAnimation
{
    std::string             name;
    std::vector<float>      times;
    std::vector<RawChannel> channels;
	bool isHasScale;  //如果有scale的话，用导出矩阵，否则导出双四元数
	int beginFrame;
	float frameRate;
	float startTime;
	float endTime;
};

struct RawCamera
{
    std::string name;
    long        nodeId;

    enum
    {
        CAMERA_MODE_PERSPECTIVE,
        CAMERA_MODE_ORTHOGRAPHIC
    } mode;

    struct
    {
        float aspectRatio;
        float fovDegreesX;
        float fovDegreesY;
        float nearZ;
        float farZ;
    } perspective;

    struct
    {
        float magX;
        float magY;
        float nearZ;
        float farZ;
    } orthographic;
};
static const Math::Mat4 ZeroMat(0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f);

class RawModel;
struct RawNode
{
	
	RawNode()
		: 
		  hasMorph(false),
		  hasMesh(false),
		  hasSkinning(false),
		  id(-1)
	{
		
	}
    bool                     isJoint;//是不是被蒙皮使用。
	bool                     isSkeleton;//是不是骨骼
    long                     id;
    std::string              name;
	std::string				 meshname;
    long                     parentId;
    std::vector<long>        childIds;
	Math::Vec3            translation;
    Math::Quaternion         rotation;
	Math::Vec3            scale;
	int                      morphAnimationId;
	int                      morpherId;


    long                     surfaceId;
	bool					 hasMesh;
	bool					 hasMorph;
	bool					 hasSkinning;
};

struct SubmeshInfo
{
	size_t baseIndex;
	size_t IndexNum;

	SubmeshInfo()
		: baseIndex(0),
		  IndexNum(0)
	{

	}
};

class RawModel
{
public:
    RawModel();

    // Add geometry.
    void AddVertexAttribute(const RawVertexAttribute attrib);
	void InitVerticeArray(int count);
	int AddVertex(const RawVertex &vertex);
    int AddVertex(const RawVertex &vertex, int index);
	void AddVertexInSplitMesh(const RawVertex &vertex);
    int AddTriangle(const int v0, const int v1, const int v2, const int materialIndex, const int surfaceIndex);
    int AddTexture(const std::string &name, const std::string &fileName, const std::string &fileLocation, RawTextureUsage usage);
    int AddMaterial(const RawMaterial &material);
    int AddMaterial(
        const char *name, const RawMaterialType materialType, const int textures[RAW_TEXTURE_USAGE_MAX],
        std::shared_ptr<RawMatProps> materialInfo);
    int AddSurface(const RawSurface &suface);
    int AddSurface(const char *name, long surfaceId);
    int AddAnimation(const RawAnimation &animation);
    int AddCameraPerspective(
        const char *name, const long nodeId, const float aspectRatio, const float fovDegreesX, const float fovDegreesY,
        const float nearZ, const float farZ);
    int
    AddCameraOrthographic(const char *name, const long nodeId, const float magX, const float magY, const float nearZ, const float farZ);
    int AddNode(const RawNode &node);
    int AddNode(const long id, const char *name, const long parentId);
    void SetRootNode(const long nodeId) { rootNodeId = nodeId; }
    const long GetRootNode() const { return rootNodeId; }

    // Remove unused vertices, textures or materials after removing vertex attributes, textures, materials or surfaces.
    void Condense();

    void TransformGeometry(ComputeNormalsOption);

    void TransformTextures(const std::vector<std::function<Math::Vec2(Math::Vec2)>> &transforms);

    size_t CalculateNormals(bool);

    // Get the attributes stored per vertex.
    int GetVertexAttributes() const { return vertexAttributes; }

    // Iterate over the vertices.
    int GetVertexCount() const {
		return (int) vertices.size(); 
	}
    const RawVertex &GetVertex(const int index) const { return vertices[index]; }
	void SetVertexJointsIndex(int i, Math::IntVec4 &jointsIndex)
	{ 
		vertices[i].jointIndices[0] = jointsIndex[0];
		vertices[i].jointIndices[1] = jointsIndex[1];
		vertices[i].jointIndices[2] = jointsIndex[2];
		vertices[i].jointIndices[3] = jointsIndex[3];
	}
	RawVertex &GetEditableVertex(const int index) { return vertices[index]; }

    // Iterate over the triangles.
    int GetTriangleCount() const { return (int) triangles.size(); }
    const RawTriangle &GetTriangle(const int index) const { return triangles[index]; }

	const std::vector<RawTriangle>& GetTriangles() const { return triangles; }
	void SetTriangle(int triangleIndex, int vertIndex ,int indice)
	{
		triangles[triangleIndex].verts[vertIndex] = indice;
	}

	std::vector<RawTriangle>& GetEditableTriangle() {
		return triangles;
	}

    // Iterate over the textures.
    int GetTextureCount() const { return (int) textures.size(); }
    const RawTexture &GetTexture(const int index) const { return textures[index]; }

    // Iterate over the materials.
    int GetMaterialCount() const { return (int) materials.size(); }
    const RawMaterial &GetMaterial(const int index) const { return materials[index]; }

    // Iterate over the surfaces.
    int GetSurfaceCount() const { return (int) surfaces.size(); }
    const RawSurface &GetSurface(const int index) const { return surfaces[index]; }
    RawSurface &GetSurface(const int index) { return surfaces[index]; }
    int GetSurfaceById(const long id) const;

    // Iterate over the animations.
    int GetAnimationCount() const { return (int) animations.size(); }
    const RawAnimation &GetAnimation(const int index) const { return animations[index]; }
	//RawAnimation &GetEditableAnimation(const int index) { return animations[index];  }

	int GetMorphAnimationCount() const { return (int)morphanimations.size(); }
	const RawMorphAnimation &GetMorphAnimation(const int index) const { return morphanimations[index]; }
	RawMorphAnimation &GetEditableAnimation(const int index) { return morphanimations[index]; }
	int AddMorphAnimation(const RawMorphAnimation &animation)
	{
		morphanimations.emplace_back(animation);
		return (int)(morphanimations.size() - 1);
	}


    // Iterate over the cameras.
    int GetCameraCount() const { return (int) cameras.size(); }
    const RawCamera &GetCamera(const int index) const { return cameras[index]; }

    // Iterate over the nodes.
    int GetNodeCount() const { return (int) nodes.size(); }
    const RawNode &GetNode(const int index) const { return nodes[index]; }
    RawNode &GetNode(const int index) { return nodes[index]; }
    int GetNodeById(const long nodeId) const;

    // Create individual attribute arrays.
    // Returns true if the vertices store the particular attribute.
    template<typename _attrib_type_>
    void GetAttributeArray(std::vector<_attrib_type_> &out, const _attrib_type_ RawVertex::* ptr) const;

    // Create an array with a raw model for each material.
    // Multiple surfaces with the same material will turn into a single model.
    // However, surfaces that are marked as 'discrete' will turn into separate models.
	void CreateMaterialModels(
		std::vector<RawModel> &materialModels, bool shortIndices, const int keepAttribs, const bool forceDiscrete, bool keepIndex) const;

	//计算切线和福发现
	void ComputeTangentBasis();
	//确定是否有动画，没有不导出
	int animationsSize() const { return animations.size(); }

	void SetOriginalIndex(int idx) { originalAnimationIndex = idx; }
	int GetOriginalIndex() const { return originalAnimationIndex; }

	void CheckDuplicatedNames(RawNode& rootNode);

	void ClearMorphInfo();

private:
    Math::Vec3 getFaceNormal(int verts[3]) const;

    long                                             rootNodeId;
    int                                              vertexAttributes;
	int												 originalAnimationIndex;
    std::unordered_map<RawVertex, int, VertexHasher> vertexHash;
	//std::map<RawVertex,int, VertexLess>             vertexHash;
    std::vector<RawVertex>                           vertices;
    std::vector<RawTriangle>                         triangles;
    std::vector<RawTexture>                          textures;
    std::vector<RawMaterial>                         materials;
    std::vector<RawSurface>                          surfaces;
    std::vector<RawAnimation>                        animations;
	std::vector<RawMorphAnimation>					 morphanimations;
    std::vector<RawCamera>                           cameras;
    std::vector<RawNode>                             nodes;

public:
	std::string                                      meshName;
	std::vector<long>                                affectedBoneIDs;
	std::vector<RawNode*>                             BoneNodes;//专门把是骨骼的node单独记录一下，方便导出和查找。
	std::vector<Math::Mat4>						 inverseBindMats;
	std::vector<SubmeshInfo>						 subMeshInfos;
};

template<typename _attrib_type_>
void RawModel::GetAttributeArray(std::vector<_attrib_type_> &out, const _attrib_type_ RawVertex::* ptr) const
{
    out.resize(vertices.size());
    for (size_t i = 0; i < vertices.size(); i++) {
        out[i] = vertices[i].*ptr;
    }
}

#endif // !__RAWMODEL_H__
