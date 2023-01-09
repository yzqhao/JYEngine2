
#pragma once

#include "JsonParserUtil.h"

namespace JsonParserUtil
{
	double get_double_number(const sajson::value & value)
	{
		switch (value.get_type())
		{
		case sajson::TYPE_INTEGER:
			return (double)value.get_integer_value();
		case sajson::TYPE_DOUBLE:
			return value.get_double_value();
		default:
			return 0;
		}
	}

	int get_integer_number(const sajson::value & value)
	{
		switch (value.get_type())
		{
		case sajson::TYPE_INTEGER:
			return value.get_integer_value();
		case sajson::TYPE_DOUBLE:
			return (int) value.get_double_value();
		default:
			return 0;
		}
	}

	sajson::document ParseJson(std::string & text)
	{
		//Trim Trailing Zeros
		size_t trueLength = text.size();
		while (trueLength > 0 && text[trueLength - 1] == '\0')
		{
			--trueLength;
		}
		return sajson::parse(
			sajson::dynamic_allocation(),
			sajson::string(const_cast<char *>(text.data()), trueLength));
	}
}
