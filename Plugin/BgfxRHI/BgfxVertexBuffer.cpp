#include "BgfxRHI.h"
#include "bgfx/bgfx.h"
#include "Engine/render/VertexStreamLayout.h"

using namespace bgfx;

NS_JYE_BEGIN

#define VertexAttrCount 15
const RHIDefine::ShaderAttribute g_VertexSlot[VertexAttrCount] =
{
	RHIDefine::PS_ATTRIBUTE_POSITION,
	RHIDefine::PS_ATTRIBUTE_NORMAL, RHIDefine::PS_ATTRIBUTE_TANGENT, RHIDefine::PS_ATTRIBUTE_BINORMAL,
	RHIDefine::PS_ATTRIBUTE_COLOR0,
	RHIDefine::PS_ATTRIBUTE_BONE_INEX, RHIDefine::PS_ATTRIBUTE_BONE_WEIGHT,
	RHIDefine::PS_ATTRIBUTE_COORDNATE0, RHIDefine::PS_ATTRIBUTE_COORDNATE1,
	RHIDefine::PS_ATTRIBUTE_COORDNATE2, RHIDefine::PS_ATTRIBUTE_COORDNATE3,
	RHIDefine::PS_ATTRIBUTE_INSTANCE0, RHIDefine::PS_ATTRIBUTE_INSTANCE1,
	RHIDefine::PS_ATTRIBUTE_INSTANCE2, RHIDefine::PS_ATTRIBUTE_INSTANCE3
};
const Attrib::Enum g_BgfxAtt[VertexAttrCount] =
{
	Attrib::Position,
	Attrib::Normal, Attrib::Tangent, Attrib::Bitangent,
	Attrib::Color0,
	Attrib::Indices, Attrib::Weight,
	Attrib::TexCoord0, Attrib::TexCoord1,
	Attrib::TexCoord2, Attrib::TexCoord3,
	Attrib::TexCoord4, Attrib::TexCoord5,
	Attrib::TexCoord6, Attrib::TexCoord7
};

#define VertexAttCount 6
const RHIDefine::DataType g_VertexType[VertexAttCount] =
{
	RHIDefine::DT_INT_8_8_8_8,
	RHIDefine::DT_UINT_8_8_8_8,
	RHIDefine::DT_INT_2_10_10_10,
	RHIDefine::DT_UINT_10_10_10_2,
	RHIDefine::DT_HALF_FLOAT,
	RHIDefine::DT_FLOAT,
};
const AttribType::Enum g_BgfxVertexType[VertexAttCount] =
{
	AttribType::Uint8,
	AttribType::Uint8,
	AttribType::Uint10,
	AttribType::Uint10,
	AttribType::Half,
	AttribType::Float,
};
const unsigned char g_BgfxMul[VertexAttCount] =
{
	4,
	4,
	4,
	4,
	1,
	1,
};

Attrib::Enum Convert2BgfxVertexAttribute(RHIDefine::ShaderAttribute slot)
{
	Attrib::Enum attri = Attrib::Count;
	for (int i = 0; i < VertexAttrCount; i++)
	{
		if (g_VertexSlot[i] == slot)
		{
			attri = g_BgfxAtt[i];
			break;
		}
	}
	if (attri == Attrib::Count)
	{
		JYLOG("BgfxVertexBuffer: Unknown vertex attribute.");
	}
	return attri;
}

AttribType::Enum Convert2BgfxVertexType(RHIDefine::DataType dataType, byte& mulCount)
{
	AttribType::Enum attribute = AttribType::Count;
	mulCount = 1;
	for (int i = 0; i < VertexAttCount; i++)
	{
		if (g_VertexType[i] == dataType)
		{
			attribute = g_BgfxVertexType[i];
			mulCount = g_BgfxMul[i];
			break;
		}
	}
	if (attribute == AttribType::Count)
	{
		JYLOG("BgfxVertexBuffer: Unknown vertex type.");
	}
	return attribute;
}

