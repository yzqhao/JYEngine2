#include "ShaderKeyWords.h"

NS_JYE_BEGIN

void ShaderKeyWords::EnableStrArray(Vector<String>& keys)
{
	size_t size = keys.size();
	for (int i = 0; i < size; i++)
	{
		Enable(keys[i]);
	}
}

int ShaderKeyWords::ComputeMatchScore(const ShaderKeyWords& materialMask, const ShaderKeyWords& globalMask)
{
	int matchCount = (materialMask & globalMask).BitEnableCount();
	int misMatchCount = (materialMask & ~globalMask).BitEnableCount();
	int score = matchCount - misMatchCount * 16;

	return score;
}

NS_JYE_END