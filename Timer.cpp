#include "Timer.h"

bool MS::MutexTimer::Start(size_t duration_ms)
{
	if (m_timer.load())
	{
		return true;
	}
	else
	{
		std::thread t(&MS::MutexTimer::Timer, this, duration_ms);
		t.detach();
	}

	return false;
}

void MS::MutexTimer::Timer(size_t dur)
{
	m_timer.store(true);
	std::this_thread::sleep_for(std::chrono::milliseconds(dur));
	m_timer.store(false);
}
