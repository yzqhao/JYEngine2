
#pragma once

#include "private/Define.h"

NS_JYE_BEGIN

class MProjectOptions
{
public:
	MProjectOptions();
	~MProjectOptions();

	void Deserialize();
	void Serialize(const String& projPath);

	const String& GetProjectPath() const { return m_ProjectPath; }

private:
	String m_ProjectPath;
};

NS_JYE_END