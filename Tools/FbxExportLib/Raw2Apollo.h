#pragma once

#include "RawModel.h"
#include <draco/compression/encode.h>
#include <draco/compression/decode.h>
#include <fstream>


struct ComponentType {
	enum GL_DataType
	{
		GL_BYTE = 5120,
		GL_UNSIGNED_BYTE,
		GL_SHORT,
		GL_UNSIGNED_SHORT,
		GL_INT,
		GL_UNSIGNED_INT,
		GL_FLOAT
	};

	const GL_DataType glType;
	const unsigned int size;
};

const ComponentType CT_USHORT = { ComponentType::GL_UNSIGNED_SHORT, 2 };
const ComponentType CT_UINT = { ComponentType::GL_UNSIGNED_INT, 4 };
const ComponentType CT_FLOAT = { ComponentType::GL_FLOAT, 4 };


struct GLType {
	GLType(const ComponentType &componentType, unsigned int count, const std::string dataType)
		: componentType(componentType),
		count(count),
		dataType(dataType)
	{}

	unsigned int byteStride() const { return componentType.size * count; }

	void write(uint8_t *buf, const float scalar) const { *((float *)buf) = scalar; }
	void write(uint8_t *buf, const uint32_t scalar) const {
		switch (componentType.size) {
		case 1:
			*buf = (uint8_t)scalar;
			break;
		case 2:
			*((uint16_t *)buf) = (uint16_t)scalar;
			break;
		case 4:
			*((uint32_t *)buf) = scalar;
			break;
		}
	}

	
	void write(uint8_t *buf, const Math::Vec3& source) const {
		((float*)buf)[0] = source.x;
		((float*)buf)[1] = source.y;
		((float*)buf)[2] = source.z;
	}
	void write(uint8_t *buf, const Math::Vec2& source) const {
		((float*)buf)[0] = source.x;
		((float*)buf)[1] = source.y;
	}
	void write(uint8_t *buf, const Math::Vec4& source) const {
		((float*)buf)[0] = source.x;
		((float*)buf)[1] = source.y;
		((float*)buf)[2] = source.z;
		((float*)buf)[3] = source.w;
	}
	void write(uint8_t *buf, const Math::IntVec4& source) const {
		((int*)buf)[0] = source.x;
		((int*)buf)[1] = source.y;
		((int*)buf)[2] = source.z;
		((int*)buf)[3] = source.w;
	}

	void write(uint8_t *buf, const std::array<uint16_t, 4>& source) const {
		((uint16_t*)buf)[0] = source[0];
		((uint16_t*)buf)[1] = source[1];
		((uint16_t*)buf)[2] = source[2];
		((uint16_t*)buf)[3] = source[3];
	}


	void write(uint8_t *buf, const Math::Quaternion& source) const {
		((float*)buf)[0] = source.x;
		((float*)buf)[1] = source.y;
		((float*)buf)[2] = source.z;
		((float*)buf)[3] = source.w;
	}

	void write(uint8_t *buf, const Math::Mat4& source) const {
		for (int i = 0; i < 16; i++)
		{
			((float*)buf)[i] = source.m[i];
		}
	}

	const ComponentType componentType;
	const uint8_t       count;
	const std::string   dataType;
};

const GLType GLT_FLOAT = { CT_FLOAT, 1, "SCALAR" };
const GLType GLT_USHORT = { CT_USHORT, 1, "SCALAR" };
const GLType GLT_UINT = { CT_UINT, 1, "SCALAR" };
const GLType GLT_VEC2F = { CT_FLOAT, 2, "VEC2" };
const GLType GLT_VEC3F = { CT_FLOAT, 3, "VEC3" };
const GLType GLT_VEC4F = { CT_FLOAT, 4, "VEC4" };
const GLType GLT_VEC4I = { CT_USHORT, 4, "VEC4" };
const GLType GLT_MAT2F = { CT_USHORT, 4, "MAT2" };
const GLType GLT_MAT3F = { CT_USHORT, 9, "MAT3" };
const GLType GLT_MAT4F = { CT_FLOAT, 16, "MAT4" };
const GLType GLT_QUATF = { CT_FLOAT, 4, "VEC4" };

