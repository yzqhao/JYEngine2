
#pragma once

#include "Vec3.h"
#include "Vec4.h"

NS_JYE_MATH_BEGIN

enum class PointSide
{
    IN_PLANE,
    FRONT_PLANE,
    BEHIND_PLANE,
};

class MATH_API Plane
{
public:     // 平面公式：Ax + By + Cz + D = 0;
	union
	{
		struct
		{
			float m_a, m_b, m_c;
		};
        Vec3 m_normal;   // (A, B, C)
	};
    float m_d;       // D
public:
    Plane() : m_normal(0, 0, 0), m_d(0) {}
	Plane(const Plane& rhs)
		: m_a(rhs.m_a),
		m_b(rhs.m_b),
		m_c(rhs.m_c),
		m_d(rhs.m_d)
	{
	}
    Plane(const Vec3& normal, float d) : m_normal(normal), m_d(d) {}
    Plane(const Vec3& normal, const Vec3& p) { Set(normal, p); }
    Plane(const Vec3& p0, const Vec3& p1, const Vec3& p2) { Set(p0, p1, p2); }

	Plane& operator=(const Plane& rhs)
	{
		m_a = rhs.m_a;
		m_b = rhs.m_b;
		m_c = rhs.m_c;
		m_d = rhs.m_d;
		return *this;
	}
    Plane& operator = (const Vec4& rhs)
    {
        m_normal = Vec3(rhs.x, rhs.y, rhs.z);
        m_d = rhs.w;
        return *this;
    }

	float A() const { return m_normal.x; }
	float B() const { return m_normal.y; }
	float C() const { return m_normal.z; }
	float D() const { return m_d; }

    void Set(const Vec3& v0, const Vec3& v1, const Vec3& v2)
    {
        Vec3 dist1 = v1 - v0;
        Vec3 dist2 = v2 - v0;

        Set(CrossProduct(dist1, dist2), v0);
    }

    void Set(const Vec3& normal, const Vec3& point)
    {
        m_normal = normal.GetNormalized();
        m_d = -Math::Dot(m_normal, point);
	}

	void SetNormalDistance(const Vec3& normal, float distance)
	{
        m_normal = normal;
        m_d = distance;
	}

    float Dot(const Vec3& v) const
    {
        return (A() * v.x + B() * v.y + C() * v.z + D());
    }

	bool IntersectSegment(const Vec3& p1, const Vec3& p2, Vec3& result)
	{
		Vec3 subV = p2 - p1;
		float vdot = m_a * subV.x + m_b * subV.y + m_c * subV.z;
		if (Math::Equals(vdot, float(0)))
		{
			return false;
		}

		float ndot = -(p1.x * m_a + p1.y * m_b + p1.z * m_c) - D();
		float factor = ndot / vdot;
		if (factor < float(0) || factor > float(1))
		{
			return false;
		}

		result = p1 + subV * factor;
		return true;
	}

	bool ClipPoly(const Vec3* input, unsigned char inputNum, Vec3* output, unsigned char* outputNum,
		Vec3* interset, unsigned char* intersetNum)
	{
		if (inputNum < 3)
		{
			return false;
		}

		unsigned char curOutPoints = 0;
		unsigned char& curIntectPoints = *intersetNum;
		Vec3 tmp;
		bool* outside = new bool[inputNum];
		int i = 0;
		for (i = 0; i < inputNum; i++)
		{
			Math::Vec3 curPoint = input[i];
			float dotV = curPoint.x * m_a + curPoint.y * m_b + curPoint.z * m_c + m_d;
			outside[i] = (dotV < float(0));
		}

		for (i = 0; i < inputNum; i++)
		{
			int next = (i + 1) % inputNum;

			if (outside[i] && outside[next])
			{
				continue;
			}

			if (outside[i])
			{
				if (IntersectSegment(input[i], input[next], tmp))
				{
					output[curOutPoints++] = tmp;
					interset[curIntectPoints++] = tmp;
				}

				output[curOutPoints++] = input[next];
				continue;
			}

			if (outside[next])
			{
				if (IntersectSegment(input[i], input[next], tmp))
				{
					output[curOutPoints++] = tmp;
					interset[curIntectPoints++] = tmp;
				}

				continue;
			}

			output[curOutPoints++] = input[next];
		}

		delete[] outside;

		*outputNum = curOutPoints;
		return curOutPoints ? true : false;
	}

    void Normalize();

    // 点到平面的距离
    float Dist2Plane(const Vec3& p) const;

    // 点与平面的关系
    PointSide GetSide(const Vec3& point) const;
};

NS_JYE_MATH_END
