#include "stopwatch.h"


Stopwatch::Stopwatch(Uint32 CycleInterval)
{
	SetCycleInterval(CycleInterval);
	Reset();
}

void Stopwatch::SetCycleInterval(Uint32 CycleInterval)
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
Uint32 Stopwatch::GetElapsedTime() const
{
	return SDL_GetTicks() - m_StartTime;
}

/*
Get the number of cycles that has passed since the last call to this function
*/
Uint32 Stopwatch::GetCycles()
{
	Uint32 num_of_cycles = GetElapsedTime() / m_CycleInterval;
	m_StartTime += num_of_cycles * m_CycleInterval;
	return num_of_cycles;
}

/*
Get the portion of the cycle interval that has passed
*/
float Stopwatch::GetCycleState() const
{
	return (float)GetElapsedTime() / (float)m_CycleInterval;
}

/*
Reset the stopwatch
*/
void Stopwatch::Reset()
{
	m_StartTime = SDL_GetTicks();
}
