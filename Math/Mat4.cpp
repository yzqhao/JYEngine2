
#include "Mat4.h"
#include "Vec3.h"
#include "Vec4.h"

NS_JYE_MATH_BEGIN

#define MATRIX_SIZE (sizeof(float) * 16)

const Mat4 Mat4::IDENTITY = Mat4(
                    1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);

const Mat4 Mat4::ZERO = Mat4(
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0,
                    0, 0, 0, 0 );

Mat4::Mat4()
{
    *this = IDENTITY;
}

Mat4::Mat4(float vf)
{
	a11 = vf;
	a12 = vf;
	a13 = vf;
	a14 = vf;
	a21 = vf;
	a22 = vf;
	a23 = vf;
	a24 = vf;
	a31 = vf;
	a32 = vf;
	a33 = vf;
	a34 = vf;
	a41 = vf;
	a42 = vf;
	a43 = vf;
	a44 = vf;
}

Mat4::Mat4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24,
           float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
{
    a11 = m11;
    a12 = m12;
    a13 = m13;
    a14 = m14;
    a21 = m21;
    a22 = m22;
    a23 = m23;
    a24 = m24;
    a31 = m31;
    a32 = m32;
    a33 = m33;
    a34 = m34;
    a41 = m41;
    a42 = m42;
    a43 = m43;
    a44 = m44;
}

Mat4::Mat4(const Vec4& v1, const Vec4& v2, const Vec4& v3, const Vec4& v4)
{
	a11 = v1.x;
	a12 = v1.y;
	a13 = v1.z;
	a14 = v1.w;
	a21 = v2.x;
	a22 = v2.y;
	a23 = v2.z;
	a24 = v2.w;
	a31 = v3.x;
	a32 = v3.y;
	a33 = v3.z;
	a34 = v3.w;
	a41 = v4.x;
	a42 = v4.y;
	a43 = v4.z;
	a44 = v4.w;
}

Mat4::Mat4(const float* mat)
{
    JY_ASSERT(mat);
    memcpy(this->m, mat, MATRIX_SIZE);
}

Mat4::Mat4(const Mat4& copy)
{
    memcpy(m, copy.m, MATRIX_SIZE);
}

Mat4 Mat4::operator*(const Mat4& mat) const
{
    Mat4 res;
    Multiply(*this, mat, &res);
    return res;
}

Mat4& Mat4::operator*=(const Mat4& mat)
{
	Multiply(*this, mat, this);
	return *this;
}

bool Mat4::IsIdentity() const
{
    return memcmp(m, &IDENTITY, MATRIX_SIZE) == 0;
}

void Mat4::SetIdentity()
{
    *this = IDENTITY;
}

void Mat4::Multiply(const Mat4& mat)
{
    Multiply(*this, m, this);
}

