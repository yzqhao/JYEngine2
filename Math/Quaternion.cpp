#include "Quaternion.h"

NS_JYE_MATH_BEGIN

const Quaternion Quaternion::IDENTITY(0.0f, 0.0f, 0.0, 1.0f);
const Quaternion Quaternion::ZERO(0.0f, 0.0f, 0.0, 0.0f);

Quaternion::Quaternion(float nx, float ny, float nz, float nw)
{
	float mag = nx*nx + ny*ny + nz*nz + nw*nw;
	x = nx / mag;
	y = ny / mag;
	z = nz / mag;
	w = nw / mag;
}

Quaternion::Quaternion(const Vec3& angles)
{
	FromYawPitchRoll(angles.y, angles.x, angles.z);
}

Quaternion::Quaternion(const Vec3& axis, float angle)
{
	FromAngleAxis(axis, angle);
}

Quaternion::Quaternion(const Vec3& start, const Vec3& end)
{
	FromRotationTo(start, end);
}

Quaternion::Quaternion(const Mat3& matrix)
{
	FromMatrix3(matrix);
}

void Quaternion::FromYawPitchRoll(float yaw, float pitch, float roll)
{
	float const angX(pitch * 0.5), angY(yaw * 0.5), angZ(roll * 0.5);

	float scx[2], scy[2], scz[2];
	SinCos(angX, scx);
	SinCos(angY, scy);
	SinCos(angZ, scz);
	*this = Quaternion(
		scx[0] * scy[1] * scz[1] + scx[1] * scy[0] * scz[0],
		scx[1] * scy[0] * scz[1] - scx[0] * scy[1] * scz[0],
		scx[1] * scy[1] * scz[0] - scx[0] * scy[0] * scz[1],
		scx[0] * scy[0] * scz[0] + scx[1] * scy[1] * scz[1]);
}

void Quaternion::FromAngleAxis(const Vec3& axis, float angle)
{
	float sc[2];
	SinCos(angle * float(0.5), sc);
	if (Math::Equals(axis.Length(), float(0.0)))
	{
		x = sc[0];
		y = sc[0];
		z = sc[0];
		w = sc[1];
	}
	else
	{
		Vec3 temp = axis;
		temp.Normalize();
		temp *= sc[0];
		x = temp.x;
		y = temp.y;
		z = temp.z;
		w = sc[1];
	}
}

void Quaternion::FromRotationTo(const Vec3& from, const Vec3& to)
{
	if (Math::Equals(from.Length(), 0.0f) || Math::Equals(to.Length(), 0.0f))
	{
		return;
	}

	Vec3 a = from;
	Vec3 b = to;
	a.Normalize();
	b.Normalize();
	float cos_theta = a.Dot(b);
	cos_theta = std::min(cos_theta, float(1));
	cos_theta = std::max(cos_theta, float(-1));
	if (Math::Equals(  cos_theta, float(1)))
	{
		*this = Quaternion(0, 0, 0, 1);
	}
	else
	{
		if (Math::Equals(cos_theta, float(-1)))
		{
			*this = Quaternion(1, 0, 0, 0);
		}
		else
		{
			Vec3 axis = a.Cross(b).GetNormalized();

			float const sin_theta = Math::Sqrt(1 - cos_theta * cos_theta);
			float const sin_half_theta = Math::Sqrt((1 - cos_theta) / 2);
			float const cos_half_theta = sin_theta / (2 * sin_half_theta);

			*this = Quaternion(axis * sin_half_theta, cos_half_theta);
		}
	}
}

void Quaternion::FromMatrix3(const Mat3& matrix)
{
	float t = matrix.a11 + matrix.a22 + matrix.a33;

	if (t > 0.0f)
	{
		float invS = 0.5f / sqrtf(1.0f + t);

		x = (matrix.a23 - matrix.a32) * invS;
		y = (matrix.a31 - matrix.a13) * invS;
		z = (matrix.a12 - matrix.a21) * invS;
		w = 0.25f / invS;
	}
	else
	{
		if (matrix.a11 > matrix.a22 && matrix.a11 > matrix.a22)
		{
			float invS = 0.5f / sqrtf(1.0f + matrix.a11 - matrix.a22 - matrix.a33);

			x = 0.25f / invS;
			y = (matrix.a21 + matrix.a12) * invS;
			z = (matrix.a13 + matrix.a31) * invS;
			w = (matrix.a23 - matrix.a32) * invS;
		}
		else if (matrix.a22 > matrix.a33)
		{
			float invS = 0.5f / sqrtf(1.0f + matrix.a22 - matrix.a11 - matrix.a33);

			x = (matrix.a21 + matrix.a12) * invS;
			y = 0.25f / invS;
			z = (matrix.a32 + matrix.a23) * invS;
			w = (matrix.a31 - matrix.a13) * invS;
		}
		else
		{
			float invS = 0.5f / sqrtf(1.0f + matrix.a33 - matrix.a11 - matrix.a22);

			x = (matrix.a31 + matrix.a13) * invS;
			y = (matrix.a32 + matrix.a23) * invS;
			z = 0.25f / invS;
			w = (matrix.a12 - matrix.a21) * invS;
		}
	}
}

Quaternion Quaternion::operator *(const Quaternion &a) const 
{
	Quaternion result;

	result.w = w*a.w - x*a.x - y*a.y - z*a.z;
	result.x = w*a.x + x*a.w + z*a.y - y*a.z;
	result.y = w*a.y + y*a.w + x*a.z - z*a.x;
	result.z = w*a.z + z*a.w + y*a.x - x*a.y;

	return result;
}

