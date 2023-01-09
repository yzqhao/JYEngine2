/**
 * Copyright (c) 2014-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
#include <string>
#include <fstream>
#include <cstdint>
#include <cstdio>
#include <cassert>


#include "File_Utils.h"
#include "String_Utils.h"
#include "RawModel.h"
#include "Fbx2Raw.h"
#include "FbxMathUtil.h"

#include <memory>
#include <fbxsdk.h>

#include "Math/3DMath.h"

extern bool verboseOutput;

float scaleFactor;

template<typename _type_>
class FbxLayerElementAccess
{
public:

    FbxLayerElementAccess(const FbxLayerElementTemplate<_type_> *layer, int count) :
        mappingMode(FbxGeometryElement::eNone),
        elements(nullptr),
        indices(nullptr)
    {
        if (count <= 0 || layer == nullptr) {
            return;
        }
        const FbxGeometryElement::EMappingMode newMappingMode = layer->GetMappingMode();
        if (newMappingMode == FbxGeometryElement::eByControlPoint ||
            newMappingMode == FbxGeometryElement::eByPolygonVertex ||
            newMappingMode == FbxGeometryElement::eByPolygon) {
            mappingMode = newMappingMode;
            elements    = &layer->GetDirectArray();
            indices     = (
                layer->GetReferenceMode() == FbxGeometryElement::eIndexToDirect ||
                layer->GetReferenceMode() == FbxGeometryElement::eIndex) ? &layer->GetIndexArray() : nullptr;
        }
    }

    bool LayerPresent() const
    {
        return (mappingMode != FbxGeometryElement::eNone);
    }

    _type_ GetElement(const int polygonIndex, const int polygonVertexIndex, const int controlPointIndex, const _type_ defaultValue) const
    {
        if (mappingMode != FbxGeometryElement::eNone) {
            int index = (mappingMode == FbxGeometryElement::eByControlPoint) ? controlPointIndex :
                        ((mappingMode == FbxGeometryElement::eByPolygonVertex) ? polygonVertexIndex : polygonIndex);
            index = (indices != nullptr) ? (*indices)[index] : index;
            _type_ element = elements->GetAt(index);
            return element;
        }
        return defaultValue;
    }

    _type_ GetElement(
        const int polygonIndex, const int polygonVertexIndex, const int controlPointIndex, const _type_ defaultValue,
        const FbxMatrix &transform, const bool normalize) const
    {
        if (mappingMode != FbxGeometryElement::eNone) {
            _type_ element = transform.MultNormalize(GetElement(polygonIndex, polygonVertexIndex, controlPointIndex, defaultValue));
            if (normalize) {
                element.Normalize();
            }
            return element;
        }
        return defaultValue;
    }

private:
    FbxGeometryElement::EMappingMode           mappingMode;
    const FbxLayerElementArrayTemplate<_type_> *elements;
    const FbxLayerElementArrayTemplate<int>    *indices;
};

struct FbxMaterialInfo {
    FbxMaterialInfo(const FbxString &name, const FbxString &shadingModel)
        : name(name),
          shadingModel(shadingModel)
    {}
    const FbxString name;
    const FbxString shadingModel;
};

struct FbxRoughMetMaterialInfo : FbxMaterialInfo {
    static constexpr const char *FBX_SHADER_METROUGH = "MetallicRoughness";

    FbxRoughMetMaterialInfo(const FbxString &name, const FbxString &shadingModel)
        : FbxMaterialInfo(name, shadingModel)
    {}
    const FbxFileTexture *texColor {};
    FbxVector4           colBase {};
    const FbxFileTexture *texNormal {};
    const FbxFileTexture *texMetallic {};
    FbxDouble            metallic {};
    const FbxFileTexture *texRoughness {};
    FbxDouble            roughness {};
    const FbxFileTexture *texEmissive {};
    FbxVector4           colEmissive {};
	const FbxFileTexture *texReflection{};
    FbxDouble            emissiveIntensity {};
    const FbxFileTexture *texAmbientOcclusion {};

    static std::unique_ptr<FbxRoughMetMaterialInfo> From(
        FbxSurfaceMaterial *fbxMaterial,
        const std::map<const FbxTexture *, FbxString> &textureLocations)
    {
        std::unique_ptr<FbxRoughMetMaterialInfo> res(new FbxRoughMetMaterialInfo(fbxMaterial->GetName(), FBX_SHADER_METROUGH));

        const FbxProperty mayaProp = fbxMaterial->FindProperty("Maya");
        if (mayaProp.GetPropertyDataType() != FbxCompoundDT) {
            return nullptr;
        }
        if (!fbxMaterial->ShadingModel.Get().IsEmpty()) {
            printf("Warning: Material %s has surprising shading model: %s\n",
                fbxMaterial->GetName(), *(fbxMaterial->ShadingModel.Get()));
        }

        auto getTex = [&](std::string propName) {
            const FbxFileTexture *ptr = nullptr;

            const FbxProperty useProp = mayaProp.FindHierarchical(("use_" + propName + "_map").c_str());
            if (useProp.IsValid() && useProp.Get<bool>()) {
                const FbxProperty texProp = mayaProp.FindHierarchical(("TEX_" + propName + "_map").c_str());
                if (texProp.IsValid()) {
                    ptr = texProp.GetSrcObject<FbxFileTexture>();
                    if (ptr != nullptr && textureLocations.find(ptr) == textureLocations.end()) {
                        ptr = nullptr;
                    }
                }
            } else if (verboseOutput && useProp.IsValid()) {
                printf("Note: Property '%s' of material '%s' exists, but is flagged as 'do not use'.\n",
					   propName.c_str(), fbxMaterial->GetName());
            }
            return ptr;
        };

        auto getVec = [&](std::string propName) -> FbxDouble3 {
            const FbxProperty vecProp = mayaProp.FindHierarchical(propName.c_str());
            return vecProp.IsValid() ? vecProp.Get<FbxDouble3>() : FbxDouble3(1, 1, 1);
        };

        auto getVal = [&](std::string propName) -> FbxDouble {
            const FbxProperty vecProp = mayaProp.FindHierarchical(propName .c_str());
            return vecProp.IsValid() ? vecProp.Get<FbxDouble>() : 0;
        };

        res->texNormal = getTex("normal");
        res->texColor = getTex("color");
        res->colBase = getVec("base_color");
        res->texAmbientOcclusion = getTex("ao");
        res->texEmissive = getTex("emissive");
        res->colEmissive = getVec("emissive");
        res->emissiveIntensity = getVal("emissive_intensity");
        res->texMetallic = getTex("metallic");
        res->metallic = getVal("metallic");
        res->texRoughness = getTex("roughness");
        res->roughness = getVal("roughness");

        return res;
    }
};

struct FbxTraditionalMaterialInfo : FbxMaterialInfo {
    static constexpr const char *FBX_SHADER_LAMBERT = "Lambert";
    static constexpr const char *FBX_SHADER_BLINN   = "Blinn";
    static constexpr const char *FBX_SHADER_PHONG   = "Phong";

    FbxTraditionalMaterialInfo(const FbxString &name, const FbxString &shadingModel)
        : FbxMaterialInfo(name, shadingModel)
    {}

    FbxFileTexture *texAmbient {};
    FbxVector4     colAmbient {};
    FbxFileTexture *texSpecular {};
    FbxVector4     colSpecular {};
    FbxFileTexture *texDiffuse {};
    FbxVector4     colDiffuse {};
    FbxFileTexture *texEmissive {};
    FbxVector4     colEmissive {};
	FbxFileTexture *texReflection{};
	FbxVector4     colReflection{};
    FbxFileTexture *texNormal {};
    FbxFileTexture *texShininess {};
    FbxDouble      shininess {};

    static std::unique_ptr<FbxTraditionalMaterialInfo> From(
        FbxSurfaceMaterial *fbxMaterial,
        const std::map<const FbxTexture *, FbxString> &textureLocations)
    {
        auto getSurfaceScalar = [&](const char *propName) -> std::tuple<FbxDouble, FbxFileTexture *> {
            const FbxProperty prop = fbxMaterial->FindProperty(propName);

            FbxDouble val(0);
            FbxFileTexture *tex = prop.GetSrcObject<FbxFileTexture>();
            if (tex != nullptr && textureLocations.find(tex) == textureLocations.end()) {
                tex = nullptr;
            }
            if (tex == nullptr && prop.IsValid()) {
                val = prop.Get<FbxDouble>();
            }
            return std::make_tuple(val, tex);
        };

        auto getSurfaceVector = [&](const char *propName) -> std::tuple<FbxDouble3, FbxFileTexture *> {
            const FbxProperty prop = fbxMaterial->FindProperty(propName);

            FbxDouble3 val(1, 1, 1);
            FbxFileTexture *tex = prop.GetSrcObject<FbxFileTexture>();
            if (tex != nullptr && textureLocations.find(tex) == textureLocations.end()) {
                tex = nullptr;
            }
            if (tex == nullptr && prop.IsValid()) {
                val = prop.Get<FbxDouble3>();
            }
            return std::make_tuple(val, tex);
        };

        auto getSurfaceValues = [&](const char *colName, const char *facName) -> std::tuple<FbxVector4, FbxFileTexture *, FbxFileTexture *> {
            const FbxProperty colProp = fbxMaterial->FindProperty(colName);
            const FbxProperty facProp = fbxMaterial->FindProperty(facName);

            FbxDouble3 colorVal(1, 1, 1);
            FbxDouble  factorVal(1);

            FbxFileTexture *colTex = colProp.GetSrcObject<FbxFileTexture>();
            if (colTex != nullptr && textureLocations.find(colTex) == textureLocations.end()) {
                colTex = nullptr;
            }
            if (colTex == nullptr && colProp.IsValid()) {
                colorVal = colProp.Get<FbxDouble3>();
            }
            FbxFileTexture *facTex = facProp.GetSrcObject<FbxFileTexture>();
            if (facTex != nullptr && textureLocations.find(facTex) == textureLocations.end()) {
                facTex = nullptr;
            }
            if (facTex == nullptr && facProp.IsValid()) {
                factorVal = facProp.Get<FbxDouble>();
            }

            auto val = FbxVector4(
                colorVal[0] * factorVal,
                colorVal[1] * factorVal,
                colorVal[2] * factorVal,
                factorVal);
            return std::make_tuple(val, colTex, facTex);
        };

        std::string name = fbxMaterial->GetName();
        std::unique_ptr<FbxTraditionalMaterialInfo> res(new FbxTraditionalMaterialInfo(name.c_str(), fbxMaterial->ShadingModel.Get()));

        // four properties are on the same structure and follow the same rules
        auto handleBasicProperty = [&](const char *colName, const char *facName) -> std::tuple<FbxVector4, FbxFileTexture *>{
            FbxFileTexture *colTex, *facTex;
            FbxVector4     vec;

            std::tie(vec, colTex, facTex) = getSurfaceValues(colName, facName);
            if (colTex) {
                if (facTex) {
                    printf("Warning: Mat [%s]: Can't handle both %s and %s textures; discarding %s.\n", name.c_str(), colName, facName, facName);
                }
                return std::make_tuple(vec, colTex);
            }
            return std::make_tuple(vec, facTex);
        };

        std::tie(res->colAmbient, res->texAmbient) =
            handleBasicProperty(FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
        std::tie(res->colSpecular, res->texSpecular) =
            handleBasicProperty(FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
        std::tie(res->colDiffuse, res->texDiffuse) =
            handleBasicProperty(FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
        std::tie(res->colEmissive, res->texEmissive) =
            handleBasicProperty(FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);
		std::tie(res->colReflection, res->texReflection) =
			handleBasicProperty(FbxSurfaceMaterial::sReflection, FbxSurfaceMaterial::sReflectionFactor);

        // the normal map can only ever be a map, ignore everything else
        std::tie(std::ignore, res->texNormal) = getSurfaceVector(FbxSurfaceMaterial::sNormalMap);

        // shininess can be a map or a factor; afaict the map is always 'ShininessExponent' and the
        // value is always found in 'Shininess' but only sometimes in 'ShininessExponent'.
        std::tie(std::ignore, res->texShininess) = getSurfaceScalar("ShininessExponent");
        std::tie(res->shininess, std::ignore) = getSurfaceScalar("Shininess");

        // for transparency we just want a constant vector value;
        FbxVector4 transparency;
        // extract any existing textures only so we can warn that we're throwing them away
        FbxFileTexture *colTex, *facTex;
        std::tie(transparency, colTex, facTex) =
            getSurfaceValues(FbxSurfaceMaterial::sTransparentColor, FbxSurfaceMaterial::sTransparencyFactor);
        if (colTex) {
            //printf("Warning: Mat [%s]: Can't handle texture for %s; discarding.\n", name, FbxSurfaceMaterial::sTransparentColor);
        }
        if (facTex) {
			printf("Warning: Mat [%s]: Can't handle texture for %s; discarding.\n", name.c_str(), FbxSurfaceMaterial::sTransparencyFactor);
        }
        // FBX color is RGB, so we calculate the A channel as the average of the FBX transparency color vector
        res->colDiffuse[3] = 1.0 - (transparency[0] + transparency[1] + transparency[2])/3.0;

        return res;
    }
};


std::unique_ptr<FbxMaterialInfo>
GetMaterialInfo(FbxSurfaceMaterial *material, const std::map<const FbxTexture *, FbxString> &textureLocations)
{
    std::unique_ptr<FbxMaterialInfo> res;
    res = FbxRoughMetMaterialInfo::From(material, textureLocations);
    if (!res) {
        res = FbxTraditionalMaterialInfo::From(material, textureLocations);
    }
    return res;
}

class FbxMaterialsAccess
{
public:

    FbxMaterialsAccess(const FbxMesh *pMesh, const std::map<const FbxTexture *, FbxString> &textureLocations) :
        mappingMode(FbxGeometryElement::eNone),
        mesh(nullptr),
        indices(nullptr)
    {
        if (pMesh->GetElementMaterialCount() <= 0) {
            return;
        }

        const FbxGeometryElement::EMappingMode materialMappingMode = pMesh->GetElementMaterial()->GetMappingMode();
        if (materialMappingMode != FbxGeometryElement::eByPolygon && materialMappingMode != FbxGeometryElement::eAllSame) {
            return;
        }

        const FbxGeometryElement::EReferenceMode materialReferenceMode = pMesh->GetElementMaterial()->GetReferenceMode();
        if (materialReferenceMode != FbxGeometryElement::eIndexToDirect) {
            return;
        }

        mappingMode = materialMappingMode;
        mesh        = pMesh;
        indices     = &pMesh->GetElementMaterial()->GetIndexArray();

        for (int ii = 0; ii < indices->GetCount(); ii++) {
            int materialNum = indices->GetAt(ii);
            if (materialNum < 0) {
                continue;
            }
            if (materialNum >= summaries.size()) {
                summaries.resize(materialNum + 1);
            }
            auto summary = summaries[materialNum];
            if (summary == nullptr) {
                summary = summaries[materialNum] = GetMaterialInfo(
                    mesh->GetNode()->GetSrcObject<FbxSurfaceMaterial>(materialNum),
                    textureLocations);
            }
        }
    }

    const std::shared_ptr<FbxMaterialInfo> GetMaterial(const int polygonIndex) const
    {
        if (mappingMode != FbxGeometryElement::eNone) {
            const int materialNum = indices->GetAt((mappingMode == FbxGeometryElement::eByPolygon) ? polygonIndex : 0);
            if (materialNum < 0) {
                return nullptr;
            }
            return summaries.at((unsigned long) materialNum);
        }
        return nullptr;
    }

private:
    FbxGeometryElement::EMappingMode              mappingMode;
    std::vector<std::shared_ptr<FbxMaterialInfo>> summaries {};
    const FbxMesh                                 *mesh;
    const FbxLayerElementArrayTemplate<int>       *indices;
};

class FbxSkinningAccess
{
public:

    static const int MAX_WEIGHTS = 4;

    FbxSkinningAccess(const FbxMesh *pMesh, FbxScene *pScene, FbxNode *pNode)
        : rootIndex(-1)
		, maxBone(0)
    {
        for (int deformerIndex = 0; deformerIndex < pMesh->GetDeformerCount(); deformerIndex++) {
            FbxSkin *skin = reinterpret_cast< FbxSkin * >( pMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
            if (skin != nullptr) 
			{
                const int clusterCount = skin->GetClusterCount();
                if (clusterCount == 0) {
                    continue;
                }
                int controlPointCount = pMesh->GetControlPointsCount();
				vertexJointIndices.resize(controlPointCount, Math::IntVec4(0, 0, 0, 0));
                //vertexJointIndices.resize(controlPointCount, Vec4i(0, 0, 0, 0));
				vertexJointWeights.resize(controlPointCount, Math::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
				vertexBoneCnt.resize(controlPointCount, 0);
                //vertexJointWeights.resize(controlPointCount, Vec4f(0.0f, 0.0f, 0.0f, 0.0f));

                for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++) {
                    FbxCluster   *cluster        = skin->GetCluster(clusterIndex);
                    const int    indexCount      = cluster->GetControlPointIndicesCount();
                    const int    *clusterIndices = cluster->GetControlPointIndices();
                    const double *clusterWeights = cluster->GetControlPointWeights();
					//cluster->getpa

                    //assert(cluster->GetLinkMode() == FbxCluster::eNormalize);

                    // Transform link matrix.
                    FbxAMatrix transformLinkMatrix;
                    cluster->GetTransformLinkMatrix(transformLinkMatrix);

                    FbxAMatrix transformMatrix;
                    cluster->GetTransformMatrix(transformMatrix);

					BindMatrices.emplace_back(cluster->GetLink()->EvaluateGlobalTransform());
                    inverseBindMatrices.emplace_back(transformLinkMatrix.Inverse() * transformMatrix);

				
					FbxVector4 transla = transformMatrix.GetT();
					FbxQuaternion r = transformMatrix.GetQ();
					FbxVector4 s = transformMatrix.GetS();


					FbxAMatrix globalBindposeInverseMatrix = transformLinkMatrix.Inverse();// *transformMatrix;   //mesh--->bone
                    jointNodes.push_back(cluster->GetLink());
                    jointIds.push_back(cluster->GetLink()->GetUniqueID());

					FbxTime pTime;
					// first frame is always at t = 0.0
					pTime.SetFrame(20, FbxTime::eFrames24);
					const FbxAMatrix globalNodeTransform = cluster->GetLink()->EvaluateGlobalTransform(pTime);
                    jointSkinningTransforms.push_back(FbxMatrix(globalNodeTransform * globalBindposeInverseMatrix));
                    jointInverseGlobalTransforms.push_back(FbxMatrix(globalNodeTransform.Inverse()));

					

                    for (int i = 0; i < indexCount; i++) 
					{
                        if (clusterIndices[i] < 0 || clusterIndices[i] >= controlPointCount) 
						{
                            continue;
                        }
                        if (clusterWeights[i] <= vertexJointWeights[clusterIndices[i]][MAX_WEIGHTS - 1]) {
                            continue;
                        }

						if (clusterIndices[i] == 19)
							int kkk = 3;

						//只取所有权重中最大的
						if (clusterWeights[i] > vertexJointWeights[clusterIndices[i]][MAX_WEIGHTS - 1])
						{
							vertexJointIndices[clusterIndices[i]][MAX_WEIGHTS - 1] = clusterIndex;
							vertexJointWeights[clusterIndices[i]][MAX_WEIGHTS - 1] = (float)clusterWeights[i];
							vertexBoneCnt[clusterIndices[i]]++;
							if (vertexBoneCnt[clusterIndices[i]] > 4)
								vertexBoneCnt[clusterIndices[i]] = 4;
							for (int j = MAX_WEIGHTS - 1; j > 0; j--)
							{
								if (vertexJointWeights[clusterIndices[i]][j - 1] >= vertexJointWeights[clusterIndices[i]][j]) {
									break;
								}
								std::swap(vertexJointIndices[clusterIndices[i]][j - 1], vertexJointIndices[clusterIndices[i]][j]);
								std::swap(vertexJointWeights[clusterIndices[i]][j - 1], vertexJointWeights[clusterIndices[i]][j]);
							}

						}
				
                    }

                }

				for (int i = 0; i < vertexJointWeights.size(); i++)
				{
					float sum = vertexJointWeights[i][0] + vertexJointWeights[i][1] + vertexJointWeights[i][2] + vertexJointWeights[i][3];
					if (sum != 0)
					{
						vertexJointWeights[i][0] /= sum;
						vertexJointWeights[i][1] /= sum;
						vertexJointWeights[i][2] /= sum;
						vertexJointWeights[i][3] /= sum;
					}
				}
	
            }
        }
		auto maxPosition = std::max_element(vertexBoneCnt.begin(), vertexBoneCnt.end());
		if (maxPosition != vertexBoneCnt.end())
			maxBone = *maxPosition;

        rootIndex = -1;
        for (size_t i = 0; i < jointNodes.size() && rootIndex == -1; i++) 
		{
            rootIndex = (int) i;
            FbxNode *parent = jointNodes[i]->GetParent();
            if (parent == nullptr) {
                break;
            }
            for (size_t j = 0; j < jointNodes.size(); j++) 
			{
                if (jointNodes[j] == parent) {
                    rootIndex = -1;
                    break;
                }
            }
        }
    }

    bool IsSkinned() const
    {
        return (vertexJointWeights.size() > 0);
    }

    int GetNodeCount() const
    {
        return (int) jointNodes.size();
    }

    FbxNode *GetJointNode(const int jointIndex) const
    {
        return jointNodes[jointIndex];
    }

    const long GetJointId(const int jointIndex) const
    {
        return jointIds[jointIndex];
    }

    const FbxMatrix &GetJointSkinningTransform(const int jointIndex) const
    {
        return jointSkinningTransforms[jointIndex];
    }

    const FbxMatrix &GetJointInverseGlobalTransforms(const int jointIndex) const
    {
        return jointInverseGlobalTransforms[jointIndex];
    }

    const long GetRootNode() const
    {
        assert(rootIndex != -1);
        return jointIds[rootIndex];
    }

	const FbxAMatrix &GetBindMatrix(const int jointIndex) const
	{
		return BindMatrices[jointIndex];
	}

    const FbxAMatrix &GetInverseBindMatrix(const int jointIndex) const
    {
        return inverseBindMatrices[jointIndex];
    }

    const Math::IntVec4 GetVertexIndices(const int controlPointIndex) const
    {
        return (!vertexJointIndices.empty()) ?
               vertexJointIndices[controlPointIndex] : Math::IntVec4(0, 0, 0, 0);
    }

    const Math::Vec4 GetVertexWeights(const int controlPointIndex) const
    {
		Math::Vec4 aaa(0, 0, 0, 0);
		Math::Vec4 rayb(0.0, 0.0, 0.0, 1.0);
        return (!vertexJointWeights.empty()) ?
               vertexJointWeights[controlPointIndex] : Math::Vec4(0, 0, 0, 0);
    }

	int GetMaxBoneCnt() const
	{
		return maxBone;
	}

private:
    int                      rootIndex;
	std::vector<int>         rootIndexs;
    std::vector<long>        jointIds;
    std::vector<FbxNode *>   jointNodes;
    std::vector<FbxMatrix>   jointSkinningTransforms;
    std::vector<FbxMatrix>   jointInverseGlobalTransforms;
    std::vector<FbxAMatrix>  inverseBindMatrices;
	std::vector<FbxAMatrix>  BindMatrices;
	std::vector<Math::IntVec4> vertexJointIndices;
	std::vector<Math::Vec4> vertexJointWeights;
	std::vector<int>         vertexBoneCnt;//每个顶点实际关联骨骼数量
	int                      maxBone;  //顶点关联最多骨骼数目
};

/**
 * At the FBX level, each Mesh can have a set of FbxBlendShape deformers; organisational units that contain no data
 * of their own. The actual deformation is determined by one or more FbxBlendShapeChannels, whose influences are all
 * additively applied to the mesh. In a simpler world, each such channel would extend each base vertex with alternate
 * position, and optionally normal and tangent.
 *
 * It's not quite so simple, though. We also have progressive morphing, where one logical morph actually consists of
 * several concrete ones, each applied in sequence. For us, this means each channel contains a sequence of FbxShapes
 * (aka target shape); these are the actual data-holding entities that provide the alternate vertex attributes. As such
 * a channel is given more weight, it moves from one target shape to another.
 *
 * The total number of alternate sets of attributes, then, is the total number of target shapes across all the channels
 * of all the blend shapes of the mesh.
 *
 * Each animation in the scene stack can yield one or zero FbxAnimCurves per channel (not target shape). We evaluate
 * these curves to get the weight of the channel: this weight is further introspected on to figure out which target
 * shapes we're currently interpolation between.
 */