void Mat4::Multiply(const Mat4& m1, const Mat4& m2, Mat4* dst)
{
	Mat4 product;
    product.a11 = m1.a11 * m2.a11 + m1.a12 * m2.a21 + m1.a13 * m2.a31 + m1.a14 * m2.a41;
    product.a12 = m1.a11 * m2.a12 + m1.a12 * m2.a22 + m1.a13 * m2.a32 + m1.a14 * m2.a42;
    product.a13 = m1.a11 * m2.a13 + m1.a12 * m2.a23 + m1.a13 * m2.a33 + m1.a14 * m2.a43;
    product.a14 = m1.a11 * m2.a14 + m1.a12 * m2.a24 + m1.a13 * m2.a34 + m1.a14 * m2.a44;
    product.a21 = m1.a21 * m2.a11 + m1.a22 * m2.a21 + m1.a23 * m2.a31 + m1.a24 * m2.a41;
    product.a22 = m1.a21 * m2.a12 + m1.a22 * m2.a22 + m1.a23 * m2.a32 + m1.a24 * m2.a42;
    product.a23 = m1.a21 * m2.a13 + m1.a22 * m2.a23 + m1.a23 * m2.a33 + m1.a24 * m2.a43;
    product.a24 = m1.a21 * m2.a14 + m1.a22 * m2.a24 + m1.a23 * m2.a34 + m1.a24 * m2.a44;
    product.a31 = m1.a31 * m2.a11 + m1.a32 * m2.a21 + m1.a33 * m2.a31 + m1.a34 * m2.a41;
    product.a32 = m1.a31 * m2.a12 + m1.a32 * m2.a22 + m1.a33 * m2.a32 + m1.a34 * m2.a42;
    product.a33 = m1.a31 * m2.a13 + m1.a32 * m2.a23 + m1.a33 * m2.a33 + m1.a34 * m2.a43;
    product.a34 = m1.a31 * m2.a14 + m1.a32 * m2.a24 + m1.a33 * m2.a34 + m1.a34 * m2.a44;
    product.a41 = m1.a41 * m2.a11 + m1.a42 * m2.a21 + m1.a43 * m2.a31 + m1.a44 * m2.a41;
    product.a42 = m1.a41 * m2.a12 + m1.a42 * m2.a22 + m1.a43 * m2.a32 + m1.a44 * m2.a42;
    product.a43 = m1.a41 * m2.a13 + m1.a42 * m2.a23 + m1.a43 * m2.a33 + m1.a44 * m2.a43;
    product.a44 = m1.a41 * m2.a14 + m1.a42 * m2.a24 + m1.a43 * m2.a34 + m1.a44 * m2.a44;
    memcpy(dst, product.GetPtr(), MATRIX_SIZE);
}

Mat4 Mat4::CreateLookAt(const Vec3& pos, const Vec3& target, const Vec3& up)
{
	Mat4 dst;
	CreateLookAt(pos.x, pos.y, pos.z, target.x, target.y, target.z, up.x, up.y, up.z, &dst);
	return dst;
}

void Mat4::CreateLookAt(const Vec3& pos, const Vec3& target, const Vec3& up, Mat4* dst)
{
    CreateLookAt(pos.x, pos.y, pos.z, target.x, target.y, target.z, up.x, up.y, up.z, dst);
}

void Mat4::CreateLookAt(float eyeX, float eyeY, float eyeZ, float targetX, float targetY, float targetZ, float upX, float upY, float upZ, Mat4* dst)
{
	Vec3 eye(eyeX, eyeY, eyeZ);
	Vec3 target(targetX, targetY, targetZ);
	Vec3 up(upX, upY, upZ);
	up.Normalize();

	Vec3 zaxis = eye - target;
	zaxis.Normalize();

	Vec3 xaxis = CrossProduct(up, zaxis);
	xaxis.Normalize();

	Vec3 yaxis = CrossProduct(zaxis, xaxis);
	yaxis.Normalize();

	dst->a11 = xaxis.x;
	dst->a12 = yaxis.x;
	dst->a13 = zaxis.x;
	dst->a14 = 0.0f;

	dst->a21 = xaxis.y;
	dst->a22 = yaxis.y;
	dst->a23 = zaxis.y;
	dst->a24 = 0.0f;

	dst->a31 = xaxis.z;
	dst->a32 = yaxis.z;
	dst->a33 = zaxis.z;
	dst->a34 = 0.0f;

	dst->a41 = -Dot(xaxis, eye);
	dst->a42 = -Dot(yaxis, eye);
	dst->a43 = -Dot(zaxis, eye);
	dst->a44 = 1.0f;
}

Mat4 Mat4::CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane)
{
	Mat4 res;
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	CreateOrthographicOffCenter(-halfWidth, halfWidth, -halfHeight, halfHeight, zNearPlane, zFarPlane, &res);
	return res;
}

void Mat4::CreateOrthographic(float width, float height, float zNearPlane, float zFarPlane, Mat4* dst)
{
	float halfWidth = width / 2.0f;
	float halfHeight = height / 2.0f;
	CreateOrthographicOffCenter(-halfWidth, halfWidth, -halfHeight, halfHeight, zNearPlane, zFarPlane, dst);
}