inline float _clamp(float d, float bottom = 0, float top = 1) {
	return std::max(bottom, std::min(top, d));
}
inline Math::Vec3 _clamp(const Math::Vec3 &vec, const Math::Vec3 &bottom = Math::Vec3(0.0, 0.0, 0.0), const Math::Vec3 &top = Math::Vec3(1.0, 1.0, 1.0)) {
	Math::Vec3 tmp(std::min(top.x, vec.x), std::min(top.y, vec.y), std::min(top.z, vec.z));
	Math::Vec3 res(std::max(bottom.x, tmp.x), std::max(bottom.y, tmp.y), std::max(bottom.z, bottom.z));
	return res;
}
inline Math::Vec4 _clamp(const Math::Vec4 &vec, const Math::Vec4 &bottom = Math::Vec4(0.0, 0.0, 0.0, 0.0), const Math::Vec4 &top = Math::Vec4(1.0, 1.0, 1.0, 1.0)) {

	Math::Vec4 tmp(std::min(top.x, vec.x), std::min(top.y, vec.y), std::min(top.z, vec.z), std::min(top.w, vec.w));
	Math::Vec4 res(std::max(bottom.x, tmp.x), std::max(bottom.y, tmp.y), std::max(bottom.z, bottom.z), std::max(bottom.w, tmp.w));
	return res;
}
void InitRaw2Apollo();


struct Holdable
{
	uint32_t ix;
};

//用来缓存二进制数据
struct BufferData : public Holdable
{
	explicit BufferData(const std::shared_ptr<std::vector<uint8_t> > &binData)
		:binData(binData)
	{

	}

	~BufferData()
	{

	}

	std::shared_ptr<std::vector<uint8_t> > binData; 
};


template<class T>
struct AttributeDefinitionWithDraco
{
	const std::string                    apolloName;   
	const T RawVertex::*                 rawAttributeIx;
	const GLType                         glType;
	const draco::GeometryAttribute::Type dracoAttribute;
	const draco::DataType                dracoComponentType;

	AttributeDefinitionWithDraco(
		const std::string apploName, const T RawVertex::*rawAttributeIx, const GLType &_glType,
		const draco::GeometryAttribute::Type dracoAttribute, const draco::DataType dracoComponentType)
		: apolloName(apploName),
		rawAttributeIx(rawAttributeIx),
		glType(_glType),
		dracoAttribute(dracoAttribute),
		dracoComponentType(dracoComponentType) {}

	AttributeDefinitionWithDraco(
		const std::string apploName, const T RawVertex::*rawAttributeIx, const GLType &_glType)
		: apolloName(apploName),
		rawAttributeIx(rawAttributeIx),
		glType(_glType),
		dracoAttribute(draco::GeometryAttribute::INVALID),
		dracoComponentType(draco::DataType::DT_INVALID) {}

};


enum CHUNK_IDENTIFIER
{
	APOLLO_MESH = 100,
	APOLLO_BONES,
	APOLLO_SKELETON,
	APOLLO_ANIMATION,
	CHUNK_END
};

template<typename T>
struct Holder
{
	std::vector<std::shared_ptr<T>> ptrs;
	std::shared_ptr<T> hold(T *ptr)
	{
		ptr->ix = ptrs.size();
		ptrs.emplace_back(ptr);
		return ptrs.back();
	}
};

//在buffer中定位数据
struct BufferLocator : Holdable
{
	enum GL_ArrayType
	{
		GL_ARRAY_NONE = 0, // no GL buffer is being set
		GL_ARRAY_BUFFER = 34962,
		GL_ELEMENT_ARRAY_BUFFER = 34963
	};

	BufferLocator(const BufferData &_buffer, const size_t _byteOffset);


	const unsigned int buffer;
	const unsigned int byteOffset;

	unsigned int byteLength = 0;
};

struct HeaderData : public Holdable
{
	HeaderData(const BufferLocator& header)
		:headerLocatorIndex(header.ix)
	{

	}

	int headerLocatorIndex;
};

/*
layout:
	chunk_identifier
	struct
	{
		unsigned int boneHierachyOfset;
		unsigned int boneHierachyByteLength;
		unsigned int inverseBindMatrixOffset;
		unsigned int inverseBindMatrixByteLength;
	}

*/
struct SkeletonData : public Holdable
{
	SkeletonData(const BufferLocator& boneH,const BufferLocator& inversematrix)
		: boneHierarchyLocatorIndex(boneH.ix)
		, inverseBindMatrixLocatorIndex(inversematrix.ix)
		, chunkIdentifier(APOLLO_SKELETON)
	{}