class FbxBlendShapesAccess
{
public:
    /**
     * A target shape is on a 1:1 basis with the eventual glTF morph target, and is the object which contains the
     * actual morphed vertex data.
     */
    struct TargetShape
    {
        explicit TargetShape(const FbxShape *shape, FbxDouble fullWeight) :
            shape(shape),
            fullWeight(fullWeight),
			uid(shape->GetUniqueID()),
            count(shape->GetControlPointsCount()),
            positions(shape->GetControlPoints()),
            normals(FbxLayerElementAccess<FbxVector4>(shape->GetElementNormal(), shape->GetElementNormalCount())),
            tangents(FbxLayerElementAccess<FbxVector4>(shape->GetElementTangent(), shape->GetElementTangentCount()))
        {}
		const uint64_t							uid;
        const FbxShape                          *shape;
        const FbxDouble                         fullWeight;
        const unsigned int                      count;
        const FbxVector4                        *positions;
        const FbxLayerElementAccess<FbxVector4> normals;
        const FbxLayerElementAccess<FbxVector4> tangents;
    };

    /**
     * A channel collects a sequence (often of length 1) of target shapes.
     */
    struct BlendChannel
    {
		BlendChannel(
			FbxMesh *mesh,
			const char* n,
			uint64_t id, 
            const unsigned int blendShapeIx,
            const unsigned int channelIx,
            const FbxDouble deformPercent,
            const std::vector<TargetShape> &targetShapes
        ) : mesh(mesh),
			name(n),
			cid(id),
            blendShapeIx(blendShapeIx),
            channelIx(channelIx),
            deformPercent(deformPercent),
            targetShapes(targetShapes)
        {}

