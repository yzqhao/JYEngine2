
#include <algorithm>
#include "Raw2Apollo.h"
#include "File_Utils.h"
#include "String_Utils.h"
#include <memory>
#define RAPIDJSON_NO_INT64DEFINE
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include "STB/stb_image.h"
#include "STB/stb_image_write.h"

using namespace rapidjson;

bool ExportMeshData(ApolloBinData& apolloData, RawModel& surfaceModel, RawModel& rawModel, const ConvertOptions& convOptions,int maxBones);
void ExportAnomationData(ApolloBinData& apolloData, const RawModel& rawModel);
void ExportBoneData(ApolloBinData& apolloData, const RawModel& rawModel);
void ExportBoneData2(ApolloBinData& apolloData, const RawModel& rawModel);
void ExportBoneData3(ApolloBinData& apolloData, const RawModel& rawModel);
void ExportMaterial(const std::string& outputFolder, const RawModel& rawModel);
void ExportMatreial_PBR(ApolloBinData& apolloData, const std::string& outputFolder, const RawModel& rawModel);
void Convert2ValidFilePath(std::string& origPath);
void SplitMesh(ApolloBinData& apolloData, RawModel& surfaceModel, std::vector<int> &trianglesCnt, std::vector<std::vector<int>> &splitedjointsAffectIDs, int maxBones);
void SplitMeshImp(RawModel& surfaceModel, std::vector<int> &trianglesCnt, std::vector<long> &affectIDs, std::vector<std::vector<int>> &splitedAffectIDs, int maxBones);
void SplitSameVertex(RawModel& surfaceModel, std::vector<int> &trianglesCnt, std::vector<std::vector<int>> &splitedIndices);

template<typename T>
T &require(std::map<std::string, std::shared_ptr<T>> map, const std::string &key)
{
	auto iter = map.find(key);
	assert(iter != map.end());
	T &result = *iter->second;
	return result;
}



char apolHeader[] = {
	'a', 'p', 'o', 'l',            // 格式名称
	0x01, 0x00, 0x00, 0x00,        // 版本号
	0x00, 0x00, 0x00, 0x00,        // 二进制数据流起始位置
};
char bindingBoxDemo[] =
{
	0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,
};
int chunkEnd = CHUNK_END;

/**将RawModel输出为自定义二进制格式*/
bool Raw2Apollo(const std::string& outputPath, RawModel& rawModel,const ConvertOptions& convOptions)
{
	//先将模型按照材质区分
	std::vector<RawModel> materialModels;
	rawModel.CreateMaterialModels(
		materialModels,
		true,
		-1,
		true,
		convOptions.bKeepIndex);


	std::unique_ptr<ApolloBinData> apolloData(new ApolloBinData());
	
	//apolloData->buffers.hold(new BufferData(apolloData->binary));
	//ExportMaterial(outputPath, rawModel);
	ExportMatreial_PBR(*apolloData, outputPath, rawModel);

	int maxBones = 30;//拆分后最大骨骼数量

	for (auto &surfaceModel : materialModels)
	{
		surfaceModel.BoneNodes = rawModel.BoneNodes;
		ExportMeshData(*apolloData, surfaceModel,rawModel,convOptions, maxBones);
	}


	for (auto &surfaceModel : materialModels)
	{
		//ExportBoneData3(*apolloData, surfaceModel);
	}
	

	//ExportAnomationData(*apolloData, rawModel);

	

	apolloData->Serialze(outputPath);  //输出到文件


	return true;
}



BufferLocator::BufferLocator(const BufferData &_buffer, const size_t _byteOffset)
	: Holdable(),
	buffer(_buffer.ix),
	byteOffset((unsigned int)_byteOffset)
{
}

//导出骨骼的父子关系
//一套skeleton 一套动画
struct BoneHierarchy
{
	char name[64];
	int  boneID;
	int  parentID;
};

std::map<int, std::shared_ptr<MeshData>> meshDataBySurfaceID;  //缓存骨骼数据和mesh面的关系
std::map<std::string, const RawTexture*>  textureByIndicesKey;  //cache the textue to export
//for now only copy the texture to the output folder

struct TextureStru
{
	std::string diffuseTexFilePath;
	std::string normalTexFilePath;
	int materialType; //0: 不透明 1: 透明

};
std::map<std::string, TextureStru> texturePathByMaterialMaterialName;
void ExportMaterial(const std::string& outputFolder, const RawModel& rawModel)
{
	for (int materialIndex = 0; materialIndex < rawModel.GetMaterialCount(); materialIndex++) 
	{
		const RawMaterial &material = rawModel.GetMaterial(materialIndex);
		
		TextureStru texS;
		//diffuse 贴图
		if (material.textures[RAW_TEXTURE_USAGE_DIFFUSE] >= 0)
		{
			const RawTexture  &rawTexture = rawModel.GetTexture(material.textures[RAW_TEXTURE_USAGE_DIFFUSE]);
			const std::string textureName = StringUtils::GetFileBaseString(rawTexture.name);
			const std::string relativeFilename = StringUtils::GetFileNameString(rawTexture.fileLocation);
			if (!relativeFilename.empty())
			{
				std::string outputPath = outputFolder + "/data/" + relativeFilename;
				std::transform(outputPath.begin(), outputPath.end(), outputPath.begin(), ::tolower);
				if (!FileUtils::CreatePath(outputPath.c_str()))
				{
					
					fprintf(stderr, "ERROR: Failed to create folder: %s'\n", outputPath.c_str());
				}

				if (FileUtils::CopyTo(rawTexture.fileLocation, outputPath))
				{
					texS.diffuseTexFilePath = "./data/" +relativeFilename;
					printf("Copied texture %s to output folder: %s\n", relativeFilename.c_str(), outputPath.c_str());

				}
			}
		}

		if (material.textures[RAW_TEXTURE_USAGE_NORMAL] >= 0)
		{
			const RawTexture  &rawTexture = rawModel.GetTexture(material.textures[RAW_TEXTURE_USAGE_NORMAL]);
			const std::string textureName = StringUtils::GetFileBaseString(rawTexture.name);
			const std::string relativeFilename = StringUtils::GetFileNameString(rawTexture.fileLocation);
			if (!relativeFilename.empty())
			{
				std::string outputPath = outputFolder + "/data/" + relativeFilename;
				std::transform(outputPath.begin(), outputPath.end(), outputPath.begin(), ::tolower);
				if (!FileUtils::CreatePath(outputPath.c_str()))
				{
					fprintf(stderr, "ERROR: Failed to create folder: %s'\n", outputPath.c_str());
					
				}

				if (FileUtils::CopyTo(rawTexture.fileLocation, outputPath))
				{
					texS.normalTexFilePath = "./data/" + relativeFilename;;
					printf("Copied texture %s to output folder: %s\n", relativeFilename.c_str(), outputPath.c_str());

				}
			}
		}
		texS.materialType = (material.type == RAW_MATERIAL_TYPE_TRANSPARENT || material.type == RAW_MATERIAL_TYPE_SKINNED_TRANSPARENT) ? 1 : 0;
		texturePathByMaterialMaterialName.insert(std::make_pair(material.name, texS));

	}

}
std::map<std::string, std::shared_ptr<TextureData>>  textureByIndicesKey_1;
std::map<std::string, std::shared_ptr<MaterialData>> materialsByName;

void InitRaw2Apollo()
{
	textureByIndicesKey_1.clear();
	materialsByName.clear();
	meshDataBySurfaceID.clear();
	textureByIndicesKey.clear();
	texturePathByMaterialMaterialName.clear();
}

static void WriteToVectorContext(void *context, void *data, int size)
{
	auto *vec = static_cast<std::vector<char> *>(context);
	for (int ii = 0; ii < size; ii++) {
		vec->push_back(((char *)data)[ii]);
	}
}

static const std::string materialHash(const RawMaterial &m) {
	return m.name + "_" + std::to_string(m.type);
}


