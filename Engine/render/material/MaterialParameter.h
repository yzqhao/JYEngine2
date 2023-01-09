
#pragma once

#include "Engine/render/texture/TextureEntity.h"
#include "math/color.h"
#include "Core/Object.h"
#include "math/Vec2.h"
#include "math/Vec3.h"
#include "math/Vec4.h"
#include "math/Mat4.h"
#include "math/VectorArray.h"
#include "math/Matrix4fArray.h"

NS_JYE_BEGIN

enum MaterialParameterType
{
	MP_NONE,
	MP_MATRIX3,
	MP_MATRIX4,
	MP_VECTOR4,
	MP_VECTOR3,
	MP_VECTOR2,
	MP_VECTOR,
	MP_FLOAT,
	MP_COLOR,
	MP_TEXTURE,
	MP_MAX,
	MP_NumBits = 4,
};
static_assert(MP_MAX <= (1 << MP_NumBits), "MP_MAX will not fit on MP_NumBits");

class ENGINE_API MaterialParameter : public Object
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameter() : m_type(MP_NONE) {}
	virtual ~MaterialParameter() = default;

	MaterialParameterType GetParamType() const { return m_type; }
protected:
	MaterialParameterType m_type;
};
DECLARE_Ptr(MaterialParameter);
TYPE_MARCO(MaterialParameter);

class ENGINE_API MaterialParameterMat3 : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterMat3() { m_type = MP_MATRIX3; }
	MaterialParameterMat3(const Math::Mat3& v) : m_mat3(v) { m_type = MP_MATRIX3; }
	virtual ~MaterialParameterMat3() = default;

	Math::Mat3 m_mat3;
};
DECLARE_Ptr(MaterialParameterMat3);
TYPE_MARCO(MaterialParameterMat3);

class ENGINE_API MaterialParameterMat4 : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterMat4() { m_type = MP_MATRIX4; }
	MaterialParameterMat4(const Math::Mat4& v) : m_mat4(v) { m_type = MP_MATRIX4; }
	virtual ~MaterialParameterMat4() = default;

	Math::Mat4 m_mat4;
};
DECLARE_Ptr(MaterialParameterMat4);
TYPE_MARCO(MaterialParameterMat4);

class ENGINE_API MaterialParameterMat3Array : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterMat3Array() { m_type = MP_MATRIX3; }
	MaterialParameterMat3Array(const Math::Matrix3fArray& v) : m_matArray(v) { m_type = MP_MATRIX3; }
	virtual ~MaterialParameterMat3Array() = default;

	Math::Matrix3fArray m_matArray;
};
DECLARE_Ptr(MaterialParameterMat3Array);
TYPE_MARCO(MaterialParameterMat3Array);

class ENGINE_API MaterialParameterMat4Array : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterMat4Array() { m_type = MP_MATRIX4; }
	MaterialParameterMat4Array(const Math::Matrix4fArray& v) : m_matArray(v) { m_type = MP_MATRIX4; }
	virtual ~MaterialParameterMat4Array() = default;

	Math::Matrix4fArray m_matArray;
};
DECLARE_Ptr(MaterialParameterMat4Array);
TYPE_MARCO(MaterialParameterMat4Array);

class ENGINE_API MaterialParameterVec4 : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterVec4() { m_type = MP_VECTOR4; }
	MaterialParameterVec4(const Math::Vec4& v) : m_vec4(v) { m_type = MP_VECTOR4; }
	virtual ~MaterialParameterVec4() = default;

	Math::Vec4 m_vec4;
};
DECLARE_Ptr(MaterialParameterVec4);
TYPE_MARCO(MaterialParameterVec4);

class ENGINE_API MaterialParameterVec4Array : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterVec4Array() { m_type = MP_VECTOR4; }
	MaterialParameterVec4Array(const Math::Vector4ArrayF& v) : m_vecArray4(v) { m_type = MP_VECTOR4; }
	virtual ~MaterialParameterVec4Array() = default;

	Math::Vector4ArrayF m_vecArray4;
};
DECLARE_Ptr(MaterialParameterVec4Array);
TYPE_MARCO(MaterialParameterVec4Array);

class ENGINE_API MaterialParameterVec3 : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterVec3() { m_type = MP_VECTOR3; }
	MaterialParameterVec3(const Math::Vec3& v) : m_vec3(v) { m_type = MP_VECTOR3; }
	virtual ~MaterialParameterVec3() = default;

	Math::Vec3 m_vec3;
};
DECLARE_Ptr(MaterialParameterVec3);
TYPE_MARCO(MaterialParameterVec3);

class ENGINE_API MaterialParameterVec3Array : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterVec3Array() { m_type = MP_VECTOR3; }
	MaterialParameterVec3Array(const Math::Vector3ArrayF& v) : m_vecArray3(v) { m_type = MP_VECTOR3; }
	virtual ~MaterialParameterVec3Array() = default;

	Math::Vector3ArrayF m_vecArray3;
};
DECLARE_Ptr(MaterialParameterVec3Array);
TYPE_MARCO(MaterialParameterVec3Array);