void Mat4::CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane, Mat4* dst)
{
	memset(dst, 0, MATRIX_SIZE);
	dst->a11 = 2 / (right - left);
	dst->a22 = 2 / (top - bottom);
	dst->a33 = 1 / (zFarPlane - zNearPlane);

	dst->a41 = (left + right) / (left - right);
	dst->a42 = (top + bottom) / (bottom - top);
	dst->a43 = (zNearPlane) / (zNearPlane - zFarPlane);
	dst->a44 = 1;
}

Mat4 Mat4::CreatePerspective(float fieldOfView, float aspectRatio, float zNearPlane, float zFarPlane)
{
	Mat4 res;
	CreatePerspective(fieldOfView, aspectRatio, zNearPlane, zFarPlane, &res);
	return res;
}

void Mat4::CreatePerspective(float fieldOfView, float aspectRatio, float zNearPlane, float zFarPlane, Mat4* dst)
{
	float h = 1.0f / Tan(fieldOfView * 0.5f);
	float w = h / aspectRatio;
	float p1 = -(zFarPlane + zNearPlane) / (zFarPlane - zNearPlane);
	float p2 = -2.0f * (zFarPlane * zNearPlane) / (zFarPlane - zNearPlane);

	//------------------------------
	*dst = Mat4(w, 0.0, 0.0, 0.0,
		0.0, h, 0.0, 0.0,
		0.0, 0.0, p1, -1.0f,
		0.0, 0.0, p2, 0.0);
}

float Mat4::Determinant() const
{
	double m00 = a11;  double m01 = a12;  double m02 = a13;  double m03 = a14;
	double m10 = a21;  double m11 = a22;  double m12 = a23;  double m13 = a24;
	double m20 = a31;  double m21 = a32;  double m22 = a33;  double m23 = a34;
	double m30 = a41;  double m31 = a42;  double m32 = a43;  double m33 = a44;

	double result =
		m03 * m12 * m21 * m30 - m02 * m13 * m21 * m30 - m03 * m11 * m22 * m30 + m01 * m13 * m22 * m30 +
		m02 * m11 * m23 * m30 - m01 * m12 * m23 * m30 - m03 * m12 * m20 * m31 + m02 * m13 * m20 * m31 +
		m03 * m10 * m22 * m31 - m00 * m13 * m22 * m31 - m02 * m10 * m23 * m31 + m00 * m12 * m23 * m31 +
		m03 * m11 * m20 * m32 - m01 * m13 * m20 * m32 - m03 * m10 * m21 * m32 + m00 * m13 * m21 * m32 +
		m01 * m10 * m23 * m32 - m00 * m11 * m23 * m32 - m02 * m11 * m20 * m33 + m01 * m12 * m20 * m33 +
		m02 * m10 * m21 * m33 - m00 * m12 * m21 * m33 - m01 * m10 * m22 * m33 + m00 * m11 * m22 * m33;
	return result;
}

