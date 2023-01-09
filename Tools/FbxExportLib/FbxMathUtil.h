#pragma once

#include "Math/3DMath.h"
#include <fbxsdk.h>

static inline Math::Vec3 toVec3f(const FbxVector4 &v) {
	return Math::Vec3((float)v[0], (float)v[1], (float)v[2]);
}

static inline Math::Vec4 toVec4f(const FbxVector4 &v) {
	return Math::Vec4((float)v[0], (float)v[1], (float)v[2], (float)v[3]);
}

//FBX SDK“≤ «¡–æÿ’Û
static inline Math::Mat4 toMat4f(const FbxAMatrix &m) {
	auto result = Math::Mat4();
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			result.m[row * 4 + col] = (float)m[row][col];
		}
	}
	return result;
}

static inline Math::Quaternion toQuatf(const FbxQuaternion &q) {
	return Math::Quaternion((float)q[0], (float)q[1], (float)q[2], (float)q[3]);
}