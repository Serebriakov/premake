#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <glm/glm.hpp>


/*
Class for helper functions
*/
class Tools
{
public :
	// linear interpolation between 2 values
	static float Interpolate(float Value1, float Value2, float Weight);

	// clamp value
	template<class T>
	static void ClampValue(T &Value, const T &Min, const T &Max)
	{
		if( Value < Min )
		{
			Value = Min;
		}
		else if( Value > Max )
		{
			Value = Max;
		}
	}

	// generate random float values between a range
	static float GetRandBetween(float Min, float Max);
};

#endif