bool Mat4::Inversed()
{
	float a0 = a11 * a22 - a12 * a21;
	float a1 = a11 * a23 - a13 * a21;
	float a2 = a11 * a24 - a14 * a21;
	float a3 = a12 * a23 - a13 * a22;
	float a4 = a12 * a24 - a14 * a22;
	float a5 = a13 * a24 - a14 * a23;
	float b0 = a31 * a42 - a32 * a41;
	float b1 = a31 * a43 - a33 * a41;
	float b2 = a31 * a44 - a34 * a41;
	float b3 = a32 * a43 - a33 * a42;
	float b4 = a32 * a44 - a34 * a42;
	float b5 = a33 * a44 - a34 * a43;

	// Calculate the determinant.
	float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

	// Close to zero, can't invert.
	if (std::abs(det) <= M_EPSILON)
		return false;

	// Support the case where m == dst.
	Mat4 inverse;
	inverse.a11 = a22 * b5 - a23 * b4 + a24 * b3;
	inverse.a12 = -a12 * b5 + a13 * b4 - a14 * b3;
	inverse.a13 = a42 * a5 - a43 * a4 + a44 * a3;
	inverse.a14 = -a32 * a5 + a33 * a4 - a34 * a3;

	inverse.a21 = -a21 * b5 + a23 * b2 - a24 * b1;
	inverse.a22 = a11 * b5 - a13 * b2 + a14 * b1;
	inverse.a23 = -a41 * a5 + a43 * a2 - a44 * a1;
	inverse.a24 = a31 * a5 - a33 * a2 + a34 * a1;

	inverse.a31 = a21 * b4 - a22 * b2 + a24 * b0;
	inverse.a32 = -a11 * b4 + a12 * b2 - a14 * b0;
	inverse.a33 = a41 * a4 - a42 * a2 + a44 * a0;
	inverse.a34 = -a31 * a4 + a32 * a2 - a34 * a0;

	inverse.a41 = -a21 * b3 + a22 * b1 - a23 * b0;
	inverse.a42 = a11 * b3 - a12 * b1 + a13 * b0;
	inverse.a43 = -a41 * a3 + a42 * a1 - a43 * a0;
	inverse.a44 = a31 * a3 - a32 * a1 + a33 * a0;

    det = 1.0 / det;
    for (int i = 0; i < 16; i++)
        m[i] = inverse.m[i] * det;

    return true;
}

