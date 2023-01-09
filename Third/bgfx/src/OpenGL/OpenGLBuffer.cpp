#include "../bgfx_p.h"

#if (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)
#	include "OpenGLDrv.h"
#	include <bx/timer.h>
#	include <bx/uint32_t.h>
#	include "../emscripten.h"

namespace bgfx { namespace gl
{
    void IndexBufferGL::create(uint32_t _size, void* _data, uint16_t _flags)
    {
        m_size  = _size;
        m_flags = _flags;

        GL_CHECK(glGenBuffers(1, &m_id) );
        BX_ASSERT(0 != m_id, "Failed to generate buffer id.");
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id) );
        GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER
            , _size
            , _data
            , (NULL==_data) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW
            ) );
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    }

    void IndexBufferGL::update(uint32_t _offset, uint32_t _size, void* _data, bool _discard)
    {
        BX_ASSERT(0 != m_id, "Updating invalid index buffer.");

        if (_discard)
        {
            // orphan buffer...
            destroy();
            create(m_size, NULL, m_flags);
        }

        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id) );
        GL_CHECK(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER
            , _offset
            , _size
            , _data
            ) );
        GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    }

    void IndexBufferGL::destroy()
	{
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
		GL_CHECK(glDeleteBuffers(1, &m_id) );
	}

    void VertexBufferGL::create(uint32_t _size, void* _data, VertexLayoutHandle _layoutHandle, uint16_t _flags)
    {
        m_size = _size;
        m_layoutHandle = _layoutHandle;
        const bool drawIndirect = 0 != (_flags & BGFX_BUFFER_DRAW_INDIRECT);

        m_target = drawIndirect ? GL_DRAW_INDIRECT_BUFFER : GL_ARRAY_BUFFER;

        GL_CHECK(glGenBuffers(1, &m_id) );
        BX_ASSERT(0 != m_id, "Failed to generate buffer id.");
        GL_CHECK(glBindBuffer(m_target, m_id) );
        GL_CHECK(glBufferData(m_target
            , _size
            , _data
            , (NULL==_data) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW
            ) );
        GL_CHECK(glBindBuffer(m_target, 0) );
    }

    void VertexBufferGL::update(uint32_t _offset, uint32_t _size, void* _data, bool _discard)
    {
        BX_ASSERT(0 != m_id, "Updating invalid vertex buffer.");

        if (_discard)
        {
            // orphan buffer...
            destroy();
            create(m_size, NULL, m_layoutHandle, 0);
        }

        GL_CHECK(glBindBuffer(m_target, m_id) );
        GL_CHECK(glBufferSubData(m_target
            , _offset
            , _size
            , _data
            ) );
        GL_CHECK(glBindBuffer(m_target, 0) );
    }

	void VertexBufferGL::destroy()
	{
		GL_CHECK(glBindBuffer(m_target, 0) );
		GL_CHECK(glDeleteBuffers(1, &m_id) );
	}
    


} } // namespace bgfx

#else

namespace bgfx { namespace gl
{
	
} /* namespace gl */ } // namespace bgfx

#endif // (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)
