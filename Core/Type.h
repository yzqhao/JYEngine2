
#pragma once

#include "Core.h"
#include "Math/3DMath.h"

NS_JYE_BEGIN

template<class T>
class Pointer;

// 定义静态类型判断

#define HAS_TRIVIAL_CONSTRUCTOR(T) std::is_trivially_constructible<T>::value
#define HAS_TRIVIAL_DESTRUCTOR(T) std::is_trivially_destructible<T>::value
#define HAS_TRIVIAL_ASSIGN(T) std::is_trivially_assignable<T>::value
#define HAS_TRIVIAL_COPY(T) std::is_trivially_copyable<T>::value
#define IS_POD(T) std::is_pod<T>::value
#define IS_ENUM(T) std::is_enum<T>::value
#define IS_EMPTY(T) std::is_empty<T>::value

template<typename T> struct TIsFloatType { enum { Value = std::is_floating_point<T>::value }; };
template<typename T> struct TIsIntegralType { enum { Value = std::is_integral<T>::value }; };
template<typename T> struct TIsArithmeticType { enum { Value = std::is_arithmetic<T>::value }; };
template<typename T> struct TIsNativePointerType { enum { Value = std::is_pointer<T>::value }; };
template<typename T> struct TIsVoidType { enum { Value = std::is_void<T>::value }; };
template<typename T> struct TIsPODType { enum { Value = IS_POD(T) }; };
template<typename T> struct TIsFundamentalType { enum { Value = std::is_fundamental<T>::Value }; };

template<typename T> struct ValueBase
{
	enum { NeedsConstructor = !HAS_TRIVIAL_CONSTRUCTOR(T) && !TIsPODType<T>::Value };
	enum { NeedsDestructor = !HAS_TRIVIAL_DESTRUCTOR(T) && !TIsPODType<T>::Value };
};

#define DEFINE_TYPE(TypeName)\
	template<typename T> struct TIs##TypeName##Type { enum { Value = false }; };\
	template<typename T> struct TIs##TypeName##PointerType { enum { Value = false }; };\
	template<typename T> struct TIs##TypeName##SmartPointerType { enum { Value = false }; };
#define DEFINE_TYPE_INSTANCE(TypeName,ClassName)\
	class ClassName;\
	template<> struct TIs##TypeName##Type<ClassName> { enum { Value = true }; }; \
	template<> struct TIs##TypeName##PointerType<ClassName *> { enum { Value = true }; }; \
	template<> struct TIs##TypeName##PointerType<const ClassName *> { enum { Value = true }; }; \
	template<> struct TIs##TypeName##SmartPointerType<Pointer<ClassName>> { enum { Value = true }; }; \
	template<> struct TIs##TypeName##SmartPointerType<const Pointer<ClassName>> { enum { Value = true }; };

// 判断是不是自定义序列化操作
// 需要继承CustomArchiveObject类，然后实现Rrchive函数，并且定义DEFINE_TYPE_INSTANCE宏
// 序列化不支持指针类型
DEFINE_TYPE(Custom)
#define CUSTOMTYPE_MARCO(ClassName) \
	DEFINE_TYPE_INSTANCE(Custom,ClassName)

// 是不是继承自Object的类
DEFINE_TYPE(EObject)
#define TYPE_MARCO(ClassName) \
	DEFINE_TYPE_INSTANCE(EObject,ClassName)

// string
template<typename T> struct TIsStringType { enum { Value = false }; };
template<>	struct TIsStringType<String> { enum { Value = true }; };

// Archive on need Loop Vector 
template<typename T> struct TIsNoNeedLoop
{
	enum { Value = IS_ENUM(T) || TIsArithmeticType<T>::Value };
};
template<>	struct TIsNoNeedLoop <Math::AABB> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::FBox2D> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::FColor> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::FLinearColor> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::CubicBezier> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Frustum> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::IntRect> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::IntVec2> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::IntVec3> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::IntVec4> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Mat3> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Mat4> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Plane> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Quaternion> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Ray> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Rect> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Sphere> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Transform> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Vec1> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Vec2> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Vec3> { enum { Value = true }; };
template<>	struct TIsNoNeedLoop <Math::Vec4> { enum { Value = true }; };

template<typename T> struct TIsNeedGC
{
	enum
	{
		Value = false
		// 先去掉功能
		// Value = TIsEObjectPointerType<T>::Value ||
		// TIsEObjectSmartPointerType<T>::Value ||
		// TIsCustomType<T>::Value ||
		// TIsEObjectType<T>::Value
	};
};
template<typename T> struct TIsNeedGC<Vector<T>>
{
	enum
	{
		Value = TIsNeedGC<T>::Value
	};
};
template<typename K, typename V> struct TIsNeedGC<Map<K, V>>
{
	enum
	{
		Value = TIsNeedGC<K>::Value || TIsNeedGC<V>::Value
	};
};
template<typename K, typename V> struct TIsNeedGC<HashMap<K, V>>
{
	enum
	{
		Value = TIsNeedGC<K>::Value || TIsNeedGC<V>::Value
	};
};

NS_JYE_END
