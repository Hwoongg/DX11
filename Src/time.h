#pragma once

#ifndef _TIMER_H_
#define _TIMER_H_

#include <windows.h>

class Time
{
public:
	Time();
	Time(const Time&);
	~Time();

	bool Initialize();
	void Update();

	float GetDeltaTime();

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_deltaTime;
};
#endif // !_TIMER_H_
