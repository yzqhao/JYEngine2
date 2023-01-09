
#pragma once

#include "Core/Interface/Serialize/ISerializer.h"
#include "generic_generated.h"

NS_JYE_BEGIN

class FlatEncoder;
class FlatDecoder;

class FlatSerializer : public ISerializer
{
private:
	typedef std::vector<FlatEncoder*> FlatEncoderArray;
	typedef std::vector<FlatDecoder*> FlatDecoderArray;
private:
	flatbuffers::FlatBufferBuilder	m_Builder;
	FlatEncoderArray				m_FlatEncoderArray;
	uint							m_EncoderIndex;
	bool							m_isEncoderFinish;

	FlatDecoderArray				m_FlatDecoderArray;
	uint							m_DecoderIndex;
public:
	FlatSerializer();
	virtual ~FlatSerializer();
private:
	void _EncoderFinish();
public:
	virtual IEncoder* GetEncoder();
	virtual const byte* GetBuffer();
	virtual uint GetBufferSize();
	virtual void ClearEncoder();
	virtual IDecoder* GetDecoderFromBuffer(const byte* buff);
	virtual void ClearDecoder();
public:
	IDecoder* GetDecoder(const vfbs::TClass* c);

};

NS_JYE_END