	int boneHierarchyLocatorIndex;
	int inverseBindMatrixLocatorIndex;
	int chunkIdentifier;
	int animationType;  //动画类型（标识是四元数动画还是矩阵动画)
	std::string meshName; //对应的mesh name


};

struct SamplerData : Holdable
{
	SamplerData()
		: Holdable()
	{
	}
};

struct ImageData : Holdable
{
	ImageData(std::string name, std::string uri)
		: Holdable(),
		name(std::move(name)),
		uri(std::move(uri))
	{
	}


	const std::string name;
	const std::string uri;
	const std::string mimeType;
};


struct TextureData : Holdable
{
	TextureData(std::string name, const SamplerData &sampler, const ImageData &source)
		: Holdable(),
		name(std::move(name)),
		sampler(sampler.ix),
		source(source.ix)
	{

	}


	const std::string name;
	const uint32_t    sampler;
	const uint32_t    source;
};

struct Tex
{
	static std::unique_ptr<Tex> ref(const TextureData *tex, uint32_t texCoord = 0)
	{
		return std::unique_ptr<Tex> { (tex != nullptr) ? new Tex(tex->ix, texCoord) : nullptr };
	}
	explicit Tex(uint32_t texRef, uint32_t texCoord)
		: texRef(texRef),
		texCoord(texCoord) {}

	const uint32_t texRef;
	const uint32_t texCoord;
};

struct PBRMetallicRoughness
{
	PBRMetallicRoughness(
		const TextureData *baseColorTexture, const TextureData *metRoughTexture,
		const Math::Vec4 &baseColorFactor, float metallic = 0.1f, float roughness = 0.6f)
		: baseColorTexture(Tex::ref(baseColorTexture)),
		metRoughTexture(Tex::ref(metRoughTexture)),
		baseColorFactor(_clamp(baseColorFactor)),
		metallic((metallic)),
		roughness((roughness))
	{
	}

	std::unique_ptr<Tex> baseColorTexture;
	std::unique_ptr<Tex> metRoughTexture;
	const Math::Vec4          baseColorFactor;
	const float          metallic;
	const float          roughness;
};


struct KHRCmnUnlitMaterial
{
	KHRCmnUnlitMaterial()
	{

	}
};


struct MaterialData : Holdable
{
	MaterialData(
		std::string name, bool isTransparent, RawShadingModel shadingModel,
		const TextureData *normalTexture, const TextureData *occlusionTexture,
		const TextureData *reflectionTexture,
		const TextureData *emissiveTexture, const Math::Vec3 &emissiveFactor,
		std::shared_ptr<KHRCmnUnlitMaterial> const khrCmnConstantMaterial,
		std::shared_ptr<PBRMetallicRoughness> const pbrMetallicRoughness)
		: Holdable(),
		name(std::move(name)),
		shadingModel(shadingModel),
		isTransparent(isTransparent),
		normalTexture(Tex::ref(normalTexture)),
		occlusionTexture(Tex::ref(occlusionTexture)),
		reflectionTexture(Tex::ref(reflectionTexture)),
		emissiveTexture(Tex::ref(emissiveTexture)),
		emissiveFactor(_clamp(emissiveFactor)),
		khrCmnConstantMaterial(khrCmnConstantMaterial),
		pbrMetallicRoughness(pbrMetallicRoughness) {}


	const std::string                name;
	const RawShadingModel            shadingModel;
	const bool                       isTransparent;
	const std::unique_ptr<const Tex> normalTexture;
	const std::unique_ptr<const Tex> occlusionTexture;
	const std::unique_ptr<const Tex> reflectionTexture;
	const std::unique_ptr<const Tex> emissiveTexture;
	const Math::Vec3                      emissiveFactor;

	const std::shared_ptr<const KHRCmnUnlitMaterial>    khrCmnConstantMaterial;
	const std::shared_ptr<const PBRMetallicRoughness>   pbrMetallicRoughness;
};


/*
layout:
		 chunk_identifier
		 struct
		 {
			vertexLayout,
			dataOffset
			dataLength
			skeletonIndex
		 }
		 
*/


