#include "tools.h"


/*
Linear interpolation between 2 values
Weight should be in the range 0..1
*/
float Tools::Interpolate(float Value1, float Value2, float Weight)
{
	return ((1.f - Weight) * Value1) + (Weight * Value2);
}

/*
Helper function to generate float values between a range
*/
float Tools::GetRandBetween(float Min, float Max)
{
	float r = float(rand()) / float(RAND_MAX);	// 0..1
	return r * (Max - Min) + Min;
}
