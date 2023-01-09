#pragma once

#include "Math/3DMath.h"

#include <sol/sol.hpp>

NS_JYE_BEGIN

void Bind_Engine(sol::state& sol_state);
void Bind_Engine_Entity(sol::state& sol_state);

NS_JYE_END

