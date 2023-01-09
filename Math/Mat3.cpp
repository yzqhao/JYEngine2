#include "Mat3.h"

NS_JYE_MATH_BEGIN

#define MATRIX_SIZE (sizeof(float) * 9)

const Mat3 Mat3::ZERO(0.0f, 0.0f, 0.0f, 
                      0.0f, 0.0f, 0.0f,
                      0.0f, 0.0f, 0.0f);
const Mat3 Mat3::IDENTITY(1.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f,
                          0.0f, 0.0f, 1.0f);

Mat3::Mat3()
{
    *this = IDENTITY;
}

Mat3::Mat3(float m11, float m12, float m13, float m21, float m22, float m23, 
           float m31, float m32, float m33)
{
    a11 = m11;
    a12 = m12;
    a13 = m13;
    a21 = m21;
    a22 = m22;
    a23 = m23;
    a31 = m31;
    a32 = m32;
    a33 = m33;
}

Mat3::Mat3(const float* mat)
{
    JY_ASSERT(mat);
    memcpy(this->m, mat, MATRIX_SIZE);
}

Mat3::Mat3(const Mat3& copy)
{
    memcpy(m, copy.m, MATRIX_SIZE);
}

Mat3::~Mat3()
{
}

Mat3& Mat3::Transpose()
{
	Mat3 res = *this;
	std::swap(res.a12, res.a21);
	std::swap(res.a13, res.a31);
	std::swap(res.a23, res.a32);
	return res;
}

bool Mat3::IsIdentity() const
{
    return memcmp(m, &IDENTITY, MATRIX_SIZE) == 0;
}

void Mat3::SetIdentity()
{
    *this = IDENTITY;
}

void Mat3::multiply(const Mat3& mat)
{
    multiply(*this, mat, this);
}

void Mat3::multiply(const Mat3& m1, const Mat3& m2, Mat3* dst)
{
	dst->a11 = m1.a11 * m2.a11 + m1.a12 * m2.a21 + m1.a13 * m2.a31;
	dst->a12 = m1.a11 * m2.a12 + m1.a12 * m2.a22 + m1.a13 * m2.a32;
	dst->a13 = m1.a11 * m2.a13 + m1.a12 * m2.a23 + m1.a13 * m2.a33;
	dst->a21 = m1.a21 * m2.a11 + m1.a22 * m2.a21 + m1.a23 * m2.a31;
	dst->a22 = m1.a21 * m2.a12 + m1.a22 * m2.a22 + m1.a23 * m2.a32;
	dst->a23 = m1.a21 * m2.a13 + m1.a22 * m2.a23 + m1.a23 * m2.a33;
	dst->a31 = m1.a31 * m2.a11 + m1.a32 * m2.a21 + m1.a33 * m2.a31;
	dst->a32 = m1.a31 * m2.a12 + m1.a32 * m2.a22 + m1.a33 * m2.a32;
	dst->a33 = m1.a31 * m2.a13 + m1.a32 * m2.a23 + m1.a33 * m2.a33;
}

float Mat3::Determinant() const
{
    return a11*a22*a33 + a12*a23*a31 + a13*a21*a32 - 
           a13*a22*a31 - m[0]*a23*a32 - a12*a21*a33;
}

Mat3 Mat3::GetInversed() const
{
    Mat3 res;
    float m11 = m[0];
    float m12 = a12;
    float m13 = a13;
    float m21 = a21;
    float m22 = a22;
    float m23 = a23;
    float m31 = a31;
    float m32 = a32;
    float m33 = a33;
    res.m[0] = m22 * m33 - m32 * m23;
    res.a12 = m32 * m13 - m12 * m33;
    res.a13 = m12 * m23 - m22 * m13;
    res.a21 = m31 * m23 - m21 * m33;
    res.a22 = m11 * m33 - m31 * m13;
    res.a23 = m21 * m13 - m11 * m23;
    res.a31 = m21 * m32 - m31 * m22;
    res.a32 = m31 * m12 - m11 * m32;
    res.a33 = m11 * m22 - m21 * m12;

    int det = Determinant();
    int inverseDet = 1.0 / det;
    for (int i = 0; i < 9; i++)
    {
        res.m[i] *= inverseDet;
    }
    
    return res;
}

std::string Mat3::toString() const
{
    char tempBuffer[256];
    sprintf(tempBuffer, "%g %g %g %g %g %g %g %g %g", 
        m[0], a12, a13, a21, a22, a23, a31, a32, a33);
    return std::string(tempBuffer);
}

void Mat3::Scale(float value)
{
	a11 *= value;
	a12 *= value;
	a13 *= value;

	a21 *= value;
	a22 *= value;
	a23 *= value;

	a31 *= value;
	a32 *= value;
	a33 *= value;
}

void Mat3::Scale(const Vec3& s)
{
	a11 *= s.x;
	a12 *= s.x;
	a13 *= s.x;

	a21 *= s.y;
	a22 *= s.y;
	a23 *= s.y;

	a31 *= s.z;
	a32 *= s.z;
	a33 *= s.z;
}

NS_JYE_MATH_END