Mat4 Mat4::GetInversed() const
{
	float tmp[12];	//临时储存
	float Src[16];	//源矩阵的转制矩阵

	//转换矩阵 行转换到序
	for (uint i = 0; i < 4; i++)
	{
		Src[i] = m[i * 4];
		Src[i + 4] = m[i * 4 + 1];
		Src[i + 8] = m[i * 4 + 2];
		Src[i + 12] = m[i * 4 + 3];
	}
	//计算逆矩阵的头8个配对数值
	tmp[0] = Src[10] * Src[15];
	tmp[1] = Src[11] * Src[14];
	tmp[2] = Src[9] * Src[15];
	tmp[3] = Src[11] * Src[13];
	tmp[4] = Src[9] * Src[14];
	tmp[5] = Src[10] * Src[13];
	tmp[6] = Src[8] * Src[15];
	tmp[7] = Src[11] * Src[12];
	tmp[8] = Src[8] * Src[14];
	tmp[9] = Src[10] * Src[12];
	tmp[10] = Src[8] * Src[13];
	tmp[11] = Src[9] * Src[12];

	//计算头8位逆矩阵
	Mat4 Rlt;
	Rlt.m[0] = tmp[0] * Src[5] + tmp[3] * Src[6] + tmp[4] * Src[7];
	Rlt.m[0] -= tmp[1] * Src[5] + tmp[2] * Src[6] + tmp[5] * Src[7];
	Rlt.m[1] = tmp[1] * Src[4] + tmp[6] * Src[6] + tmp[9] * Src[7];
	Rlt.m[1] -= tmp[0] * Src[4] + tmp[7] * Src[6] + tmp[8] * Src[7];
	Rlt.m[2] = tmp[2] * Src[4] + tmp[7] * Src[5] + tmp[10] * Src[7];
	Rlt.m[2] -= tmp[3] * Src[4] + tmp[6] * Src[5] + tmp[11] * Src[7];
	Rlt.m[3] = tmp[5] * Src[4] + tmp[8] * Src[5] + tmp[11] * Src[6];
	Rlt.m[3] -= tmp[4] * Src[4] + tmp[9] * Src[5] + tmp[10] * Src[6];
	Rlt.m[4] = tmp[1] * Src[1] + tmp[2] * Src[2] + tmp[5] * Src[3];
	Rlt.m[4] -= tmp[0] * Src[1] + tmp[3] * Src[2] + tmp[4] * Src[3];
	Rlt.m[5] = tmp[0] * Src[0] + tmp[7] * Src[2] + tmp[8] * Src[3];
	Rlt.m[5] -= tmp[1] * Src[0] + tmp[6] * Src[2] + tmp[9] * Src[3];
	Rlt.m[6] = tmp[3] * Src[0] + tmp[6] * Src[1] + tmp[11] * Src[3];
	Rlt.m[6] -= tmp[2] * Src[0] + tmp[7] * Src[1] + tmp[10] * Src[3];
	Rlt.m[7] = tmp[4] * Src[0] + tmp[9] * Src[1] + tmp[10] * Src[2];
	Rlt.m[7] -= tmp[5] * Src[0] + tmp[8] * Src[1] + tmp[11] * Src[2];

	/* calculate pairs for second 8 elements (cofactors) */
	tmp[0] = Src[2] * Src[7];
	tmp[1] = Src[3] * Src[6];
	tmp[2] = Src[1] * Src[7];
	tmp[3] = Src[3] * Src[5];
	tmp[4] = Src[1] * Src[6];
	tmp[5] = Src[2] * Src[5];
	tmp[6] = Src[0] * Src[7];
	tmp[7] = Src[3] * Src[4];
	tmp[8] = Src[0] * Src[6];
	tmp[9] = Src[2] * Src[4];
	tmp[10] = Src[0] * Src[5];
	tmp[11] = Src[1] * Src[4];
	/* calculate second 8 elements (cofactors) */
	Rlt.m[8] = tmp[0] * Src[13] + tmp[3] * Src[14] + tmp[4] * Src[15];
	Rlt.m[8] -= tmp[1] * Src[13] + tmp[2] * Src[14] + tmp[5] * Src[15];
	Rlt.m[9] = tmp[1] * Src[12] + tmp[6] * Src[14] + tmp[9] * Src[15];
	Rlt.m[9] -= tmp[0] * Src[12] + tmp[7] * Src[14] + tmp[8] * Src[15];
	Rlt.m[10] = tmp[2] * Src[12] + tmp[7] * Src[13] + tmp[10] * Src[15];
	Rlt.m[10] -= tmp[3] * Src[12] + tmp[6] * Src[13] + tmp[11] * Src[15];
	Rlt.m[11] = tmp[5] * Src[12] + tmp[8] * Src[13] + tmp[11] * Src[14];
	Rlt.m[11] -= tmp[4] * Src[12] + tmp[9] * Src[13] + tmp[10] * Src[14];
	Rlt.m[12] = tmp[2] * Src[10] + tmp[5] * Src[11] + tmp[1] * Src[9];
	Rlt.m[12] -= tmp[4] * Src[11] + tmp[0] * Src[9] + tmp[3] * Src[10];
	Rlt.m[13] = tmp[8] * Src[11] + tmp[0] * Src[8] + tmp[7] * Src[10];
	Rlt.m[13] -= tmp[6] * Src[10] + tmp[9] * Src[11] + tmp[1] * Src[8];
	Rlt.m[14] = tmp[6] * Src[9] + tmp[11] * Src[11] + tmp[3] * Src[8];
	Rlt.m[14] -= tmp[10] * Src[11] + tmp[2] * Src[8] + tmp[7] * Src[9];
	Rlt.m[15] = tmp[10] * Src[10] + tmp[4] * Src[8] + tmp[9] * Src[9];
	Rlt.m[15] -= tmp[8] * Src[9] + tmp[11] * Src[10] + tmp[5] * Src[8];
	/* calculate determinant */
	float det = Src[0] * Rlt.m[0] + Src[1] * Rlt.m[1] + Src[2] * Rlt.m[2] + Src[3] * Rlt.m[3];
	if (false == Math::Equals(det, 0.0f))
	{
		return Rlt / det;
	}
	return Mat4::IDENTITY;
}

void Mat4::GetScale(Vec3* scale) const
{
	scale->x = Math::Sqrt(a11 * a11 + a12 * a12 + a13 * a13);
	scale->y = Math::Sqrt(a21 * a21 + a22 * a22 + a23 * a23);
	scale->z = Math::Sqrt(a31 * a31 + a32 * a32 + a33 * a33);
}

