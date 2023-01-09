#include "BgfxRHI.h"
#include "bgfx/bgfx.h"

NS_JYE_BEGIN

handle BgfxRHI::CreateIndiceBuffer(uint datasize, uint typesize, const void* dataptr, RHIDefine::MemoryUseage use)
{
	const bgfx::Memory* _mem = nullptr;
	if (dataptr != nullptr && datasize > 0)
	{
		_mem = bgfx::copy(dataptr, datasize * typesize);
	}
	else
	{
		uint newSize = 8;
		_mem = bgfx::alloc(newSize);
	}
	handle indiceHd;
	uint16 flags = BGFX_BUFFER_NONE;
	if (typesize == 4)
	{
		flags |= BGFX_BUFFER_INDEX32;
	}

	bool isDynamic = (RHIDefine::MU_STATIC != use);
	if (isDynamic)
	{
		indiceHd = bgfx::createDynamicIndexBuffer(_mem, flags | BGFX_BUFFER_ALLOW_RESIZE).idx;
		indiceHd = (indiceHd | DYNAMIC_BUFFER_MASK_BIT);
	}
	else
	{
		indiceHd = bgfx::createIndexBuffer(_mem, flags).idx;
	}

	return indiceHd;
}

void BgfxRHI::UpdateIndiceBuffer(handle ih, uint startIndex, uint datasize, uint typesize, const void* dataptr)
{
	bool isDynamic = ((ih & DYNAMIC_BUFFER_MASK_BIT) != 0);

	if (isDynamic)
	{
		const bgfx::Memory* _mem = nullptr;
		if (dataptr != nullptr && datasize > 0)
		{
			_mem = bgfx::copy(dataptr, datasize * typesize);
		}

		bgfx::DynamicIndexBufferHandle dIdxBf;
		dIdxBf.idx = (ih & HANDLE_MASK);
		bgfx::update(dIdxBf, startIndex, _mem);
	}
	else
	{
		JYLOGTAG(LogBgfxRHI, "BgfxRHI Update IndexBuffer failed, attempt to update static buffer!");
	}
}

void BgfxRHI::DestroyIndiceBuffer(handle ih)
{
	bool isDynamic = ((ih & DYNAMIC_BUFFER_MASK_BIT) != 0);

	if (isDynamic)
	{
		bgfx::DynamicIndexBufferHandle IndexHandle;
		IndexHandle.idx = (ih & HANDLE_MASK);
		bgfx::destroy(IndexHandle);
	}
	else
	{
		bgfx::IndexBufferHandle IndexHandle;
		IndexHandle.idx = ih & HANDLE_MASK;
		bgfx::destroy(IndexHandle);
	}
}

NS_JYE_END