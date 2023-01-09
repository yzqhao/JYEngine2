
#include "BindingMath.h"

#include <sol/sol.hpp>
#include <type_traits>

NS_JYE_BEGIN

void Bind_Math(sol::state& sol_state)	
{
	auto math_ns_table = sol_state["Math"].get_or_create<sol::table>();

	{	// IntVec2
		math_ns_table.new_usertype<Math::IntVec2>("IntVec2", sol::call_constructor, sol::constructors<
			sol::types<>,
			sol::types<int, int>,
			sol::types<const Math::IntVec2&>>(),

			"x", &Math::IntVec2::x,
			"y", &Math::IntVec2::y,
			sol::meta_function::to_string, [](const Math::IntVec2* vec) -> std::string {return vec->toString(); },
			sol::meta_function::addition, [](const Math::IntVec2* vec_a, const  Math::IntVec2* vec_b) {return (*vec_a) + (*vec_b); },
			sol::meta_function::subtraction, [](const Math::IntVec2* vec_a, const  Math::IntVec2* vec_b) {return (*vec_a) - (*vec_b); },
			sol::meta_function::multiplication, [](const Math::IntVec2* vec, const float a) {return (*vec) * a; },
			sol::meta_function::division, [](const Math::IntVec2* vec, const float a) {return (*vec) / a; },
			sol::meta_function::unary_minus, [](const Math::IntVec2* vec) {return (*vec) * -1; },
			sol::meta_function::equal_to, [](const Math::IntVec2* vec_a, const  Math::IntVec2* vec_b) {return (*vec_a) == (*vec_b); }
		);
	}

	{	// IntVec4
		math_ns_table.new_usertype<Math::IntVec4>("IntVec4", sol::call_constructor, sol::constructors<
			sol::types<>,
			sol::types<int, int, int, int>,
			sol::types<const Math::IntVec4&>>(),

			"x", & Math::IntVec4::x,
			"y", & Math::IntVec4::y,
			"z", & Math::IntVec4::z,
			"w", & Math::IntVec4::w,
			sol::meta_function::to_string, [](const Math::IntVec4* vec) -> std::string {return vec->toString(); },
			sol::meta_function::addition, [](const Math::IntVec4* vec_a, const  Math::IntVec4* vec_b) {return (*vec_a) + (*vec_b); },
			sol::meta_function::subtraction, [](const Math::IntVec4* vec_a, const  Math::IntVec4* vec_b) {return (*vec_a) - (*vec_b); },
			sol::meta_function::multiplication, [](const Math::IntVec4* vec, const float a) {return (*vec) * a; },
			sol::meta_function::division, [](const Math::IntVec4* vec, const float a) {return (*vec) / a; },
			sol::meta_function::unary_minus, [](const Math::IntVec4* vec) {return (*vec) * -1; },
			sol::meta_function::equal_to, [](const Math::IntVec4* vec_a, const  Math::IntVec4* vec_b) {return (*vec_a) == (*vec_b); }
		);
	}

	{	// Vec2
		math_ns_table.new_usertype<Math::Vec2>("Vec2", sol::call_constructor, sol::constructors<
			sol::types<>, 
			sol::types<float, float>, 
			sol::types<const Math::Vec2&>>(),

			"x", &Math::Vec2::x,
			"y", & Math::Vec2::y,
			"GetPtr",
			sol::property([](Math::Vec2& s) {
				return sol::var(std::ref(s.x));
				}),
			sol::meta_function::to_string, [](const Math::Vec2* vec) -> std::string {return vec->toString(); },
			sol::meta_function::addition, [](const Math::Vec2* vec_a, const  Math::Vec2* vec_b) {return (*vec_a) + (*vec_b); },
			sol::meta_function::subtraction, [](const Math::Vec2* vec_a, const  Math::Vec2* vec_b) {return (*vec_a) - (*vec_b); },
			sol::meta_function::multiplication, [](const Math::Vec2* vec, const float a) {return (*vec) * a; },
			sol::meta_function::division, [](const Math::Vec2* vec, const float a) {return (*vec) / a; },
			sol::meta_function::unary_minus, [](const Math::Vec2* vec) {return (*vec) * -1; },
			sol::meta_function::equal_to, [](const Math::Vec2* vec_a, const  Math::Vec2* vec_b) {return (*vec_a) == (*vec_b); }
		);
	}

	{
		// Vec3
		math_ns_table.new_usertype<Math::Vec3>("Vec3", sol::call_constructor, sol::constructors<
			sol::types<>, 
			sol::types<float>, 
			sol::types<float, float, float>,
			sol::types<const Math::Vec3&>>(),

			"x", &Math::Vec3::x,
			"y", &Math::Vec3::y,
			"z", &Math::Vec3::z,
			"Equals", & Math::Vec3::Equals,
			"Set", & Math::Vec3::Set,
			"Normalize", & Math::Vec3::Normalize,
			"GetNormalized", & Math::Vec3::GetNormalized,
			"LengthSquared", & Math::Vec3::LengthSquared,
			"Length", & Math::Vec3::Length,
			"Dot", & Math::Vec3::Dot,
			"GetPtr", 
			sol::property([](Math::Vec3& s) {
				return sol::var(std::ref(s.x));
				}),
			sol::meta_function::to_string, [](const Math::Vec3* vec) -> std::string {return vec->toString(); },
			sol::meta_function::addition, [](const Math::Vec3* vec_a, const  Math::Vec3* vec_b) {return (*vec_a) + (*vec_b); },
			sol::meta_function::subtraction, [](const Math::Vec3* vec_a, const  Math::Vec3* vec_b) {return (*vec_a) - (*vec_b); },
			sol::meta_function::multiplication, [](const Math::Vec3* vec, const float a) {return (*vec) * a; },
			sol::meta_function::division, [](const Math::Vec3* vec, const float a) {return (*vec) / a; },
			sol::meta_function::unary_minus, [](const Math::Vec3* vec) {return (*vec) * -1; },
			sol::meta_function::equal_to, [](const Math::Vec3* vec_a, const  Math::Vec3* vec_b) {return (*vec_a) == (*vec_b); }
		);

		math_ns_table.set_function("Dot", sol::overload([](const Math::Vec3& v1, const Math::Vec3& v2) {return Math::Dot(v1, v2); }));
		math_ns_table.set_function("CrossProduct", sol::overload([](const Math::Vec3& v1, const Math::Vec3& v2) {return Math::CrossProduct(v1, v2); }));
		math_ns_table.set_function("Distance", sol::overload([](const Math::Vec3& v1, const Math::Vec3& v2) {return Math::Distance(v1, v2); }));
		math_ns_table.set_function("DistanceSquared", sol::overload([](const Math::Vec3& v1, const Math::Vec3& v2) {return Math::DistanceSquared(v1, v2); }));
	}

	{	// Vec4
		math_ns_table.new_usertype<Math::Vec4>("Vec4", sol::call_constructor, sol::constructors<
			sol::types<>,
			sol::types<const float&>,
			sol::types<const float&, const float&, const float&, const float&>,
			sol::types<const Math::Vec3&>,
			sol::types<const Math::Vec3&, const float&>,
			sol::types<const Math::Vec4&>>(),

			"x", &Math::Vec4::x,
			"y", &Math::Vec4::y,
			"z", &Math::Vec4::z,
			"w", & Math::Vec4::w,
			"GetPtr", [](Math::Vec4& vec) { 
				return sol::var(std::ref(vec.x)); 
			},
			/* sol::property([](Math::Vec4& s) {
				return sol::var(std::ref(s.x));
				}), */
			sol::meta_function::to_string, [](const Math::Vec4* vec) -> std::string {return vec->toString(); },
			sol::meta_function::addition, [](const Math::Vec4* vec_a, const  Math::Vec4* vec_b) {return (*vec_a) + (*vec_b); },
			sol::meta_function::subtraction, [](const Math::Vec4* vec_a, const  Math::Vec4* vec_b) {return (*vec_a) - (*vec_b); },
			sol::meta_function::multiplication, [](const Math::Vec4* vec, const float a) {return (*vec) * a; },
			sol::meta_function::division, [](const Math::Vec4* vec, const float a) {return (*vec) / a; },
			sol::meta_function::unary_minus, [](const Math::Vec4* vec) {return (*vec) * -1; },
			sol::meta_function::equal_to, [](const Math::Vec4* vec_a, const  Math::Vec4* vec_b) {return (*vec_a) == (*vec_b); }
		);
	}
	{	// Mat3
		math_ns_table.new_usertype<Math::Mat3>("Mat3", sol::call_constructor, sol::constructors<
			sol::types<>,
			sol::types<const float*>>(),

			"a11", &Math::Mat3::a11,
			"a12", &Math::Mat3::a12,
			"a13", &Math::Mat3::a13,
			"a21", &Math::Mat3::a21,
			"a22", &Math::Mat3::a22,
			"a23", &Math::Mat3::a23,
			"a31", &Math::Mat3::a31,
			"a32", &Math::Mat3::a32,
			"a33", & Math::Mat3::a33,
			"GetPtr",
			sol::property([](Math::Mat3& s) {
				return sol::var(std::ref(s.a11));
				}),
			sol::meta_function::to_string, [](const Math::Mat3* mat) -> std::string {return mat->toString(); }
			);
	}
	{	// Mat4
		math_ns_table.new_usertype<Math::Mat4>("Mat4", sol::call_constructor, sol::constructors<
			sol::types<>,
			sol::types<float>,
			sol::types<float, float, float, float,
				float, float, float, float,
				float, float, float, float,
				float, float, float, float>>(),

			"a11", &Math::Mat4::a11,
			"a12", &Math::Mat4::a12,
			"a13", &Math::Mat4::a13,
			"a14", &Math::Mat4::a14,
			"a21", &Math::Mat4::a21,
			"a22", &Math::Mat4::a22,
			"a23", &Math::Mat4::a23,
			"a24", &Math::Mat4::a24,
			"a31", &Math::Mat4::a31,
			"a32", &Math::Mat4::a32,
			"a33", &Math::Mat4::a33,
			"a34", &Math::Mat4::a34,
			"a41", &Math::Mat4::a41,
			"a42", &Math::Mat4::a42,
			"a43", &Math::Mat4::a43,
			"a44", & Math::Mat4::a44,
			"GetPtr",
			sol::property([](Math::Mat4& s) {
				return sol::var(std::ref(s.a11));
				}),
			sol::meta_function::to_string, [](const Math::Mat4* mat) -> std::string {return mat->toString(); },
			//"Get", &Math::Mat4::Get,
			"ToMatrix3", &Math::Mat4::ToMatrix3,
			"IsIdentity", &Math::Mat4::IsIdentity,
			"SetIdentity", &Math::Mat4::SetIdentity,
			//"Multiply", &Math::Mat4::Multiply,
			"Determinant", &Math::Mat4::Determinant,
			"Inversed", &Math::Mat4::Inversed,
			"GetInversed", &Math::Mat4::GetInversed,
			"GetScale", &Math::Mat4::GetScale,
			//"Decompose", &Math::Mat4::Decompose,
			//"GetRotation", &Math::Mat4::GetRotation,
			"GetTranslation", &Math::Mat4::GetTranslation,
			"Rotate", sol::overload(
				(void (Math::Mat4::*)(const Math::Quaternion&))& Math::Mat4::Rotate,
				(void (Math::Mat4::*)(const Math::Vec3& axis, float angle))& Math::Mat4::Rotate),
			"RotateX", &Math::Mat4::RotateX,
			"RotateY", &Math::Mat4::RotateY,
			"RotateZ", &Math::Mat4::RotateZ,
			"Scale", sol::overload(
				(void (Math::Mat4::*)(float)) &Math::Mat4::Scale,
				(void (Math::Mat4::*)(float, float, float))&Math::Mat4::Scale,
				(void (Math::Mat4::*)(const Math::Vec3&)) &Math::Mat4::Scale),
			"Translate", sol::overload(
				(void (Math::Mat4::*)(float, float, float))&Math::Mat4::Translate,
				(void (Math::Mat4::*)(const Math::Vec3&))&Math::Mat4::Translate),
			"Transpose", &Math::Mat4::Transpose,
			"GetTransposed", &Math::Mat4::GetTransposed
		);
	}

	{	// FColor
		math_ns_table.new_usertype<Math::FColor>("FColor", sol::call_constructor, sol::constructors<
			sol::types<>,
			sol::types<byte, byte, byte, byte>,
			sol::types<uint>>(),

			"A", &Math::FColor::A,
			"R", &Math::FColor::R,
			"G", &Math::FColor::G,
			"B", &Math::FColor::B,
			sol::meta_function::to_string, [](const Math::Mat3* mat) -> std::string {return mat->toString(); }
			);
	}

	{	// FLinearColor
		math_ns_table.new_usertype<Math::FLinearColor>("FLinearColor", sol::call_constructor, sol::constructors<
			sol::types<>,
			sol::types<float, float, float, float>,
			sol::types<const Math::FColor&>,
			sol::types<const Math::Vec3&>,
			sol::types<const Math::Vec4&>>(),

			"R", &Math::FLinearColor::R,
			"G", &Math::FLinearColor::G,
			"B", & Math::FLinearColor::B,
			"A", & Math::FLinearColor::A,
			sol::meta_function::to_string, [](const Math::Mat3* mat) -> std::string {return mat->toString(); }
		);
	}
}


NS_JYE_END

