
#pragma once

#include "private/Define.h"
#include "math/3DMath.h"
#include "System/Delegate.h"
#include "../../selectwindow/MSelectWindow.h"
#include "RHI/RHIDefine.h"

NS_JYE_BEGIN

namespace InspectorUtil
{
	DECLARE_DELEGATE_RET(ErrorDelegate, void);

	bool Vector1(float& vec, const String& name, float speed = 1.0, const String& hashname = "", float min = 0.0f, float max = 0.0f, const String& hintname = "");
	bool Vector1_dec(float& vec, const String& name, float speed = 1.0, const String& hashname = "", float min = 0.0f, float max = 0.0f);
	bool IVector1(int& vec, const String& name, float speed = 1.0, const String& hashname = "", int min = 0, int max = 0);
	bool Vector2(Math::Vec2& vec, const String& name, float speed = 1.0, const String& hashname = "", float min = 0.0f, float max = 0.0f, const String& hintname = "");
	bool IVector2(Math::IntVec2& vec, const String& name, float speed = 1.0, const String& hashname = "", int min = 0, int max = 0);
	bool Vector3(Math::Vec3& vec, const String& name, float speed = 1.0, const String& hashname = "", float min = 0.0f, float max = 0.0f, const String& hintname = "");
	bool Vector3_dec(Math::Vec3& vec, const String& name, float speed = 1.0, const String& hashname = "", float min = 0.0f, float max = 0.0f);
	bool EularAngleInspect(Math::Vec3& vec, const String& name, float speed = 1.0, const String& hashname = "");
	bool Vector4(Math::Vec4& vec, const String& name, float speed = 1.0, const String& hashname = "", const String& hintname = "");
	bool Vector4_dec(Math::Vec4& vec, const String& name, float speed = 1.0, const String& hashname = "", float min = 0.0f, float max = 0.0f, const String& hintname = "");
	bool Quaternion(Math::Quaternion& vec, const String& name, float speed = 1.0, const String& hashname = "");
	bool FileSelecter(const String& name, const String& path, const String& btnName, Object* filter, const String& hash, 
		MSelectWindow::SelectDelegate& callback, const String& uniformname, RHIDefine::TextureType textype, const Vector<String>& extraPath);
	void ErrorView(const String& name, const String& text, const String& moreHint, ErrorDelegate& deleteEvent);
	void Hint(const String& str);
}

NS_JYE_END