		FbxAnimCurve * GetAnimation(size_t animIx) const {
			return ExtractAnimation(animIx);
		}

        FbxAnimCurve *ExtractAnimation(unsigned int animIx) const {
            FbxAnimStack *stack = mesh->GetScene()->GetSrcObject<FbxAnimStack>(animIx);
            FbxAnimLayer *layer = stack->GetMember<FbxAnimLayer>(0);
            return mesh->GetShapeChannel(blendShapeIx, channelIx, layer, true);
        }

        FbxMesh *const mesh;

        const unsigned int blendShapeIx;
        const unsigned int channelIx;
        const std::vector<TargetShape> targetShapes;
		std::string name;
		uint64_t cid;

        const FbxDouble deformPercent;
    };

	/**
	* A morpher collects a sequence (often of length 1) of BlendChannel.
	*/
	struct MeshDeformer
	{
		MeshDeformer(
			FbxMesh *mesh,
			const char* n,
			uint64_t id,
			const unsigned int blendShapeIx,
			const std::vector<BlendChannel> &channels
		) : mesh(mesh),
			name(n),
			uid(id),
			blendShapeIx(blendShapeIx),
			channels(channels)
		{}

		FbxMesh *const mesh;

		const unsigned int blendShapeIx;
		const std::vector<BlendChannel> channels;
		std::string name;
		uint64_t uid;
	};

    explicit FbxBlendShapesAccess(FbxMesh *mesh) :
        deformers(extractChannels(mesh))
    { }

    size_t GetDefomerCount() const { return deformers.size(); }
    const MeshDeformer &GetMeshDeformer(size_t bsIdx) const {
        return deformers.at(bsIdx);
    }

private:
    std::vector<MeshDeformer> extractChannels(FbxMesh *mesh) const {
        std::vector<MeshDeformer> deformers;

        for (int shapeIx = 0; shapeIx < mesh->GetDeformerCount(FbxDeformer::eBlendShape); shapeIx++) {
            auto *fbxBlendShape = static_cast<FbxBlendShape *>(mesh->GetDeformer(shapeIx, FbxDeformer::eBlendShape));
			auto fbxBlendShapeName = fbxBlendShape->GetName();

			std::vector<BlendChannel> channels;
            for (int channelIx = 0; channelIx < fbxBlendShape->GetBlendShapeChannelCount(); ++channelIx) {
                FbxBlendShapeChannel *fbxChannel = fbxBlendShape->GetBlendShapeChannel(channelIx);

                if (fbxChannel->GetTargetShapeCount() > 0) {
                    std::vector<TargetShape> targetShapes;
                    const double *fullWeights = fbxChannel->GetTargetShapeFullWeights();
                    for (int targetIx = 0; targetIx < fbxChannel->GetTargetShapeCount(); targetIx ++) {
                        FbxShape *fbxShape = fbxChannel->GetTargetShape(targetIx);
                        targetShapes.push_back(TargetShape(fbxShape, fullWeights[targetIx]));
                    }
					std::string channelName = fbxChannel->GetName();
					channelName.erase(0, strlen(fbxBlendShapeName) + 1);
                    channels.push_back(BlendChannel(mesh, channelName.c_str(), fbxChannel->GetUniqueID(), shapeIx, channelIx, fbxChannel->DeformPercent * 0.01, targetShapes));
                }
            }
			deformers.push_back(MeshDeformer(mesh, fbxBlendShapeName, fbxBlendShape->GetUniqueID(), shapeIx, channels));
        }
        return deformers;
    }

    const std::vector<MeshDeformer> deformers;
};

static bool TriangleTexturePolarity(const Math::Vec2 &uv0, const Math::Vec2 &uv1, const Math::Vec2 &uv2)
{
    const Math::Vec2 d0 = uv1 - uv0;
    const Math::Vec2 d1 = uv2 - uv0;

    return (d0[0] * d1[1] - d0[1] * d1[0] < 0.0f);
}

static RawMaterialType
GetMaterialType(const RawModel &raw, const int textures[RAW_TEXTURE_USAGE_MAX], const bool vertexTransparency, const bool skinned)
{
    // If diffusely texture, determine material type based on texture occlusion.
    if (textures[RAW_TEXTURE_USAGE_DIFFUSE] >= 0) {
        switch (raw.GetTexture(textures[RAW_TEXTURE_USAGE_DIFFUSE]).occlusion) {
            case RAW_TEXTURE_OCCLUSION_OPAQUE:
                return skinned ? RAW_MATERIAL_TYPE_SKINNED_OPAQUE : RAW_MATERIAL_TYPE_OPAQUE;
            case RAW_TEXTURE_OCCLUSION_TRANSPARENT:
                return skinned ? RAW_MATERIAL_TYPE_SKINNED_TRANSPARENT : RAW_MATERIAL_TYPE_TRANSPARENT;
        }
    }

	// else if there is any vertex transparency, treat whole mesh as transparent
	if (vertexTransparency) {
		return skinned ? RAW_MATERIAL_TYPE_SKINNED_TRANSPARENT : RAW_MATERIAL_TYPE_TRANSPARENT;
	}


    // Default to simply opaque.
    return skinned ? RAW_MATERIAL_TYPE_SKINNED_OPAQUE : RAW_MATERIAL_TYPE_OPAQUE;
}

/**
* Compute the local scale vector to use for a given node. This is an imperfect hack to cope with
* the FBX node transform's eInheritRrs inheritance type, in which ancestral scale is ignored
*/
static FbxVector4 computeLocalScale(FbxNode *pNode, FbxTime pTime = FBXSDK_TIME_INFINITE)
{
	const FbxVector4 lScale = pNode->EvaluateLocalTransform(pTime).GetS();

	if (pNode->GetParent() == nullptr ||
		pNode->GetTransform().GetInheritType() != FbxTransform::eInheritRrs) {
		return lScale;
	}
	// This is a very partial fix that is only correct for models that use identity scale in their rig's joints.
	// We could write better support that compares local scale to parent's global scale and apply the ratio to
	// our local translation. We'll always want to return scale 1, though -- that's the only way to encode the
	// missing 'S' (parent scale) in the transform chain.
	return FbxVector4(1, 1, 1, 1);
}
static void SetSkeleton(RawModel &raw, FbxScene *pScene, FbxNode *pNode)
{
	int nodeId = raw.GetNodeById(pNode->GetUniqueID());
	if (nodeId >= 0) {
		RawNode &node = raw.GetNode(nodeId);
		node.isSkeleton = true;
		
		bool alreadyin = false;
		for (int i = 0; i < raw.BoneNodes.size(); i++)
		{
			if (raw.BoneNodes[i]->id == node.id)
			{
				alreadyin = true;
			}
		}
		if (alreadyin == false)
		{
			raw.BoneNodes.push_back(&node);
		}

	}
}

// 根据Target的名字 找到这个Target对应的Mesh  需要确保名字不重复
// 还有一种方法是 根据相邻三角形面法向量  来计算平均的法线?? 这个应该就是不带mesh的morphtarget导出fbx的法线?
static void CorrectMorphTargetNormal(RawModel &raw)
{
	if (raw.GetMorphAnimationCount() == 0)
	{
		printf("[CorrectMorphTargetNormal] no morph found skip \n");
		return;
	}

	int totalSurface = raw.GetSurfaceCount(); 
 
	for (int rawSurfaceIndex = 0; rawSurfaceIndex < totalSurface; rawSurfaceIndex++)
	{
		RawSurface &rawSurface = raw.GetSurface(rawSurfaceIndex);

		const auto& meshName = rawSurface.name;

		for (int subSurfaceIndex = 0; subSurfaceIndex < totalSurface; subSurfaceIndex++)
		{
			if (rawSurfaceIndex == subSurfaceIndex)
			{
				continue; 
			}
			RawSurface &subSurface = raw.GetSurface(subSurfaceIndex);
			for (int i = 0; i < subSurface.blendChannels.size(); i++)
			{
				const auto& channel = subSurface.blendChannels[i]; // 每个Channel这里实际代表一个Target

				printf("[CorrectMorphTargetNormal] mesh %s target %s\n", meshName.c_str() , channel.name.c_str());

				if (channel.name == meshName)
				{
					// 在这里把Target的法向量替换成Target对应mesh的 
					std::vector<const RawTriangle*> originTriangle;
					std::vector<RawTriangle*> targetTriangle;
					 
					for (RawTriangle& one : raw.GetEditableTriangle())
					{
						if (one.surfaceIndex == rawSurfaceIndex)
						{
							originTriangle.emplace_back(&one);
						}
						else if (one.surfaceIndex == subSurfaceIndex)
						{
							targetTriangle.emplace_back(&one);
						}
					}

					if (targetTriangle.size() != originTriangle.size())
					{
						printf("[CorrectMorphTargetNormal] target triangle %d not equal origin %d\n",
							targetTriangle.size(), originTriangle.size());
					}
					// ??? 导出两个模型的三角形保存顺序不一样 ??

					int size = targetTriangle.size(); 
					for (int j = 0; j < size; j++)
					{
						const auto* o = originTriangle[j];
						auto* t = targetTriangle[j];
						for (int k = 0; k < 3; k++)
						{
							const RawVertex& origin_vertex = raw.GetVertex(o->verts[k]);
							const Math::Vec3& origin_normal = origin_vertex.normal;
							
							// i是这个RawSurface的通道的讯号 一个Vertex如果存在Blend那么这个RawVertex实例属于对应的RawSurface 
							// 也就是一个具有blend的RawVertex的 blends[] 应该跟所属的RawSurface的blendChannels[] 顺序对应 

							RawVertex& target_vertex = raw.GetEditableVertex(t->verts[k]);
							Math::Vec3&  target_normal = target_vertex.blends[i].normal;  

							target_normal = (origin_normal - target_vertex.normal) ;
						}
					}
				}	
			}
		}
	}
}


void ZeroPivotsForSkinsRecursive(FbxScene* Scene, FbxNode* Node)
{
	if (Node)
	{
		// Activate pivot converting 
		Node->SetPivotState(FbxNode::eSourcePivot, FbxNode::ePivotActive);
		Node->SetPivotState(FbxNode::eDestinationPivot, FbxNode::ePivotActive);

		FbxVector4 Zero(0, 0, 0);

		Node->SetPostRotation(FbxNode::eDestinationPivot, Zero);
		Node->SetPreRotation(FbxNode::eDestinationPivot, Zero);
		Node->SetRotationOffset(FbxNode::eDestinationPivot, Zero);
		Node->SetScalingOffset(FbxNode::eDestinationPivot, Zero);
		Node->SetRotationPivot(FbxNode::eDestinationPivot, Zero);
		Node->SetScalingPivot(FbxNode::eDestinationPivot, Zero);

		Node->SetRotationOrder(FbxNode::eDestinationPivot, eEulerXYZ);

		Node->SetGeometricTranslation(FbxNode::eDestinationPivot, Zero);
		Node->SetGeometricRotation(FbxNode::eDestinationPivot, Zero);
		Node->SetGeometricScaling(FbxNode::eDestinationPivot, Zero);

		//Node->ResetPivotSetAndConvertAnimation(FbxTime::GetFrameRate(Scene->GetGlobalSettings().GetTimeMode()));

		for (int i = 0; i < Node->GetChildCount(); i++)
			ZeroPivotsForSkinsRecursive(Scene, Node->GetChild(i));
	}
}

