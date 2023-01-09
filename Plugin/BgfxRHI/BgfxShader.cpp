#include "BgfxRHI.h"
#include "bgfx/bgfx.h"
#include "bx/readerwriter.h"
#include "bx/allocator.h"

using namespace bgfx;

NS_JYE_BEGIN

bgfx::UniformType::Enum ShaderType2BgfxType(RHIDefine::VariablesType varType)
{
	bgfx::UniformType::Enum bgfxType = bgfx::UniformType::End;
	switch (varType)
	{
	case RHIDefine::VT_ERRORCODE:
		break;
	case RHIDefine::VT_FLOAT:
		bgfxType = bgfx::UniformType::Float;
		break;
	case RHIDefine::VT_VEC2:
		bgfxType = bgfx::UniformType::Vec2;
		break;
	case RHIDefine::VT_VEC3:
		bgfxType = bgfx::UniformType::Vec3;
		break;
	case RHIDefine::VT_VEC4:
		bgfxType = bgfx::UniformType::Vec4;
		break;
	case RHIDefine::VT_FLOAT_ARRAY:
		bgfxType = bgfx::UniformType::Float;
		break;
	case RHIDefine::VT_VEC2_ARRAY:
		bgfxType = bgfx::UniformType::Vec2;
		break;
	case RHIDefine::VT_VEC3_ARRAY:
		bgfxType = bgfx::UniformType::Vec3;
		break;
	case RHIDefine::VT_VEC4_ARRAY:
		bgfxType = bgfx::UniformType::Vec4;
		break;
	case RHIDefine::VT_MAT3:
		bgfxType = bgfx::UniformType::Mat3;
		break;
	case RHIDefine::VT_MAT4:
		bgfxType = bgfx::UniformType::Mat4;
		break;
	case RHIDefine::VT_TEXTURE1D:
		bgfxType = bgfx::UniformType::Sampler;
		break;
	case RHIDefine::VT_TEXTURE2D:
		bgfxType = bgfx::UniformType::Sampler;
		break;
	case RHIDefine::VT_TEXTURE3D:
		bgfxType = bgfx::UniformType::Sampler;
		break;
	case RHIDefine::VT_TEXTURECUBE:
		bgfxType = bgfx::UniformType::Sampler;
		break;
	case RHIDefine::VT_BUFFER:
		bgfxType = bgfx::UniformType::Buffer;
		break;
	case RHIDefine::VT_CONSTBUFFER:
		bgfxType = bgfx::UniformType::ConstBuffer;
		break;
	case RHIDefine::VT_COUNT:
	default:
		JYLOGTAG(LogBgfxRHI, "BgfxShader: Unknown uniform type.");
		break;
	}

	return bgfxType;
}

static uint8_t decodeAlpha(char input)
{
	if (input <= '9' && input >= '0')
	{
		return input - '0';
	}
	else if (input <= 'Z' && input >= 'A')
	{
		return input - 'A' + 10;
	}
	JYLOGTAG(LogBgfxRHI, "BgfxShader: Invalid DX shader code.");
	return '?';
}

inline uint32_t DecodeBinaryShader(const std::string& input, unsigned char*& output)
{
	uint32_t size = input.size();

	for (int i = 0; i < size; i += 2)
	{
		int highPart = decodeAlpha(input[i]);
		int lowPart = decodeAlpha(input[i + 1]);
		output[i >> 1] = (highPart << 4) + lowPart;
	}
	return size >> 1;
}

static bool IsBinaryShaderPlatform()
{
	const bgfx::Caps* caps = bgfx::getCaps();
	if (caps->rendererType == bgfx::RendererType::Direct3D12
		|| caps->rendererType == bgfx::RendererType::Direct3D11
		|| caps->rendererType == bgfx::RendererType::Vulkan)
	{
		return true;
	}
	else
	{
		return false;
	}
}