bgfx::VertexLayout ConvertBgfxLayout(const VertexStreamLayout& _layout)
{
	const bgfx::Caps* caps = bgfx::getCaps();
	bgfx::VertexLayout layout;
	layout.begin(caps->rendererType);

	const std::vector<VertexStreamLayout::Layout*> inputLayouts = _layout.GetVertexLayouts();
	for (int i = 0; i < inputLayouts.size(); i++)
	{
		VertexStreamLayout::Layout* curInputLayout = inputLayouts[i];
		RHIDefine::DataType datType = curInputLayout->DataType();
		RHIDefine::ShaderAttribute attribute = curInputLayout->GetAttributes();
		byte dataCount = curInputLayout->DataCount();
		byte offset = curInputLayout->Offset();
		bool normalized = curInputLayout->isNormalize();

		byte mulCount = 1;
		Attrib::Enum attr = Convert2BgfxVertexAttribute(attribute);
		AttribType::Enum attrType = Convert2BgfxVertexType(datType, mulCount);
		byte totalCount = dataCount;
		layout.add(attr, totalCount, offset, attrType, normalized);
	}

	layout.end();

	return layout;
}

handle BgfxRHI::CreateVertexBuffer(const void* buffer, uint size, const VertexStreamLayout& layout, RHIDefine::MemoryUseage usage, uint16 flags)
{
	bgfx::VertexLayout blayout = ConvertBgfxLayout(layout);
	bool isDynamic = (RHIDefine::MU_STATIC != usage);
	handle _vertexHandle = -1;

	const bgfx::Memory* _mem = nullptr;

	if (buffer != nullptr && size > 0)
	{
		_mem = bgfx::copy(buffer, size);
	}
	else
	{
		// 初始化顶点为空的情况下默认给8个顶点
		uint newSize = 8 * blayout.getStride();
		_mem = bgfx::alloc(newSize);
	}

	if (isDynamic)
	{
		flags = (flags | BGFX_BUFFER_ALLOW_RESIZE);
		bgfx::DynamicVertexBufferHandle vertexHandle = bgfx::createDynamicVertexBuffer(_mem, blayout, flags);
		_vertexHandle = vertexHandle.idx;
		_vertexHandle = _vertexHandle | DYNAMIC_BUFFER_MASK_BIT;
	}
	else
	{
		bgfx::VertexBufferHandle vertexHandle = bgfx::createVertexBuffer(_mem, blayout, flags);
		_vertexHandle = vertexHandle.idx;
	}

	return _vertexHandle;
}


void BgfxRHI::UpdateVertexBuffer(handle handle, uint startVertex, const void* buffer, uint size)
{
	bool isDynamic = ((handle & DYNAMIC_BUFFER_MASK_BIT) != 0);

	if (isDynamic)
	{
		const bgfx::Memory* _mem = nullptr;
		if (buffer != nullptr && size > 0)
		{
			_mem = bgfx::copy(buffer, size);
		}

		bgfx::DynamicVertexBufferHandle vertexHandle;
		vertexHandle.idx = (handle & HANDLE_MASK);
		bgfx::update(vertexHandle, startVertex, _mem);
	}
	else
	{
		JYLOG("BgfxVertexBuffer: Update VertexBuffer failed, attempt to update static buffer!");
	}
}

void BgfxRHI::DestoryVertexBuffer(handle handle)
{
	bool isDynamic = ((handle & DYNAMIC_BUFFER_MASK_BIT) != 0);

	if (isDynamic)
	{
		bgfx::DynamicVertexBufferHandle vertexHandle;
		vertexHandle.idx = (handle & HANDLE_MASK);
		bgfx::destroy(vertexHandle);
	}
	else
	{
		bgfx::VertexBufferHandle vertexHandle;
		vertexHandle.idx = handle & HANDLE_MASK;
		bgfx::destroy(vertexHandle);
	}
}

NS_JYE_END