Quaternion &Quaternion::operator *=(const Quaternion &a) 
{
	*this = *this * a;
	return *this;
}

Vec3 Quaternion::operator*(const Vec3& rhs) const
{
	Vec3 qVec(x, y, z);
	Vec3 cross1(CrossProduct(qVec, rhs));
	Vec3 cross2(CrossProduct(qVec, cross1));

	return rhs + (cross1 * w + cross2) * 2.0f;
}

void Quaternion::Normalize() 
{
	float mag = (float)sqrt(w*w + x*x + y*y + z*z);

	if (mag > 0.0f) {
		float   oneOverMag = 1.0f / mag;
		w *= oneOverMag;
		x *= oneOverMag;
		y *= oneOverMag;
		z *= oneOverMag;
	} else {
		identity();
	}
}

Quaternion Quaternion::conjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::Inverse() const
{
	float const inv(1.0 / (x*x + y*y + z*z + w*w));
	return Quaternion(
		-x * inv,
		-y * inv,
		-z * inv,
		w * inv);
}

Vec3 Quaternion::ToEulerAngle() const
{
	float yaw, pitch, roll;

	float check = 2.0f * (-y * z + w * x);

	if (check < -0.995f)
	{
		pitch = -PI * 0.5f;
		yaw = 0.0f;
		roll = -atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z));
	}
	else if (check > 0.995f)
	{
		pitch = PI * 0.5f;
		yaw = 0.0f;
		roll = atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z));
	}
	else
	{
		pitch = asinf(check);
		yaw = atan2f(2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + y * y));
		roll = atan2f(2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z));
	}


	return Vec3(yaw, pitch, roll);
}

Mat3 Quaternion::ToMatrix33() const
{
	// calculate coefficients
	float const x2(x + x);
	float const y2(y + y);
	float const z2(z + z);

	float const xx2(x* x2), xy2(x* y2), xz2(x* z2);
	float const yy2(y* y2), yz2(y* z2), zz2(z* z2);
	float const wx2(w* x2), wy2(w* y2), wz2(w* z2);

	return Mat3(
		1 - yy2 - zz2, xy2 + wz2, xz2 - wy2,
		xy2 - wz2, 1 - xx2 - zz2, yz2 + wx2,
		xz2 + wy2, yz2 - wx2, 1 - xx2 - yy2);
}

float Quaternion::getRotationAngle() const 
{
	float thetaOver2 = acos(w);

	return thetaOver2 * 2.0f;
}

Vec3 Quaternion::getRotationAxis() const 
{
	// sin^2(x) + cos^2(x) = 1
	float sinThetaOver2Sq = 1.0f - w*w;

	if (sinThetaOver2Sq <= 0.0f) {
		return Vec3(1.0f, 0.0f, 0.0f);		
	}

	// Compute 1 / sin(theta/2)
	float   oneOverSinThetaOver2 = 1.0f / sqrt(sinThetaOver2Sq);

	return Vec3(
		x * oneOverSinThetaOver2,
		y * oneOverSinThetaOver2,
		z * oneOverSinThetaOver2
	);
}

float dotProduct(const Quaternion &a, const Quaternion &b) 
{
	return a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;
}

Quaternion slerp(const Quaternion &q0, const Quaternion &q1, float t) 
{
	if (t <= 0.0f) return q0;
	if (t >= 1.0f) return q1;

	// Compute "cosine of angle between quaternions" using dot product

	float cosOmega = dotProduct(q0, q1);

	// If negative dot, use -q1.  Two quaternions q and -q
	// represent the same rotation, but may produce
	// different slerp.  We chose q or -q to rotate using
	// the acute angle.

	float q1w = q1.w;
	float q1x = q1.x;
	float q1y = q1.y;
	float q1z = q1.z;
	if (cosOmega < 0.0f) {
		q1w = -q1w;
		q1x = -q1x;
		q1y = -q1y;
		q1z = -q1z;
		cosOmega = -cosOmega;
	}

	// We should have two unit quaternions, so dot should be <= 1.0

	assert(cosOmega < 1.1f);

	// Compute interpolation fraction, checking for quaternions
	// almost exactly the same

	float k0, k1;
	if (cosOmega > 0.9999f) {

		// Very close - just use linear interpolation,
		// which will protect againt a divide by zero

		k0 = 1.0f-t;
		k1 = t;

	} else {

		// Compute the sin of the angle using the
		// trig identity sin^2(omega) + cos^2(omega) = 1

		float sinOmega = sqrt(1.0f - cosOmega*cosOmega);

		// Compute the angle from its sin and cosine

		float omega = atan2(sinOmega, cosOmega);

		// Compute inverse of denominator, so we only have
		// to divide once

		float oneOverSinOmega = 1.0f / sinOmega;

		// Compute interpolation parameters

		k0 = sin((1.0f - t) * omega) * oneOverSinOmega;
		k1 = sin(t * omega) * oneOverSinOmega;
	}

	Quaternion result;
	result.x = k0*q0.x + k1*q1x;
	result.y = k0*q0.y + k1*q1y;
	result.z = k0*q0.z + k1*q1z;
	result.w = k0*q0.w + k1*q1w;

	return result;
}

NS_JYE_MATH_END