void ExportMatreial_PBR(ApolloBinData& apolloData,const std::string& outputFolder, const RawModel& rawModel)
{
	SamplerData &defaultSampler = *apolloData.samplers.hold(new SamplerData());

	using pixel = std::array<float, 4>; // pixel components are floats in [0, 1]
	using pixel_merger = std::function<pixel(const std::vector<const pixel *>)>;

	auto texIndicesKey = [&](std::vector<int> ixVec, std::string tag) -> std::string {
		std::string result = tag;
		for (int ix : ixVec) {
			result += "_" + std::to_string(ix);
		}
		return result;
	};

	auto getDerivedTexture = [&](
		std::vector<int> rawTexIndices,
		const pixel_merger &combine,
		const std::string &tag,
		bool transparentOutput
		) -> std::shared_ptr<TextureData>
	{
		const std::string key = texIndicesKey(rawTexIndices, tag);
		auto iter = textureByIndicesKey_1.find(key);
		if (iter != textureByIndicesKey_1.end()) {
			return iter->second;
		}

		auto describeChannel = [&](int channels) -> std::string {
			switch (channels) {
			case 1: return "G";
			case 2: return "GA";
			case 3: return "RGB";
			case 4: return "RGBA";
			default:
				printf("?%d?", channels);
					return "";
			}
		};

		struct TexInfo {
			explicit TexInfo(int rawTexIx) : rawTexIx(rawTexIx) {}

			const int rawTexIx;
			int width{};
			int height{};
			int channels{};
			uint8_t *pixels{};
		};

		int width = -1, height = -1;
		std::string mergedFilename = tag;
		std::vector<TexInfo> texes{};
		for (const int rawTexIx : rawTexIndices) {
			TexInfo info(rawTexIx);
			if (rawTexIx >= 0) {
				const RawTexture  &rawTex = rawModel.GetTexture(rawTexIx);
				const std::string &fileLoc = rawTex.fileLocation;
				const std::string &name = StringUtils::GetFileBaseString(StringUtils::GetFileNameString(fileLoc));
				if (!fileLoc.empty()) {
					info.pixels = stbi_load(fileLoc.c_str(), &info.width, &info.height, &info.channels, 0);
					if (!info.pixels) {
						printf("Warning: merge texture [%d](%s) could not be loaded.\n",
							rawTexIx,
							name.c_str());
					}
					else {
						if (width < 0) {
							width = info.width;
							height = info.height;
						}
						else if (width != info.width || height != info.height) {
							printf("Warning: texture %s (%d, %d) can't be merged with previous texture(s) of dimension (%d, %d)\n",
								name.c_str(),
								info.width, info.height, width, height);
							// this is bad enough that we abort the whole merge
							return nullptr;
						}
						mergedFilename += "_" + name;
					}
				}
			}
			texes.push_back(info);
		}
		const std::string mergedName = mergedFilename;

		if (width < 0) {
			return nullptr;
		}

		// write 3 or 4 channels depending on whether or not we need transparency
		int channels = transparentOutput ? 4 : 3;

		std::vector<uint8_t> mergedPixels(static_cast<size_t>(channels * width * height));
		std::vector<pixel> pixels(texes.size());
		std::vector<const pixel *> pixelPointers(texes.size());
		for (int xx = 0; xx < width; xx++)
		{
			for (int yy = 0; yy < height; yy++)
			{
				for (int jj = 0; jj < texes.size(); jj++)
				{
					const TexInfo &tex = texes[jj];
					int ii = tex.channels * (xx + yy*width);
					int kk = 0;
					if (tex.pixels != nullptr)
					{
						for (; kk < tex.channels; kk++)
						{
							pixels[jj][kk] = tex.pixels[ii++] / 255.0f;
						}
					}
					for (; kk < pixels[jj].size(); kk++)
					{
						pixels[jj][kk] = 1.0f;
					}
					pixelPointers[jj] = &pixels[jj];
				}
				const pixel merged = combine(pixelPointers);
				int ii = channels * (xx + yy*width);
				for (int jj = 0; jj < channels; jj++)
				{
					mergedPixels[ii + jj] = static_cast<uint8_t>(fmax(0, fmin(255.0f, merged[jj] * 255.0f)));
				}
			}
		}

		bool png = transparentOutput;

		std::vector<char> imgBuffer;
		int res;
		if (png)
		{
			res = stbi_write_png_to_func(WriteToVectorContext, &imgBuffer,
				width, height, channels, mergedPixels.data(), width * channels);
		}
		else
		{
			res = stbi_write_jpg_to_func(WriteToVectorContext, &imgBuffer,
				width, height, channels, mergedPixels.data(), 80);
		}
		if (!res)
		{
			printf("Warning: failed to generate merge texture '%s'.\n", mergedFilename.c_str());
			return nullptr;
		}

		ImageData *image;
		{
			std::string imageFilename = mergedFilename + (png ? ".png" : ".jpg");
			std::string imagePath = outputFolder + "/data/" + imageFilename;
			std::transform(imagePath.begin(), imagePath.end(), imagePath.begin(), ::tolower);
			if (!FileUtils::CreatePath(imagePath.c_str()))
			{
				fprintf(stderr, "ERROR: Failed to create folder: %s'\n", imagePath.c_str());

			}
			
			FILE *fp = fopen(imagePath.c_str(), "wb");
			if (fp == nullptr)
			{
				printf("Warning:: Couldn't write file '%s' for writing.\n", imagePath.c_str());
				return nullptr;
			}

			if (fwrite(imgBuffer.data(), imgBuffer.size(), 1, fp) != 1)
			{
				printf("Warning: Failed to write %lu bytes to file '%s'.\n", imgBuffer.size(), imagePath.c_str());
				fclose(fp);
				return nullptr;
			}
			fclose(fp);
			image = new ImageData(mergedName, imageFilename);
		}

		std::shared_ptr<TextureData> texDat = apolloData.textures.hold(new TextureData(mergedName, defaultSampler, *apolloData.images.hold(image)));
		return texDat;
	};

	auto getSimpleTexture = [&](int rawTexIndex, const std::string &tag)
	{
		const std::string key = texIndicesKey({ rawTexIndex }, tag);
		auto iter = textureByIndicesKey_1.find(key);
		if (iter != textureByIndicesKey_1.end())
		{
			return iter->second;
		}

		const RawTexture  &rawTexture = rawModel.GetTexture(rawTexIndex);
		const std::string textureName = StringUtils::GetFileBaseString(rawTexture.name);
		const std::string relativeFilename = StringUtils::GetFileNameString(rawTexture.fileLocation);
		const std::string textureFileName = StringUtils::GetFileNameString(rawTexture.fileName);

		ImageData *image = nullptr;
		std::shared_ptr<TextureData> texDat = nullptr;
		if (!relativeFilename.empty())
		{
			
			std::string outputPath = outputFolder + "/data/" + relativeFilename;
			std::transform(outputPath.begin(), outputPath.end(), outputPath.begin(), ::tolower);
			if (!FileUtils::CreatePath(outputPath.c_str()))
			{
				fprintf(stderr, "ERROR: Failed to create folder: %s'\n", outputPath.c_str());

			}
			//image = new ImageData(relativeFilename, relativeFilename);
			if (FileUtils::CopyTo(rawTexture.fileLocation, outputPath))
			{
				printf("Copied texture '%s' to output folder: %s\n", textureName.c_str(), outputPath.c_str());
			}

			//texDat = apolloData.textures.hold(
			//	new TextureData(textureName, defaultSampler, *apolloData.images.hold(image)));
			//textureByIndicesKey_1.insert(std::make_pair(key, texDat));
		}

		if (relativeFilename.empty())
			image = new ImageData(textureFileName, textureFileName);
		else
			image = new ImageData(relativeFilename, relativeFilename);
		

		texDat = apolloData.textures.hold(
			new TextureData(textureName, defaultSampler, *apolloData.images.hold(image)));
		textureByIndicesKey_1.insert(std::make_pair(key, texDat));
		/*else
		{
			const std::string relativeFileName  = StringUtils::GetFileNameString(rawTexture.fileName);
			std::string outputPath = outputFolder + "/data/" + relativeFileName;
			std::transform(outputPath.begin(), outputPath.end(), outputPath.begin(), ::tolower);
			if (!FileUtils::CreatePath(outputPath.c_str()))
			{
				fprintf(stderr, "ERROR: Failed to create folder: %s'\n", outputPath.c_str());

			}
			image = new ImageData(relativeFileName, relativeFileName);
			if (FileUtils::CopyTo(rawTexture.fileLocation, outputPath))
			{
				printf("Copied texture '%s' to output folder: %s\n", textureName.c_str(), outputPath.c_str());
			}

			texDat = apolloData.textures.hold(
				new TextureData(textureName, defaultSampler, *apolloData.images.hold(image)));
			textureByIndicesKey_1.insert(std::make_pair(key, texDat));
		}
		*/
		return texDat;
	};


	for (int materialIndex = 0; materialIndex < rawModel.GetMaterialCount(); materialIndex++)
	{
		const RawMaterial &material = rawModel.GetMaterial(materialIndex);
		const bool isTransparent =
			material.type == RAW_MATERIAL_TYPE_TRANSPARENT ||
			material.type == RAW_MATERIAL_TYPE_SKINNED_TRANSPARENT;

		Math::Vec3 emissiveFactor;
		float emissiveIntensity;

		const Math::Vec3 dielectric(0.04f, 0.04f, 0.04f);

		auto simpleTex = [&](RawTextureUsage usage) -> std::shared_ptr<TextureData>
		{
			return (material.textures[usage] >= 0) ? getSimpleTexture(material.textures[usage], "simple") : nullptr;
		};

		TextureData *normalTexture = simpleTex(RAW_TEXTURE_USAGE_NORMAL).get();
		TextureData *emissiveTexture = simpleTex(RAW_TEXTURE_USAGE_EMISSIVE).get();
		TextureData *occlusionTexture = nullptr;
		TextureData *reflectionTexture = nullptr;

		auto merge1Tex = [&](
			const std::string tag,
			RawTextureUsage usage,
			const pixel_merger &combine,
			bool outputHasAlpha
			) -> std::shared_ptr<TextureData>
		{
			return getDerivedTexture({ material.textures[usage] }, combine, tag, outputHasAlpha);
		};

		auto merge2Tex = [&](
			const std::string tag,
			RawTextureUsage u1,
			RawTextureUsage u2,
			const pixel_merger &combine,
			bool outputHasAlpha
			) -> std::shared_ptr<TextureData>
		{
			return getDerivedTexture(
			{ material.textures[u1], material.textures[u2] },
				combine, tag, outputHasAlpha);
		};

		// acquire derived texture of two RawTextureUsage as *TextData, or nullptr if neither exists
		auto merge3Tex = [&](
			const std::string tag,
			RawTextureUsage u1,
			RawTextureUsage u2,
			RawTextureUsage u3,
			const pixel_merger &combine,
			bool outputHasAlpha
			) -> std::shared_ptr<TextureData>
		{
			return getDerivedTexture(
			{ material.textures[u1], material.textures[u2], material.textures[u3] },
				combine, tag, outputHasAlpha);
		};

		std::shared_ptr<PBRMetallicRoughness> pbrMetRough;
		//强制使用PBR材质，普通材质转为pbr材质
		//if (options.usePBRMetRough)
		{
			std::shared_ptr<TextureData> baseColorTex, aoMetRoughTex;

			Math::Vec4 diffuseFactor;
			float metallic, roughness;
			if (material.info->shadingModel == RAW_SHADING_MODEL_PBR_MET_ROUGH)
			{
				/**
				* PBR FBX Material -> PBR Met/Rough glTF.
				*
				* METALLIC and ROUGHNESS textures are packed in G and B channels of a rough/met texture.
				* Other values translate directly.
				*/
				RawMetRoughMatProps *props = (RawMetRoughMatProps *)material.info.get();
				// merge metallic into the blue channel and roughness into the green, of a new combinatory texture
				aoMetRoughTex = merge3Tex("ao_met_rough",
					RAW_TEXTURE_USAGE_OCCLUSION, RAW_TEXTURE_USAGE_METALLIC, RAW_TEXTURE_USAGE_ROUGHNESS,
					[&](const std::vector<const pixel *> pixels) -> pixel {
					return{ (*pixels[0])[0], (*pixels[2])[0], (*pixels[1])[0], 1 };},
					false);
				baseColorTex = simpleTex(RAW_TEXTURE_USAGE_ALBEDO);
				diffuseFactor = props->diffuseFactor;
				metallic = props->metallic;
				roughness = props->roughness;
				emissiveFactor = props->emissiveFactor;
				emissiveIntensity = props->emissiveIntensity;
				// add the occlusion texture only if actual occlusion pixels exist in the aoNetRough texture.
				if (material.textures[RAW_TEXTURE_USAGE_OCCLUSION] >= 0) {
					occlusionTexture = aoMetRoughTex.get();
				}
			}
			else
			{
				/**
				* Traditional FBX Material -> PBR Met/Rough glTF.
				*
				* Diffuse channel is used as base colour. Simple constants for metallic and roughness.
				*/
				const RawTraditionalMatProps *props = ((RawTraditionalMatProps *)material.info.get());
				diffuseFactor = props->diffuseFactor;

				if (material.info->shadingModel == RAW_SHADING_MODEL_BLINN ||
					material.info->shadingModel == RAW_SHADING_MODEL_PHONG)
				{
					// blinn/phong hardcoded to 0.4 metallic
					metallic = 0.4f;

					// fairly arbitrary conversion equation, with properties:
					//   shininess 0 -> roughness 1
					//   shininess 2 -> roughness ~0.7
					//   shininess 6 -> roughness 0.5
					//   shininess 16 -> roughness ~0.33

					//   as shininess ==> oo, roughness ==> 0
					auto getRoughness = [&](float shininess)
					{
						return sqrtf(2.0f / (2.0f + shininess));
					};
					aoMetRoughTex = merge1Tex("ao_met_rough",
						RAW_TEXTURE_USAGE_SHININESS,
						[&](const std::vector<const pixel *> pixels) -> pixel {
						// do not multiply with props->shininess; that doesn't work like the other factors.
						float shininess = props->shininess * (*pixels[0])[0];
						return{ 0, getRoughness(shininess), metallic, 1 };
					},
						false);
					if (aoMetRoughTex != nullptr) {
						// if we successfully built a texture, factors are just multiplicative identity
						metallic = roughness = 1.0f;
					}
					else {
						// no shininess texture,
						roughness = getRoughness(props->shininess);
					}

				}
				else {
					metallic = 0.2f;
					roughness = 0.8f;
				}

				baseColorTex = simpleTex(RAW_TEXTURE_USAGE_DIFFUSE);

				emissiveFactor = props->emissiveFactor;
				emissiveIntensity = 1.0f;
			}
			pbrMetRough.reset(new PBRMetallicRoughness(baseColorTex.get(), aoMetRoughTex.get(), diffuseFactor, metallic, roughness));
		}

		std::shared_ptr<KHRCmnUnlitMaterial> khrCmnUnlitMat;
	
		if (!occlusionTexture) {
			occlusionTexture = simpleTex(RAW_TEXTURE_USAGE_OCCLUSION).get();
		}

		if (!reflectionTexture) {
			reflectionTexture = simpleTex(RAW_TEXTURE_USAGE_REFLECTION).get();
		}

		std::shared_ptr<MaterialData> mData = apolloData.materials.hold(
			new MaterialData(
				material.name, isTransparent, material.info->shadingModel,
				normalTexture, occlusionTexture, reflectionTexture, emissiveTexture,
				emissiveFactor * emissiveIntensity, khrCmnUnlitMat, pbrMetRough));
		materialsByName[materialHash(material)] = mData;
	}
}


void ExportBoneData(ApolloBinData& apolloData, const RawModel& rawModel)
{
	for (int i = 0; i < rawModel.GetNodeCount(); i++) {

		const RawNode &node = rawModel.GetNode(i);
		if (node.surfaceId > 0)
		{
			int surfaceIndex = rawModel.GetSurfaceById(node.surfaceId);
			const RawSurface &rawSurface = rawModel.GetSurface(surfaceIndex);
			

			if (!rawSurface.jointIds.empty()) 
			{
				//buffers.hold(skeletonBuffer);
				//写入所有的骨骼ID和parentID,用于在以后读取时建立父子关系
				std::vector<BoneHierarchy> bones;
				int skeletonRoot = rawSurface.skeletonRootId;
				for (const auto &jointId : rawSurface.jointIds)
				{
					int nodeIndex = rawModel.GetNodeById(jointId);
					RawNode boneNode = rawModel.GetNode(nodeIndex);
					BoneHierarchy bh;
					bh.boneID = boneNode.id;
					strcpy(bh.name, boneNode.name.c_str());
					if (jointId == skeletonRoot)
						bh.parentID = -1;  //根骨骼不再需要parent（忽略上层node的变换）
					else
					{
						auto iter = std::find(rawSurface.jointIds.begin(), rawSurface.jointIds.end(), boneNode.parentId);
						if (iter != rawSurface.jointIds.end())
							bh.parentID = *iter;
						else
							bh.parentID = -1;
					}
					//bh.parentID = boneNode.parentId;
					bones.push_back(bh);
				}
				int byteLength = bones.size() * sizeof(bones[0]);
				char* rawBuffer = (char*)malloc(byteLength);
				char *tmpBuf = rawBuffer;
				for (const auto &bh : bones)
				{
					memcpy(tmpBuf, &bh, sizeof(BoneHierarchy));
					tmpBuf += sizeof(BoneHierarchy);
				}
				BoneHierarchy* test = (BoneHierarchy*)rawBuffer;
				std::shared_ptr<BufferLocator> boneHierarchyLocator = apolloData.AddRawBuffer(*apolloData.skeletonBuffer, rawBuffer, byteLength);
				free(rawBuffer);

				//写入绑定矩阵
				std::shared_ptr<BufferLocator> inverseBindMatrixLocator = apolloData.AddTypedBuffer(*apolloData.skeletonBuffer, GLT_MAT4F, rawSurface.inverseBindMatrices);

				
				auto skin = apolloData.skeDats.hold(new SkeletonData(*boneHierarchyLocator, *inverseBindMatrixLocator));
				auto iter = meshDataBySurfaceID.find(rawSurface.id);// , rawSurface.id);
				if (iter != meshDataBySurfaceID.end())
				{
					iter->second->SetSkeleton(skin);  //为mesh关联骨骼
					iter->second->SetMaxBoneCnt(rawSurface.maxBone);
				}
			}
		}
	}
}