static void ReadMesh(RawModel &raw, FbxScene *pScene, FbxNode *pNode, const std::map<const FbxTexture *, FbxString> &textureLocations, bool keepIndex){
    FbxGeometryConverter meshConverter(pScene->GetFbxManager());
    meshConverter.Triangulate(pNode->GetNodeAttribute(), true);
    FbxMesh *pMesh = pNode->GetMesh();
	if (pMesh->GetControlPoints() == nullptr)
	{
		return;
	}

	//我们不需要node的父子关系，所以考虑全局矩阵
	const FbxAMatrix    nodeGlobalTransform = pNode->EvaluateGlobalTransform();


	//node.translation = toVec3f(localTranslation) * scaleFactor;
	//node.rotation = toQuatf(localRotation);
	//node.scale = toVec3f(localScaling);

    // Obtains the surface Id
    const long surfaceId = pMesh->GetUniqueID();
	

    // Associate the node to this surface
    int nodeId = raw.GetNodeById(pNode->GetUniqueID());
    if (nodeId >= 0) {
        RawNode &node = raw.GetNode(nodeId);
        node.surfaceId = surfaceId;
		node.hasMesh = true;
    }

    if (raw.GetSurfaceById(surfaceId) >= 0) {
        // This surface is already loaded
        return;
    }

    const char *meshName = (pNode->GetName()[0] != '\0') ? pNode->GetName() : pMesh->GetName();
    const int rawSurfaceIndex = raw.AddSurface(meshName, surfaceId);
	if (nodeId >= 0) 
	{
		RawNode &node = raw.GetNode(nodeId);
		node.meshname = meshName;
	}


    const FbxVector4 *controlPoints = pMesh->GetControlPoints();
    const FbxLayerElementAccess<FbxVector4> normalLayer(pMesh->GetElementNormal(), pMesh->GetElementNormalCount());
    const FbxLayerElementAccess<FbxVector4> binormalLayer(pMesh->GetElementBinormal(), pMesh->GetElementBinormalCount());
    const FbxLayerElementAccess<FbxVector4> tangentLayer(pMesh->GetElementTangent(), pMesh->GetElementTangentCount());
    const FbxLayerElementAccess<FbxColor>   colorLayer(pMesh->GetElementVertexColor(), pMesh->GetElementVertexColorCount());
    const FbxLayerElementAccess<FbxVector2> uvLayer0(pMesh->GetElementUV(0), pMesh->GetElementUVCount());
    const FbxLayerElementAccess<FbxVector2> uvLayer1(pMesh->GetElementUV(1), pMesh->GetElementUVCount());
	const FbxLayerElementAccess<FbxVector2> uvLayer2(pMesh->GetElementUV(2), pMesh->GetElementUVCount());
	const FbxLayerElementAccess<FbxVector2> uvLayer3(pMesh->GetElementUV(3), pMesh->GetElementUVCount());
    const FbxSkinningAccess                 skinning(pMesh, pScene, pNode);
    const FbxMaterialsAccess                materials(pMesh, textureLocations);
    const FbxBlendShapesAccess              blendShapes(pMesh);

	bool bSkinned = skinning.IsSkinned();
	//if (!bSkinned)
	{
		//ZeroPivotsForSkinsRecursive(pNode);
		//pNode->ResetPivotSetAndConvertAnimation(FbxTime::GetFrameRate(pScene->GetGlobalSettings().GetTimeMode()), false, false);
	}

	if (skinning.GetNodeCount() > 0)
	{
		RawNode &node = raw.GetNode(nodeId);
		node.hasSkinning = true;
	}

    if (verboseOutput) {
        printf(
            "mesh %d: %s (skinned: %s)\n", rawSurfaceIndex, meshName,
            skinning.IsSkinned() ? raw.GetNode(raw.GetNodeById(skinning.GetRootNode())).name.c_str() : "NO");
    }

	
    FbxVector4 meshTranslation           = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
    FbxVector4 meshRotation              = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
    FbxVector4 meshScaling               = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
    FbxAMatrix meshTransform(meshTranslation, meshRotation, meshScaling);
    FbxAMatrix  transformA               =  nodeGlobalTransform * meshTransform;
	FbxVector4 rotPivot					 = pNode->GetRotationPivot(FbxNode::eSourcePivot);
	FbxVector4 scalePivot				 = pNode->GetScalingPivot(FbxNode::eSourcePivot);
	//const FbxMatrix   transform = transformA;
	FbxMatrix  transform;
	if (skinning.IsSkinned())
		transform = meshTransform;
	else
		transform = transformA;
	transform = transformA;
	FbxMatrix boxTransform = transformA;//用來計算包围盒
	
	
    // Remove translation & scaling from transforms that will bi applied to normals, tangents & binormals
    const FbxMatrix  normalTransform(FbxVector4(), transformA.GetQ(), transformA.GetS());
    const FbxMatrix  inverseTransposeTransform = bSkinned ? normalTransform.Inverse().Transpose() : meshTransform.Inverse().Transpose();
	

    raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_POSITION);
    if (normalLayer.LayerPresent()) { raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_NORMAL); }
    if (tangentLayer.LayerPresent()) { raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_TANGENT); }
    if (binormalLayer.LayerPresent()) { raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_BINORMAL); }
    if (colorLayer.LayerPresent()) { raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_COLOR); }
    if (uvLayer0.LayerPresent()) { raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_UV0); }
    if (uvLayer1.LayerPresent()) { raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_UV1); }
	if (uvLayer2.LayerPresent()) { raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_UV2); }
	if (uvLayer3.LayerPresent()) { raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_UV3); }
    if (skinning.IsSkinned()) {
        raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_JOINT_WEIGHTS);
        raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_JOINT_INDICES);
    }

    RawSurface &rawSurface = raw.GetSurface(rawSurfaceIndex);
    



	FbxVector4 wPos = nodeGlobalTransform.GetT();
	FbxVector4 wScale = nodeGlobalTransform.GetS();
	FbxQuaternion wRot = nodeGlobalTransform.GetQ();
	
	
    rawSurface.skeletonRootId = (skinning.IsSkinned()) ? skinning.GetRootNode() : pNode->GetUniqueID();
	rawSurface.meshGlobalInverseMatrix = nodeGlobalTransform.Inverse();
	rawSurface.meshNode = pNode;
	rawSurface.maxBone = skinning.GetMaxBoneCnt();
	rawSurface.translation = bSkinned ? Math::Vec3(0.0f, 0.0f, 0.0f) : Math::Vec3(wPos[0], wPos[1], wPos[2]);
	rawSurface.rotation = bSkinned ? Math::Quaternion(0.0f, 0.0f, 0.0f, 1.0f) : Math::Quaternion(wRot[0], wRot[1], wRot[2], wRot[3]);
	rawSurface.scale = bSkinned ? Math::Vec3(1.0f, 1.0f, 1.0f) : Math::Vec3(wScale[0], wScale[1], wScale[2]);

	//rawSurface.translation = toVec3f(nodeGlobalTranslation);
	//rawSurface.rotation = toQuatf(nodeGlobalRotation);
	//rawSurface.scale = toVec3f(nodeGlobalScale);
    for (int jointIndex = 0; jointIndex < skinning.GetNodeCount(); jointIndex++) {
        const long jointId = skinning.GetJointId(jointIndex);
        raw.GetNode(raw.GetNodeById(jointId)).isJoint = true;
		RawNode &jointnode = raw.GetNode(raw.GetNodeById(jointId));
		bool alreadyin = false;
		for(int i=0;i<raw.BoneNodes.size();i++)
		{
			if (raw.BoneNodes[i]->id == jointnode.id)
			{
				alreadyin = true;
			}
		}
		if (alreadyin == false)
		{
			raw.BoneNodes.push_back(&jointnode);
		}
		
		
        rawSurface.jointIds.emplace_back(jointId);
		//FbxVector4 scale(scaleFactor, scaleFactor, scaleFactor, 0);
		FbxAMatrix invBindMat = skinning.GetInverseBindMatrix(jointIndex);

		FbxVector4 invst = invBindMat.GetT();
		invst = invst * scaleFactor;
		invBindMat.SetT(invst);

		FbxAMatrix bindMat = skinning.GetBindMatrix(jointIndex);
		FbxVector4 bst = bindMat.GetT();
		bst = bst * scaleFactor;
		bindMat.SetT(bst);

        rawSurface.inverseBindMatrices.push_back(toMat4f(invBindMat));
        rawSurface.jointGeometryMins.emplace_back(FLT_MAX, FLT_MAX, FLT_MAX);
        rawSurface.jointGeometryMaxs.emplace_back(-FLT_MAX, -FLT_MAX, -FLT_MAX);
    }

	rawSurface.bonesBoundingBox.resize(skinning.GetNodeCount());

    rawSurface.blendChannels.clear();
    std::vector<const FbxBlendShapesAccess::TargetShape *> targetShapes;
	for (size_t blendshapeIdx = 0; blendshapeIdx < blendShapes.GetDefomerCount(); blendshapeIdx++) {
		auto& channels = blendShapes.GetMeshDeformer(blendshapeIdx).channels;
		for (size_t channelIx = 0; channelIx < channels.size(); channelIx++) {
			auto& channel = channels[channelIx];
			for (size_t targetIx = 0; targetIx < channel.targetShapes.size(); targetIx++) {
				const FbxBlendShapesAccess::TargetShape &shape = channel.targetShapes[targetIx];
				targetShapes.push_back(&shape);
				// 一个Target 作为一个RawBlendChannel 
				rawSurface.blendChannels.push_back(RawBlendChannel{
					static_cast<float>(channel.deformPercent),
					shape.normals.LayerPresent(),
					shape.tangents.LayerPresent(),
					shape.shape->GetName()
					});
			}
		}
	}
	if (keepIndex)
	{
		raw.InitVerticeArray(pMesh->GetControlPointsCount());
	}
    int polygonVertexIndex = 0;
    for (int polygonIndex = 0; polygonIndex < pMesh->GetPolygonCount(); polygonIndex++) 
	{
        FBX_ASSERT(pMesh->GetPolygonSize(polygonIndex) == 3);
        const std::shared_ptr<FbxMaterialInfo> fbxMaterial = materials.GetMaterial(polygonIndex);

        int textures[RAW_TEXTURE_USAGE_MAX];
        std::fill_n(textures, (int) RAW_TEXTURE_USAGE_MAX, -1);

        std::shared_ptr<RawMatProps> rawMatProps;
        FbxString materialName;

        if (fbxMaterial == nullptr) {
            materialName = "DefaultMaterial";
            rawMatProps.reset(new RawTraditionalMatProps(RAW_SHADING_MODEL_LAMBERT,
                Math::Vec3(0, 0, 0), Math::Vec4(.5, .5, .5, 1), Math::Vec3(0, 0, 0), Math::Vec3(0, 0, 0), 0.5));

        } else {
            materialName = fbxMaterial->name;

            const auto maybeAddTexture = [&](const FbxFileTexture *tex, RawTextureUsage usage) {
                if (tex != nullptr) {
                    // dig out the inferred filename from the textureLocations map
                    FbxString inferredPath = textureLocations.find(tex)->second;
                    textures[usage] = raw.AddTexture(tex->GetName(), tex->GetFileName(), inferredPath.Buffer(), usage);
                }
            };

            std::shared_ptr<RawMatProps> matInfo;
            if (fbxMaterial->shadingModel == FbxRoughMetMaterialInfo::FBX_SHADER_METROUGH) {
                FbxRoughMetMaterialInfo *fbxMatInfo = static_cast<FbxRoughMetMaterialInfo *>(fbxMaterial.get());

                maybeAddTexture(fbxMatInfo->texColor, RAW_TEXTURE_USAGE_ALBEDO);
                maybeAddTexture(fbxMatInfo->texNormal, RAW_TEXTURE_USAGE_NORMAL);
                maybeAddTexture(fbxMatInfo->texEmissive, RAW_TEXTURE_USAGE_EMISSIVE);
                maybeAddTexture(fbxMatInfo->texRoughness, RAW_TEXTURE_USAGE_ROUGHNESS);
                maybeAddTexture(fbxMatInfo->texMetallic, RAW_TEXTURE_USAGE_METALLIC);
                maybeAddTexture(fbxMatInfo->texAmbientOcclusion, RAW_TEXTURE_USAGE_OCCLUSION);
				maybeAddTexture(fbxMatInfo->texReflection, RAW_TEXTURE_USAGE_REFLECTION);

                rawMatProps.reset(new RawMetRoughMatProps(
                    RAW_SHADING_MODEL_PBR_MET_ROUGH, toVec4f(fbxMatInfo->colBase), toVec3f(fbxMatInfo->colEmissive),
                    fbxMatInfo->emissiveIntensity, fbxMatInfo->metallic, fbxMatInfo->roughness));
            } else {

                FbxTraditionalMaterialInfo *fbxMatInfo = static_cast<FbxTraditionalMaterialInfo *>(fbxMaterial.get());
                RawShadingModel shadingModel;
                if (fbxMaterial->shadingModel == "Lambert") {
                    shadingModel = RAW_SHADING_MODEL_LAMBERT;
                } else if (fbxMaterial->shadingModel == "Blinn") {
                    shadingModel = RAW_SHADING_MODEL_BLINN;
                } else if (fbxMaterial->shadingModel == "Phong") {
                    shadingModel = RAW_SHADING_MODEL_PHONG;
                } else if (fbxMaterial->shadingModel == "Constant") {
                    shadingModel = RAW_SHADING_MODEL_PHONG;
				} else {
                    shadingModel = RAW_SHADING_MODEL_UNKNOWN;
                }
                maybeAddTexture(fbxMatInfo->texDiffuse, RAW_TEXTURE_USAGE_DIFFUSE);
                maybeAddTexture(fbxMatInfo->texNormal, RAW_TEXTURE_USAGE_NORMAL);
                maybeAddTexture(fbxMatInfo->texEmissive, RAW_TEXTURE_USAGE_EMISSIVE);
                maybeAddTexture(fbxMatInfo->texShininess, RAW_TEXTURE_USAGE_SHININESS);
                maybeAddTexture(fbxMatInfo->texAmbient, RAW_TEXTURE_USAGE_AMBIENT);
                maybeAddTexture(fbxMatInfo->texSpecular, RAW_TEXTURE_USAGE_SPECULAR);
				maybeAddTexture(fbxMatInfo->texReflection, RAW_TEXTURE_USAGE_REFLECTION);
                rawMatProps.reset(new RawTraditionalMatProps(shadingModel,
                    toVec3f(fbxMatInfo->colAmbient), toVec4f(fbxMatInfo->colDiffuse), toVec3f(fbxMatInfo->colEmissive),
                    toVec3f(fbxMatInfo->colSpecular), fbxMatInfo->shininess));
            }
        }

        RawVertex rawVertices[3];
		int vertexIndiceArray[3];
        bool vertexTransparency = false;
        for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++, polygonVertexIndex++) 
		{
            const int controlPointIndex = pMesh->GetPolygonVertex(polygonIndex, vertexIndex);
			vertexIndiceArray[vertexIndex] = controlPointIndex;
            // Note that the default values here must be the same as the RawVertex default values!
			FbxMatrix  identitymat;
			identitymat.SetIdentity();

            const FbxVector4 fbxPosition = controlPoints[controlPointIndex];
            const FbxVector4 fbxNormal   = normalLayer.GetElement(
                polygonIndex, polygonVertexIndex, controlPointIndex, FbxVector4(0.0f, 0.0f, 0.0f, 0.0f), identitymat, true);
            const FbxVector4 fbxTangent  = tangentLayer.GetElement(
                polygonIndex, polygonVertexIndex, controlPointIndex, FbxVector4(1.0f, 0.0f, 0.0f, 0.0f), identitymat, true);
            const FbxVector4 fbxBinormal = binormalLayer.GetElement(
                polygonIndex, polygonVertexIndex, controlPointIndex, FbxVector4(0.0f, 0.0f, 1.0f, 0.0f), identitymat, true);
            const FbxColor   fbxColor    = colorLayer
                .GetElement(polygonIndex, polygonVertexIndex, controlPointIndex, FbxColor(0.0f, 0.0f, 0.0f, 0.0f));
            const FbxVector2 fbxUV0      = uvLayer0.GetElement(polygonIndex, polygonVertexIndex, controlPointIndex, FbxVector2(0.0f, 0.0f));
            const FbxVector2 fbxUV1      = uvLayer1.GetElement(polygonIndex, polygonVertexIndex, controlPointIndex, FbxVector2(0.0f, 0.0f));
			const FbxVector2 fbxUV2		 = uvLayer2.GetElement(polygonIndex, polygonVertexIndex, controlPointIndex, FbxVector2(0.0f, 0.0f));
			const FbxVector2 fbxUV3      = uvLayer3.GetElement(polygonIndex, polygonVertexIndex, controlPointIndex, FbxVector2(0.0f, 0.0f));

			RawVertex &vertex = rawVertices[vertexIndex];
            vertex.position[0]   = (float) fbxPosition[0] * scaleFactor;
            vertex.position[1]   = (float) fbxPosition[1] * scaleFactor;
            vertex.position[2]   = (float) fbxPosition[2] * scaleFactor;
            vertex.normal[0]     = (float) fbxNormal[0];
            vertex.normal[1]     = (float) fbxNormal[1];
            vertex.normal[2]     = (float) fbxNormal[2];
            vertex.tangent[0]    = (float) fbxTangent[0];
            vertex.tangent[1]    = (float) fbxTangent[1];
            vertex.tangent[2]    = (float) fbxTangent[2];
            vertex.tangent[3]    = (float) fbxTangent[3];
            vertex.binormal[0]   = (float) fbxBinormal[0];
            vertex.binormal[1]   = (float) fbxBinormal[1];
            vertex.binormal[2]   = (float) fbxBinormal[2];
            vertex.color[0]      = (float) fbxColor.mRed;
            vertex.color[1]      = (float) fbxColor.mGreen;
            vertex.color[2]      = (float) fbxColor.mBlue;
            vertex.color[3]      = (float) fbxColor.mAlpha;
            vertex.uv0[0]        = (float) fbxUV0[0];
            vertex.uv0[1]        = (float) fbxUV0[1];
            vertex.uv1[0]        = (float) fbxUV1[0];
            vertex.uv1[1]        = (float) fbxUV1[1];
			vertex.uv2[0]		 = (float) fbxUV2[0];
			vertex.uv2[1]		 = (float) fbxUV2[1];
			vertex.uv3[0]		 = (float) fbxUV3[0];
			vertex.uv3[1]		 = (float) fbxUV3[1];
            vertex.jointIndices = skinning.GetVertexIndices(controlPointIndex);
            vertex.jointWeights = skinning.GetVertexWeights(controlPointIndex);
            vertex.polarityUv0  = false;

			if (bSkinned)
			{
				for (size_t ji = 0; ji < 4; ++ji)
				{
					const auto jointIndex = vertex.jointIndices[ji];
					Math::AABB& boneBox = rawSurface.bonesBoundingBox[jointIndex];
					boneBox.Merge(vertex.position * rawSurface.inverseBindMatrices[jointIndex]);
				}
			}
			
            //rawSurface.bounds.AddPoint(vertex.position);

            if (!targetShapes.empty()) {
                vertex.blendSurfaceIx = rawSurfaceIndex;
                for (const auto *targetShape : targetShapes) {
                    RawBlendVertex blendVertex;
                    // the morph target data must be transformed just as with the vertex positions above
                    const FbxVector4 &shapePosition = transform.MultNormalize(targetShape->positions[controlPointIndex]);
                    blendVertex.position = toVec3f(shapePosition - fbxPosition) * scaleFactor;
                    if (targetShape->normals.LayerPresent()) {
                        const FbxVector4 &normal = targetShape->normals.GetElement(
                            polygonIndex, polygonVertexIndex, controlPointIndex, FbxVector4(0.0f, 0.0f, 0.0f, 0.0f), inverseTransposeTransform, true);
                        blendVertex.normal = toVec3f(normal - fbxNormal);
                    }
                    if (targetShape->tangents.LayerPresent()) {
                        const FbxVector4 &tangent = targetShape->tangents.GetElement(
                            polygonIndex, polygonVertexIndex, controlPointIndex, FbxVector4(0.0f, 0.0f, 0.0f, 0.0f), inverseTransposeTransform, true);
                        blendVertex.tangent = toVec4f(tangent - fbxTangent);
                    }
					blendVertex.targetId = targetShape->uid;
                    vertex.blends.push_back(blendVertex);
                }
            } else {
                vertex.blendSurfaceIx = -1;
            }

			//计算包围盒
			{
				//rawSurface.bounds.AddPoint(toVec3f(boxTransform.MultNormalize(controlPoints[controlPointIndex])));
				rawSurface.bounds.AddPoint(toVec3f(controlPoints[controlPointIndex]) * scaleFactor);
			}
        }

	

        if (textures[RAW_TEXTURE_USAGE_NORMAL] != -1) {
            // Distinguish vertices that are used by triangles with a different texture polarity to avoid degenerate tangent space smoothing.
            const bool polarity = TriangleTexturePolarity(rawVertices[0].uv0, rawVertices[1].uv0, rawVertices[2].uv0);
            rawVertices[0].polarityUv0 = polarity;
            rawVertices[1].polarityUv0 = polarity;
            rawVertices[2].polarityUv0 = polarity;
        }

        int rawVertexIndices[3];
        for (int vertexIndex = 0; vertexIndex < 3; vertexIndex++) {
			if (keepIndex)
			{
				rawVertexIndices[vertexIndex] = raw.AddVertex(rawVertices[vertexIndex],
					vertexIndiceArray[vertexIndex]);
			} 
			else
			{
				rawVertexIndices[vertexIndex] = raw.AddVertex(rawVertices[vertexIndex]);
			}
        }

        const RawMaterialType materialType = GetMaterialType(raw, textures, vertexTransparency, skinning.IsSkinned());
        const int rawMaterialIndex = raw.AddMaterial(materialName, materialType, textures, rawMatProps);

		raw.AddTriangle(rawVertexIndices[0], rawVertexIndices[1], rawVertexIndices[2], rawMaterialIndex, rawSurfaceIndex);

    }

	// 打印这个raw model的三角形数目和顶点数目 
	printf("raw model total triangle %d  vertex %d\n", raw.GetTriangleCount(), raw.GetVertexCount() );

}

