#pragma once

#include "../../Core.h"

NS_JYE_BEGIN

namespace Serialize
{
	enum DataType
	{
		DT_NULL = 0,
		DT_BOOL,
		DT_SHORT,
		DT_USHORT,
		DT_INT,
		DT_UINT,
		DT_FLOAT,
		DT_DOUBLE,
		DT_STRING,
		DT_NATIVE,
		DT_CLASS,
		DT_INT64,
		DT_UINT64,
		DT_BUFFER,
		DT_FLOAT_ARRAY,
		DT_VOID,
		DT_NATIVEPTR,
		DT_OBJECTPTR,
		DT_WEAKREF,
		DT_DYNAMIC,
	};
}

NS_JYE_END