int FindIntInVector(const std::vector<long>& jointidx, long idx)
{
	for (int i = 0; i < jointidx.size(); i++)
	{
		if (jointidx[i] == idx)
		{
			return i;
		}
	}
	return -1;
}

void ReverseGetBoneData(ApolloBinData& apolloData, const RawModel& rawModel, const RawNode& parent,std::vector<BoneHierarchy>& boneHierarchyList, std::map<long, Math::Mat4>& boneID2MatrixMap)
{
	if (parent.isSkeleton)
	{
		BoneHierarchy bone;
		strcpy(bone.name, parent.name.c_str());

		int parentIdx = rawModel.GetNodeById(parent.parentId);
		const RawNode& rawParentNode = rawModel.GetNode(parentIdx);
		if (!rawParentNode.isSkeleton)
			bone.parentID = -1;
		else
			bone.parentID = parent.parentId;
		bone.boneID = parent.id;
		boneHierarchyList.push_back(bone);
	}
	int surfaceIndex = rawModel.GetSurfaceById(parent.surfaceId);
	if (surfaceIndex > 0)
	{
		const RawSurface &rawSurface = rawModel.GetSurface(surfaceIndex);
		for (int i = 0; i < rawSurface.jointIds.size(); i++)
		{
			boneID2MatrixMap.insert(std::pair<long, Math::Mat4>(rawSurface.jointIds[i], rawSurface.inverseBindMatrices[i]));
		}
		// 			int idx = FindIntInVector(rawSurface.jointIds, parent.id);
		// 			boneID2MatrixMap.push_back(rawSurface.inverseBindMatrices[idx]);
	}

	for (int i=0;i<parent.childIds.size();i++)
	{
		int idx = rawModel.GetNodeById(parent.childIds[i]);
		const RawNode& rootNode = rawModel.GetNode(idx);
		ReverseGetBoneData(apolloData, rawModel, rootNode, boneHierarchyList, boneID2MatrixMap);
	}
}



void ExportBoneData2(ApolloBinData& apolloData, const RawModel& rawModel)
{
	long rootNodeID = rawModel.GetRootNode();
	int idx = rawModel.GetNodeById(rootNodeID);
	const RawNode& rootNode = rawModel.GetNode(idx);

	std::vector<BoneHierarchy> boneHierarchyList;
	std::vector<Math::Mat4> boneMatrixList;
	std::map<long, Math::Mat4> boneID2MatrixMap;

	ReverseGetBoneData(apolloData, rawModel, rootNode, boneHierarchyList, boneID2MatrixMap);

	boneMatrixList.resize(boneHierarchyList.size());

	for (int i=0;i<boneHierarchyList.size();i++)
	{
		boneMatrixList[i] = boneID2MatrixMap[boneHierarchyList[i].boneID];
	}

	int byteLength = boneHierarchyList.size() * sizeof(boneHierarchyList[0]);
	BoneHierarchy* startptr = &boneHierarchyList[0];
	char* start = (char*)(startptr);
	std::shared_ptr<BufferLocator> boneHierarchyLocator = apolloData.AddRawBuffer(*apolloData.skeletonBufferV2, start, byteLength);
	std::shared_ptr<BufferLocator> inverseBindMatrixLocator = apolloData.AddTypedBuffer(*apolloData.skeletonBufferV2, GLT_MAT4F, boneMatrixList);
	SkeletonData* ptr = new SkeletonData(*boneHierarchyLocator, *inverseBindMatrixLocator);
	ptr->meshName = "skel";
	auto skin = apolloData.skeDatV2.hold(ptr);

}

void ExportBoneData3(ApolloBinData& apolloData, const RawModel& rawModel)
{
	std::vector<BoneHierarchy> bones;
	std::vector<Math::Mat4>           allBindMatrices;
	std::vector<Math::Mat4>           allInverseBindMatrices;
	
	for (int i = 0; i < rawModel.GetNodeCount(); i++) {
		const RawNode &node = rawModel.GetNode(i);
		if (node.surfaceId > 0)
		{
			int surfaceIndex = rawModel.GetSurfaceById(node.surfaceId);
			const RawSurface &rawSurface = rawModel.GetSurface(surfaceIndex);

			if (!rawSurface.jointIds.empty())
			{
				auto iter = meshDataBySurfaceID.find(rawSurface.id);// , rawSurface.id);
				if (iter != meshDataBySurfaceID.end())
				{
					iter->second->SetMaxBoneCnt(rawSurface.maxBone);
				}
				
			}
		}
	}
	
	for (auto pBoneNode : rawModel.BoneNodes)
	{
		BoneHierarchy bh;
		const RawSurface& rs = rawModel.GetSurface(0);

		auto find_itr = std::find(rs.jointIds.cbegin(), rs.jointIds.cend(), pBoneNode->id);
		if (find_itr != rs.jointIds.cend())
		{
			bh.boneID = pBoneNode->id;
			strcpy(bh.name, pBoneNode->name.c_str());
			bh.parentID = -1;
			bool isRoot = true;
			RawNode* pParentNode = NULL;
			for (auto pBoneNodeID : rawModel.BoneNodes)
			{
				if (pBoneNode->parentId == pBoneNodeID->id)
				{
					isRoot = false;
					pParentNode = pBoneNodeID;
					bh.parentID = pBoneNode->parentId;
					break;
				}
			}
			bones.push_back(bh);
		}
		
	}

	int byteLength = bones.size() * sizeof(bones[0]);
	char* rawBuffer = (char*)malloc(byteLength);
	char *tmpBuf = rawBuffer;
	for (const auto &bh : bones)
	{
		memcpy(tmpBuf, &bh, sizeof(BoneHierarchy));
		tmpBuf += sizeof(BoneHierarchy);
	}
	if (byteLength>0)
	{
		BoneHierarchy* test = (BoneHierarchy*)rawBuffer;
		std::shared_ptr<BufferLocator> boneHierarchyLocator = apolloData.AddRawBuffer(*apolloData.skeletonBufferV2, rawBuffer, byteLength);
		free(rawBuffer);

		//写入绑定矩阵
		std::shared_ptr<BufferLocator> inverseBindMatrixLocator = apolloData.AddTypedBuffer(*apolloData.skeletonBufferV2, GLT_MAT4F, allInverseBindMatrices);

		auto skin = apolloData.skeDatV2.hold(new SkeletonData(*boneHierarchyLocator, *inverseBindMatrixLocator));
		skin->meshName = rawModel.meshName;
	}

}


void ExportAnomationData(ApolloBinData& apolloData, const RawModel& rawModel)
{
	for (int i = 0; i < rawModel.GetAnimationCount(); i++) 
	{
		const RawAnimation &animation = rawModel.GetAnimation(i);
		if (i == rawModel.GetOriginalIndex()) apolloData.originAnimation = animation.name;

		if (animation.channels.size() != 0) // 判断这个动画片段 是否有骨骼动画
		{  

			std::vector<uint8_t>* aniBinary = new std::vector<uint8_t>();
			apolloData.animationBinary.push_back(aniBinary);
			BufferData* aniBuffer = new BufferData(std::shared_ptr<std::vector<uint8_t>>(aniBinary));
			apolloData.buffers.hold(aniBuffer);
			apolloData.animationBuffer.push_back(aniBuffer);
			//std::shared_ptr<BufferLocator> timesLocator = apolloData.AddTypedBuffer(*apolloData.skeletonBuffer, GLT_FLOAT, animation.times);

			std::shared_ptr<BufferLocator> timesLocator = apolloData.AddTypedBuffer(*aniBuffer, GLT_FLOAT, animation.times);
			AnimationData &aDat = *apolloData.aniDats.hold(new AnimationData(animation.name, *timesLocator, animation.times.size(), animation.beginFrame));

			std::vector<int> boneIDs;
			std::vector<int> transformAttributes;
			std::vector<int> translationOffset(animation.channels.size(), -1);
			std::vector<int> translationByteLength(animation.channels.size(), -1);
			std::vector<int> rotationOffset(animation.channels.size(), -1);
			std::vector<int> rotationByteLength(animation.channels.size(), -1);
			std::vector<int> scaleOffset(animation.channels.size(), -1);
			std::vector<int> scaleByteLength(animation.channels.size(), -1);
			std::vector<int> transformOffset(animation.channels.size(), -1);
			std::vector<int> transformLength(animation.channels.size(), -1);


			std::vector<int> keyTimeOffset(animation.channels.size(), -1);
			std::vector<int> keyTimeLength(animation.channels.size(), -1);

			//if (animation.isHasScale)  //导出矩阵
			if (true)//新材质不支持双四元数
			{
				aDat.isHasScale = true;
				for (size_t channelIx = 0; channelIx < animation.channels.size(); channelIx++)
				{
					//一个channel就是一块骨头，附带着每一帧的变换信息
					const RawChannel &channel = animation.channels[channelIx];
					const RawNode &rawNode = rawModel.GetNode(channel.nodeIndex);
					int boneID = rawNode.id;
					boneIDs.push_back(boneID);

					std::shared_ptr<BufferLocator> locator = apolloData.AddTypedBuffer(*aniBuffer, GLT_MAT4F, channel.transforms);
					transformOffset[channelIx] = locator->byteOffset;
					transformLength[channelIx] = locator->byteLength;


					std::shared_ptr<BufferLocator> timesLocator = apolloData.AddTypedBuffer(*aniBuffer, GLT_FLOAT, channel.times);
					keyTimeOffset[channelIx] = timesLocator->byteOffset;
					keyTimeLength[channelIx] = timesLocator->byteLength;

				}

				auto boneIDLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)boneIDs.data(), sizeof(boneIDs[0]) * boneIDs.size());
				aDat.boneIDsIndex = boneIDLocator->ix;
				auto transformOffsetLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)transformOffset.data(), sizeof(transformOffset[0]) * transformOffset.size());
				auto transformLengthLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)transformLength.data(), sizeof(transformLength[0]) * transformLength.size());
				aDat.transformOffsetIndex = transformOffsetLocator->ix;
				aDat.transformLengthIndex = transformLengthLocator->ix;

				auto keyTimeOffsetLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)keyTimeOffset.data(), sizeof(keyTimeOffset[0]) * keyTimeOffset.size());
				auto keyTimeLengthLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)keyTimeLength.data(), sizeof(keyTimeLength[0]) * keyTimeLength.size());
				aDat.keyTimeOffsetIndex = keyTimeOffsetLocator->ix;
				aDat.keyTimeLengthIndex = keyTimeLengthLocator->ix;

			}
			else  //导出双四元数
			{
				aDat.isHasScale = false;
				for (size_t channelIx = 0; channelIx < animation.channels.size(); channelIx++)
				{
					//一个channel就是一块骨头，附带着每一帧的变换信息
					const RawChannel &channel = animation.channels[channelIx];
					const RawNode &rawNode = rawModel.GetNode(channel.nodeIndex);
					int boneID = rawNode.id;
					boneIDs.push_back(boneID);

					int transformAttribute = 0;
					if (!channel.translations.empty())
					{
						transformAttribute |= ATA_TRANSLATION;
						std::shared_ptr<BufferLocator> locator = apolloData.AddTypedBuffer(*aniBuffer, GLT_VEC3F, channel.translations);
						translationOffset[channelIx] = locator->byteOffset;
						translationByteLength[channelIx] = locator->byteLength;

					}
					if (!channel.rotations.empty())
					{
						transformAttribute |= ATA_ROTATION;
						std::shared_ptr<BufferLocator> locator = apolloData.AddTypedBuffer(*aniBuffer, GLT_QUATF, channel.rotations);
						rotationOffset[channelIx] = locator->byteOffset;
						rotationByteLength[channelIx] = locator->byteLength;
					}

					//std::shared_ptr<BufferLocator> locator = apolloData.AddTypedBuffer(*apolloData.animationBuffer, GLT_MAT4F, channel.transforms);
					//transformOffset[channelIx] = locator->byteOffset;
					//transformLength[channelIx] = locator->byteLength;

					transformAttributes.push_back(transformAttribute);

					std::shared_ptr<BufferLocator> timesLocator = apolloData.AddTypedBuffer(*aniBuffer, GLT_FLOAT, channel.times);
					keyTimeOffset[channelIx] = timesLocator->byteOffset;
					keyTimeLength[channelIx] = timesLocator->byteLength;
				}

				auto boneIDLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)boneIDs.data(), sizeof(boneIDs[0]) * boneIDs.size());


				auto translationOffsetLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)translationOffset.data(), sizeof(translationOffset[0]) * translationOffset.size());
				auto translationLengthLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)translationByteLength.data(), sizeof(translationByteLength[0]) * translationByteLength.size());

				auto rotationOffsetLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)rotationOffset.data(), sizeof(rotationOffset[0]) * rotationByteLength.size());
				auto rotationLengthLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)rotationByteLength.data(), sizeof(rotationByteLength[0]) * rotationByteLength.size());

				auto transformAttributeLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)transformAttributes.data(), sizeof(transformAttributes[0]) * transformAttributes.size());

				aDat.boneIDsIndex = boneIDLocator->ix;
				aDat.transformAttributeIndex = transformAttributeLocator->ix;
				aDat.translationOffsetIndex = translationOffsetLocator->ix;
				aDat.translationLengthIndex = translationLengthLocator->ix;
				aDat.rotationOffsetIndex = rotationOffsetLocator->ix;
				aDat.rotationLengthIndex = rotationLengthLocator->ix;

				auto keyTimeOffsetLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)keyTimeOffset.data(), sizeof(keyTimeOffset[0]) * keyTimeOffset.size());
				auto keyTimeLengthLocator = apolloData.AddRawBuffer(*aniBuffer, (char*)keyTimeLength.data(), sizeof(keyTimeLength[0]) * keyTimeLength.size());
				aDat.keyTimeOffsetIndex = keyTimeOffsetLocator->ix;
				aDat.keyTimeLengthIndex = keyTimeLengthLocator->ix;
			} 
		}
	}

	// morph动画 RawModel -> ApolloBinData
	for (int i = 0; i < rawModel.GetMorphAnimationCount(); i++)
	{
		const RawMorphAnimation &animation = rawModel.GetMorphAnimation(i);
		if (animation.morphers.size() != 0)
		{
			RawMorphAnimation tmp;
			tmp.beginTime = animation.beginTime;
			tmp.endTime = animation.endTime;
			for (auto& m : animation.morphers)
			{
				tmp.morphers.emplace_back(std::move(m));
			}
			apolloData.morpherAnimations.emplace_back(std::move(tmp));
		}
	}
	
}