bool Mat4::Decompose(Vec3* scale, Quaternion* rotation, Vec3* translation) const
{
	scale->x = Math::Invsqrt(this->a11 * this->a11 + this->a12 * this->a12 + this->a13 * this->a13);
	scale->y = Math::Invsqrt(this->a21 * this->a21 + this->a22 * this->a22 + this->a23 * this->a23);
	scale->z = Math::Invsqrt(this->a31 * this->a31 + this->a32 * this->a32 + this->a33 * this->a33);

	translation->Set(this->a41, this->a42, this->a43);

	Mat4 rot_mat;
	rot_mat.a11 = this->a11 * scale->x;
	rot_mat.a12 = this->a12 * scale->x;
	rot_mat.a13 = this->a13 * scale->x;
	rot_mat.a14 = 0;
	rot_mat.a21 = this->a21 * scale->y;
	rot_mat.a22 = this->a22 * scale->y;
	rot_mat.a23 = this->a23 * scale->y;
	rot_mat.a24 = 0;
	rot_mat.a31 = this->a31 * scale->z;
	rot_mat.a32 = this->a32 * scale->z;
	rot_mat.a33 = this->a33 * scale->z;
	rot_mat.a34 = 0;
	rot_mat.a41 = 0;
	rot_mat.a42 = 0;
	rot_mat.a43 = 0;
	rot_mat.a44 = 1;
	*rotation = rot_mat.ToQuaternion();

	scale->x = float(1.0) / scale->x;
	scale->y = float(1.0) / scale->y;
	scale->z = float(1.0) / scale->z;

	return true;
}

Quaternion Mat4::ToQuaternion() const
{
	Quaternion qut;
	float s;
	float const tr(a11 + a22 + a33);

	// check the diagonal
	if (tr > 0)
	{
		s = Math::Sqrt(tr + 1);
		qut.w = s * float(0.5);
		s = float(0.5) / s;
		qut.x = (a23 - a32) * s;
		qut.y = (a31 - a13) * s;
		qut.z = (a12 - a21) * s;
	}
	else
	{
		if ((a22 > a11) && (a33 <= a22))
		{
			s = Math::Sqrt((a22 - (a33 + a11)) + 1);

			qut.y = s * float(0.5);

			if (!Math::Equals<float>(s, 0))
			{
				s = float(0.5) / s;
			}

			qut.w = (a31 - a13) * s;
			qut.z = (a32 + a23) * s;
			qut.x = (a12 + a21) * s;
		}
		else
		{
			if (((a22 <= a11) && (a33 > a11)) || (a33 > a22))
			{
				s = Math::Sqrt((a33 - (a11 + a22)) + 1);

				qut.z = s * float(0.5);

				if (!Math::Equals<float>(s, 0))
				{
					s = float(0.5) / s;
				}

				qut.w = (a12 - a21) * s;
				qut.x = (a13 + a31) * s;
				qut.y = (a23 + a32) * s;
			}
			else
			{
				s = Math::Sqrt((a11 - (a22 + a33)) + 1);

				qut.x = s * float(0.5);

				if (!Math::Equals<float>(s, 0))
				{
					s = float(0.5) / s;
				}

				qut.w = (a23 - a32) * s;
				qut.y = (a21 + a12) * s;
				qut.z = (a31 + a13) * s;
			}
		}
	}
	qut.Normalize();
	return qut;
}

void Mat4::GetTranslation(Vec3* translation) const
{
	translation->x = a41;
	translation->y = a42;
	translation->z = a43;
}

void Mat4::Rotate(const Quaternion& q)
{

}