handle BgfxRHI::CreateShader(const std::string& in_code, RHIDefine::ShaderType shaderType, const std::vector<InputUniformInfo>& iuniformInfo, const std::vector<InputAttributes>& vsAttributes, unsigned short constBufferSize, std::vector<OutputUniformInfo>& oUniformInfo)
{
#define BGFX_SHADER_BIN_VERSION 6

#define BGFX_UNIFORM_FRAGMENTBIT UINT8_C(0x10)
#define BGFX_UNIFORM_SAMPLERBIT  UINT8_C(0x20)
#define BGFX_UNIFORM_COMPUTEBIT  UINT8_C(0x40)
#define BGFX_UNIFORM_BUFFERBIT   UINT8_C(0x80)

	uint32_t vshMagic = 0;
	uint32_t pshMagic = 0;
	uint32_t cshMagic = 0;

	cshMagic = BX_MAKEFOURCC('C', 'S', 'G', BGFX_SHADER_BIN_VERSION);
	vshMagic = BX_MAKEFOURCC('V', 'S', 'G', BGFX_SHADER_BIN_VERSION);
	pshMagic = BX_MAKEFOURCC('F', 'S', 'G', BGFX_SHADER_BIN_VERSION);

	int32_t tempLen = in_code.length() + (4 << 10);
	char* temp = (char*)new char[tempLen];
	bx::StaticMemoryBlockWriter writer(temp, tempLen);
	bx::ErrorAssert errorAssert;
	unsigned int totalSize = 0;

	if (shaderType == RHIDefine::ShaderType::VS)
	{
		totalSize += bx::write(&writer, vshMagic, errorAssert);
	}
	else if (shaderType == RHIDefine::ShaderType::PS)
	{
		totalSize += bx::write(&writer, pshMagic, errorAssert);
	}
	else if (shaderType == RHIDefine::ShaderType::CS)
	{
		totalSize += bx::write(&writer, cshMagic, errorAssert);
	}

	//hashOut
	uint hashOut = 0;
	uint hashIn = 0;
	uint16 uniformSize = iuniformInfo.size();
	totalSize += bx::write(&writer, hashOut, errorAssert);
	totalSize += bx::write(&writer, hashIn, errorAssert);
	totalSize += bx::write(&writer, uniformSize, errorAssert);

	for (int i = 0; i < uniformSize; i++)
	{
		const InputUniformInfo& curInfo = iuniformInfo[i];
		unsigned char nameSize = curInfo.name.length();
		bgfx::UniformType::Enum bgfxType;
		bgfxType = ShaderType2BgfxType(curInfo.type);

		if (bgfxType == bgfx::UniformType::Sampler || bgfxType == UniformType::End)
		{
			bgfxType = (bgfx::UniformType::Enum)(BGFX_UNIFORM_SAMPLERBIT | bgfxType);
		}

		if (bgfxType == UniformType::Buffer)
		{
			bgfxType = (bgfx::UniformType::Enum)(BGFX_UNIFORM_BUFFERBIT);
		}

		if (shaderType == RHIDefine::ShaderType::PS)
		{
			bgfxType = (bgfx::UniformType::Enum)(BGFX_UNIFORM_FRAGMENTBIT | bgfxType);
		}

		if (shaderType == RHIDefine::ShaderType::CS)
		{
			bgfxType = (bgfx::UniformType::Enum)(BGFX_UNIFORM_COMPUTEBIT | bgfxType);
		}

		totalSize += bx::write(&writer, nameSize, errorAssert);
		totalSize += bx::write(&writer, curInfo.name.c_str(), nameSize, errorAssert);
		totalSize += bx::write(&writer, (unsigned char)bgfxType, errorAssert);
		totalSize += bx::write(&writer, (unsigned char)curInfo.num, errorAssert);
		totalSize += bx::write(&writer, (unsigned short)curInfo.regIndex, errorAssert);
		totalSize += bx::write(&writer, (unsigned short)curInfo.regCount, errorAssert);
	}

	unsigned int shaderCodeSize = in_code.length();
	unsigned char* code = nullptr;
	if (IsBinaryShaderPlatform())
	{
		code = new unsigned char[in_code.size()];
		shaderCodeSize = DecodeBinaryShader(in_code, code);
		totalSize += bx::write(&writer, shaderCodeSize, errorAssert);
		totalSize += bx::write(&writer, code, shaderCodeSize, errorAssert);
		delete[] code;
	}
	else
	{
		code = (unsigned char*)in_code.c_str();
		totalSize += bx::write(&writer, shaderCodeSize, errorAssert);
		totalSize += bx::write(&writer, code, shaderCodeSize, errorAssert);
	}

	totalSize += bx::write(&writer, uint8_t(0), errorAssert);

	// vs attributes
	totalSize += bx::write(&writer, (unsigned char)vsAttributes.size(), errorAssert);
	for (int i = 0; i < vsAttributes.size(); i++)
	{
		unsigned short attID = vsAttributes[i].id;
		totalSize += bx::write(&writer, attID, errorAssert);
	}
	// const buffer Size
	totalSize += bx::write(&writer, (unsigned short)constBufferSize, errorAssert);

	const bgfx::Memory* pMem = bgfx::copy(temp, totalSize);
	delete[] temp;

	bgfx::ShaderHandle shaderHandle = bgfx::createShader(pMem);

	if (bgfx::kInvalidHandle != shaderHandle.idx)
	{
#define MaxUniformCount 256
		bgfx::UniformHandle uniforms[MaxUniformCount];
		unsigned short realCount = bgfx::getShaderUniforms(shaderHandle, uniforms, MaxUniformCount);

		for (int i = 0; i < realCount; i++)
		{
			oUniformInfo.emplace_back();
			OutputUniformInfo& outUniformInfo = oUniformInfo.back();

			bgfx::UniformHandle curHandle = uniforms[i];
			bgfx::UniformInfo uniformInfo;
			bgfx::getUniformInfo(curHandle, uniformInfo);

			outUniformInfo.name = uniformInfo.name;
			outUniformInfo.uniHandle = curHandle.idx;


			if (strcmp(uniformInfo.name, iuniformInfo[i].name.c_str()) == 0)
			{
				const InputUniformInfo& info = iuniformInfo[i];
				outUniformInfo.num = info.num;
				outUniformInfo.type = info.type;
				outUniformInfo.regIndex = info.regIndex;
				outUniformInfo.regCount = info.regCount;
				outUniformInfo.slot = info.slot;
				outUniformInfo.srv_uav = info.srv_uav;
				int subsize = info.subUniform.size();
				std::vector<OutputUniformInfo>& subOutInfos = outUniformInfo.subInfos;
				for (int i = 0; i < subsize; i++)
				{
					subOutInfos.emplace_back();
					OutputUniformInfo& subOutInfo = subOutInfos.back();
					const InputUniformInfo& subInInfo = info.subUniform[i];

					subOutInfo.name = subInInfo.name;
					subOutInfo.type = subInInfo.type;
					subOutInfo.slot = subInInfo.slot;
					subOutInfo.regIndex = info.regIndex;
					subOutInfo.num = info.num;
				}
			}
			else
			{
				//refind
				for (int j = 0; j < uniformSize; j++)
				{
					if (strcmp(uniformInfo.name, iuniformInfo[j].name.c_str()) == 0)
					{
						const InputUniformInfo& info = iuniformInfo[j];
						outUniformInfo.num = info.num;
						outUniformInfo.type = info.type;
						outUniformInfo.slot = info.slot;
						outUniformInfo.regIndex = info.regIndex;
						outUniformInfo.regCount = info.regCount;
						outUniformInfo.srv_uav = info.srv_uav;
						break;
					}
				}
			}
		}
	}
	return shaderHandle.idx;
}

