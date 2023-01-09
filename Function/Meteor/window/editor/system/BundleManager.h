
#pragma once

#include "private/Define.h"
#include "Engine/object/Scene.h"

NS_JYE_BEGIN

namespace BundleManager
{
	void SaveScene(const String& file);

	template<typename T>
	T* Deserialize(const String& filepath);
}

template<typename T>
T* BundleManager::Deserialize(const String& filepath)
{
	Stream stream;
	stream.Load(filepath.c_str());
	return static_cast<T*>(stream.GetObjectByRtti(T::RTTI()));
}

NS_JYE_END
