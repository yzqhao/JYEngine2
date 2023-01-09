#include "../bgfx_p.h"

#if (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)
#	include "OpenGLDrv.h"
#	include <bx/timer.h>
#	include <bx/uint32_t.h>
#	include "../emscripten.h"

namespace bgfx { namespace gl
{
    GLuint SamplerStateCache::add(uint32_t _hash)
    {
        invalidate(_hash);

        GLuint samplerId;
        GL_CHECK(glGenSamplers(1, &samplerId) );

        m_hashMap.insert(stl::make_pair(_hash, samplerId) );

        return samplerId;
    }

    GLuint SamplerStateCache::find(uint32_t _hash)
    {
        HashMap::iterator it = m_hashMap.find(_hash);
        if (it != m_hashMap.end() )
        {
            return it->second;
        }

        return UINT32_MAX;
    }

    void SamplerStateCache::invalidate(uint32_t _hash)
    {
        HashMap::iterator it = m_hashMap.find(_hash);
        if (it != m_hashMap.end() )
        {
            GL_CHECK(glDeleteSamplers(1, &it->second) );
            m_hashMap.erase(it);
        }
    }

    void SamplerStateCache::invalidate()
    {
        for (HashMap::iterator it = m_hashMap.begin(), itEnd = m_hashMap.end(); it != itEnd; ++it)
        {
            GL_CHECK(glDeleteSamplers(1, &it->second) );
        }
        m_hashMap.clear();
    }
    
} } // namespace bgfx

#else

namespace bgfx { namespace gl
{
	
} /* namespace gl */ } // namespace bgfx

#endif // (BGFX_CONFIG_RENDERER_OPENGLES || BGFX_CONFIG_RENDERER_OPENGL)