class ENGINE_API MaterialParameterVec2 : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterVec2() { m_type = MP_VECTOR2; }
	MaterialParameterVec2(const Math::Vec2& v) : m_vec2(v) { m_type = MP_VECTOR2; }
	virtual ~MaterialParameterVec2() = default;

	Math::Vec2 m_vec2;
};
DECLARE_Ptr(MaterialParameterVec2);
TYPE_MARCO(MaterialParameterVec2);

class ENGINE_API MaterialParameterVec2Array : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterVec2Array() { m_type = MP_VECTOR2; }
	MaterialParameterVec2Array(const Math::Vector2ArrayF& v) : m_vecArray2(v) { m_type = MP_VECTOR2; }
	virtual ~MaterialParameterVec2Array() = default;

	Math::Vector2ArrayF m_vecArray2;
};
DECLARE_Ptr(MaterialParameterVec2Array);
TYPE_MARCO(MaterialParameterVec2Array);

class ENGINE_API MaterialParameterVecArray : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterVecArray() { m_type = MP_VECTOR; }
	MaterialParameterVecArray(const Math::VectorArrayF& v) : m_vecArray(v) { m_type = MP_VECTOR; }
	virtual ~MaterialParameterVecArray() = default;

	Math::VectorArrayF m_vecArray;
};
DECLARE_Ptr(MaterialParameterVecArray);
TYPE_MARCO(MaterialParameterVecArray);

class ENGINE_API MaterialParameterFloat : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterFloat() { m_type = MP_FLOAT; }
	MaterialParameterFloat(float val) : m_float(val) { m_type = MP_FLOAT; }
	virtual ~MaterialParameterFloat() = default;

	float m_float;
};
DECLARE_Ptr(MaterialParameterFloat);
TYPE_MARCO(MaterialParameterFloat);

class ENGINE_API MaterialParameterColor : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
public:
	MaterialParameterColor() { m_type = MP_COLOR; }
	MaterialParameterColor(float x, float y, float z, float w) : m_color(x, y, z, w) { m_type = MP_COLOR; }
	MaterialParameterColor(const Math::FLinearColor&v) : m_color(v) { m_type = MP_COLOR; }
	virtual ~MaterialParameterColor() = default;

	Math::FLinearColor m_color;
};
DECLARE_Ptr(MaterialParameterColor);
TYPE_MARCO(MaterialParameterColor);

class ENGINE_API MaterialParameterTex : public MaterialParameter
{
	DECLARE_RTTI
	DECLARE_INITIAL
private:
	TextureEntity* m_tex;
public:
	MaterialParameterTex() { m_type = MP_TEXTURE; }
	MaterialParameterTex(TextureEntity* tex) : m_tex(tex) { m_type = MP_TEXTURE; }
	virtual ~MaterialParameterTex() = default;

	TextureEntity* GetTex() { return m_tex; }
};
DECLARE_Ptr(MaterialParameterTex);
TYPE_MARCO(MaterialParameterTex);

template<class T>
struct TMaterialParamTraits
{
};

template<>
struct TMaterialParamTraits<float>
{
	typedef MaterialParameterFloat TConcreteType;
};

template<>
struct TMaterialParamTraits<Math::VectorArrayF>
{
	typedef MaterialParameterVecArray TConcreteType;
};

template<>
struct TMaterialParamTraits<Math::Vec2>
{
	typedef MaterialParameterVec2 TConcreteType;
};
template<>
struct TMaterialParamTraits<Math::Vector2ArrayF>
{
	typedef MaterialParameterVec2Array TConcreteType;
};
template<>
struct TMaterialParamTraits<Math::Vec3>
{
	typedef MaterialParameterVec3 TConcreteType;
};
template<>
struct TMaterialParamTraits<Math::Vector3ArrayF>
{
	typedef MaterialParameterVec3Array TConcreteType;
};
template<>
struct TMaterialParamTraits<Math::Vec4>
{
	typedef MaterialParameterVec4 TConcreteType;
};
template<>
struct TMaterialParamTraits<Math::Vector4ArrayF>
{
	typedef MaterialParameterVec4Array TConcreteType;
};
template<>
struct TMaterialParamTraits<Math::FColor>
{
	typedef MaterialParameterColor TConcreteType;
};
template<>
struct TMaterialParamTraits<Math::Mat3>
{
	typedef MaterialParameterMat3 TConcreteType;
};
template<>
struct TMaterialParamTraits<Math::Mat4>
{
	typedef MaterialParameterMat4 TConcreteType;
};
template<>
struct TMaterialParamTraits<Math::Matrix4fArray>
{
	typedef MaterialParameterMat4Array TConcreteType;
};
template<>
struct TMaterialParamTraits<TextureEntity*>
{
	typedef MaterialParameterTex TConcreteType;
};

template<typename TType>
static inline typename TMaterialParamTraits<TType>::TConcreteType* MakeMaterialParam(TType val)
{
	return _NEW typename TMaterialParamTraits<TType>::TConcreteType(val);
}

NS_JYE_END
