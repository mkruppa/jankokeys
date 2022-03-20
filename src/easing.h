#pragma once

typedef double (*easing_function_t)(double t);

double easing_lerp(double t, float y1, float y2);
double easing_crossfade(double t, easing_function_t fn1, easing_function_t fn2);
double easing_mix(double t, double weight, easing_function_t fn1, easing_function_t fn2);
double easing_flip(double t);

double easing_smooth_start2(double t);
double easing_smooth_start3(double t);
double easing_smooth_start4(double t);
double easing_smooth_start5(double t);

double easing_smooth_stop2(double t);
double easing_smooth_stop3(double t);
double easing_smooth_stop4(double t);
double easing_smooth_stop5(double t);