template<class T>
void AddAttribute(const RawModel& surfaceModel, const std::shared_ptr<draco::Mesh> dracoMesh, const AttributeDefinitionWithDraco<T>& attribute, 
	DRACO_GENERIC_UNIQUE_ID attr_uid = OLD_VERSION_COMPATIBLE)
{
	std::vector<T> attribArr;
	surfaceModel.GetAttributeArray<T>(attribArr, attribute.rawAttributeIx);
	draco::PointAttribute att;
	int8_t componentCount = attribute.glType.count;
	att.Init(
		attribute.dracoAttribute, componentCount, attribute.dracoComponentType,
		false, componentCount * draco::DataTypeLength(attribute.dracoComponentType));

	const int dracoAttId = dracoMesh->AddAttribute(att, true, attribArr.size());
	draco::PointAttribute *attPtr = dracoMesh->attribute(dracoAttId);
	attPtr->set_unique_id(attr_uid);

	std::vector<uint8_t> buf(sizeof(T));
	int tmpSize = sizeof(T);
	for (uint32_t ii = 0; ii < attribArr.size(); ii++) {
		uint8_t *ptr = &buf[0];
		attribute.glType.write(ptr, attribArr[ii]);
		attPtr->SetAttributeValue(attPtr->mapped_index(draco::PointIndex(ii)), ptr);
	}
}


bool FindJoint(const std::vector<uint16_t>& joints, uint16_t a)
{
	for (size_t i = 0; i < joints.size(); ++i)
	{
		if (a == joints[i])
		{
			return true;
		}
	}
	return false;
}


void SplitSameVertex(RawModel& surfaceModel, std::vector<int> &trianglesCnt, std::vector<std::vector<int>> &splitedIndices)
{
	std::map<int, std::vector<int>> dumplicateVertex;
	std::map<int, int> extraVertexIndex;
	splitedIndices.resize(trianglesCnt.size());
	std::vector<std::vector<int>> materialIdxVec;
	materialIdxVec.resize(trianglesCnt.size());
	int vertexCnt = surfaceModel.GetVertexCount();
	int count = 0;
	for (size_t i = 0; i < trianglesCnt.size(); ++i)
	{
		for (size_t j = 0; j < trianglesCnt[i]; ++j)
		{
			for (size_t k = 0; k < 3; ++k)
			{
				int index = surfaceModel.GetTriangle(count).verts[k];
				if (dumplicateVertex.find(index) != dumplicateVertex.end())
				{
					if (dumplicateVertex[index][dumplicateVertex[index].size() - 1] != i)
					{
						RawVertex extraVertex = surfaceModel.GetVertex(index);
						surfaceModel.AddVertexInSplitMesh(extraVertex);
						vertexCnt++;
						index = vertexCnt - 1;
						surfaceModel.SetTriangle(count, k, index);
						dumplicateVertex[index].push_back(i);
						extraVertexIndex[index] = vertexCnt - 1;
					}
					else
					{
						index = extraVertexIndex[index];
					}
				}
				else 
				{
					dumplicateVertex[index].push_back(i);
				}
				splitedIndices[i].push_back(index);
				materialIdxVec[i].push_back(surfaceModel.GetTriangle(count).materialIndex);
			}
			count++;
		}
	}
	materialIdxVec.clear();
}


void SplitMeshImp(RawModel& surfaceModel, std::vector<int> &trianglesCnt, std::vector<long> &affectIDs, std::vector<std::vector<int>> &splitedAffectIDs, int maxBones)
{
	std::vector<uint16_t> jointstemp;
	std::vector<int> affecttemp;
	int count = 0;

	int prevMatidx = 0;

	for (size_t ii = 0; ii < surfaceModel.GetTriangles().size(); ++ii)
	{
		const RawTriangle& triangle = surfaceModel.GetTriangles()[ii];
		const int curMatIdx = triangle.materialIndex;
		bool isFull = false;
		for (size_t i = 0; i < 3; ++i)
		{
			if (isFull) break;
			int index = triangle.verts[i];
			for (size_t k = 0; k < surfaceModel.GetVertex(index).jointIndices.Size(); ++k)
			{
				uint16_t jointIndex = surfaceModel.GetVertex(index).jointIndices[k];
				if (jointstemp.size() < maxBones && curMatIdx == prevMatidx)
				{
					if (!FindJoint(jointstemp, jointIndex))
					{
						jointstemp.push_back(jointIndex);
						affecttemp.push_back((int)affectIDs[jointIndex]);
					}
				}
				else
				{
					isFull = true;
					ii--;
					break;
				}
			}
			if (i == 2 && !isFull) count++;
		}
		if (isFull || (ii == surfaceModel.GetTriangles().size() -1))
		{
			trianglesCnt.push_back(count);
			count = 0;
			jointstemp.clear();
			splitedAffectIDs.push_back(affecttemp);
			affecttemp.clear();
		}
		prevMatidx = curMatIdx;
	}
}


void changeJointsIndex(std::vector<long> &affectedJointIDs, std::vector<std::vector<int>> &splitedJointsIDs, RawModel& surfaceModel, std::vector<std::vector<int>> &splitedIndices)
{
	std::set<int> temp;
	for (size_t ii = 0; ii < splitedIndices.size(); ++ii)
	{
		for (size_t jj = 0; jj < splitedIndices[ii].size(); ++jj)
		{
			if (temp.find(splitedIndices[ii][jj]) != temp.end()) continue;
			temp.insert(splitedIndices[ii][jj]);
			Math::IntVec4 lhs = surfaceModel.GetVertex(splitedIndices[ii][jj]).jointIndices;
			Math::IntVec4 rhs;
			for (size_t i = 0; i < lhs.Size(); ++i)
			{
				int j = 0;
				while (j < splitedJointsIDs[ii].size())
				{
					if ((int)affectedJointIDs[lhs[i]] == splitedJointsIDs[ii][j])
					{
						rhs[i] = j;
						break;;
					}
					j++;
				}
				assert(!(j == splitedJointsIDs[ii].size()));
			}
			surfaceModel.SetVertexJointsIndex(splitedIndices[ii][jj], rhs);
		}
	}
}


void SplitMesh(ApolloBinData& apolloData, RawModel& surfaceModel, std::vector<int> &trianglesCnt, std::vector<std::vector<int>> &splitedjointsAffectIDs, int maxBones)
{
	std::vector<long> affectjoints = surfaceModel.GetSurface(0).jointIds;
	if (affectjoints.size() <= maxBones)
	{
		splitedjointsAffectIDs.resize(1);
		for (size_t i = 0; i < affectjoints.size(); ++i)
		{
			splitedjointsAffectIDs[0].push_back((int)affectjoints[i]);
		}
		trianglesCnt.push_back(surfaceModel.GetTriangleCount());
		return;
	}
	std::vector<std::vector<int>> splitedindices;//拆分ibo
	SplitMeshImp(surfaceModel, trianglesCnt, affectjoints, splitedjointsAffectIDs, maxBones);
	SplitSameVertex(surfaceModel, trianglesCnt, splitedindices);
	changeJointsIndex(affectjoints, splitedjointsAffectIDs, surfaceModel, splitedindices);
}

bool ExportMeshDataWithDracoObj(ApolloBinData& apolloData, RawModel& surfaceModel, RawModel& rawModel, int maxBones, bool bCompress)
{
	std::vector<int> trianglesCnt;//拆分后每个mesh的三角形个数
	std::vector<std::vector<int>> splitedjointsAffectIDs;//每个mesh中受影响的骨骼id
	SplitMesh(apolloData, surfaceModel, trianglesCnt, splitedjointsAffectIDs, maxBones);

	//顶点
	int vertexAttributes = surfaceModel.GetVertexAttributes();

	std::shared_ptr<MeshData> meshData;
	auto dracoMesh(std::make_shared<draco::Mesh>());

	TextureStru texS;
	const RawSurface &rawSurface = surfaceModel.GetSurface(0);
	const long surfaceId = rawSurface.id;
	int triangleCount = surfaceModel.GetTriangleCount();
	//索引
	std::string meshName = rawSurface.name;
	dracoMesh->SetNumFaces(static_cast<size_t>(triangleCount));
	for (uint32_t ii = 0; ii < triangleCount; ii++) {
		draco::Mesh::Face face;
		face[0] = surfaceModel.GetTriangle(ii).verts[0];
		face[1] = surfaceModel.GetTriangle(ii).verts[1];
		face[2] = surfaceModel.GetTriangle(ii).verts[2];
		dracoMesh->SetFace(draco::FaceIndex(ii), face);
	}

	int a = surfaceModel.GetVertexCount();
	dracoMesh->set_num_points(surfaceModel.GetVertexCount());


	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_POSITION)
	{
		const AttributeDefinitionWithDraco<Math::Vec3> ATTR_POSITION("POSITION", &RawVertex::position,
			GLT_VEC3F, draco::GeometryAttribute::POSITION, draco::DT_FLOAT32);
		AddAttribute<Math::Vec3>(surfaceModel, dracoMesh, ATTR_POSITION);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_NORMAL)
	{
		const AttributeDefinitionWithDraco<Math::Vec3> ATTR_NORMAL("NORMAL", &RawVertex::normal,
			GLT_VEC3F, draco::GeometryAttribute::NORMAL, draco::DT_FLOAT32);
		AddAttribute<Math::Vec3>(surfaceModel, dracoMesh, ATTR_NORMAL);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_TANGENT)
	{
		const AttributeDefinitionWithDraco<Math::Vec3> ATTR_TANGENT("TANGENT", &RawVertex::tangent,
			GLT_VEC3F, draco::GeometryAttribute::NORMAL, draco::DT_FLOAT32);
		AddAttribute<Math::Vec3>(surfaceModel, dracoMesh, ATTR_TANGENT);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_BINORMAL)
	{
		const AttributeDefinitionWithDraco<Math::Vec3> ATTR_BINORMAL("BINORMAL", &RawVertex::binormal,
			GLT_VEC3F, draco::GeometryAttribute::NORMAL, draco::DT_FLOAT32);
		AddAttribute<Math::Vec3>(surfaceModel, dracoMesh, ATTR_BINORMAL);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_COLOR)
	{
		const AttributeDefinitionWithDraco<Math::Vec4> ATTR_COLOR("COLOR_0", &RawVertex::color, GLT_VEC4F,
			draco::GeometryAttribute::COLOR, draco::DT_FLOAT32);
		AddAttribute<Math::Vec4>(surfaceModel, dracoMesh, ATTR_COLOR);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_UV0)
	{
		const AttributeDefinitionWithDraco<Math::Vec2> ATTR_TEXCOORD_0("TEXCOORD_0", &RawVertex::uv0,
			GLT_VEC2F, draco::GeometryAttribute::TEX_COORD, draco::DT_FLOAT32);
		AddAttribute<Math::Vec2>(surfaceModel, dracoMesh, ATTR_TEXCOORD_0);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_UV1)
	{
		const AttributeDefinitionWithDraco<Math::Vec2> ATTR_TEXCOORD_1("TEXCOORD_1", &RawVertex::uv1,
			GLT_VEC2F, draco::GeometryAttribute::TEX_COORD, draco::DT_FLOAT32);
		AddAttribute<Math::Vec2>(surfaceModel, dracoMesh, ATTR_TEXCOORD_1);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_UV2)
	{
		const AttributeDefinitionWithDraco<Math::Vec2> ATTR_TEXCOORD_2("TEXCOORD_2", &RawVertex::uv2,
			GLT_VEC2F, draco::GeometryAttribute::TEX_COORD, draco::DT_FLOAT32);
		AddAttribute<Math::Vec2>(surfaceModel, dracoMesh, ATTR_TEXCOORD_2);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_UV3)
	{
		const AttributeDefinitionWithDraco<Math::Vec2> ATTR_TEXCOORD_3("TEXCOORD_3", &RawVertex::uv3,
			GLT_VEC2F, draco::GeometryAttribute::TEX_COORD, draco::DT_FLOAT32);
		AddAttribute<Math::Vec2>(surfaceModel, dracoMesh, ATTR_TEXCOORD_3);
	}

	draco::Encoder encoder;
	encoder.SetSpeedOptions(10, 10);
	encoder.SetAttributeQuantization(draco::GeometryAttribute::POSITION, bCompress ? 14 : 0);
	encoder.SetAttributeQuantization(draco::GeometryAttribute::TEX_COORD, bCompress ? 20 : 0);
	encoder.SetAttributeQuantization(draco::GeometryAttribute::NORMAL, bCompress ? 10 : 0);
	encoder.SetAttributeQuantization(draco::GeometryAttribute::COLOR, bCompress ? 8 : 0);
	encoder.SetAttributeQuantization(draco::GeometryAttribute::GENERIC, bCompress ? 8 : 0);

	draco::EncoderBuffer dracoBuffer;
	draco::Status status = encoder.EncodeMeshToBuffer(*dracoMesh, &dracoBuffer);
	assert(status.code() == draco::Status::OK);
	std::shared_ptr<BufferLocator> locator = apolloData.AddRawBuffer(*apolloData.meshBuffer, dracoBuffer.data(), dracoBuffer.size());  //加入缓存中
	std::shared_ptr<BufferLocator> countlocator = apolloData.AddRawBuffer(*apolloData.meshBuffer, (const char*)trianglesCnt.data(), trianglesCnt.size() * 4);

	meshData = apolloData.meshDats.hold(new MeshData(meshName, *locator, vertexAttributes, nullptr));
	meshData->SetTriangleCount(countlocator);
	meshData->SetSubMeshCount(1);
	meshData->minP = rawSurface.bounds.bmin;
	meshData->maxP = rawSurface.bounds.bmax;
	meshData->scale = Math::Vec3(1, 1, 1);
	return true;
}

