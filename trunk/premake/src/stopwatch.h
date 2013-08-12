#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

#include "platformspecific.h"


/*
Helper class for measuring elapsed time
*/
class Stopwatch
{
protected :
	Uint32						m_StartTime;									// the time when the stopwatch was started
	Uint32						m_CycleInterval;								// length of 1 cycle


public :
	Stopwatch(Uint32 CycleInterval = 1);

	void SetCycleInterval(Uint32 CycleInterval);

	// get the current value of the stopwatch in milliseconds
	Uint32 GetElapsedTime() const;

	// get the number of cycles that has passed since the last call to this function
	Uint32 GetCycles();

	// get the portion of the cycle interval that has passed
	float GetCycleState() const;

	// reset the stopwatch
	void Reset();
};

#endif