void BgfxRHI::SetShaderName(handle _handle, const std::string& shaderName)
{
#ifdef _DEBUG
	bgfx::ShaderHandle shaderHandle;
	shaderHandle.idx = _handle;
	bgfx::setName(shaderHandle, shaderName.c_str());
#endif
}

void BgfxRHI::DestoryShader(handle _handle)
{
	bgfx::ShaderHandle shaderHandle;
	shaderHandle.idx = _handle;
	bgfx::destroy(shaderHandle);
}

handle BgfxRHI::CreateProgram(handle vsShader, handle psShader)
{
	bgfx::ShaderHandle vsShaderHandle;
	bgfx::ShaderHandle psShaderHandle;
	bgfx::ProgramHandle programHandle;

	vsShaderHandle.idx = vsShader;
	psShaderHandle.idx = psShader;
	// program 创建完成后shader会被删掉
	programHandle = bgfx::createProgram(vsShaderHandle, psShaderHandle, true);

	return programHandle.idx;
}

handle BgfxRHI::CreateComputeProgram(handle csShader)
{
	bgfx::ShaderHandle csShaderHandle;
	bgfx::ProgramHandle programHandle;
	csShaderHandle.idx = csShader;
	programHandle = bgfx::createProgram(csShaderHandle, true);

	return programHandle.idx;
}

void BgfxRHI::DestoryProgram(handle _handle)
{
	bgfx::ProgramHandle programHandle;
	programHandle.idx = _handle;
	bgfx::destroy(programHandle);
}

void BgfxRHI::DestoryUniform(handle _handle)
{
	bgfx::UniformHandle uniformHandle;
	uniformHandle.idx = _handle;
	bgfx::destroy(uniformHandle);
}

NS_JYE_END