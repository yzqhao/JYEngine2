#pragma once

#include "RHI/RHIDefine.h"

NS_JYE_BEGIN

class BaseStateType
{
public:
	virtual const std::string& GetName() const = 0;
	// 返回值true 表示数值有变化 false 无变化
	virtual bool SetValue(float fVal) = 0;
};

template<typename T>
class ParserStateType :public BaseStateType
{
public:
	std::string valueName;
	T value;

	ParserStateType(T val)
	{
		this->value = val;
	}

	ParserStateType()
	{

	}

	virtual const std::string& GetName() const
	{
		return valueName;
	}

	virtual bool SetValue(float fVal)
	{
		if ((float)this->value != fVal)
		{
			this->value = (T)(int)fVal;
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual float GetValue() const
	{
		return (float)this->value;
	}
};

typedef ParserStateType<RHIDefine::Boolean> ParserBoolean;
typedef ParserStateType<RHIDefine::Function> ParserFunction;
typedef ParserStateType<RHIDefine::Blend> ParserBlend;
typedef ParserStateType<RHIDefine::Operation> ParserOperation;
typedef ParserStateType<RHIDefine::CullFaceMode> ParserCullFaceMode;
typedef ParserStateType<RHIDefine::RasterizerCullMode> ParserRasterizerCull;
typedef ParserStateType<RHIDefine::ColorMask> ParserColorMask;
typedef ParserStateType<RHIDefine::BitMask> ParserBitMask;
typedef ParserStateType<RHIDefine::Uint> ParserUint;
typedef ParserStateType<RHIDefine::PolygonMode> ParserPolygonMode;

NS_JYE_END