bool ExportMeshDataWithDraco(ApolloBinData& apolloData, RawModel& surfaceModel, RawModel& rawModel,int maxBones, bool bCompress)
{
	//顶点
	int vertexAttributes = surfaceModel.GetVertexAttributes();
	//outStream.write((char*)&vertexAttributes, sizeof(int));

	//layerment 写到头的位置去
	//std::shared_ptr<BufferLocator> laymentLocator = apolloData.AddRawBuffer(*apolloData.buffer, (char*)&vertexAttributes, sizeof(int));

	std::shared_ptr<MeshData> meshData;
	auto dracoMesh(std::make_shared<draco::Mesh>());

	TextureStru texS;
	const RawSurface &rawSurface = surfaceModel.GetSurface(0);
	int surId = rawModel.GetSurfaceById(rawSurface.id);
	RawSurface& rawModel_rs = rawModel.GetSurface(surId);

	const RawMaterial &rawMaterial = surfaceModel.GetMaterial(surfaceModel.GetTriangle(0).materialIndex);
	const MaterialData &mData = require(materialsByName, materialHash(rawMaterial));


	auto iter = texturePathByMaterialMaterialName.find(rawMaterial.name);
	if (iter != texturePathByMaterialMaterialName.end())
		texS = iter->second;

	const long surfaceId = rawSurface.id;

	int triangleCount = surfaceModel.GetTriangleCount();
	
	std::vector<int> trianglesCnt;//拆分后每个mesh的三角形个数
	std::vector<std::vector<int>> splitedjointsAffectIDs;//每个mesh中受影响的骨骼id
	SplitMesh(apolloData, surfaceModel, trianglesCnt, splitedjointsAffectIDs, maxBones);

	//索引
	std::string meshName = surfaceModel.meshName;
	dracoMesh->SetNumFaces(static_cast<size_t>(triangleCount));
	surfaceModel.subMeshInfos.resize(surfaceModel.GetMaterialCount());
	int prevMatidx = -1;
	int subMeshIdx = -1;
	for (uint32_t ii = 0; ii < triangleCount; ii++) {
		draco::Mesh::Face face;
		face[0] = surfaceModel.GetTriangle(ii).verts[0];
		face[1] = surfaceModel.GetTriangle(ii).verts[1];
		face[2] = surfaceModel.GetTriangle(ii).verts[2];
		dracoMesh->SetFace(draco::FaceIndex(ii), face);

		const int& matIdx = surfaceModel.GetTriangle(ii).materialIndex;
		if (matIdx != prevMatidx)
		{
			prevMatidx = matIdx;
			subMeshIdx++;
			
			if (subMeshIdx > 0)
			{
				surfaceModel.subMeshInfos[subMeshIdx].baseIndex = surfaceModel.subMeshInfos[subMeshIdx - 1].baseIndex + surfaceModel.subMeshInfos[subMeshIdx - 1].IndexNum;
			}
		}
		surfaceModel.subMeshInfos[subMeshIdx].IndexNum += 3;
	}

	rawModel_rs.subMeshCount = std::max(surfaceModel.GetMaterialCount(), (int)trianglesCnt.size());

	if (surfaceModel.GetMaterialCount() > trianglesCnt.size())
	{
		trianglesCnt.clear();
		trianglesCnt.resize(surfaceModel.GetMaterialCount());
		for (size_t i = 0; i < surfaceModel.GetMaterialCount(); ++i)
		{
			trianglesCnt[i] = surfaceModel.subMeshInfos[i].IndexNum / 3;
		}
	}

	int a = surfaceModel.GetVertexCount();
	dracoMesh->set_num_points(surfaceModel.GetVertexCount());
	

	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_POSITION)
	{
		const AttributeDefinitionWithDraco<Math::Vec3> ATTR_POSITION("POSITION", &RawVertex::position,
			GLT_VEC3F, draco::GeometryAttribute::POSITION, draco::DT_FLOAT32);
		AddAttribute<Math::Vec3>(surfaceModel, dracoMesh, ATTR_POSITION);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_NORMAL)
	{
		const AttributeDefinitionWithDraco<Math::Vec3> ATTR_NORMAL("NORMAL", &RawVertex::normal,
			GLT_VEC3F, draco::GeometryAttribute::NORMAL, draco::DT_FLOAT32);
		AddAttribute<Math::Vec3>(surfaceModel, dracoMesh, ATTR_NORMAL);
	}
 	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_TANGENT)
	{
		const AttributeDefinitionWithDraco<Math::Vec3> ATTR_TANGENT("TANGENT", &RawVertex::tangent,
			GLT_VEC3F,draco::GeometryAttribute::NORMAL,draco::DT_FLOAT32);
		AddAttribute<Math::Vec3>(surfaceModel, dracoMesh, ATTR_TANGENT);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_BINORMAL)
	{
		const AttributeDefinitionWithDraco<Math::Vec3> ATTR_BINORMAL("BINORMAL", &RawVertex::binormal,
			GLT_VEC3F, draco::GeometryAttribute::NORMAL, draco::DT_FLOAT32);
		AddAttribute<Math::Vec3>(surfaceModel, dracoMesh, ATTR_BINORMAL);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_COLOR)
	{
		const AttributeDefinitionWithDraco<Math::Vec4> ATTR_COLOR("COLOR_0", &RawVertex::color, GLT_VEC4F,
			draco::GeometryAttribute::COLOR, draco::DT_FLOAT32);
		AddAttribute<Math::Vec4>(surfaceModel, dracoMesh, ATTR_COLOR);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_UV0)
	{
		const AttributeDefinitionWithDraco<Math::Vec2> ATTR_TEXCOORD_0("TEXCOORD_0", &RawVertex::uv0,
			GLT_VEC2F, draco::GeometryAttribute::TEX_COORD, draco::DT_FLOAT32);
		AddAttribute<Math::Vec2>(surfaceModel, dracoMesh, ATTR_TEXCOORD_0);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_UV1)
	{
		const AttributeDefinitionWithDraco<Math::Vec2> ATTR_TEXCOORD_1("TEXCOORD_1", &RawVertex::uv1,
			GLT_VEC2F, draco::GeometryAttribute::TEX_COORD, draco::DT_FLOAT32);
		AddAttribute<Math::Vec2>(surfaceModel, dracoMesh, ATTR_TEXCOORD_1);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_UV2)
	{
		const AttributeDefinitionWithDraco<Math::Vec2> ATTR_TEXCOORD_2("TEXCOORD_2", &RawVertex::uv2,
			GLT_VEC2F, draco::GeometryAttribute::TEX_COORD, draco::DT_FLOAT32);
		AddAttribute<Math::Vec2>(surfaceModel, dracoMesh, ATTR_TEXCOORD_2);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_UV3)
	{
		const AttributeDefinitionWithDraco<Math::Vec2> ATTR_TEXCOORD_3("TEXCOORD_3", &RawVertex::uv3,
			GLT_VEC2F, draco::GeometryAttribute::TEX_COORD, draco::DT_FLOAT32);
		AddAttribute<Math::Vec2>(surfaceModel, dracoMesh, ATTR_TEXCOORD_3);
	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_JOINT_INDICES)
	{
		const AttributeDefinitionWithDraco<Math::IntVec4> ATTR_JOINTS("JOINTS_0", &RawVertex::jointIndices,
			GLT_VEC4I, draco::GeometryAttribute::GENERIC, draco::DT_UINT16);
		AddAttribute<Math::IntVec4>(surfaceModel, dracoMesh, ATTR_JOINTS, DRACO_GENERIC_UNIQUE_ID::BOND_INDEX);

	}
	if (vertexAttributes & RAW_VERTEX_ATTRIBUTE_JOINT_WEIGHTS)
	{
		const AttributeDefinitionWithDraco<Math::Vec4> ATTR_WEIGHTS("WEIGHTS_0", &RawVertex::jointWeights,
			GLT_VEC4F, draco::GeometryAttribute::GENERIC, draco::DT_FLOAT32);
		AddAttribute<Math::Vec4>(surfaceModel, dracoMesh, ATTR_WEIGHTS, DRACO_GENERIC_UNIQUE_ID::BOND_WEIGHTS);
	}

	// 保存blend target到mesh/drc文件 并且更新RawModel中的Morph动画通道目标信息
	// 备注: Raw2Gltf把Target保存到bin文件中  
	for (int channelIx = 0; channelIx < rawSurface.blendChannels.size(); channelIx++)
	{
		//const auto& channel = rawSurface.blendChannels[channelIx];
		JY_ASSERT(surfaceModel.GetVertexCount() != 0);

		int componentCount = 3;

		draco::PointAttribute attrPos;
		attrPos.Init(draco::GeometryAttribute::GENERIC, componentCount, draco::DT_FLOAT32,false,
			componentCount * draco::DataTypeLength(draco::DT_FLOAT32));
		const int pos_attr_id = dracoMesh->AddAttribute(attrPos, true, surfaceModel.GetVertexCount() );
		draco::PointAttribute* subPosAttrPtr = dracoMesh->attribute(pos_attr_id);
		subPosAttrPtr->set_unique_id(DRACO_GENERIC_UNIQUE_ID::MORPH_TARGETS_VERTEX);// 用于区分不同的generic attributer


		draco::PointAttribute attrNormal; // 可能是0 
		attrNormal.Init(draco::GeometryAttribute::GENERIC, componentCount, draco::DT_FLOAT32, false,
			componentCount * draco::DataTypeLength(draco::DT_FLOAT32));
		const int normal_attr_id = dracoMesh->AddAttribute(attrNormal, true, surfaceModel.GetVertexCount());
		draco::PointAttribute* subNormalAttrPtr = dracoMesh->attribute(normal_attr_id);
		subNormalAttrPtr->set_unique_id(DRACO_GENERIC_UNIQUE_ID::MORPH_TARGETS_VERTEX);

		std::vector<uint8_t> buf(sizeof(Math::Vec3));

		uint64_t tid = surfaceModel.GetVertex(0).blends[channelIx].targetId;

		for (int jj = 0; jj < surfaceModel.GetVertexCount(); jj++)
		{
			// 每个channel 的 blend target 顶点都保存到各个顶点RawVertex中 这里获取保存到mesh/drc文件中
			const auto& blendVertex = surfaceModel.GetVertex(jj).blends[channelIx];

			((float*)buf.data())[0] = blendVertex.position.x;
			((float*)buf.data())[1] = blendVertex.position.y;
			((float*)buf.data())[2] = blendVertex.position.z;
			subPosAttrPtr->SetAttributeValue(subPosAttrPtr->mapped_index(draco::PointIndex(jj)), buf.data());

			((float*)buf.data())[0] = blendVertex.normal.x;
			((float*)buf.data())[1] = blendVertex.normal.y;
			((float*)buf.data())[2] = blendVertex.normal.z;

			subNormalAttrPtr->SetAttributeValue(subNormalAttrPtr->mapped_index(draco::PointIndex(jj)), buf.data());

			if (tid != blendVertex.targetId)
			{
				printf("[ExportMeshDataWithDraco] error : morph target is not same ");
			}
			JY_ASSERT(tid == blendVertex.targetId); // 所有Vertex的同个blends应该属于同一个blendtarget
		}

		// 找出blendshape中 对应这个morph target的 修改其索引
		bool keep = true;
		for (int a = 0; a< rawModel.GetMorphAnimationCount(); a++)
		{
			RawMorphAnimation& ra = rawModel.GetEditableAnimation(a);
			for (int i = 0 ; i < ra.morphers.size() && keep; i++)
			{
				auto& m = ra.morphers[i];
				for (int j = 0 ; j < m.deformers.size() && keep; j++)
				{
					auto& d = m.deformers[j];
					for (int k = 0; k < d.channels.size() && keep; k++)
					{
						auto& c = d.channels[k];
						for (int l = 0; l < c.targetids.size() && keep; l++)
						{
							auto & t = c.targetids[l];
							if (t == tid)
							{
								printf("morph target %" PRId64  " -> %d \n", t, pos_attr_id);
								//t = pos_attr_id; // 替换成保存在drc文件中的属性id
								keep = false;
								c.posTargetID2DrcID.emplace(std::make_pair(tid, pos_attr_id));
								c.normalTargetID2DrcID.emplace(std::make_pair(tid, normal_attr_id));
							}
						}
					}
				}
			}
		}

		if (keep)
		{
			printf("[ExportMeshDataWithDraco] error : target not found  \n");
		}
	}


	draco::Encoder encoder;
	encoder.SetSpeedOptions(10, 10);
	encoder.SetAttributeQuantization(draco::GeometryAttribute::POSITION, bCompress ? 14 : 0);
	encoder.SetAttributeQuantization(draco::GeometryAttribute::TEX_COORD, bCompress ? 20 : 0);
	encoder.SetAttributeQuantization(draco::GeometryAttribute::NORMAL, bCompress ? 10 : 0);
	encoder.SetAttributeQuantization(draco::GeometryAttribute::COLOR, bCompress ? 8 : 0);
	encoder.SetAttributeQuantization(draco::GeometryAttribute::GENERIC, bCompress ? 8 : 0);

	draco::EncoderBuffer dracoBuffer;
	draco::Status status = encoder.EncodeMeshToBuffer(*dracoMesh, &dracoBuffer);
	assert(status.code() == draco::Status::OK);
	std::shared_ptr<BufferLocator> jointlocator = nullptr;

	std::shared_ptr<BufferLocator> locator = apolloData.AddRawBuffer(*apolloData.meshBuffer, dracoBuffer.data(), dracoBuffer.size());  //加入缓存中

	meshData = apolloData.meshDats.hold(new MeshData(meshName, *locator, vertexAttributes, jointlocator));

	rawModel_rs.splitedjointsAffectIDs = splitedjointsAffectIDs;

	std::shared_ptr<BufferLocator> countlocator = apolloData.AddRawBuffer(*apolloData.meshBuffer, (const char*)trianglesCnt.data(), trianglesCnt.size() * 4);

	meshData->SetTriangleCount(countlocator);

	meshData->SetSubMeshCount(rawModel_rs.subMeshCount);

	meshData->SetDiffuseTex(texS.diffuseTexFilePath);
	meshData->SetNormalTex(texS.normalTexFilePath);
	meshData->SetMaterialType(texS.materialType);

	meshData->affectedBoneIDs = rawSurface.jointIds;
	if (!rawSurface.jointIds.empty())
	{
		meshData->isSkinning = true;
		if (rawModel.animationsSize() != 0)
		{
			const RawAnimation &rawAnimation = rawModel.GetAnimation(0);
			meshData->animationName = rawAnimation.name;
			meshData->frameCount = rawAnimation.times.size();
			meshData->isHasScale = rawAnimation.isHasScale;
		}
		
	}
	else
		meshData->isSkinning = false;
	meshData->minP = rawSurface.bounds.bmin;
	meshData->maxP = rawSurface.bounds.bmax;
	meshData->translation = rawSurface.translation;
	meshData->rotation = rawSurface.rotation;
	meshData->scale = rawSurface.scale;
	meshDataBySurfaceID.insert(std::make_pair(surfaceId, meshData));  //缓存一下mesh data 方便后边为mesh data设置skeleton信息

	return true;

}

