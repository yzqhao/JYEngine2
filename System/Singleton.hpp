#pragma once


#define SINGLETON_DECLARE( Type )						\
private:												\
	static Type*		m_psSelf;						\
public:													\
	static inline Type* Create()						\
	{													\
		JY_ASSERT( NULL == m_psSelf );					\
		m_psSelf = new Type;							\
		return m_psSelf;								\
	}													\
	static inline void Destroy()						\
	{													\
		delete m_psSelf;								\
	}													\
	static inline Type* Instance()						\
	{													\
		return m_psSelf;								\
	}	

#define SINGLETON_DECLARE_ONCREATE_ONDESTROY( Type )	\
private:												\
	static Type*		m_psSelf;						\
public:													\
	static inline Type* Create()						\
	{													\
		JY_ASSERT( NULL == m_psSelf );					\
		m_psSelf = new Type;							\
		m_psSelf->_OnCreate();							\
		return m_psSelf;								\
	}													\
	static inline void Destroy()						\
	{													\
		m_psSelf->_OnDestroy();							\
		delete m_psSelf;								\
		m_psSelf = NULL;								\
	}													\
	static inline Type* Instance()						\
	{													\
		return m_psSelf;								\
	}													\


#define SINGLETON_IMPLEMENT( Type )	Type* Type::m_psSelf = NULL;




#define SYSTEM_SINGLETON_DECLEAR( type )\
	private:\
	static type*			s_rpSelf;\
	public:\
	static inline type* Instance()\
		{\
		return s_rpSelf;\
		}\
	static inline void Destroy()\
		{\
		delete s_rpSelf;\
		}

#define SYSTEM_SINGLETON_IMPLEMENT( type ) type* type::s_rpSelf = NULL;
#define SYSTEM_SINGLETON_INITIALIZE JY_ASSERT(NULL==s_rpSelf);s_rpSelf = this;
#define SYSTEM_SINGLETON_DESTROY s_rpSelf = NULL;