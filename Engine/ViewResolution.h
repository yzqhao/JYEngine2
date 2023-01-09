#pragma once

#include "Engine/private/Define.h"
#include "Math/IntVec4.h"
#include "Math/IntVec2.h"

NS_JYE_BEGIN

struct ViewResolution
{
	Math::IntVec4 m_ViewSize;//ÆÁÄ»³ß´ç
	Math::IntVec2 m_Resolution;//·Ö±æÂÊ
	ViewResolution() {}
	ViewResolution(const Math::IntVec2& ns)
		: m_ViewSize(0, 0, ns.x, ns.y)
		, m_Resolution(ns)
	{
	}
};

NS_JYE_END