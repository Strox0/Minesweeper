#pragma once
#include <random>
#include <mutex>

class RandomInt
{
public:
	RandomInt(int64_t min, int64_t max) : m_min(min), m_max(max), m_engine(std::random_device{}())
	{
		if (m_min > m_max)
		{
			m_min = max;
			m_max = min;
		}
	};

	void SetRange(int64_t min, int64_t max)
	{
		m_mtx.lock();
		if (min < max) [[likely]]
		{
			m_min = min;
			m_max = max;
		}
		else
		{
			m_min = max;
			m_max = min;
		}
		m_mtx.unlock();
	}

	int64_t GetNumber()
	{
		m_mtx.lock();
		std::uniform_int_distribution<int64_t> dist(m_min, m_max);
		int64_t res = dist(m_engine);
		m_mtx.unlock();
		return res;
	}

private:
	int64_t m_min;
	int64_t m_max;
	std::mt19937_64 m_engine;
	std::mutex m_mtx;
};