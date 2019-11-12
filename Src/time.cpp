#include "time.h"

Time::Time()
{

}

Time::Time(const Time& other)
{

}

Time::~Time()
{

}

bool Time::Initialize()
{
	// 시스템이 고해상도 타이머를 지원하는지 체크
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0)
	{
		return false;
	}

	// 밀리초 단위이므로 1000으로 나눔
	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

void Time::Update()
{
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)& currentTime);
	timeDifference = (float)(currentTime - m_startTime);
	m_deltaTime = timeDifference / m_ticksPerMs;
	m_startTime = currentTime;
	return;
}

float Time::GetDeltaTime()
{
	return m_deltaTime;
}