static void ReadCamera(RawModel &raw, FbxScene *pScene, FbxNode *pNode)
{
    const FbxCamera *pCamera = pNode->GetCamera();
    if (pCamera->ProjectionType.Get() == FbxCamera::EProjectionType::ePerspective) {
        raw.AddCameraPerspective(
            "", pNode->GetUniqueID(), (float) pCamera->FilmAspectRatio,
            (float) pCamera->FieldOfViewX, (float) pCamera->FieldOfViewX,
            (float) pCamera->NearPlane, (float) pCamera->FarPlane);
    } else {
        raw.AddCameraOrthographic(
            "", pNode->GetUniqueID(),
            (float) pCamera->OrthoZoom, (float) pCamera->OrthoZoom,
            (float) pCamera->FarPlane, (float) pCamera->NearPlane);
    }
}

static void ReadNodeAttributes(
    RawModel &raw, FbxScene *pScene, FbxNode *pNode, const std::map<const FbxTexture *, FbxString> &textureLocations, bool keepIndex = false)
{
    if (!pNode->GetVisibility()) {
        return;
    }

	FbxBool isShow = pNode->Show.Get();
	printf("Node %s %s", pNode->GetName(), isShow ? "True" : "False");

    FbxNodeAttribute *pNodeAttribute = pNode->GetNodeAttribute();
    if (pNodeAttribute != nullptr) {
        const FbxNodeAttribute::EType attributeType = pNodeAttribute->GetAttributeType();
        switch (attributeType) {
            case FbxNodeAttribute::eMesh:
            case FbxNodeAttribute::eNurbs:
            case FbxNodeAttribute::eNurbsSurface:
            case FbxNodeAttribute::eTrimNurbsSurface:
            case FbxNodeAttribute::ePatch: {
                ReadMesh(raw, pScene, pNode, textureLocations, keepIndex);
                break;
            }
            case FbxNodeAttribute::eCamera: {
                ReadCamera(raw, pScene, pNode);
                break;
            }
            case FbxNodeAttribute::eUnknown:
				break;
            case FbxNodeAttribute::eNull:
				break;
            case FbxNodeAttribute::eMarker:
				break;
            case FbxNodeAttribute::eSkeleton:
				SetSkeleton(raw, pScene, pNode);
				break;
            case FbxNodeAttribute::eCameraStereo:
            case FbxNodeAttribute::eCameraSwitcher:
            case FbxNodeAttribute::eLight:
            case FbxNodeAttribute::eOpticalReference:
            case FbxNodeAttribute::eOpticalMarker:
            case FbxNodeAttribute::eNurbsCurve:
            case FbxNodeAttribute::eBoundary:
            case FbxNodeAttribute::eShape:
            case FbxNodeAttribute::eLODGroup:
            case FbxNodeAttribute::eSubDiv:
            case FbxNodeAttribute::eCachedEffect:
            case FbxNodeAttribute::eLine: {
                break;
            }
        }
    }

    for (int child = 0; child < pNode->GetChildCount(); child++) {
        ReadNodeAttributes(raw, pScene, pNode->GetChild(child), textureLocations, keepIndex);
    }
}