int string_replase(std::string &s1, const std::string &s2, const std::string &s3)
{
	std::string::size_type pos = 0;
	std::string::size_type a = s2.size();
	std::string::size_type b = s3.size();
	while ((pos = s1.find(s2, pos)) != std::string::npos)
	{
		s1.replace(pos, a, s3);
		pos += b;
	}
	return 0;
}

void Convert2ValidFilePath(std::string& origPath)
{
	/*
	\/:*?"<>|
	*/
	string_replase(origPath, "\\", "_");
	string_replase(origPath, "/", "_");
	string_replase(origPath, ":", "_");
	string_replase(origPath, "*", "_");
	string_replase(origPath, "?", "_");
	string_replase(origPath, "\"", "_");
	string_replase(origPath, "<", "_");
	string_replase(origPath, ">", "_");
	string_replase(origPath, "|", "_");


	return;
}

bool ExportMeshData(ApolloBinData& apolloData, RawModel& surfaceModel, RawModel& rawModel,const ConvertOptions& convOptions, int maxBones)
{
	if (convOptions.useDarco)
	{
		ExportMeshDataWithDraco(apolloData,surfaceModel, rawModel, maxBones, convOptions.bCompress);
	}
	else if (convOptions.useCorto)
	{

	}
	else
	{
		//不压缩
	}
	return true;
}