void Mat4::Rotate(const Vec3& axis, float angle)
{
	float s = sinf(angle),
		c = cosf(angle),
		xx,
		yy,
		zz,
		xy,
		yz,
		zx,
		xs,
		ys,
		zs,
		c1;
    Mat4 mat;
	const Vec3& t = axis;
	if (!angle || !t.Length()) return;
	xx = t.x * t.x;
	yy = t.y * t.y;
	zz = t.z * t.z;
	xy = t.x * t.y;
	yz = t.y * t.z;
	zx = t.z * t.x;
	xs = t.x * s;
	ys = t.y * s;
	zs = t.z * s;
	c1 = 1.0f - c;

	mat.a11 = (c1 * xx) + c;
	mat.a12 = (c1 * xy) - zs;
	mat.a13 = (c1 * zx) + ys;

	mat.a21 = (c1 * xy) + zs;
	mat.a22 = (c1 * yy) + c;
	mat.a23 = (c1 * yz) - xs;

	mat.a31 = (c1 * zx) - ys;
	mat.a32 = (c1 * yz) + xs;
	mat.a33 = (c1 * zz) + c;
	*this *= mat;
}

void Mat4::RotateX(float angle)
{
    memcpy(this, &IDENTITY, MATRIX_SIZE);

    float c = std::cos(angle);
    float s = std::sin(angle);

    this->a22  = c;
    this->a23  = -s;
    this->a32  = s;
    this->a33 = c;
}

void Mat4::RotateY(float angle)
{
    memcpy(this, &IDENTITY, MATRIX_SIZE);

    float c = std::cos(angle);
    float s = std::sin(angle);

    this->a11  = c;
    this->a13  = s;
    this->a31  = -s;
    this->a33 = c;
}

void Mat4::RotateZ(float angle)
{
    memcpy(this, &IDENTITY, MATRIX_SIZE);

    float c = std::cos(angle);
    float s = std::sin(angle);

    this->a11  = c;
    this->a12  = -s;
    this->a21  = s;
    this->a22 = c;
}

void Mat4::Scale(float value)
{
	a11 *= value;
	a12 *= value;
	a13 *= value;
	a14 *= value;
	a21 *= value;
	a22 *= value;
	a23 *= value;
	a24 *= value;
	a31 *= value;
	a32 *= value;
	a33 *= value;
	a34 *= value;
}

void Mat4::Scale(const Vec3& s)
{
	a11 *= s.x;
	a12 *= s.x;
	a13 *= s.x;
	a14 *= s.x;
	a21 *= s.y;
	a22 *= s.y;
	a23 *= s.y;
	a24 *= s.y;
	a31 *= s.z;
	a32 *= s.z;
	a33 *= s.z;
	a34 *= s.z;
}

void Mat4::Scale(float xScale, float yScale, float zScale)
{
	a11 *= xScale;
	a12 *= xScale;
	a13 *= xScale;
	a14 *= xScale;
	a21 *= yScale;
	a22 *= yScale;
	a23 *= yScale;
	a24 *= yScale;
	a31 *= zScale;
	a32 *= zScale;
	a33 *= zScale;
	a34 *= zScale;
}

void Mat4::Translate(float x, float y, float z)
{
    a41 += x;
    a42 += y;
    a43 += z;
}

void Mat4::Translate(const Vec3& t)
{
    a41 += t.x;
    a42 += t.y;
    a43 += t.z;
}

void Mat4::Transpose()
{
	std::swap(a12, a21);
	std::swap(a13, a31);
	std::swap(a14, a41);
	std::swap(a23, a32);
	std::swap(a24, a42);
	std::swap(a34, a43);
}

Mat4 Mat4::GetTransposed() const
{
	return Mat4(a11, a21, a31, a41,
                a12, a22, a32, a42, 
                a13, a23, a33, a43, 
                a14, a24, a34, a44);
}

std::string Mat4::toString() const
{
    char tempBuffer[1024];
    sprintf(tempBuffer, "%g %g %g %g, %g %g %g %g, %g %g %g %g, %g %g %g %g", 
		a11, a12, a13, a14, a21, a22, a23, a24, a31, a32, a33, a34, a41, a42, a43, a44);
    return std::string(tempBuffer);
}

NS_JYE_MATH_END