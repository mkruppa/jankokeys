#include "easing.h"

double easing_lerp(double t, float y1, float y2)
{
	return (y2 - y1) * t + y1;
}

double easing_crossfade(double t, easing_function_t fn1, easing_function_t fn2)
{
	return easing_lerp(t, fn1(t), fn2(t));
}

double easing_mix(double t, double weight, easing_function_t fn1, easing_function_t fn2)
{
	return easing_lerp(weight, fn1(t), fn2(t));
}

double easing_flip(double t)
{
	return 1 - t;
}

double easing_smooth_start2(double t)
{
	return t * t;
}

double easing_smooth_start3(double t)
{
	return t * t * t;
}

double easing_smooth_start4(double t)
{
	return t * t * t * t;
}

double easing_smooth_start5(double t)
{
	return t * t * t * t * t;
}

// double easing_smooth_stop2(double t) // 1 - (1 - t)^2
// {
// 	return easing_flip(easing_smooth_start2(easing_flip(t)));
// }

double easing_smooth_stop2(double t)
{
	return 1 - (1 - t) * (1 - t);
}

double easing_smooth_stop3(double t)
{
	return 1 - (1 - t) * (1 - t) * (1 - t);
}

double easing_smooth_stop4(double t)
{
	return 1 - (1 - t) * (1 - t) * (1 - t) * (1 - t);
}

double easing_smooth_stop5(double t)
{
	return 1 - (1 - t) * (1 - t) * (1 - t) * (1 - t) * (1 - t);
}