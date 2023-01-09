
#include "KeyWordUtil.h"
#include "Core/Interface/ILogSystem.h"
#include "Math/Math.h"

#include <algorithm>
#include <atomic>

NS_JYE_BEGIN

namespace KeyWordUtil
{
	class SpinMutex
	{
		int mLock;

	public:
		SpinMutex()
		{
			mLock = 0;
		}
		void lock()
		{
			while (!tryLock())
			{
				// spin
			}
		}
		void unlock()
		{
			std::atomic<int>* aDest = reinterpret_cast<std::atomic<int>*>(&mLock);
			std::atomic_store_explicit(aDest, int(0), std::memory_order_release);
		}
	private:
		bool tryLock()
		{
			std::atomic<int>* aDest = reinterpret_cast<std::atomic<int>*>(&mLock);
			int expected = 0;
			return std::atomic_compare_exchange_weak_explicit(aDest, &expected, int(1), std::memory_order_acq_rel, std::memory_order_acquire);
		}
	};

	struct strCmp
	{
		bool operator () (const char* lhs, const char* rhs) const { return strcmp(lhs, rhs) < 0; }
	};

	struct SHKH
	{
		unsigned operator()(const char* str) const
		{
			return Math::MathInstance::GetMathInstance().CRC32Compute(str, (unsigned int)strlen(str));
		}
	};

	struct SHKE
	{
		bool operator () (const char* lhs, const char* rhs) const { return strcmp(lhs, rhs) == 0; }
	};

	typedef std::unordered_map<const char*, ShaderKey, SHKH, SHKE> KeyMap;
	static KeyMap* g_keymap;
	static SpinMutex s_mutex;

#define EngineKey(x) key = Create(#x);\
                    JY_ASSERT(key == x)\

	void Init()
	{
		g_keymap = new KeyMap();
		ShaderKey key;
		EngineKey(NoLight);
		EngineKey(SpotLight);
		EngineKey(DirLight);
		EngineKey(PointLight);
		EngineKey(ShadowOff);
		EngineKey(ShadowOn);
		EngineKey(GPUSKIN4);
		EngineKey(GPUINSTANCING);
	}

	void Clean()
	{
		s_mutex.lock();
		KeyMap::iterator iter = g_keymap->begin();
		for (; iter != g_keymap->end(); iter++)
		{
			char* name = (char*)iter->first;
			SAFE_DELETE(name);
		}
		SAFE_DELETE(g_keymap);
		s_mutex.unlock();
	}

	ShaderKey Create(const char* name)
	{
		s_mutex.lock();
		KeyMap::iterator iter = g_keymap->find(name);
		if (iter != g_keymap->end())
		{
			s_mutex.unlock();
			return iter->second;
		}

		size_t size = g_keymap->size();
		if (size >= MaxKeyWords)
		{
			s_mutex.unlock();
			JYLOGTAG(LogEngineRHI, "keyword count is too many, please delete some");
			return 0;
		}
		else
		{
			size_t len = strlen(name);
			char* strBuf = new char[len + 1];
			strcpy(strBuf, name);
			g_keymap->emplace(strBuf, size);
			s_mutex.unlock();
			return size;
		}
		s_mutex.unlock();
	}

	const char* GetKeyName(ShaderKey key)
	{
		if (!g_keymap)
		{
			Init();
		}

		s_mutex.lock();
		KeyMap::iterator iter = g_keymap->begin();

		for (; iter != g_keymap->end(); iter++)
		{
			if (iter->second == key)
			{
				s_mutex.unlock();
				return iter->first;
			}
		}
		s_mutex.unlock();

		JYLOGTAG(LogEngineRHI, "not find keyname for %d keynum", key);
		return NULL;
	}
}

NS_JYE_END