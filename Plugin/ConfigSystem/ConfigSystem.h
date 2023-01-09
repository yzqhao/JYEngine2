
#pragma once

#include "private/Define.h"
#include "JsonConfig.h"
#include "Core/Interface/IConfigSystem.h"

NS_JYE_BEGIN

class ConfigSystem : public IConfigSystem
{
public:
    explicit ConfigSystem();
    ~ConfigSystem();

	virtual IConfig* ConfigureFactory(const String& path);
	virtual void RecycleBin(IConfig* cf);
	virtual void SetDefaultConfigure(const String& path);
	virtual IConfig* GetDefaultConfigure();
    
private:
	IConfig* m_pDefaultConfigure;
};

NS_JYE_END