struct MeshData : Holdable
{
	MeshData(const std::string& meshName, const BufferLocator& locator, int vertexLayout, const std::shared_ptr<BufferLocator>& jointLocator)
		:locatorIndex(locator.ix)
		, skeletonDataLocatorIndex(-1)
		, vertexLayout(vertexLayout)
		, chunkIdentifier(APOLLO_MESH)
		, name(meshName)
		, materialIndex(0)
	{
		if (jointLocator != nullptr)
		{
			jointsID = jointLocator->ix;
		}
	}

	void AddAffectjointIDLocator(const std::shared_ptr<BufferLocator>& jointLocator)
	{
		joinsAffectIDslocator.push_back(jointLocator->ix);
	}

	void SetSkeleton(std::shared_ptr<SkeletonData> skeleton)
	{
		skeletonDataLocatorIndex = skeleton->ix;//
		skeleton->meshName = name;
	}

	void SetMaxBoneCnt(int boneCnt)
	{
		maxBone = boneCnt;
	}

	void SetMaterialIndex(int matIndex)
	{
		materialIndex = matIndex;
	}

	void SetDiffuseTex(const std::string& diffuse)
	{
		diffuseTex = diffuse;
	}

	void SetNormalTex(const std::string& normal)
	{
		normalTex = normal;
	}

	void SetMaterialType(int type)
	{
		materialType = type;
	}

	void SetTriangleCount(const std::shared_ptr<BufferLocator>& countLocator)
	{
		triangleCnt = countLocator->ix;
	}

	void SetSubMeshCount(int a)
	{
		subMeshCnt = a;
	}

	int subMeshCnt;
	int triangleCnt;
	std::vector<int> joinsAffectIDslocator;
	int jointsID;
	int locatorIndex;
	int vertexLayout;  //顶点格式
	int skeletonDataLocatorIndex;
	int maxBone;
	int chunkIdentifier;
	std::string name;
	int materialIndex;
	std::string diffuseTex;
	std::string normalTex;
	int materialType; //材质类型，目前只区分：透明和不透明（0： 不透明，1： 透明）
	bool isSkinning;
	std::string animationName;
	int  frameCount;
	bool isHasScale;  //动画数据是否有scale
	//mesh的全局变换
	Math::Vec3 translation;
	Math::Quaternion rotation;
	Math::Vec3 scale;
	Math::Vec3 minP;
	Math::Vec3 maxP;
	std::vector<long> affectedBoneIDs;
	//int materialIndex;  //mesh 附带的materal

};

enum AnimationTransformAttribute
{
	ATA_TRANSLATION = 1 << 0,
	ATA_ROTATION = 1 << 1,
	ATA_SCALE = 1 << 2
};

/*
layout:
		chunk_identifier
		struct
		{
			char name[64],  //暂时将name长度固定位64个字节（TODO)
			unsigned int timesOffset;
			unsigned int timesByteLength;
			unsigned int transformAttributeOffset;
			unsigned int transformAttributeByteLength;
			unsigned int rotationOffset;
			unsigned int rotationLength;
			unsigned int translationOffset;
			unsibned int translationByteLength;
			unsigned int scaleOffset;
			unsigned int scaleByteLength;
			unsigned int boneIDOffset;
			unsigned int boneIDByteLength;
		}

*/
struct AnimationData : public Holdable
{
	AnimationData(const std::string& name, const BufferLocator& locator, int count, int begin)
		: name(name)
		, timesIndex(locator.ix)
		, transformAttributeIndex(-1)
		, translationOffsetIndex(-1)
		, translationLengthIndex(-1)
		, rotationOffsetIndex(-1)
		, rotationLengthIndex(-1)
		, scaleOffsetIndex(-1)
		, scaleLengthIndex(-1)
		, boneIDsIndex(-1)
		, frameCount(count)
		, beginFrame(begin)
		, chunkIdentifier(APOLLO_ANIMATION)
	{

	}

	int transformAttributeIndex;
	int boneIDsIndex;
	int parentBoneIDsIndex;

	int translationOffsetIndex;
	int translationLengthIndex;
	int rotationOffsetIndex;
	int rotationLengthIndex;
	int scaleOffsetIndex;
	int scaleLengthIndex;
	int transformOffsetIndex;
	int transformLengthIndex;

	int keyTimeOffsetIndex;
	int keyTimeLengthIndex;

	int timesIndex;
	int chunkIdentifier;
	std::string name;
	bool isHasScale;
	int animationType; //
	int frameCount;
	int beginFrame;
};










