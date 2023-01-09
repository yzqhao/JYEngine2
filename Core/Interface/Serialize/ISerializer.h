#pragma once

#include "../../Core.h"
#include "DataType.h"

NS_JYE_BEGIN

class IEncoder;
class IDecoder;

class CORE_API ISerializer
{
public:
	ISerializer() {}
	virtual ~ISerializer() {}
public:
	//���л�
	virtual IEncoder* GetEncoder() = 0;
	virtual const byte* GetBuffer() = 0;
	virtual uint GetBufferSize() = 0;
	virtual void ClearEncoder() = 0;
	//�����л�
	virtual IDecoder* GetDecoderFromBuffer(const byte* buff) = 0;
	virtual void ClearDecoder() = 0;
};

NS_JYE_END