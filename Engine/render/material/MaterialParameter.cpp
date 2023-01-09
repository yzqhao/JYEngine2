#include "MaterialParameter.h"

NS_JYE_BEGIN

IMPLEMENT_RTTI(MaterialParameter, Object);

IMPLEMENT_INITIAL_BEGIN(MaterialParameter)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameter, Object)
END_ADD_PROPERTY


//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterMat3, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterMat3)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterMat3, MaterialParameter)
REGISTER_PROPERTY(m_mat3, m_mat3, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterMat4, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterMat4)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterMat4, MaterialParameter)
REGISTER_PROPERTY(m_mat4, m_mat4, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterMat3Array, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterMat3Array)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterMat3Array, MaterialParameter)
REGISTER_PROPERTY(m_matArray, m_matArray, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterMat4Array, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterMat4Array)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterMat4Array, MaterialParameter)
REGISTER_PROPERTY(m_matArray, m_matArray, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterVec4, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterVec4)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterVec4, MaterialParameter)
REGISTER_PROPERTY(m_vec4, m_vec4, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterVec4Array, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterVec4Array)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterVec4Array, MaterialParameter)
REGISTER_PROPERTY(m_vecArray4, m_vecArray4, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterVec3, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterVec3)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterVec3, MaterialParameter)
REGISTER_PROPERTY(m_vec3, m_vec3, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterVec3Array, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterVec3Array)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterVec3Array, MaterialParameter)
REGISTER_PROPERTY(m_vecArray3, m_vecArray3, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterVec2, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterVec2)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterVec2, MaterialParameter)
REGISTER_PROPERTY(m_vec2, m_vec2, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterVec2Array, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterVec2Array)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterVec2Array, MaterialParameter)
REGISTER_PROPERTY(m_vecArray2, m_vecArray2, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterVecArray, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterVecArray)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterVecArray, MaterialParameter)
REGISTER_PROPERTY(m_vecArray, m_vecArray, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterFloat, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterFloat)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterFloat, MaterialParameter)
REGISTER_PROPERTY(m_float, m_float, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterColor, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterColor)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterColor, MaterialParameter)
REGISTER_PROPERTY(m_color, m_color, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

//--------------------------------------------------------------------
IMPLEMENT_RTTI(MaterialParameterTex, MaterialParameter);

IMPLEMENT_INITIAL_BEGIN(MaterialParameterTex)
IMPLEMENT_INITIAL_END

BEGIN_ADD_PROPERTY(MaterialParameterTex, MaterialParameter)
REGISTER_PROPERTY(m_tex, m_tex, Property::F_SAVE_LOAD_CLONE)
END_ADD_PROPERTY

NS_JYE_END