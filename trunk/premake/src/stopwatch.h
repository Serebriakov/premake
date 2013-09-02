#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

#include "platformspecific.h"


/*
Helper class for measuring elapsed time
*/
class Stopwatch
{
protected :
	Uint64						m_StartTime;									// the time when the stopwatch was started
	double						m_CycleInterval;								// length of 1 cycle


public :
	Stopwatch(double CycleInterval = 1);

	void SetCycleInterval(double CycleInterval);

	// get the current value of the stopwatch in milliseconds
	double GetElapsedTime() const;

	// get the number of cycles that has passed since the last call to this function
	Uint64 GetCycles();

	// get the portion of the cycle interval that has passed
	double GetCycleState() const;

	// reset the stopwatch
	void Reset();
};

#endif
