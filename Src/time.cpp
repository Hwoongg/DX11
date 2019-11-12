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
	// �ý����� ���ػ� Ÿ�̸Ӹ� �����ϴ��� üũ
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0)
	{
		return false;
	}

	// �и��� �����̹Ƿ� 1000���� ����
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