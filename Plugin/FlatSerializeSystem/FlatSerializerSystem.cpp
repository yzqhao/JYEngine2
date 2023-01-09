
#include "FlatSerializerSystem.h"
#include "FlatSerializer.h"


NS_JYE_BEGIN

FlatSerializerSystem::FlatSerializerSystem()
{
	
}

FlatSerializerSystem::~FlatSerializerSystem()
{
}

ISerializer* JYE::FlatSerializerSystem::SerializerFactory()
{
	return _NEW FlatSerializer;
}

void FlatSerializerSystem::RecycleBin(ISerializer* ser)
{
	SAFE_DELETE(ser);
}

NS_JYE_END