static void ReadNodeHierarchy(
    RawModel &raw, FbxScene *pScene, FbxNode *pNode,
    const long parentId, const std::string &path)
{
    const FbxUInt64 nodeId = pNode->GetUniqueID();
    const char *nodeName = pNode->GetName();
    const int  nodeIndex = raw.AddNode(nodeId, nodeName, parentId);
    RawNode    &node     = raw.GetNode(nodeIndex);

    FbxTransform::EInheritType lInheritType;
    pNode->GetTransformationInheritType(lInheritType);

    std::string newPath = path + "/" + nodeName;
    if (verboseOutput) {
        printf("node %d: %s\n", nodeIndex, newPath.c_str());
    }

    static int warnRrSsCount = 0;
    static int warnRrsCount  = 0;
    if (lInheritType == FbxTransform::eInheritRrSs && parentId) {
        if (++warnRrSsCount == 1) {
            printf("Warning: node uses unsupported transform inheritance type 'eInheritRrSs'.\n", newPath.c_str());
            printf("         (Further warnings of this type squelched.)\n");
        }

    } else if (lInheritType == FbxTransform::eInheritRrs) {
        if (++warnRrsCount == 1) {
            printf(
                "Warning: node %s uses unsupported transform inheritance type 'eInheritRrs'\n"
                    "     This tool will attempt to partially compensate, but glTF cannot truly express this mode.\n"
                    "     If this was a Maya export, consider turning off 'Segment Scale Compensate' on all joints.\n"
                    "     (Further warnings of this type squelched.)\n",
                newPath.c_str());
        }
    }

    // Set the initial node transform.
    const FbxAMatrix    localTransform   = pNode->EvaluateLocalTransform();
    const FbxVector4    localTranslation = localTransform.GetT();
    const FbxQuaternion localRotation    = localTransform.GetQ();
    const FbxVector4    localScaling     = computeLocalScale(pNode);

    node.translation = toVec3f(localTranslation) * scaleFactor;
    node.rotation    = toQuatf(localRotation);
    node.scale       = toVec3f(localScaling);

    if (parentId) {
        RawNode &parentNode = raw.GetNode(raw.GetNodeById(parentId));
        // Add unique child name to the parent node.
        if (std::find(parentNode.childIds.begin(), parentNode.childIds.end(), nodeId) == parentNode.childIds.end()) {
            parentNode.childIds.push_back(nodeId);
        }
    } else {
        // If there is no parent then this is the root node.
        raw.SetRootNode(nodeId);
    }

    for (int child = 0; child < pNode->GetChildCount(); child++) {
        ReadNodeHierarchy(raw, pScene, pNode->GetChild(child), nodeId, newPath);
    }
}
static void ParseLayer(FbxNode* pNode, FbxAnimLayer* pLayer);
static void ParseChannel(FbxNode* pNode, FbxAnimLayer* pLayer);
static void ReadAnimations2(FbxScene* pSDKScene)
{
	FbxNode* pNode = pSDKScene->GetRootNode();
	const int nStackCount = pSDKScene->GetSrcObjectCount<FbxAnimStack>();
	for (int i = 0; i < nStackCount; ++i)
	{
		FbxAnimStack* pStack = pSDKScene->GetSrcObject<FbxAnimStack>(i);
		const char* szStackName = pStack->GetName();
		const int nLayerCount = pStack->GetMemberCount<FbxAnimLayer>();
		for (int j = 0; j < nLayerCount; ++j)
		{
			FbxAnimLayer* pLayer = pStack->GetMember<FbxAnimLayer>(j);
			const char* szLayerName = pLayer->GetName();
			ParseLayer(pNode, pLayer);
		}
	}
}
//----------------------------------------------------------------
static void ParseLayer(FbxNode* pNode, FbxAnimLayer* pLayer)
{
	ParseChannel(pNode, pLayer);

	const int nChildCount = pNode->GetChildCount();
	for (int k = 0; k < nChildCount; ++k)
	{
		ParseLayer(pNode->GetChild(k), pLayer);
	}
}
//----------------------------------------------------------------
static void ParseChannel(FbxNode* pNode, FbxAnimLayer* pLayer)
{
	FbxAnimCurve* pCurve = pNode->LclTranslation.GetCurve(pLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (pCurve == 0)
	{
		return;
	}

	FbxTime kTime;
	char lTimeString[256];

	const int nKeyCount = pCurve->KeyGetCount();
	for (int i = 0; i < nKeyCount; ++i)
	{
		kTime = pCurve->KeyGetTime(i);
		const FbxAMatrix& kMatLocal = pNode->EvaluateLocalTransform(kTime);
		const FbxAMatrix& kMatGlobal = pNode->EvaluateGlobalTransform(kTime);
		//
		kTime.GetTimeString(lTimeString, FbxUShort(256));
		//SoLogDebug("FBXTime : %s", lTimeString);
	}
}

struct KeyFrame 
{
	float time;
	FbxAMatrix transform;
};
void GetKeyFrameStdError(const std::vector<KeyFrame>& keyFrames, FbxVector4& posvariance, FbxVector4& scalvariance,FbxVector4& rotvariance);
void ComporessKeyFrame(std::vector<KeyFrame>& keyFrames, const AnimExportOption& opt);
void ComporessKeyFrameV1(std::vector<KeyFrame>& keyFrames, const AnimExportOption& opt);

template < class T>
void FilterInvalidFileNameChar(T& str)
{
	T t;
	t.resize(9);
    t[0] = 0x5C; //    \ 
	t[1] = 0x2F; //    /
	t[2] = 0x3A; //    : 
	t[3] = 0x2A; //    * 
	t[4] = 0x3F; //    ? 
	t[5] = 0x22; //    " 
	t[6] = 0x3C; //    < 
	t[7] = 0x3E; //    > 
	t[8] = 0x7C; //    |
	int length = str.length();
	for (int i = 0; i < length; ++i)
	{
		if (t.find(str[i]) != T::npos)
		{
			str[i] = 0x5F;
		}
	}
}


static int GetNodeIndex(FbxScene *pScene, FbxNode *pParent)
{
	const int nodeCount = pScene->GetNodeCount();
	for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {
		if (pScene->GetNode(nodeIndex)==pParent)
		{
			return nodeIndex;
		}
	}
	return -1;
}

static void ReadBoneTree(RawModel &raw, FbxScene *pScene)
{
	const int nodeCount = pScene->GetNodeCount();
	std::vector<int> boneIdxArr;
	for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) {
		FbxNode *pNode = pScene->GetNode(nodeIndex);
		FbxNode *pParent = pNode->GetParent();
		int parentIdx = raw.GetNodeById(pParent->GetUniqueID());
	}
}

static void FindCurveMinMaxTime(FbxAnimCurve* pAnimCurve, FbxLongLong& minFrame, FbxLongLong& maxFrame, const FbxTime::EMode eMode)
{
	if (pAnimCurve != NULL)
	{
		int nXCount = pAnimCurve->KeyGetCount();
		if (nXCount > 0)
		{
			FbxLongLong x_maxFrame;
			FbxLongLong x_minFrame;
			if (nXCount == 1)
			{
				x_minFrame = pAnimCurve->KeyGetTime(0).GetFrameCount(eMode);
				x_maxFrame = x_minFrame;
			}
			else
			{
				x_minFrame = pAnimCurve->KeyGetTime(0).GetFrameCount(eMode);
				x_maxFrame = pAnimCurve->KeyGetTime(nXCount - 1).GetFrameCount(eMode);
			}
			minFrame = std::min(minFrame, x_minFrame);
			maxFrame = std::max(maxFrame, x_maxFrame);
		}
	}
}

static void ConvertTangent(FbxAnimCurve* pAnimCurve, const FbxLongLong curFrameIdx, const FbxTime::EMode eMode, float& leftTan, float& rightTan, uint8_t& tanMode)
{
	int nCount = pAnimCurve->KeyGetCount();

	FbxLongLong maxFrame;
	FbxLongLong minFrame;
	if (nCount == 1)
	{
		minFrame = pAnimCurve->KeyGetTime(0).GetFrameCount();
		maxFrame = minFrame;
	}
	else
	{
		minFrame = pAnimCurve->KeyGetTime(0).GetFrameCount();
		maxFrame = pAnimCurve->KeyGetTime(nCount - 1).GetFrameCount();
	}

	if (curFrameIdx < minFrame)
	{
		FbxAnimCurveDef::EInterpolationType etype = pAnimCurve->KeyGetInterpolation(0);
		tanMode = static_cast<uint8_t>(etype);
		if (etype&FbxAnimCurveDef::eInterpolationConstant != 0)
		{
			leftTan = pAnimCurve->KeyGetLeftDerivative(0);
			rightTan = pAnimCurve->KeyGetRightDerivative(0);
		}
	}
	else if (curFrameIdx > maxFrame)
	{
		FbxAnimCurveDef::EInterpolationType etype = pAnimCurve->KeyGetInterpolation(nCount - 1);
		tanMode = static_cast<uint8_t>(etype);
		if (etype&FbxAnimCurveDef::eInterpolationConstant != 0)
		{
			leftTan = pAnimCurve->KeyGetLeftDerivative(nCount - 1);
			rightTan = pAnimCurve->KeyGetRightDerivative(nCount - 1);
		}
	}
	else
	{
		FbxTime ftime;
		ftime.SetFrame(curFrameIdx, eMode);
		FbxAnimCurveDef::EInterpolationType start_etype = pAnimCurve->KeyGetInterpolation(0);
		FbxAnimCurveDef::EInterpolationType end_etype = pAnimCurve->KeyGetInterpolation(nCount - 1);
		
		int midFrameidx = (minFrame + maxFrame)*0.5f;
		
		if (curFrameIdx <= midFrameidx)
		{
			tanMode = static_cast<uint8_t>(start_etype);
		}
		else
		{
			tanMode = static_cast<uint8_t>(end_etype);
		}

		leftTan = pAnimCurve->EvaluateLeftDerivative(ftime);
		rightTan = pAnimCurve->EvaluateRightDerivative(ftime);
	}
}

static void AddKeyFrameValues(FbxAnimCurve* pAnimCurve, const FbxDouble3 defaultValue, const FbxLongLong frameIndex, const FbxTime pTime, const FbxTime::EMode eMode, Vec1fKeyFrames& keyframes)
{
	animationKey<float> key_value;
	if (pAnimCurve != NULL)
	{
		key_value.value = pAnimCurve->Evaluate(pTime);
		key_value.time = pTime.GetSecondDouble();
		ConvertTangent(pAnimCurve, frameIndex, eMode, key_value.leftTan, key_value.rightTan, key_value.tangentMode);
		keyframes.push_back(key_value);
	}
	else
	{
		key_value.value = defaultValue[0];
		key_value.time = pTime.GetSecondDouble();
		key_value.leftTan = 0.0f;
		key_value.leftTan = 0.0f;
		key_value.tangentMode = FbxAnimCurveDef::eInterpolationLinear;
		keyframes.push_back(key_value);
	}
}

static void AddKeyFrameValues(FbxAnimCurve* pAnimCurve, Vec1fKeyFrames& keyframes, bool bScale)
{
	int nCount = pAnimCurve->KeyGetCount();
	for (size_t i = 0; i < nCount; ++i)
	{
		animationKey<float> key_value;
		float value = pAnimCurve->KeyGetValue(i);
		FbxTime ftime = pAnimCurve->KeyGetTime(i);
		key_value.value =  bScale ? value * scaleFactor : value;
		key_value.time = ftime.GetSecondDouble();
		key_value.tangentMode = pAnimCurve->KeyGetInterpolation(i);
		key_value.leftTan = bScale ? pAnimCurve->KeyGetLeftDerivative(i) * scaleFactor : pAnimCurve->KeyGetLeftDerivative(i);
		key_value.rightTan = bScale ? pAnimCurve->KeyGetRightDerivative(i) * scaleFactor : pAnimCurve->KeyGetLeftDerivative(i);
		keyframes.push_back(key_value);
	}

}

static void ConvertTrans_Scale_Curves(Vec1fKeyFrames* vec3Keys, FbxAnimLayer* pAnimLayer, FbxNode* pNode, const FbxTime::EMode eMode, FbxPropertyT<FbxDouble3>& property, bool bScale = false)
{
	FbxAnimCurve* pAnimCurveX = property.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* pAnimCurveY = property.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* pAnimCurveZ = property.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	if (pAnimCurveX != NULL)
	{
		AddKeyFrameValues(pAnimCurveX, vec3Keys[0], bScale);
	}

	if (pAnimCurveY != NULL)
	{
		AddKeyFrameValues(pAnimCurveY, vec3Keys[1], bScale);
	}

	if (pAnimCurveZ != NULL)
	{
		AddKeyFrameValues(pAnimCurveZ, vec3Keys[2], bScale);
	}

}

static void ConvertRotationCurves(Vec1fKeyFrames* vec3Keys, FbxAnimLayer* pAnimLayer, FbxNode* pNode, const FbxTime::EMode eMode, FbxPropertyT<FbxDouble3>& property)
{
	FbxAnimCurve* pAnimCurveX = property.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* pAnimCurveY = property.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* pAnimCurveZ = property.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	if (pAnimCurveX == NULL && pAnimCurveY == NULL && pAnimCurveZ == NULL)
	{
		return;
	}

	FbxLongLong minFrame = std::numeric_limits<FbxLongLong>::infinity();
	FbxLongLong maxFrame = -std::numeric_limits<FbxLongLong>::infinity();

	FindCurveMinMaxTime(pAnimCurveX, minFrame, maxFrame, eMode);
	FindCurveMinMaxTime(pAnimCurveY, minFrame, maxFrame, eMode);
	FindCurveMinMaxTime(pAnimCurveZ, minFrame, maxFrame, eMode);

	for (FbxLongLong frameIndex = minFrame; frameIndex <= maxFrame; frameIndex++)
	{
		FbxTime pTime;
		pTime.SetFrame(frameIndex, eMode);
		FbxDouble3 defaultValue = property.Get();

		AddKeyFrameValues(pAnimCurveX, defaultValue, frameIndex, pTime, eMode, vec3Keys[0]);
		AddKeyFrameValues(pAnimCurveY, defaultValue, frameIndex, pTime, eMode, vec3Keys[1]);
		AddKeyFrameValues(pAnimCurveZ, defaultValue, frameIndex, pTime, eMode, vec3Keys[2]);
	}
}

static void ConvertQuatCurves(QuatfKeyFrames& quatKeys, FbxAnimLayer* pAnimLayer, FbxNode* pNode, const FbxTime::EMode eMode, FbxPropertyT<FbxDouble3>& property)
{
	FbxAnimCurve* pAnimCurveX = property.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* pAnimCurveY = property.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* pAnimCurveZ = property.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);

	if (pAnimCurveX == NULL && pAnimCurveY == NULL && pAnimCurveZ == NULL)
	{
		return;
	}

	FbxLongLong minFrame = std::numeric_limits<FbxLongLong>::infinity();
	FbxLongLong maxFrame = -std::numeric_limits<FbxLongLong>::infinity();

	FindCurveMinMaxTime(pAnimCurveX, minFrame, maxFrame, eMode);
	FindCurveMinMaxTime(pAnimCurveY, minFrame, maxFrame, eMode);
	FindCurveMinMaxTime(pAnimCurveZ, minFrame, maxFrame, eMode);

	for (FbxLongLong frameIndex = minFrame; frameIndex <= maxFrame; frameIndex++)
	{
		FbxTime pTime;
		pTime.SetFrame(frameIndex, eMode);
		
		FbxVector4 euler = pNode->EvaluateLocalRotation(pTime);
		FbxAMatrix rotm;
		rotm.SetIdentity();
		rotm.SetROnly(euler);
		FbxQuaternion quat = rotm.GetQ();

		animationKey<Math::Quaternion> rotkey;
		rotkey.value = toQuatf(quat);
		rotkey.time = pTime.GetSecondDouble();

		if (!quatKeys.empty())
		{
			Math::Quaternion& lastQuat = quatKeys.back().value;
			if (lastQuat.Dot(rotkey.value) < 0.0f)
			{
				rotkey.value = -rotkey.value;
			}
		}

		quatKeys.push_back(rotkey);
	}
}

