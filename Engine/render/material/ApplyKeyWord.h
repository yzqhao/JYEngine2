#pragma once

#include "ShaderKeyWords.h"

NS_JYE_BEGIN

class ENGINE_API ApplyKeyWord
{
public:
	ApplyKeyWord(ShaderKeyWords* keybindings, const ShaderKeyWords& keys)
	{
		JY_ASSERT(keybindings != NULL);
		m_keybindings = keybindings;
		m_PreKeys = *m_keybindings;
		*m_keybindings = (m_PreKeys | keys);
	}

	~ApplyKeyWord()
	{
		*m_keybindings = m_PreKeys;
	}

private:
	ShaderKeyWords* m_keybindings;
	ShaderKeyWords  m_PreKeys;
};

NS_JYE_END

