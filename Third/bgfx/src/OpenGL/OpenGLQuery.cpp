#include "../bgfx_p.h"

#if (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)
#	include "OpenGLDrv.h"
#	include <bx/timer.h>
#	include <bx/uint32_t.h>
#	include "../emscripten.h"

namespace bgfx { namespace gl
{
    TimerQueryGL::TimerQueryGL()
        : m_control(BX_COUNTOF(m_query) )
    {
    }

    void TimerQueryGL::create()
    {
        for (uint32_t ii = 0; ii < BX_COUNTOF(m_query); ++ii)
        {
            Query& query = m_query[ii];
            query.m_ready = false;
            GL_CHECK(glGenQueries(1, &query.m_begin) );
            GL_CHECK(glGenQueries(1, &query.m_end) );
        }

        for (uint32_t ii = 0; ii < BX_COUNTOF(m_result); ++ii)
        {
            Result& result = m_result[ii];
            result.reset();
        }
    }

    void TimerQueryGL::destroy()
    {
        for (uint32_t ii = 0; ii < BX_COUNTOF(m_query); ++ii)
        {
            Query& query = m_query[ii];
            GL_CHECK(glDeleteQueries(1, &query.m_begin) );
            GL_CHECK(glDeleteQueries(1, &query.m_end) );
        }
    }

    uint32_t TimerQueryGL::begin(uint32_t _resultIdx)
    {
        while (0 == m_control.reserve(1) )
        {
            update();
        }

        Result& result = m_result[_resultIdx];
        ++result.m_pending;

        const uint32_t idx = m_control.m_current;
        Query& query = m_query[idx];
        query.m_resultIdx = _resultIdx;
        query.m_ready     = false;

        GL_CHECK(glQueryCounter(query.m_begin
            , GL_TIMESTAMP
            ) );

        m_control.commit(1);

        return idx;
    }

    void TimerQueryGL::end(uint32_t _idx)
    {
        Query& query = m_query[_idx];
        query.m_ready = true;

        GL_CHECK(glQueryCounter(query.m_end
            , GL_TIMESTAMP
            ) );

        while (update() )
        {
        }
    }

    bool TimerQueryGL::update()
    {
        if (0 != m_control.available() )
        {
            Query& query = m_query[m_control.m_read];

            if (!query.m_ready)
            {
                return false;
            }

            GLint available;
            GL_CHECK(glGetQueryObjectiv(query.m_end
                , GL_QUERY_RESULT_AVAILABLE
                , &available
                ) );

            if (available)
            {
                m_control.consume(1);

                Result& result = m_result[query.m_resultIdx];
                --result.m_pending;

                GL_CHECK(glGetQueryObjectui64v(query.m_begin
                    , GL_QUERY_RESULT
                    , &result.m_begin
                    ) );

                GL_CHECK(glGetQueryObjectui64v(query.m_end
                    , GL_QUERY_RESULT
                    , &result.m_end
                    ) );

                return true;
            }
        }

        return false;
    }



    void OcclusionQueryGL::create()
	{
		for (uint32_t ii = 0; ii < BX_COUNTOF(m_query); ++ii)
		{
			Query& query = m_query[ii];
			GL_CHECK(glGenQueries(1, &query.m_id) );
		}
	}

	void OcclusionQueryGL::destroy()
	{
		for (uint32_t ii = 0; ii < BX_COUNTOF(m_query); ++ii)
		{
			Query& query = m_query[ii];
			GL_CHECK(glDeleteQueries(1, &query.m_id) );
		}
	}

	void OcclusionQueryGL::begin(Frame* _render, OcclusionQueryHandle _handle)
	{
		while (0 == m_control.reserve(1) )
		{
			resolve(_render, true);
		}

		Query& query = m_query[m_control.m_current];
		GL_CHECK(glBeginQuery(GL_SAMPLES_PASSED, query.m_id) );
		query.m_handle = _handle;
	}

	void OcclusionQueryGL::end()
	{
		GL_CHECK(glEndQuery(GL_SAMPLES_PASSED) );
		m_control.commit(1);
	}

	void OcclusionQueryGL::resolve(Frame* _render, bool _wait)
	{
		while (0 != m_control.available() )
		{
			Query& query = m_query[m_control.m_read];

			if (isValid(query.m_handle) )
			{
				int32_t result;

				if (!_wait)
				{
					GL_CHECK(glGetQueryObjectiv(query.m_id, GL_QUERY_RESULT_AVAILABLE, &result) );

					if (!result)
					{
						break;
					}
				}

				GL_CHECK(glGetQueryObjectiv(query.m_id, GL_QUERY_RESULT, &result) );
				_render->m_occlusion[query.m_handle.idx] = int32_t(result);
			}

			m_control.consume(1);
		}
	}

	void OcclusionQueryGL::invalidate(OcclusionQueryHandle _handle)
	{
		const uint32_t size = m_control.m_size;

		for (uint32_t ii = 0, num = m_control.available(); ii < num; ++ii)
		{
			Query& query = m_query[(m_control.m_read + ii) % size];
			if (query.m_handle.idx == _handle.idx)
			{
				query.m_handle.idx = bgfx::kInvalidHandle;
			}
		}
	}

} } // namespace bgfx

#else

namespace bgfx { namespace gl
{
	
} /* namespace gl */ } // namespace bgfx

#endif // (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)