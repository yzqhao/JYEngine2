
#pragma once

#include "Vec3.h"
#include "Vec4.h"
#include "Quaternion.h"
#include "Mat3.h"

NS_JYE_MATH_BEGIN

class MATH_API Mat4
{
public:
	union
	{
		struct
		{
			float a11, a12, a13, a14;
			float a21, a22, a23, a24;
			float a31, a32, a33, a34;
			float a41, a42, a43, a44;
		};
        float m[16];
	};

	Mat4();
	Mat4(float vf);
    Mat4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
           float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44);
    Mat4(const Vec4& v1, const Vec4& v2, const Vec4& v3, const Vec4& v4);
    Mat4(const float* mat);
    Mat4(const Mat4& copy);
    ~Mat4() = default;

    Mat4 operator*(const Mat4& mat) const;
	Mat4& operator*=(const Mat4& mat);

    Mat4 operator * (float k) const 
    { 
        return Mat4(a11 * k, a12 * k, a13 * k, a14 * k,
            a21 * k, a22 * k, a23 * k, a24 * k, 
            a31 * k, a32 * k, a33 * k, a34 * k, 
            a41 * k, a42 * k, a43 * k, a44 * k);
    }
    Mat4 operator / (float inv) const 
    { 
        float k = 1.0f / inv;
		return Mat4(a11 * k, a12 * k, a13 * k, a14 * k,
			a21 * k, a22 * k, a23 * k, a24 * k,
			a31 * k, a32 * k, a33 * k, a34 * k,
			a41 * k, a42 * k, a43 * k, a44 * k);
    }

	static void Multiply(const Mat4& m1, const Mat4& m2, Mat4* dst);
	static Mat4 CreateLookAt(const Vec3& pos, const Vec3& target, const Vec3& up);
	static void CreateLookAt(const Vec3& pos, const Vec3& target, const Vec3& up, Mat4* dst);
	static void CreateLookAt(float eyeX, float eyeY, float eyeZ, float targetX, float targetY, float targetZ, float upX, float upY, float upZ, Mat4* dst);
	static Mat4 CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane);
	static void CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane, Mat4* dst);
	static void CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane, Mat4* dst);
	static Mat4 CreatePerspective(float fieldOfView, float aspectRatio, float zNearPlane, float zFarPlane);
    static void CreatePerspective(float fieldOfView, float aspectRatio, float zNearPlane, float zFarPlane, Mat4* dst);

	float& Get(int row, int column) { return m[row + (column * 4)]; }
	const float& Get(int row, int column)const { return m[row + (column * 4)]; }
    Mat3 ToMatrix3() const { return Mat3(a11, a21, a31, a12, a22, a32, a13, a23, a33);  }

    bool IsIdentity() const;
    void SetIdentity();

    void Multiply(const Mat4& mat);

    float Determinant() const;
    bool Inversed();
    Mat4 GetInversed() const;

    void GetScale(Vec3* scale) const;
    bool Decompose(Vec3* scale, Quaternion* rotation, Vec3* translation) const;
    Quaternion ToQuaternion() const;
    void GetTranslation(Vec3* translation) const;
    void Rotate(const Quaternion& q);
    void Rotate(const Vec3& axis, float angle);
    void RotateX(float angle);
    void RotateY(float angle);
    void RotateZ(float angle);
    void Scale(float value);
    void Scale(const Vec3& s);
    void Scale(float xScale, float yScale, float zScale);
    void Translate(float x, float y, float z);
    void Translate(const Vec3& t);
    void Transpose();
    Mat4 GetTransposed() const;

	std::string toString() const;

	const float* GetPtr() const { return &a11; }

    static const Mat4 ZERO;     
    static const Mat4 IDENTITY;     
};

inline Vec3& operator*=(Vec3& v, const Mat4& m)
{
    Vec3 temp(v.x * m.a11 + v.y * m.a21 + v.z * m.a31 + m.a41,
            v.x * m.a12 + v.y * m.a22 + v.z * m.a32 + m.a42,
            v.x * m.a13 + v.y * m.a23 + v.z * m.a33 + m.a43);
    v = temp;
    return v;
}

inline Vec3 operator*(const Vec3& v, const Mat4& m)
{
    return Vec3(v.x * m.a11 + v.y * m.a21 + v.z * m.a31 + m.a41,
                v.x * m.a12 + v.y * m.a22 + v.z * m.a32 + m.a42,
                v.x * m.a13 + v.y * m.a23 + v.z * m.a33 + m.a43);
}

inline Vec4& operator*=(Vec4& v, const Mat4& m)
{
    Vec4 temp(v.x * m.a11 + v.y * m.a21 + v.z * m.a31 + v.w * m.a41,
            v.x * m.a12 + v.y * m.a22 + v.z * m.a32 + v.w * m.a42,
            v.x * m.a13 + v.y * m.a23 + v.z * m.a33 + v.w * m.a43,
            v.x * m.a14 + v.y * m.a24 + v.z * m.a34 + v.w * m.a44);
    v = temp;
    return v;
}

inline Vec4 operator*(const Vec4& v, const Mat4& m)
{
    return Vec4(v.x * m.a11 + v.y * m.a21 + v.z * m.a31 + v.w * m.a41,
                v.x * m.a12 + v.y * m.a22 + v.z * m.a32 + v.w * m.a42,
                v.x * m.a13 + v.y * m.a23 + v.z * m.a33 + v.w * m.a43,
                v.x * m.a14 + v.y * m.a24 + v.z * m.a34 + v.w * m.a44);
}

NS_JYE_MATH_END
