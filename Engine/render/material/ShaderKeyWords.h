#pragma once

#include "Engine/private/Define.h"
#include "KeyWordUtil.h"

#include <bitset>

NS_JYE_BEGIN

class ENGINE_API ShaderKeyWords
{
public:
	ShaderKeyWords()
	{
		Reset();
	}
	ShaderKeyWords(const ShaderKeyWords& o)
	{
		this->m_Bits = o.m_Bits;
	}
	void EnableStrArray(Vector<String>& keys);
	inline void Enable(const String& key)
	{
		ShaderKey shaderKey = KeyWordUtil::Create(key.c_str());
		Enable(shaderKey);
	}
	inline void Disable(const String& key)
	{
		ShaderKey shaderKey = KeyWordUtil::Create(key.c_str());
		Disable(shaderKey);
	}
	inline bool IsEnabled(const String& key) const
	{
		ShaderKey shaderKey = KeyWordUtil::Create(key.c_str());
		return IsEnabled(shaderKey);
	}
	inline void Reset()
	{
		m_Bits.reset();
	}
	inline void Enable(ShaderKey key)
	{
		m_Bits.set(key);
	}
	inline void Disable(ShaderKey key)
	{
		m_Bits.reset(key);
	}
	inline bool IsEnabled(ShaderKey key) const
	{
		return m_Bits.test(key);
	}
	inline bool IsEmpty()
	{
		return m_Bits.none();
	}

	inline bool operator== (const ShaderKeyWords& o) const
	{
		return m_Bits == o.m_Bits;
	}

	inline bool operator!= (const ShaderKeyWords& o) const
	{
		return m_Bits != o.m_Bits;
	}

	inline const ShaderKeyWords& operator|= (const ShaderKeyWords& o)
	{
		m_Bits |= o.m_Bits;
		return *this;
	}

	inline const ShaderKeyWords& operator&= (const ShaderKeyWords& o)
	{
		m_Bits &= o.m_Bits;
		return *this;
	}

	inline const ShaderKeyWords& operator^= (const ShaderKeyWords& o)
	{
		m_Bits ^= o.m_Bits;
		return *this;
	}

	inline ShaderKeyWords operator| (const ShaderKeyWords& o) const
	{
		ShaderKeyWords c = *this;
		c.m_Bits |= o.m_Bits;
		return c;
	}

	inline ShaderKeyWords operator& (const ShaderKeyWords& o) const
	{
		ShaderKeyWords c = *this;
		c.m_Bits &= o.m_Bits;
		return c;
	}

	inline ShaderKeyWords operator^ (const ShaderKeyWords& o) const
	{
		ShaderKeyWords c = *this;
		c.m_Bits ^= o.m_Bits;
		return c;
	}

	inline ShaderKeyWords operator~() const
	{
		ShaderKeyWords c = *this;
		c.m_Bits = ~c.m_Bits;
		return c;
	}

	inline int BitEnableCount() const
	{
		return m_Bits.count();
	}

	inline unsigned GetHashCode() const
	{
		std::hash<std::bitset<MaxKeyWords>> hash_fn;
		return hash_fn(m_Bits);
	}

	static int ComputeMatchScore(const ShaderKeyWords& materialMask, const ShaderKeyWords& globalMask);

private:
	std::bitset<MaxKeyWords> m_Bits;
};

NS_JYE_END
