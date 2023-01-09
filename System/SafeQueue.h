#pragma once

#include "DataStruct.h"
#include <mutex>

NS_JYE_BEGIN

template <class T>
class SYSTEM_API SafeQueue
{
public:

	SafeQueue() {}
	~SafeQueue() {}

	inline unsigned int GetNum()const;

	void Enqueue(const T& Element);

	bool Dequeue(T& Element);

	bool GetTop(T& Element);

	void Clear();

private:
	Queue<T> m_queue;
	std::mutex m_mutex;
};

template <class T>
void SafeQueue<T>::Clear()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_queue.clear();
}

template <class T>
unsigned int SafeQueue<T>::GetNum() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_queue.size();
}

template <class T>
void SafeQueue<T>::Enqueue(const T& Element)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_queue.push(Element);
}

template <class T>
bool SafeQueue<T>::Dequeue(T& Element)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	Element = m_queue.front();
	m_queue.pop();
}

template <class T>
bool SafeQueue<T>::GetTop(T& Element)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	Element = m_queue.front();
}

NS_JYE_END
