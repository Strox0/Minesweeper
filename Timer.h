#pragma once
#include <atomic>
#include <thread>

namespace MS
{
	class MutexTimer
	{
	public:
		bool Start(size_t duration_ms);

	private:
		void Timer(size_t dur);

	private:
		std::atomic_bool m_timer;
	
	};
}