//输出到二进制文件
//mesh文件（mesh + animation)
//skeleton （bone tree)
void ApolloBinData::Serialze(const std::string& outputPath)
{
	try
	{
		//mesh data
		if (meshDats.ptrs.empty())
		{
			return;  //必须有顶点数据
		}


		Document jsonConfigDoc;
		jsonConfigDoc.SetObject();
		Document::AllocatorType& all = jsonConfigDoc.GetAllocator();
		Value meshes(kArrayType);

		int framecount = 0;
		int index = 0;
		//FBX中两个mesh的名字是可以相同的，需要特殊处理
		std::map<std::string, bool> nameMapTmp;
		int nameSuffix = 1;
		for (const auto &ptr : meshDats.ptrs)
		{
			
			std::ofstream meshStream;
			std::string meshName = ptr->name;
		
			Convert2ValidFilePath(meshName);
			std::transform(meshName.begin(), meshName.end(), meshName.begin(), ::tolower);
			if (nameMapTmp.find(meshName) != nameMapTmp.end())
			{
				meshName = meshName + std::to_string(nameSuffix);
				nameSuffix++;
			}
			nameMapTmp[meshName] = true;
			std::string meshFullPath = outputPath + "/data/" + meshName + ".mesh"; //mesh 文件
			//Convert2ValidFilePath(meshFullPath);
			std::transform(meshFullPath.begin(), meshFullPath.end(),meshFullPath.begin(), ::tolower);
			if (!FileUtils::CreatePath(meshFullPath.c_str()))
			{
				fprintf(stderr, "ERROR: Failed to create folder: %s'\n", meshFullPath.c_str());
				return ;
			}
			meshStream.open(meshFullPath, std::ios::trunc | std::ios::ate | std::ios::out | std::ios::binary);
			if (meshStream.fail()) {
				fprintf(stderr, "ERROR:: Couldn't open file for writing: %s\n", meshFullPath.c_str());
				return;
			}

			Value meshData;
			meshData.SetObject();

			Value strV;
			meshData.AddMember("name", strV.SetString(std::string("./data/" + meshName + ".mesh").c_str(),all),all);

			Value vertexAttribute(kArrayType);
			if (ptr->vertexLayout & RAW_VERTEX_ATTRIBUTE_POSITION)
			{
				Value ATTRPos(kObjectType);
				ATTRPos.SetObject();
				ATTRPos.SetString("position");
				vertexAttribute.PushBack(ATTRPos, all);
			}
			if (ptr->vertexLayout & RAW_VERTEX_ATTRIBUTE_NORMAL)
			{
				Value ATTRPos(kObjectType);
				ATTRPos.SetObject();
				ATTRPos.SetString("normal");
				vertexAttribute.PushBack(ATTRPos, all);
			}
			if (ptr->vertexLayout & RAW_VERTEX_ATTRIBUTE_TANGENT)
			{
				Value ATTRPos(kObjectType);
				ATTRPos.SetObject();
				ATTRPos.SetString("tangent");
				vertexAttribute.PushBack(ATTRPos, all);
			}
			if (ptr->vertexLayout & RAW_VERTEX_ATTRIBUTE_BINORMAL)
			{
				Value ATTRPos(kObjectType);
				ATTRPos.SetObject();
				ATTRPos.SetString("binormal");
				vertexAttribute.PushBack(ATTRPos, all);
			}
			if (ptr->vertexLayout & RAW_VERTEX_ATTRIBUTE_UV0)
			{
				Value ATTRPos(kObjectType);
				ATTRPos.SetObject();
				ATTRPos.SetString("texcoord0");
				vertexAttribute.PushBack(ATTRPos, all);
			}
			if (ptr->vertexLayout & RAW_VERTEX_ATTRIBUTE_UV1)
			{
				Value ATTRPos(kObjectType);
				ATTRPos.SetObject();
				ATTRPos.SetString("texcoord1");
				vertexAttribute.PushBack(ATTRPos, all);
			}
			if (ptr->vertexLayout & RAW_VERTEX_ATTRIBUTE_UV2)
			{
				Value ATTRPos(kObjectType);
				ATTRPos.SetObject();
				ATTRPos.SetString("texcoord2");
				vertexAttribute.PushBack(ATTRPos, all);
			}
			if (ptr->vertexLayout & RAW_VERTEX_ATTRIBUTE_UV3)
			{
				Value ATTRPos(kObjectType);
				ATTRPos.SetObject();
				ATTRPos.SetString("texcoord3");
				vertexAttribute.PushBack(ATTRPos, all);
			}
			if (ptr->vertexLayout & RAW_VERTEX_ATTRIBUTE_COLOR)
			{
				Value ATTRPos(kObjectType);
				ATTRPos.SetObject();
				ATTRPos.SetString("color");
				vertexAttribute.PushBack(ATTRPos, all);
			}
			if (ptr->vertexLayout & RAW_VERTEX_ATTRIBUTE_JOINT_INDICES)
			{
				Value ATTRPos(kObjectType);
				ATTRPos.SetObject();
				ATTRPos.SetString("boneindex");
				vertexAttribute.PushBack(ATTRPos, all);
			}
			if (ptr->vertexLayout & RAW_VERTEX_ATTRIBUTE_JOINT_WEIGHTS)
			{
				Value ATTRPos(kObjectType);
				ATTRPos.SetObject();
				ATTRPos.SetString("boneweight");
				vertexAttribute.PushBack(ATTRPos, all);
			}
			meshData.AddMember("vertexattribute", vertexAttribute, all);

			//bounding box
			Value boundingData(kObjectType);
			//min
			Value minP(kArrayType);
			Value minX(kObjectType); minX.SetObject(); minX.SetDouble(ptr->minP.x);
			Value minY(kObjectType); minY.SetObject(); minY.SetDouble(ptr->minP.y);
			Value minZ(kObjectType); minZ.SetObject(); minZ.SetDouble(ptr->minP.z);
			minP.PushBack(minX, all); minP.PushBack(minY, all); minP.PushBack(minZ, all);
			boundingData.AddMember("min", minP, all);
			//max
			Value maxP(kArrayType);
			Value maxX(kObjectType); maxX.SetObject(); maxX.SetDouble(ptr->maxP.x);
			Value maxY(kObjectType); maxY.SetObject(); maxY.SetDouble(ptr->maxP.y);
			Value maxZ(kObjectType); maxZ.SetObject(); maxZ.SetDouble(ptr->maxP.z);
			maxP.PushBack(maxX, all); maxP.PushBack(maxY, all); maxP.PushBack(maxZ, all);
			boundingData.AddMember("max", maxP, all);
			meshData.AddMember("boundingbox", boundingData, all);

			if (ptr->isSkinning)	{			
				framecount = ptr->frameCount;
				meshData.AddMember("maxbonecnt", ptr->maxBone, all);
			}

			if (!materials.ptrs.empty())
						{
							auto mData = materials.ptrs[ptr->materialIndex];
				Value materialData(kObjectType);

				materialData.AddMember("name", strV.SetString(mData->name.c_str(), all), all);
				int alphaMode = mData->isTransparent ? 1 : 0;
				materialData.AddMember("alphaMode", alphaMode, all);
				std::string shadingMode = Describe(mData->shadingModel);
				materialData.AddMember("shadingModel", strV.SetString(shadingMode.c_str(), all), all);
				materialData.AddMember("isTruePBR", mData->shadingModel == RAW_SHADING_MODEL_PBR_MET_ROUGH, all);

				if (mData->normalTexture)
				{
					Value normalTexData(kObjectType);
					std::string texPath = "./data/" + images.ptrs[textures.ptrs[mData->normalTexture->texRef]->source]->uri;
					int texCoord = mData->normalTexture->texCoord;
					normalTexData.AddMember("path", strV.SetString(texPath.c_str(), all), all);
					normalTexData.AddMember("texCoord", texCoord, all);
					materialData.AddMember("normalTexture", normalTexData, all);
				}

				if (mData->occlusionTexture)
				{
					Value occlusionTexData(kObjectType);
					std::string texPath = "./data/" + images.ptrs[textures.ptrs[mData->occlusionTexture->texRef]->source]->uri;
					int texCoord = mData->occlusionTexture->texCoord;
					occlusionTexData.AddMember("path", strV.SetString(texPath.c_str(), all), all);
					occlusionTexData.AddMember("texCoord", texCoord, all);
					materialData.AddMember("occlusionTexture", occlusionTexData, all);
				}

				if (mData->reflectionTexture)
				{
					Value reflectionTexData(kObjectType);
					std::string texPath = "./data/" + images.ptrs[textures.ptrs[mData->reflectionTexture->texRef]->source]->uri;
					int texCoord = mData->reflectionTexture->texCoord;
					reflectionTexData.AddMember("path", strV.SetString(texPath.c_str(), all), all);
					reflectionTexData.AddMember("texCoord", texCoord, all);
					materialData.AddMember("reflectionTexture", reflectionTexData, all);
				}

				Value pbrMetallicRoughnessData(kObjectType);

				if (mData->pbrMetallicRoughness->baseColorTexture)
				{
					Value baseColorTexData(kObjectType);
					std::string texPath = "./data/" + images.ptrs[textures.ptrs[mData->pbrMetallicRoughness->baseColorTexture->texRef]->source]->uri;
					int texCoord = mData->pbrMetallicRoughness->baseColorTexture->texCoord;
					baseColorTexData.AddMember("path", strV.SetString(texPath.c_str(), all), all);
					baseColorTexData.AddMember("texCoord", texCoord, all);
					pbrMetallicRoughnessData.AddMember("baseColorTexture", baseColorTexData, all);
				}

				Value baseColorFactorData(kArrayType);
				Value factorX(kObjectType); factorX.SetObject(); factorX.SetDouble(mData->pbrMetallicRoughness->baseColorFactor.x);
				Value factorY(kObjectType); factorY.SetObject(); factorY.SetDouble(mData->pbrMetallicRoughness->baseColorFactor.y);
				Value factorZ(kObjectType); factorZ.SetObject(); factorZ.SetDouble(mData->pbrMetallicRoughness->baseColorFactor.z);
				baseColorFactorData.PushBack(factorX, all); baseColorFactorData.PushBack(factorY, all); baseColorFactorData.PushBack(factorZ, all);
				pbrMetallicRoughnessData.AddMember("baseColorFactor", baseColorFactorData, all);


				if (mData->pbrMetallicRoughness->metRoughTexture)
				{
					Value mrTexData(kObjectType);
					std::string mrPath = "./data/" + images.ptrs[textures.ptrs[mData->pbrMetallicRoughness->metRoughTexture->texRef]->source]->uri;
					int mrTexCoord = mData->pbrMetallicRoughness->metRoughTexture->texCoord;
					mrTexData.AddMember("path", strV.SetString(mrPath.c_str(), all), all);
					mrTexData.AddMember("texCoord", mrTexCoord, all);
					pbrMetallicRoughnessData.AddMember("metallicRoughnessTexture", mrTexData, all);
				}



				pbrMetallicRoughnessData.AddMember("roughnessFactor", mData->pbrMetallicRoughness->roughness, all);
				pbrMetallicRoughnessData.AddMember("metallicFactor", mData->pbrMetallicRoughness->metallic, all);

				materialData.AddMember("pbrMetallicRoughness", pbrMetallicRoughnessData, all);

				meshData.AddMember("material", materialData, all);
			}
			
			Value transformData(kObjectType);
			//pos
			Value wPos(kArrayType);
			Value posX(kObjectType);
			posX.SetObject();
			posX.SetDouble(ptr->translation.x);
			Value posY(kObjectType);
			posY.SetObject();
			posY.SetDouble(ptr->translation.y);
			Value posZ(kObjectType);
			posZ.SetObject();
			posZ.SetDouble(ptr->translation.z);
			wPos.PushBack(posX, all);
			wPos.PushBack(posY, all);
			wPos.PushBack(posZ, all);
			transformData.AddMember("pos", wPos, all);

			//scale
			Value wScale(kArrayType);
			Value scaleX(kObjectType);
			scaleX.SetObject();
			scaleX.SetDouble(ptr->scale.x);
			Value scaleY(kObjectType);
			scaleY.SetObject();
			scaleY.SetDouble(ptr->scale.y);
			Value scaleZ(kObjectType);
			scaleZ.SetObject();
			scaleZ.SetDouble(ptr->scale.z);
			wScale.PushBack(scaleX, all);
			wScale.PushBack(scaleY, all);
			wScale.PushBack(scaleZ, all);
			transformData.AddMember("scale", wScale, all);

			//rotation
			Value wRot(kArrayType);
			Value rotX(kObjectType);
			rotX.SetObject();
			rotX.SetDouble(ptr->rotation.x);
			Value rotY(kObjectType);
			rotY.SetObject();
			rotY.SetDouble(ptr->rotation.y);
			Value rotZ(kObjectType);
			rotZ.SetObject();
			rotZ.SetDouble(ptr->rotation.z);
			Value rotW(kObjectType);
			rotW.SetObject();
			rotW.SetDouble(ptr->rotation.w);
			wRot.PushBack(rotX, all);
			wRot.PushBack(rotY, all);
			wRot.PushBack(rotZ, all);
			wRot.PushBack(rotW, all);
			transformData.AddMember("rotation", wRot, all);
			meshData.AddMember("transform", transformData, all);


			meshStream.write(apolHeader, sizeof(apolHeader));
			meshStream.write(bindingBoxDemo, sizeof(bindingBoxDemo));
			meshStream.write((char*)(&ptr->chunkIdentifier), sizeof(int));
			meshStream.write((char*)(&ptr->vertexLayout), sizeof(int));
			int offsetPos = meshStream.tellp();
			unsigned int offset = bufferLocs.ptrs[ptr->locatorIndex]->byteOffset;
			meshStream.write((char*)(&offset), sizeof(unsigned int));
			unsigned int byteLength = bufferLocs.ptrs[ptr->locatorIndex]->byteLength;
			meshStream.write((char*)(&byteLength), sizeof(unsigned int));
			meshStream.write((char*)(&ptr->subMeshCnt), sizeof(int));
			
			int jointOffsetPos = meshStream.tellp();
			 
			std::vector<int> jointIDOffset;
			std::vector<int> jointIDLength;

			int trianglesCntOffset = bufferLocs.ptrs[ptr->triangleCnt]->byteOffset;
			meshStream.write((char*)(&trianglesCntOffset), sizeof(unsigned int));
			int trianglesCntLength = bufferLocs.ptrs[ptr->triangleCnt]->byteLength;
			meshStream.write((char*)(&trianglesCntLength), sizeof(unsigned int));

			int offsetPos2 = meshStream.tellp();


			meshStream.write((char*)(&chunkEnd), sizeof(int));

			int currPos = meshStream.tellp();
			meshStream.seekp(0, std::ios_base::beg);  //跳转到起始位置

			char apolHeaderMeshV3[] = {
				'a', 'p', 'o', 'l',            // 格式名称
				0x05, 0x00, 0x00, 0x00,        // 版本号
				0x00, 0x00, 0x00, 0x00,        // 二进制数据流起始位置
			};
			char* pHeader = &apolHeaderMeshV3[8];
			int* pBinaryBeginPos = (int*)pHeader;
			*pBinaryBeginPos = currPos;
			meshStream.write(apolHeaderMeshV3, sizeof(apolHeaderMeshV3));
			//写入包围盒
			float bindingBox[] =
			{
				ptr->minP.x,ptr->minP.y,ptr->minP.z,
				ptr->maxP.x,ptr->maxP.y,ptr->maxP.z,
			};
			char bindingBoxCharArray[24];
			for (int i = 0; i < 6; i++)
			{
				unsigned char* fp = (unsigned char*)&bindingBox[i];
				for (int j = 0; j < 4; j++)
				{
					bindingBoxCharArray[(i * 4 + j)] = *fp++;//把相应地址中的数据保存到unsigned char数组中       
				}
			}
			meshStream.write(bindingBoxCharArray, sizeof(bindingBoxCharArray));
			meshStream.seekp(currPos, std::ios_base::beg);
			
			//mesh二进制流数据


			meshStream.write((char*)(meshBuffer->binData->data() + offset), byteLength);
			
			meshStream.write((char*)(meshBuffer->binData->data() + trianglesCntOffset), trianglesCntLength);

			meshStream.close();

			meshes.PushBack(meshData, all);
		}
		jsonConfigDoc.AddMember("version", 4, all);

		jsonConfigDoc.AddMember("meshes", meshes, all);

		std::string skeName;
		for (const auto &ptr : skeDatV2.ptrs)
		{
			std::ofstream skeletonStream;
			skeName = ptr->meshName;
			Convert2ValidFilePath(skeName);
			std::transform(skeName.begin(), skeName.end(), skeName.begin(), ::tolower);
			std::string skeletonFullPath = outputPath + "/Data/" + skeName + ".ske"; //动画
			skeName = "./Data/" + skeName + ".ske";
			std::transform(skeletonFullPath.begin(), skeletonFullPath.end(), skeletonFullPath.begin(), ::tolower);
			if (!FileUtils::CreatePath(skeletonFullPath.c_str()))
			{
				fprintf(stderr, "ERROR: Failed to create folder: %s'\n", skeletonFullPath.c_str());
				return;
			}
			skeletonStream.open(skeletonFullPath, std::ios::trunc | std::ios::ate | std::ios::out | std::ios::binary);
			if (skeletonStream.fail()) {
				fprintf(stderr, "ERROR:: Couldn't open file for writing: %s\n", skeletonFullPath.c_str());
				return;
			}

			char apolHeaderSkelV2[] = {
				'a', 'p', 'o', 'l',            // 格式名称
				0x03, 0x00, 0x00, 0x00,        // 版本号
				0x00, 0x00, 0x00, 0x00,        // 二进制数据流起始位置
			};

			skeletonStream.write(apolHeaderSkelV2, sizeof(apolHeaderSkelV2));

			int begPos = 0;
			skeletonStream.write((char*)(&ptr->chunkIdentifier), sizeof(int));
			unsigned int offset = begPos;// bufferLocs.ptrs[ptr->boneHierarchyLocatorIndex]->byteOffset;
			skeletonStream.write((char*)(&offset), sizeof(unsigned int));
			unsigned int byteLength = bufferLocs.ptrs[ptr->boneHierarchyLocatorIndex]->byteLength;
			skeletonStream.write((char*)(&byteLength), sizeof(unsigned int));
			offset = offset + byteLength;// bufferLocs.ptrs[ptr->inverseBindMatrixLocatorIndex]->byteOffset;
			skeletonStream.write((char*)(&offset), sizeof(unsigned int));
			byteLength = bufferLocs.ptrs[ptr->inverseBindMatrixLocatorIndex]->byteLength;
			skeletonStream.write((char*)(&byteLength), sizeof(unsigned int));
			offset = offset + byteLength;// bufferLocs.ptrs[ptr->inverseBindMatrixLocatorIndex]->byteOffset;
			skeletonStream.write((char*)(&offset), sizeof(unsigned int));
			

			skeletonStream.write((char*)(&chunkEnd), sizeof(int));
			int currPos = skeletonStream.tellp();
			skeletonStream.seekp(0, std::ios_base::beg);  //跳转到起始位置
			char* pHeader = &apolHeaderSkelV2[8];
			int* pBinaryBeginPos = (int*)pHeader;
			*pBinaryBeginPos = currPos;
			skeletonStream.write(apolHeaderSkelV2, sizeof(apolHeaderSkelV2));
			skeletonStream.seekp(currPos, std::ios_base::beg);

			//写二进制数据
			skeletonStream.write((char*)(skeletonBufferV2->binData->data() + bufferLocs.ptrs[ptr->boneHierarchyLocatorIndex]->byteOffset), bufferLocs.ptrs[ptr->boneHierarchyLocatorIndex]->byteLength);
			skeletonStream.write((char*)(skeletonBufferV2->binData->data() + bufferLocs.ptrs[ptr->inverseBindMatrixLocatorIndex]->byteOffset), bufferLocs.ptrs[ptr->inverseBindMatrixLocatorIndex]->byteLength);

			skeletonStream.close();

		}
		Value SkeletonData;
		SkeletonData.SetObject();
		Value strV;
		SkeletonData.AddMember("name", strV.SetString(skeName.c_str(), all) , all);
		
		Value animData(kArrayType);
		Value scale;
		scale.SetObject();

		Value oriG;
		Value originalAni;
		originalAni.SetObject();

		//animation data
		std::string aniName;
		bool hasscale = false;
		int bufferIndex = -1;
		for (const auto &ptr : aniDats.ptrs)
		{
			bufferIndex++;
			Value animation(kObjectType);
			animation.SetObject();
			std::ofstream animationStream;
			std::string skeletonFullPath = outputPath + "/Data/" + ptr->name + ".ani"; //动画
			aniName = "./Data/" + ptr->name + ".ani";
			std::transform(skeletonFullPath.begin(), skeletonFullPath.end(), skeletonFullPath.begin(), ::tolower);
			if (!FileUtils::CreatePath(skeletonFullPath.c_str()))
			{
				fprintf(stderr, "ERROR: Failed to create folder: %s'\n", skeletonFullPath.c_str());
				return;
			}
			animationStream.open(skeletonFullPath, std::ios::trunc | std::ios::ate | std::ios::out | std::ios::binary);
			if (animationStream.fail()) {
				fprintf(stderr, "ERROR:: Couldn't open file for writing: %s\n", skeletonFullPath.c_str());
				return;
			}

			char apolHeaderAnim[] = {
				'a', 'p', 'o', 'l',            // 格式名称
				0x02, 0x00, 0x00, 0x00,        // 版本号
				0x00, 0x00, 0x00, 0x00,        // 二进制数据流起始位置
			};

			animationStream.write(apolHeaderAnim, sizeof(apolHeaderAnim));

			animationStream.write((char*)(&ptr->chunkIdentifier), sizeof(int));
			char tmpName[64] = { 0 };
			std::string temp = ptr->name;
			std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
            memcpy(tmpName, temp.c_str(), temp.length());
			animationStream.write(tmpName, 64);
			unsigned int offset = bufferLocs.ptrs[ptr->timesIndex]->byteOffset;
			animationStream.write((char*)(&offset), sizeof(unsigned int));
			unsigned int byteLength = bufferLocs.ptrs[ptr->timesIndex]->byteLength;
			animationStream.write((char*)(&byteLength), sizeof(unsigned int));


			offset = bufferLocs.ptrs[ptr->keyTimeOffsetIndex]->byteOffset;
			animationStream.write((char*)(&offset), sizeof(unsigned int));
			byteLength = bufferLocs.ptrs[ptr->keyTimeOffsetIndex]->byteLength;
			animationStream.write((char*)(&byteLength), sizeof(unsigned int));

			offset = bufferLocs.ptrs[ptr->keyTimeLengthIndex]->byteOffset;
			animationStream.write((char*)(&offset), sizeof(unsigned int));
			byteLength = bufferLocs.ptrs[ptr->keyTimeLengthIndex]->byteLength;
			animationStream.write((char*)(&byteLength), sizeof(unsigned int));

			if (ptr->isHasScale)
			{
				hasscale = true;
				int animationType = 0;
				animationStream.write((char*)&animationType, sizeof(int)); //动画类型（0为矩阵动画类型）
				offset = bufferLocs.ptrs[ptr->transformOffsetIndex]->byteOffset;
				animationStream.write((char*)(&offset), sizeof(unsigned int));
				byteLength = bufferLocs.ptrs[ptr->transformOffsetIndex]->byteLength;
				animationStream.write((char*)(&byteLength), sizeof(unsigned int));

				offset = bufferLocs.ptrs[ptr->transformLengthIndex]->byteOffset;
				animationStream.write((char*)(&offset), sizeof(unsigned int));
				byteLength = bufferLocs.ptrs[ptr->transformLengthIndex]->byteLength;
				animationStream.write((char*)(&byteLength), sizeof(unsigned int));


				offset = bufferLocs.ptrs[ptr->boneIDsIndex]->byteOffset;
				byteLength = bufferLocs.ptrs[ptr->boneIDsIndex]->byteLength;
				animationStream.write((char*)(&offset), sizeof(unsigned int));
				animationStream.write((char*)(&byteLength), sizeof(unsigned int));

				int currPos = animationStream.tellp();
				animationStream.seekp(0, std::ios_base::beg);  //跳转到起始位置
				char* pHeader = &apolHeaderAnim[8];
				int* pBinaryBeginPos = (int*)pHeader;
				*pBinaryBeginPos = currPos;
				animationStream.write(apolHeaderAnim, sizeof(apolHeaderAnim));
				animationStream.seekp(currPos, std::ios_base::beg);

				animationStream.write((char*)(animationBuffer[bufferIndex]->binData->data()), animationBuffer[bufferIndex]->binData->size());
			}
			else
			{
				int animationType = 1;
				animationStream.write((char*)&animationType, sizeof(int));
				offset = bufferLocs.ptrs[ptr->transformAttributeIndex]->byteOffset;
				animationStream.write((char*)(&offset), sizeof(unsigned int));
				byteLength = bufferLocs.ptrs[ptr->transformAttributeIndex]->byteLength;
				animationStream.write((char*)(&byteLength), sizeof(unsigned int));

				offset = bufferLocs.ptrs[ptr->rotationOffsetIndex]->byteOffset;
				animationStream.write((char*)(&offset), sizeof(unsigned int));
				byteLength = bufferLocs.ptrs[ptr->rotationOffsetIndex]->byteLength;
				animationStream.write((char*)(&byteLength), sizeof(unsigned int));

				offset = bufferLocs.ptrs[ptr->rotationLengthIndex]->byteOffset;
				animationStream.write((char*)(&offset), sizeof(unsigned int));
				byteLength = bufferLocs.ptrs[ptr->rotationLengthIndex]->byteLength;
				animationStream.write((char*)(&byteLength), sizeof(unsigned int));

				offset = bufferLocs.ptrs[ptr->translationOffsetIndex]->byteOffset;
				animationStream.write((char*)(&offset), sizeof(unsigned int));
				byteLength = bufferLocs.ptrs[ptr->translationOffsetIndex]->byteLength;
				animationStream.write((char*)(&byteLength), sizeof(unsigned int));

				offset = bufferLocs.ptrs[ptr->translationLengthIndex]->byteOffset;
				animationStream.write((char*)(&offset), sizeof(unsigned int));
				byteLength = bufferLocs.ptrs[ptr->translationLengthIndex]->byteLength;
				animationStream.write((char*)(&byteLength), sizeof(unsigned int));

				offset = bufferLocs.ptrs[ptr->boneIDsIndex]->byteOffset;
				byteLength = bufferLocs.ptrs[ptr->boneIDsIndex]->byteLength;
				animationStream.write((char*)(&offset), sizeof(unsigned int));
				animationStream.write((char*)(&byteLength), sizeof(unsigned int));

				int currPos = animationStream.tellp();
				animationStream.seekp(0, std::ios_base::beg);  //跳转到起始位置
				char* pHeader = &apolHeaderAnim[8];
				int* pBinaryBeginPos = (int*)pHeader;
				*pBinaryBeginPos = currPos;
				animationStream.write(apolHeaderAnim, sizeof(apolHeaderAnim));
				animationStream.seekp(currPos, std::ios_base::beg);

				animationStream.write((char*)(animationBuffer[bufferIndex]->binData->data()), animationBuffer[bufferIndex]->binData->size());
			}
			
			animationStream.write((char*)(&ptr->beginFrame), sizeof(int));

			animationStream.close();
			animation.AddMember("name", strV.SetString(aniName.c_str(), all), all);
			animation.AddMember("framecount", ptr->frameCount, all);
			animData.PushBack(animation, all);
		}

		originalAni.AddMember("original", oriG.SetString(originAnimation.c_str(), all), all);

		scale.AddMember("hasscale", hasscale, all);

		if (aniDats.ptrs.size() > 0)
		{
			SkeletonData.AddMember("scale", scale, all);
			SkeletonData.AddMember("animation", animData, all);
			SkeletonData.AddMember("originalAni", originalAni, all);
		}
		
		if (skeDatV2.ptrs.size() > 0)
		{
			jsonConfigDoc.AddMember("skeleton", SkeletonData, all);
		}
		

		// 导出moprh动画信息到json文件

		if (morpherAnimations.size() > 0)
		{
			
			rapidjson::Value jAnimationList(rapidjson::kArrayType);

			for(const auto& clip: morpherAnimations)
			{

				rapidjson::Value jClip;
				jClip.SetObject();

				jClip.AddMember("clipstart", clip.beginTime, all);
				jClip.AddMember("clipend", clip.endTime, all);


				rapidjson::Value jModelList(rapidjson::kArrayType);
				for (auto morph : clip.morphers)
				{
					rapidjson::Value jMesh;
					jMesh.SetObject();

					rapidjson::Value jName;
					jMesh.AddMember("name", morph.originid , all);

					rapidjson::Value jPath; // FIXME(hhl) 路径需要根据保存的修改 
					jMesh.AddMember("path", jPath.SetString(std::string("./data/" + morph.originname + ".mesh").c_str(), all), all);

					rapidjson::Value jDeformers(rapidjson::kArrayType);
					for (auto& deformer : morph.deformers)
					{
						rapidjson::Value jDeformer;
						jDeformer.SetObject();

						// "name"
						rapidjson::Value jName;
						jDeformer.AddMember("name", jName.SetString(deformer.name.c_str(), all), all);

						//
						rapidjson::Value JCid;
						jDeformer.AddMember("gid", deformer.deformerid, all);

						rapidjson::Value jChannels(rapidjson::kArrayType);
						for (auto& curve : deformer.channels)
						{
							rapidjson::Value jCruve;
							jCruve.SetObject();

							// "name"
							rapidjson::Value jName;
							jCruve.AddMember("name", jName.SetString(curve.name.c_str(), all), all);

							//
							rapidjson::Value JCid;
							jCruve.AddMember("cid", curve.cid, all);


							// "keys"
							rapidjson::Value jKeyFrames(rapidjson::kArrayType);
							for (auto& keyframe : curve.keyframes)
							{
								rapidjson::Value jKeyFrame(rapidjson::kArrayType);
								{
									rapidjson::Value temp(rapidjson::kObjectType);
									temp.SetObject();
									temp.SetDouble(keyframe.x);
									jKeyFrame.PushBack(temp, all);
								}
								{
									rapidjson::Value temp(rapidjson::kObjectType);
									temp.SetObject();
									temp.SetDouble(keyframe.y);
									jKeyFrame.PushBack(temp, all);
								}
								{
									rapidjson::Value temp(rapidjson::kObjectType);
									temp.SetObject();
									temp.SetDouble(keyframe.z);
									jKeyFrame.PushBack(temp, all);
								}
								{
									rapidjson::Value temp(rapidjson::kObjectType);
									temp.SetObject();
									temp.SetDouble(keyframe.w);
									jKeyFrame.PushBack(temp, all);
								}
								jKeyFrames.PushBack(jKeyFrame, all);
							}
							jCruve.AddMember("keys", jKeyFrames, all);

							// "position"
							// "normal"
							// "weights"
							rapidjson::Value targetPositionAttriID(rapidjson::kArrayType);
							rapidjson::Value targetNormalAttriID(rapidjson::kArrayType);
							rapidjson::Value targetsPercent(rapidjson::kArrayType);

							for (int t = 0; t < curve.targetids.size(); t++)
							{
								uint64_t ids = curve.targetids[t];

								auto itor = curve.posTargetID2DrcID.find(ids);
								if (itor == curve.posTargetID2DrcID.end())
								{
									printf("[Serialze] error %llu not found position", ids);
									JY_ASSERT(false);
									continue;
								}
								uint64_t pos_id = itor->second;

								itor = curve.normalTargetID2DrcID.find(ids);
								if (itor == curve.normalTargetID2DrcID.end())
								{
									printf("[Serialze] error %llu not found normal", ids);
									JY_ASSERT(false);
									continue;
								}
								uint64_t normal_id = itor->second;

								{
									rapidjson::Value temp(rapidjson::kObjectType);
									temp.SetObject();
									temp.SetInt(pos_id);
									targetPositionAttriID.PushBack(temp, all);
								}

								{
									rapidjson::Value temp(rapidjson::kObjectType);
									temp.SetObject();
									temp.SetInt(normal_id);
									targetNormalAttriID.PushBack(temp, all);
								}


								{
									rapidjson::Value temp(rapidjson::kObjectType);
									temp.SetObject();
									temp.SetInt(curve.weights[t]);
									targetsPercent.PushBack(temp, all);
								}
							}

							jCruve.AddMember("position", targetPositionAttriID, all);
							jCruve.AddMember("normal", targetNormalAttriID, all);
							jCruve.AddMember("weights", targetsPercent, all);


							jChannels.PushBack(jCruve, all);
						}

						jDeformer.AddMember("channels", jChannels, all);

						jDeformers.PushBack(jDeformer, all);						
					}

					jMesh.AddMember("group", jDeformers, all);

					jModelList.PushBack(jMesh, all);
				}

				jClip.AddMember("modellist", jModelList, all);

				jAnimationList.PushBack(jClip, all);

			}

			jsonConfigDoc.AddMember("morpher", jAnimationList, all);

		}


		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		jsonConfigDoc.Accept(writer);
		std::string dataString = buffer.GetString();

		std::transform(dataString.begin(), dataString.end(), dataString.begin(), ::tolower);

		std::ofstream configStream;
		std::string configFullPath = outputPath + "/config.json"; //mesh 文件
		if (!FileUtils::CreatePath(configFullPath.c_str()))
		{
			fprintf(stderr, "ERROR: Failed to create folder: %s'\n", configFullPath.c_str());
			return;
		}
		configStream.open(configFullPath, std::ios::trunc | std::ios::ate | std::ios::out);
		if (configStream.fail()) {
			fprintf(stderr, "ERROR:: Couldn't open file for writing: %s\n", configFullPath.c_str());
			return;
		}
		configStream.write(dataString.c_str(), dataString.size());
	}
	
	catch (std::ofstream::failure &writeErr) {
		std::cerr << "\n\nException occured when writing to a file\n"
			<< writeErr.what()
			<< std::endl;
		return ;
	}

	


}