static void ReadAnimations(RawModel &raw, FbxScene *pScene,const AnimExportOption& option)
{    
	FbxGlobalSettings& lTimeSettings = pScene->GetGlobalSettings();
	FbxTime::EMode  eMode = lTimeSettings.GetTimeMode();
    const double epsilon = 1e-4f;

    const int animationCount = pScene->GetSrcObjectCount<FbxAnimStack>();
    for (size_t animIx = 0; animIx < animationCount; animIx++) 
	{
        FbxAnimStack *pAnimStack = pScene->GetSrcObject<FbxAnimStack>(animIx);
        FbxString animStackName = pAnimStack->GetName();

		//我们只导出当前被应用的动画
		FbxAnimStack *currentStack = pScene->GetCurrentAnimationStack();
		if (pAnimStack == currentStack)
			raw.SetOriginalIndex(raw.GetAnimationCount());

        pScene->SetCurrentAnimationStack(pAnimStack);
		FbxAnimLayer* pAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(0);

        FbxTakeInfo *takeInfo = pScene->GetTakeInfo(animStackName);
        if (takeInfo == nullptr) {
            printf("Warning:: animation '%s' has no Take information. Skipping.\n", animStackName.Buffer());
            // not all animstacks have a take
            continue;
        }
        if (verboseOutput) {
            printf("animation %zu: %s (%d%%)", animIx, (const char *)animStackName.Buffer(), 0);
        }

        FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
        FbxTime end   = takeInfo->mLocalTimeSpan.GetStop();

		double clipStart = takeInfo->mLocalTimeSpan.GetStart().GetSecondDouble();
		double clipEnd = takeInfo->mLocalTimeSpan.GetStop().GetSecondDouble();

        RawAnimation animation;
		animation.startTime = static_cast<float>(clipStart);
		animation.endTime = static_cast<float>(clipEnd);
        animation.name = animStackName;
		FilterInvalidFileNameChar(animation.name);
		animation.isHasScale = option.UseMatrixKeyFrame;  //默认没有scale，使用双四元数

        FbxLongLong firstFrameIndex = start.GetFrameCount(eMode);
        FbxLongLong lastFrameIndex  = end.GetFrameCount(eMode);
		animation.beginFrame = firstFrameIndex;
		animation.frameRate = FbxTime::GetFrameRate(eMode);

		int totalkeyframes = 0;
		int compressedkeyframes = 0;
        for (FbxLongLong frameIndex = firstFrameIndex; frameIndex <= lastFrameIndex; frameIndex++) 
		{
            FbxTime pTime;
            // first frame is always at t = 0.0
            //pTime.SetFrame(frameIndex - firstFrameIndex, eMode);
			pTime.SetFrame(frameIndex, eMode);
            animation.times.emplace_back((float) pTime.GetSecondDouble());
        }

		RawMorphAnimation morphAnimation;

        size_t totalSizeInBytes = 0;
		std::vector<KeyFrame> keyFrames;
        const int nodeCount = pScene->GetNodeCount();
        for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++) 
		{
			keyFrames.clear();
		
            FbxNode *pNode = pScene->GetNode(nodeIndex);
			RawChannel channel;
			channel.nodeIndex = raw.GetNodeById(pNode->GetUniqueID());
			if (channel.nodeIndex == -1)
				continue;
			RawNode& rawNode = raw.GetNode(channel.nodeIndex);
			//if (!rawNode.isSkeleton)
				//continue;
			 
			FbxNode *pParent = pNode->GetParent();
			if (pParent == nullptr)
			{
				continue;
			}
			int parentIdx = raw.GetNodeById(pParent->GetUniqueID());
			RawNode& rawParentNode = raw.GetNode(parentIdx);
			//if (!rawParentNode.isSkeleton)
				//parentIdx = -1;
			channel.parentIndex = parentIdx;

			ConvertTrans_Scale_Curves(channel.translationKeys, pAnimLayer, pNode, eMode, pNode->LclTranslation, true);
			ConvertTrans_Scale_Curves(channel.scaleKeys, pAnimLayer, pNode, eMode, pNode->LclScaling);
			ConvertQuatCurves(channel.quatKeys, pAnimLayer, pNode, eMode, pNode->LclRotation);

			// 解析moprh动画
			FbxNodeAttribute* nodeAttr = pNode->GetNodeAttribute();
			if (nodeAttr != nullptr && nodeAttr->GetAttributeType() == FbxNodeAttribute::EType::eMesh)
			{
				// 这里保存每个morph deformer的通道,targetids,full_weights等信息 不在这里保存target的顶点属性

				FbxBlendShapesAccess blendshapes(static_cast<FbxMesh*>(nodeAttr));

				RawMorphMesh morphMesh; // 有效的一个modifier object(3dsMax Curve Editor)
				morphMesh.originid = nodeAttr->GetUniqueID(); // 标记是哪个模型的deformer 
				morphMesh.originname = rawNode.name; // mesh name

				for (int i = 0; i < blendshapes.GetDefomerCount(); i++)
				{
					auto& blendshape = blendshapes.GetMeshDeformer(i);
					RawMorphMeshDeformer deformer; // 有效的一个modifier object(3dsMax Curve Editor)
					deformer.deformerid = blendshape.uid; // 标记是哪个模型的deformer 
					deformer.name = blendshape.name; // mesh name

					bool isValid = false;

					for (int c = 0; c < blendshape.channels.size(); c++)
					{
						auto& mchannel = blendshape.channels[c];
						FbxAnimCurve* curve = mchannel.GetAnimation(animIx);

						int targetCount = mchannel.targetShapes.size();

						if (targetCount > 0)
						{

							int index = deformer.channels.size();
							deformer.channels.emplace_back();
							RawMorphChannel&  rawmorph = deformer.channels[index];

							rawmorph.name = std::string(mchannel.name); // curve->GetName() 为null但是channel name不是nul

							rawmorph.cid = mchannel.cid;
							//rawmorph.cid = curve->GetUniqueID(); 	// curve的id跟channel的id不一样,fbx用不一样的id标识

							MorphInfo morphInfo;
							
							// 如果存在曲线, 就记录曲线信息
							if (curve != nullptr && curve->KeyGetCount())
							{
								AddKeyFrameValues(curve, morphInfo.floatKeys, true);
								morphInfo.ind = index;
								morphInfo.pRawMorphChannel = new RawMorphChannel;
								*morphInfo.pRawMorphChannel = rawmorph;
								channel.morphInfoVec.push_back(morphInfo);
							}


							// 记录Targets和Weights 
							for (int k = 0; k < targetCount; k++)
							{
								const FbxBlendShapesAccess::TargetShape & fbxShape = mchannel.targetShapes[k];
								rawmorph.targetids.emplace_back(fbxShape.shape->GetUniqueID());
								rawmorph.weights.emplace_back(fbxShape.fullWeight);
							}

							isValid = true;

							// Morph动画文件 通过 FbxMesh.GetUniqueID 得到原来模型  通过 FbxShape.GetUniqueID 得到通道对应的Target
						}
					}

					if (isValid)
					{
						morphMesh.deformers.emplace_back(deformer);
					}

				}

				if (morphMesh.deformers.size() != 0)
				{
					rawNode.hasMorph = true;
					rawNode.morpherId = morphAnimation.morphers.size();
					rawNode.morphAnimationId = raw.GetMorphAnimationCount();

					morphAnimation.morphers.emplace_back(morphMesh);
				}
			}

			if (!channel.translationKeys[0].empty() || !channel.translationKeys[1].empty() || !channel.translationKeys[2].empty() ||
				!channel.scaleKeys[0].empty() || !channel.scaleKeys[1].empty() || !channel.scaleKeys[2].empty() || 
				!channel.quatKeys.empty() || !channel.morphInfoVec.empty())
			{
				animation.channels.emplace_back(channel);
			}

			

			totalSizeInBytes += channel.translations.size() * sizeof(channel.translations[0]) +
				channel.rotations.size() * sizeof(channel.rotations[0]) +
				channel.scales.size() * sizeof(channel.scales[0]);


            if (verboseOutput) {
                printf("\ranimation %d: %s (%d%%)", animIx, (const char *) animStackName, nodeIndex * 100 / nodeCount);
            }
        }
		printf("\ranimation %d: %s ", animIx, (const char *)animStackName);
		printf(" have %d keyframes in total ", totalkeyframes);
		printf(" comporessed %d keyframes", compressedkeyframes);
		printf(" lost %f percent keyframes \n", (float)compressedkeyframes*100.f/ totalkeyframes);

		if (!animation.channels.empty())
		{
			raw.AddAnimation(animation);
		}
        

        if (verboseOutput) {
            printf(
                "\ranimation %d: %s (%d channels, %3.1f MB)\n", animIx, (const char *) animStackName,
                (int) animation.channels.size(), (float) totalSizeInBytes * 1e-6f);
        }

		
		if (morphAnimation.morphers.size() != 0)
		{
			raw.AddMorphAnimation(morphAnimation);
		}
    }
}
FbxVector4 LerpVec4(const FbxVector4& fs,const FbxVector4& fe,float per)
{
	FbxVector4 ret = fs*(1 - per) + fe*per;
	return ret;
}

bool IsNeedVec4(const FbxVector4& fs, const FbxVector4& fe, float per)
{
	FbxVector4 ret = fs - fe;
	if (ret.Length()>per)
	{
		return true;
	}
	//printf("abandon some frames:position or scale changes less than 0.01");
	return false;
}

bool IsNeedQuat(const FbxQuaternion& fs, const FbxQuaternion& fe, float per)
{
	float ret = fe.DotProduct(fs);
	if (ret+per>=1)
	{
		return false;
	}
	//printf("abandon some frames:rotation changes less than 0.01");
	return true;
}

void ComporessKeyFrame(std::vector<KeyFrame>& keyFrames, const AnimExportOption& opt)
{
	
	for (auto itr = keyFrames.begin(); itr+3!= keyFrames.end();)
	{
		auto itr1 = itr + 1;
		auto itr2 = itr + 2;
		FbxAMatrix trans0 = (*itr).transform;
		FbxAMatrix trans1 = (*itr1).transform;
		FbxAMatrix trans2 = (*itr2).transform;

		float time0 = (*itr).time;
		float time1 = (*itr1).time;
		float time2 = (*itr2).time;

		float percent = (time1 - time0) / (time2 - time0);

		const FbxVector4    pos0 = trans0.GetT();
		const FbxQuaternion rot0 = trans0.GetQ();
		const FbxVector4    scl0 = trans0.GetS();

	
		const FbxVector4    pos1 = trans1.GetT();
		const FbxQuaternion rot1 = trans1.GetQ();
		const FbxVector4    scl1 = trans1.GetS();

		const FbxVector4    pos2 = trans2.GetT();
		const FbxQuaternion rot2 = trans2.GetQ();
		const FbxVector4    scl2 = trans2.GetS();

		bool needpos = true;
		bool needrot = true;
		bool needscl = true;

		const FbxVector4 posLerp = LerpVec4(pos0, pos2, percent);
		const FbxQuaternion rotLerp = rot0.Slerp(rot2, percent);

		const FbxVector4 sclLerp = LerpVec4(scl0, scl2, percent);

		FbxVector4 sxcyzl = rotLerp.DecomposeSphericalXYZ();
		FbxVector4 sxcyz2 = rot2.DecomposeSphericalXYZ();


		needpos = IsNeedVec4(posLerp, pos1, 0.02);
		needrot = IsNeedQuat(rotLerp, rot1, 0.001);
		needscl = IsNeedVec4(sclLerp, scl1, 0.001);

		if (needpos == false && needrot == false && needscl == false)
		{
			keyFrames.erase(itr2);
		}
		else
		{
			itr++;
		}
	}

}

