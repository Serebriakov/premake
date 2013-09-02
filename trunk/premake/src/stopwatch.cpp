#include "stopwatch.h"


Stopwatch::Stopwatch(double CycleInterval)
{
	SetCycleInterval(CycleInterval);
	Reset();
}

void Stopwatch::SetCycleInterval(double CycleInterval)
{
	if( CycleInterval > 0 )
	{
		m_CycleInterval = CycleInterval;
	}
	else
	{
		m_CycleInterval = 1;
	}
}

/*
Get the current value of the stopwatch in milliseconds
*/
double Stopwatch::GetElapsedTime() const
{
	return (SDL_GetPerformanceCounter() - m_StartTime) / (double)SDL_GetPerformanceFrequency() * 1000.0;
}

/*
Get the number of cycles that has passed since the last call to this function
*/
Uint64 Stopwatch::GetCycles()
{
	Uint64 num_of_cycles = (Uint64)(GetElapsedTime() / m_CycleInterval);
	m_StartTime += (Uint64)(num_of_cycles * m_CycleInterval / 1000.0 * SDL_GetPerformanceFrequency());
	return num_of_cycles;
}

/*
Get the portion of the cycle interval that has passed
*/
double Stopwatch::GetCycleState() const
{
	return GetElapsedTime() / m_CycleInterval;
}

/*
Reset the stopwatch
*/
void Stopwatch::Reset()
{
	m_StartTime = SDL_GetPerformanceCounter();
}
