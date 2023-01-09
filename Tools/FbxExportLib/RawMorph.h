#pragma once

#include <vector>
#include "Math/3DMath.h"

USING_JYE_CC

enum DRACO_GENERIC_UNIQUE_ID
{
	OLD_VERSION_COMPATIBLE = 0,
	BOND_INDEX = 1,
	BOND_WEIGHTS = 2,
	MORPH_TARGETS_VERTEX = 3,
	MORPH_TARGETS_NORMAL = 4,
};

struct RawMorphChannel
{
	std::string name;
	uint64_t cid = 0;
	std::vector<uint64_t> targetids;
	std::map<uint64_t, uint64_t> posTargetID2DrcID;
	std::map<uint64_t, uint64_t> normalTargetID2DrcID;
	std::vector<float> weights;
	std::vector<Math::Vec4> keyframes; // std::vector<Math::Vec4> keyframe;  // 时间 值 左导 右导 
};

struct RawMorphMeshDeformer
{
	uint64_t deformerid = 0 ;		// uid@Deformer
	std::string name;				// name@FDeformer
	std::vector<RawMorphChannel> channels;
};

struct RawMorphMesh
{
	uint64_t originid = 0;		// uid@FbxMesh
	std::string originname;     // name@FbxMesh
	std::vector<RawMorphMeshDeformer> deformers;
};

struct RawMorphAnimation
{
	std::string             name;
	std::vector<RawMorphMesh> morphers;
	double beginTime = 0.0; // 单位:秒
	double endTime = 0.0;
};
