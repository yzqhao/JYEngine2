#pragma once

#include "RHI/RHI.h"
#include "Engine/private/Define.h"
#include "ParserStateType.h"

NS_JYE_BEGIN

class ParserMaterial
{
public:
	enum ParamsType
	{
		VT_ERRORCODE = 0,
		VT_FLOAT,
		VT_FLOATRANGE,
		VT_VEC2,
		VT_VEC3,
		VT_VEC4,
		VT_COLOR,
		VT_TEXTURE1D,
		VT_TEXTURE2D,
		VT_TEXTURE3D,
		VT_TEXTURECUBE,
		VT_KEYWORDENUM,
		VT_COUNT,
	};

	struct ParserRenderState
	{
		ParserBoolean m_isDepthTest;
		ParserBoolean m_isDepthMask;
		ParserFunction m_DepthFunction;
		ParserCullFaceMode m_isCullFace;

		ParserBoolean m_isAlphaEnable;
		ParserBlend m_Src;
		ParserBlend m_Des;
		ParserBlend m_SrcA;
		ParserBlend m_DesA;

		ParserColorMask m_ColorMask;

		ParserBoolean m_isStencil;
		ParserBitMask m_StencilMask;

		/**
		* Front face stencil states.
		*/
		ParserFunction m_StencilFuncFront;
		ParserUint m_StencilFuncRefFront;
		ParserBitMask m_StencilFuncMaskFront;
		ParserOperation m_StencilFailFront;
		ParserOperation m_StencilZFailFront;
		ParserOperation m_StencilZPassFront;

		/**
		* Back face stencil states.
		*/
		ParserFunction m_StencilFuncBack;
		ParserUint m_StencilFuncRefBack;
		ParserBitMask m_StencilFuncMaskBack;
		ParserOperation m_StencilFailBack;
		ParserOperation m_StencilZFailBack;
		ParserOperation m_StencilZPassBack;

		ParserPolygonMode m_PolygonMode;
		ParserUint	m_PointSize;
		ParserUint	m_LineWidth;

		ParserUint	m_ScissorX;
		ParserUint	m_ScissorY;
		ParserUint	m_ScissorZ;
		ParserUint	m_ScissorW;

		unsigned m_RenderQueue;
		unsigned m_uHashCode;
	};

		
	struct UniformData
	{
		String name;
		String type;
		unsigned short num;
		unsigned short sit;
		unsigned short regIndex;
		unsigned short regCount;
		Vector<unsigned short> subIndexs;
	};

	struct ShaderCode
	{
		String shaderCode;
		bool compress;
		int blockIndex;
		int blockOffset;
		int blockSize;
	};

	struct AttributeData
	{
		String attName;
		unsigned short attId;
	};

	struct ConstBuffer
	{
		Vector<unsigned short> varIndexs;
	};

	struct ProgramData
	{
		String shaderAPI;
		Vector<String> keyWords;
		Vector<UniformData> vsUniforms;
		Vector<AttributeData> vsAttributes;
		Vector<UniformData> psUniforms;
		Vector<UniformData> csUniforms;
		Vector<int> vsUniformIndices;
		Vector<int> psUniformIndices;
		Vector<int> csUniformIndices;
		Vector<int> vsAttributeIndices;
		Vector<ConstBuffer> vsConstBuffers;
		Vector<ConstBuffer> psConstBuffers;
		ShaderCode vshaderCode;
		ShaderCode pshaderCode;
		ShaderCode csShaderCode;
		unsigned short vsBufferSize;
		unsigned short psBufferSize;
		unsigned short csBufferSize;
		int vsBlockIndex;
		int vsBlockOffset;
		int psBlockIndex;
		int psBlockOffset;
	};

	class PassData
	{
	public:
		RHIDefine::PassType passType;
		String passName;
		ParserRenderState renderState;
		Vector<ProgramData> programDatas;
		bool indexingUniforms = false;
		bool indexingAttributes = false;
		Vector<UniformData> passUniforms;
		Vector<AttributeData> passAttributes;
	};

	struct ParamInfo
	{
		String name;
		String desc;
		String attribute;
		ParserMaterial::ParamsType type;
		float defValue[4];
		String defTexPath;
	};

	Vector<PassData> PassDatas;
	Vector<ParamInfo> Params;
	String ShaderName;
	int Version;
	int RenderQueue;  // RHIDefine::RenderQueue
};

NS_JYE_END