class ApolloBinData
{
public:
	explicit ApolloBinData()
		:meshBuffer(0)
		,skeletonBuffer(0)
		,animationBuffer(0)
		,meshBianry(new std::vector<uint8_t>())
		,skeletonBinary(new std::vector<uint8_t>())
		, skeletonBinaryV2(new std::vector<uint8_t>())
	{
		meshBuffer = new BufferData(std::shared_ptr<std::vector<uint8_t>>(meshBianry));
		skeletonBuffer = new BufferData(std::shared_ptr<std::vector<uint8_t>>(skeletonBinary));
		skeletonBufferV2 = new BufferData(std::shared_ptr<std::vector<uint8_t>>(skeletonBinaryV2));
		buffers.hold(meshBuffer);
		buffers.hold(skeletonBuffer);
		buffers.hold(skeletonBufferV2);
	}

	~ApolloBinData()
	{

	}

	std::shared_ptr<BufferLocator> GetAlignedBufferLocator(BufferData &buffer)
	{
		//unsigned long bufferSize = this->binary->size();
		unsigned long bufferSize = buffer.binData->size();
// 		if ((bufferSize % 4) > 0) {
// 			bufferSize += (4 - (bufferSize % 4));
// 			buffer.binData->resize(bufferSize);
// 			//this->binary->resize(bufferSize);
// 		}
		return this->bufferLocs.hold(new BufferLocator(buffer, bufferSize));
	}

	std::shared_ptr<BufferLocator> AddRawBuffer(BufferData &buffer, const char *source, uint32_t bytes)
	{
		auto locator = GetAlignedBufferLocator(buffer);
		locator->byteLength = bytes;

		//unsigned long bufferSize = this->binary->size();
		unsigned long bufferSize = buffer.binData->size();
		buffer.binData->resize(bufferSize + bytes);
		//this->binary->resize(bufferSize + bytes);
		memcpy(&(*buffer.binData)[bufferSize], source, bytes);
		//memcpy(&(*this->binary)[bufferSize], source, bytes);
		return locator;
	}

	template<class T>
	std::shared_ptr<BufferLocator> AddTypedBuffer(BufferData& buffer, const GLType &type, const std::vector<T> &source)
	{
		auto locator = GetAlignedBufferLocator(buffer);
		const unsigned int stride = type.byteStride();
		const size_t       offset = buffer.binData->size();
		//const size_t       offset = binary->size();
		const size_t       count = source.size();

		//binary->resize(offset + count * stride);
		buffer.binData->resize(offset + count * stride);
		for (int ii = 0; ii < count; ii++) {
			type.write(&((*buffer.binData)[offset + ii * stride]), source[ii]);
			//type.write(&((*binary)[offset + ii * stride]), source[ii]);
		}
		locator->byteLength = type.byteStride() * count;
		return locator;
	}

	void Serialze(const std::string& outputPath);

public:
	//std::shared_ptr<std::vector<uint8_t> > binary;
	std::vector<uint8_t>* meshBianry;
	std::vector<uint8_t>* skeletonBinary;
	std::vector<uint8_t>* skeletonBinaryV2;
	std::vector<std::vector<uint8_t>*> animationBinary;
	BufferData* meshBuffer;
	BufferData* skeletonBuffer;
	BufferData* skeletonBufferV2;
	std::vector<BufferData*> animationBuffer;
	//std::vector<BufferData*> skeletonBuffer;
	//BufferData *buffer;
	Holder<BufferData> buffers;  //所有的二进制数据
	Holder<BufferLocator> bufferLocs; //用来定位数据块在二进制数据中的offset 和 size
	Holder<MeshData> meshDats;  //所有模型顶点数据
	Holder<AnimationData> aniDats; //所有动画数据
	Holder<SkeletonData> skeDats;  //骨骼数据
	Holder<SkeletonData> skeDatV2;  //骨骼数据
	Holder<HeaderData> Heads; //acturally only one
	Holder<TextureData>    textures; //texture data holder
	Holder<ImageData>      images;
	Holder<SamplerData>    samplers;
	Holder<MaterialData>   materials;
	std::string	originAnimation;
	std::vector<RawMorphAnimation> morpherAnimations;
};



bool Raw2Apollo(const std::string& outputPath, RawModel& rawModel, const ConvertOptions& convOptions);
void Convert2ValidFilePath(std::string& origPath);


