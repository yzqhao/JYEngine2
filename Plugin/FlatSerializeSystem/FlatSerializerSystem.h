
#pragma once

#include "Core/Interface/Serialize/ISerializeSystem.h"

NS_JYE_BEGIN

class FlatSerializerSystem : public ISerializeSystem
{
public:
    explicit FlatSerializerSystem();
    ~FlatSerializerSystem();

	virtual ISerializer* SerializerFactory() override;
	virtual void RecycleBin(ISerializer* ser) override;
};

NS_JYE_END
