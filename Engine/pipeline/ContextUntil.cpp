#include "ContextUntil.h"
#include "RenderContext.h"
#include "Engine/render/material/IShaderResource.h"
#include "Engine/render/material/ParameterArray.h"
#include "RHI/RHI.h"
#include "Engine/render/material/IMaterialSystem.h"

NS_JYE_BEGIN

bool ContextUntil::SetShaderParam(RenderContext& con, ShaderUniform& uniform, ParameterArray* spa, ParameterArray* jpa)
{
	RHIDefine::ParameterSlot it = uniform.slot;
	handle hunifrom = uniform.handle;
	RHIDefine::VariablesType varType = uniform.type;
	MaterialParameterPtr data;
	bool result = false;

	if (uniform.bindIndex >= 0)
	{
		BuiltinParams::BuilitParamVal& paramV = con.GetBuiltinParams()->GetValue(uniform.bindIndex);
		data = paramV.pObj;
	}

	if (!IsValidRef(data))
	{
		MaterialParameter* objt1 = (nullptr != jpa) ? jpa->GetParameter(it) : nullptr;
		MaterialParameter* objt2 = (nullptr != objt1) ? objt1 : spa->GetParameter(it);
		data = objt2;
	}

	if (data)
	{
		switch (varType)
		{
		case RHIDefine::VT_FLOAT:
		{
			if (data->IsSameType(MaterialParameterFloat::ms_Type))
			{
				MaterialParameterFloat* pVector1f = DynamicCast<MaterialParameterFloat>(data);
				RHISetUniform(hunifrom, &pVector1f->m_float);
				result = true;
			}
			break;
		}
		case RHIDefine::VT_VEC2:
		{
			MaterialParameterVec2* pData = DynamicCast<MaterialParameterVec2>(data);
			RHISetUniform(hunifrom, pData->m_vec2.GetPtr());
			result = true;
			break;
		}
		case RHIDefine::VT_VEC3:
		{
			MaterialParameterVec3* pData = DynamicCast<MaterialParameterVec3>(data);
			RHISetUniform(hunifrom, pData->m_vec3.GetPtr());
			result = true;
			break;
		}
		case RHIDefine::VT_VEC4:
		{
			if (data->IsSameType(MaterialParameterVec4::ms_Type))
			{
				MaterialParameterVec4* pData = StaticCast<MaterialParameterVec4>(data);
				RHISetUniform(hunifrom, pData->m_vec4.GetPtr());
			}
			else if (data->IsSameType(MaterialParameterColor::ms_Type))
			{
				MaterialParameterColor* pData = StaticCast<MaterialParameterColor>(data);
				RHISetUniform(hunifrom, pData->m_color.GetPtr());
			}
			else if (data->IsSameType(MaterialParameterVec4Array::ms_Type))
			{
				MaterialParameterVec4Array* pData = StaticCast<MaterialParameterVec4Array>(data);
				RHISetUniform(hunifrom, pData->m_vecArray4.Data());
			}
			result = true;
			break;
		}
		case RHIDefine::VT_MAT3:
		{
			//TODO
			assert(false);
			result = false;
			break;
		}
		case RHIDefine::VT_MAT3_ARRAY:
		{
			MaterialParameterMat3Array* pparam = StaticCast<MaterialParameterMat3Array>(data);
			RHISetUniform(hunifrom, pparam->m_matArray.Data());
			result = true;
			break;
		}
		case RHIDefine::VT_MAT4:
		{
			MaterialParameterMat4* pData = StaticCast<MaterialParameterMat4>(data);
			RHISetUniform(hunifrom, pData->m_mat4.GetPtr());
			result = true;
			break;
		}
		case RHIDefine::VT_FLOAT_ARRAY:
		{
			//TODO
			MaterialParameterVecArray* pData = StaticCast<MaterialParameterVecArray>(data);
			RHISetUniform(hunifrom, pData->m_vecArray.Data());
			result = true;
			break;
		}
		case RHIDefine::VT_VEC2_ARRAY:
		{
			MaterialParameterVec2Array* pData = StaticCast<MaterialParameterVec2Array>(data);
			RHISetUniform(hunifrom, pData->m_vecArray2.Data());
			result = true;
			break;
		}
		case RHIDefine::VT_VEC3_ARRAY:
		{
			MaterialParameterVec3Array* pData = StaticCast<MaterialParameterVec3Array>(data);
			RHISetUniform(hunifrom, pData->m_vecArray3.Data(), pData->m_vecArray3.Size());
			result = true;
			break;
		}
		case RHIDefine::VT_VEC4_ARRAY:
		{
			MaterialParameterVec4Array* pData = StaticCast<MaterialParameterVec4Array>(data);
			RHISetUniform(hunifrom, pData->m_vecArray4.Data(), pData->m_vecArray4.Size());
			result = true;
			break;
		}
		case RHIDefine::VT_TEXTURE2D:
		case RHIDefine::VT_TEXTURE1D:
		case RHIDefine::VT_TEXTURE3D:
		case RHIDefine::VT_TEXTURECUBE:
		{
			MaterialParameterTex* pparam = DynamicCast<MaterialParameterTex>(data);

			if (pparam)
			{
				if (uniform.srv_uav)
				{
					// TODO
				}
				else
				{
					RHISetTexture(
						uniform.shaderType,
						uniform.m_stage,
						uniform.regIndex,
						hunifrom,
						pparam->GetTex()->GetHandle(),
						pparam->GetTex()->GetFlags()
					);
				}

				result = true;
			}
			break;
		}
		case RHIDefine::VT_BUFFER:
		{
			//TODO
			assert(false);
			break;
		}
		case RHIDefine::VT_CONSTBUFFER:
		{
			//TODO
			assert(false);
			break;
		}
		break;
		default:
		{
			result = false;
			JYERROR("type of input '%s' is not suport", uniform.name.c_str());
			break;
		}
		}
	}

	if (!result)
	{
		const IMaterialSystem::InputParameter& info = IMaterialSystem::Instance()->GetParameterInfomation(it);
		const char* pName = info.Name.c_str();
		JYERROR("================================================"
			"material : uniform parameter is empty or not match, name = %s"
			"================================================", info.Name.c_str());
	}

	return result;
}

NS_JYE_END