void GetKeyFrameStdError(const std::vector<KeyFrame>& keyFrames,FbxVector4& posvariance, FbxVector4& scalvariance, FbxVector4& rotvariance)
{
	int keyframelengh = keyFrames.size();
	FbxVector4 postotal = FbxVector4(0, 0, 0, 0);
	FbxVector4 scaletotal = FbxVector4(0, 0, 0, 0);
	FbxVector4 rottotal = FbxVector4(0, 0, 0, 0);
	for (auto itr = keyFrames.begin(); itr != keyFrames.end(); itr++)
	{
		FbxAMatrix trans = (*itr).transform;

		const FbxVector4    pos = trans.GetT();
		const FbxQuaternion rot = trans.GetQ();
		const FbxVector4    scl = trans.GetS();

		rottotal += rot.DecomposeSphericalXYZ();
		postotal += pos;
		scaletotal += scl;
	}

	FbxVector4 posmean = postotal / keyframelengh;
	FbxVector4 scalemean = scaletotal / keyframelengh;
	FbxVector4 rotmean = rottotal / keyframelengh;

	FbxVector4 posv = FbxVector4(0, 0, 0, 0);
	FbxVector4 scalev = FbxVector4(0, 0, 0, 0);
	FbxVector4 rotv = FbxVector4(0, 0, 0, 0);
	for (auto itr = keyFrames.begin(); itr != keyFrames.end(); itr++)
	{
		FbxAMatrix trans = (*itr).transform;


		const FbxVector4    pos = trans.GetT();
		const FbxQuaternion rot = trans.GetQ();
		const FbxVector4    scl = trans.GetS();

		posv += (pos - posmean)*(pos - posmean);
		scalev += (scl - scalemean)*(scl - scalemean);
		rotv += (rot.DecomposeSphericalXYZ() - rotmean)*(rot.DecomposeSphericalXYZ() - rotmean);
	}

	posv = posv / keyframelengh;
	scalev = scalev / keyframelengh;
	rotv = rotv / keyframelengh;

	posvariance.Set(sqrt(posv[0]), sqrt(posv[1]), sqrt(posv[2]), sqrt(posv[3]));

	scalvariance.Set(sqrt(scalev[0]), sqrt(scalev[1]), sqrt(scalev[2]), sqrt(scalev[3]));
	rotvariance.Set(sqrt(rotv[0]), sqrt(rotv[1]), sqrt(rotv[2]), sqrt(rotv[3]));
}

void ComporessKeyFrameV1(std::vector<KeyFrame>& keyFrames, const AnimExportOption& opt)
{
	bool needNext = false;
	for (auto itr = keyFrames.begin(); itr  != keyFrames.end()&& itr + 1 != keyFrames.end()&& itr + 2 != keyFrames.end();)
	{
		auto itr1 = itr + 1;
		auto itr2 = itr + 2;
		FbxAMatrix trans0 = (*itr).transform;
		FbxAMatrix trans1 = (*itr1).transform;
		FbxAMatrix trans2 = (*itr2).transform;

		float time0 = (*itr).time;
		float time1 = (*itr1).time;
		float time2 = (*itr2).time;

		float percent = (time1 - time0) / (time2 - time0);

		const FbxVector4    pos0 = trans0.GetT();
		const FbxQuaternion rot0 = trans0.GetQ();
		const FbxVector4    scl0 = trans0.GetS();

		const FbxVector4    pos1 = trans1.GetT();
		const FbxQuaternion rot1 = trans1.GetQ();
		const FbxVector4    scl1 = trans1.GetS();

		const FbxVector4    pos2 = trans2.GetT();
		const FbxQuaternion rot2 = trans2.GetQ();
		const FbxVector4    scl2 = trans2.GetS();

		bool needpos = true;
		bool needrot = true;
		bool needscl = true;

		const FbxVector4 posLerp = LerpVec4(pos0, pos2, percent);
		const FbxQuaternion rotLerp = rot0.Slerp(rot2, percent);
		const FbxVector4 sclLerp = LerpVec4(scl0, scl2, percent);

		FbxVector4 sxcyzl = rotLerp.DecomposeSphericalXYZ();
		FbxVector4 sxcyz2 = rot2.DecomposeSphericalXYZ();


		needpos = IsNeedVec4(posLerp, pos1, opt.PositionError);
		needrot = IsNeedQuat(rotLerp, rot1, opt.RotationError);
		needscl = IsNeedVec4(sclLerp, scl1, opt.ScaleError);

		if (needpos == false && needrot == false && needscl == false)
		{
			needNext = true;
			keyFrames.erase(itr1);
		}
		else
		{
			itr++;
		}
		itr++;
	}
	if (needNext)
	{
		ComporessKeyFrameV1(keyFrames, opt);
	}
	
}


static std::string GetInferredFileName(const std::string &fbxFileName, const std::string &directory, const std::vector<std::string> &directoryFileList)
{
    if (FileUtils::FileExists(fbxFileName)) {
        return fbxFileName;
    }
    // Get the file name with file extension.
    const std::string fileName = StringUtils::GetFileNameString(StringUtils::GetCleanPathString(fbxFileName));

    // Try to find a match with extension.
    for (const auto &file : directoryFileList) {
        if (StringUtils::CompareNoCase(fileName, file) == 0) {
            return std::string(directory) + file;
        }
    }

    // Get the file name without file extension.
    const std::string fileBase = StringUtils::GetFileBaseString(fileName);

    // Try to find a match without file extension.
    for (const auto &file : directoryFileList) {
        // If the two extension-less base names match.
        if (StringUtils::CompareNoCase(fileBase, StringUtils::GetFileBaseString(file)) == 0) {
            // Return the name with extension of the file in the directory.
            return std::string(directory) + file;
        }
    }

    return "";
}

/*
    The texture file names inside of the FBX often contain some long author-specific
    path with the wrong extensions. For instance, all of the art assets may be PSD
    files in the FBX metadata, but in practice they are delivered as TGA or PNG files.

    This function takes a texture file name stored in the FBX, which may be an absolute
    path on the author's computer such as "C:\MyProject\TextureName.psd", and matches
    it to a list of existing texture files in the same directory as the FBX file.
*/
static void
FindFbxTextures(
    FbxScene *pScene, const char *fbxFileName, const char* textureSearchPath,const char *extensions, std::map<const FbxTexture *, FbxString> &textureLocations)
{
    // Get the folder the FBX file is in.
    const std::string folder = StringUtils::GetFolderString(fbxFileName);

	//先用用户指定路径去查找
	const std::string userSearchFolder = textureSearchPath;
	if (!userSearchFolder.empty())
	{
		std::vector<std::string> userFileList = FileUtils::ListFolderFiles(userSearchFolder.c_str(), extensions);
		for (int i = 0; i < pScene->GetTextureCount(); i++) {
			const FbxFileTexture *pFileTexture = FbxCast<FbxFileTexture>(pScene->GetTexture(i));
			if (pFileTexture == nullptr) {
				continue;
			}
			std::string inferredName = GetInferredFileName(pFileTexture->GetFileName(), userSearchFolder, userFileList);
			if (inferredName.empty()) 
			{
				printf("Warning: could not find a local image file for texture: %s.with user specified search path\n"
					"Original filename: %s\nTry reletaive filename\n", pFileTexture->GetName(), pFileTexture->GetFileName());
			}
			else
				textureLocations.emplace(pFileTexture, inferredName.c_str());
		}
	}

    // Check if there is a filename.fbm folder to which embedded textures were extracted.
    const std::string fbmFolderName = folder + StringUtils::GetFileBaseString(fbxFileName) + ".fbm/";

    // Search either in the folder with embedded textures or in the same folder as the FBX file.
    const std::string searchFolder = FileUtils::FolderExists(fbmFolderName) ? fbmFolderName : folder;

    // Get a list with all the texture files from either the folder with embedded textures or the same folder as the FBX file.
    std::vector<std::string> fileList = FileUtils::ListFolderFiles(searchFolder.c_str(), extensions);

    // Try to match the FBX texture names with the actual files on disk.
    for (int i = 0; i < pScene->GetTextureCount(); i++) {
        const FbxFileTexture *pFileTexture = FbxCast<FbxFileTexture>(pScene->GetTexture(i));
        if (pFileTexture == nullptr) {
            continue;
        }

		if (textureLocations.find(pFileTexture) != textureLocations.end()) {  //已经在用户指定路径找到了
			continue;
		}

        std::string inferredName = GetInferredFileName(pFileTexture->GetFileName(), searchFolder, fileList);
        if (inferredName.empty()) {
            printf("Warning: could not find a local image file for texture: %s.\n"
            "Original filename: %s\nTry reletaive filename\n", pFileTexture->GetName(), pFileTexture->GetFileName());

			const std::string relativeFilePath = folder + pFileTexture->GetRelativeFileName();
			inferredName = GetInferredFileName(relativeFilePath, searchFolder, fileList);
			if (inferredName.empty())
			{
				printf("Warning: could not find a local image file for texture: %s. even use relative path\n"
					"Reletaive filename:%s\n", pFileTexture->GetName(), pFileTexture->GetRelativeFileName());
			}
        }
		

        // always extend the mapping, even for files we didn't find
        textureLocations.emplace(pFileTexture, inferredName.c_str());
    }
}

static void WriteObj(const char *objFileName, RawModel& raw)
{
	std::ofstream outfile(objFileName);
	for (int i = 0; i < raw.GetVertexCount(); i++)
	{
		RawVertex vertex = raw.GetVertex(i);
		double x = (double)vertex.position.x;
		double y = (double)vertex.position.y;
		double z = (double)vertex.position.z;
		outfile << "v " << x << " " << y << " " << z << std::endl;
	}
	for (int i = 0; i < raw.GetVertexCount(); i++)
	{
		RawVertex vertex = raw.GetVertex(i);
		double x = (double)vertex.normal.x;
		double y = (double)vertex.normal.y;
		double z = (double)vertex.normal.z;
		outfile << "vn " << x << " " << y << " " << z << std::endl;
	}
	for (int i = 0; i < raw.GetVertexCount(); i++)
	{
		RawVertex vertex = raw.GetVertex(i);
		double x = (double)vertex.uv0.x;
		double y = (double)vertex.uv0.y;
		outfile << "vt " << x << " " << y << std::endl;
	}
	for (int i = 0; i < raw.GetTriangleCount(); i++)
	{
		RawTriangle triangle = raw.GetTriangle(i);
		int x = triangle.verts[0] + 1;
		int y = triangle.verts[1] + 1;
		int z = triangle.verts[2] + 1;
		outfile << "f " << x << "/" << x << "/" << x << " " << y << "/" << y << "/" << y << " " << z << "/" << z << "/" << z << std::endl;
	}
	outfile.close();
}

bool LoadFBXFile(RawModel &raw, const char *fbxFileName, const char* textureSearchPath, const char *textureExtensions, const AnimExportOption& animOption)
{
	FbxManager    *pManager = FbxManager::Create();
	FbxIOSettings *pIoSettings = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(pIoSettings);

	FbxImporter *pImporter = FbxImporter::Create(pManager, "");

	if (!pImporter->Initialize(fbxFileName, -1, pManager->GetIOSettings())) {
		if (verboseOutput) {
			printf("%s\n", pImporter->GetStatus().GetErrorString());
			printf("Can't exoprt fbx file : %s", fbxFileName);
		}
		pImporter->Destroy();
		pManager->Destroy();
		return false;
	}

	FbxScene *pScene = FbxScene::Create(pManager, "fbxScene");

	bool result = pImporter->Import(pScene);
	if (!result)
	{
		printf("FbxImporter import fail \n");
		return false;
	}

	pImporter->Destroy();


	if (pScene == nullptr) {
		pImporter->Destroy();
		pManager->Destroy();
		return false;
	}

	std::map<const FbxTexture *, FbxString> textureLocations;
	FindFbxTextures(pScene, fbxFileName, textureSearchPath, textureExtensions, textureLocations);

	// Use Y up
	FbxAxisSystem axisSys = pScene->GetGlobalSettings().GetAxisSystem();
	if (axisSys != FbxAxisSystem::OpenGL)
		FbxAxisSystem::OpenGL.ConvertScene(pScene);






	const FbxSystemUnit::ConversionOptions lConversionOptions = {
		true, /* mConvertRrsNodes */
		true, /* mConvertAllLimits */
		true, /* mConvertClusters */
		true, /* mConvertLightIntensity */
		true, /* mConvertPhotometricLProperties */
		true  /* mConvertCameraClipPlanes */
	};
	FbxSystemUnit sceneSystemUnit = pScene->GetGlobalSettings().GetSystemUnit();

	FbxSystemUnit meterSystemUnit = FbxSystemUnit::m;

	if (sceneSystemUnit != FbxSystemUnit::m) {
		// FbxSystemUnit::m.ConvertScene(pScene,lConversionOptions);
	}

	//// this is always 0.01, but let's opt for clarity.
	scaleFactor = FbxSystemUnit::m.GetConversionFactorFrom(sceneSystemUnit);

	ReadNodeHierarchy(raw, pScene, pScene->GetRootNode(), 0, "");

	FbxGlobalSettings& lTimeSettings = pScene->GetGlobalSettings();
	FbxTime::EMode  eMode = lTimeSettings.GetTimeMode();
	FbxNode* pRoot = pScene->GetRootNode();
	ZeroPivotsForSkinsRecursive(pScene, pRoot);
	pRoot->ConvertPivotAnimationRecursive(nullptr, FbxNode::eDestinationPivot, FbxTime::GetFrameRate(eMode), false);
	ReadNodeAttributes(raw, pScene, pScene->GetRootNode(), textureLocations, animOption.KeepIndex);
	ReadAnimations(raw, pScene, animOption);
	
	CorrectMorphTargetNormal(raw);
	

	raw.ComputeTangentBasis();  //计算切线和副法线
	raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_TANGENT);
	raw.AddVertexAttribute(RAW_VERTEX_ATTRIBUTE_BINORMAL);

	pScene->Destroy();
	pManager->Destroy();
	if (animOption.WriteObj)
	{
		std::string objFileName = fbxFileName;
		size_t start_pos = objFileName.length() - 4;
		objFileName.replace(start_pos, 4, ".obj");
		WriteObj(objFileName.c_str(), raw);
	}
    return true;
}
