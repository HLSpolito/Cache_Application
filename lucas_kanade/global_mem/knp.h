#pragma once

#include <math.h>
#include <stdio.h>
#include "param.h"


//2.0 #define of constant variables
#ifndef M_PI
#define M_PI 3.1415926535f
#endif
// Flow vector scaling factor
#define FLOW_SCALING_FACTOR (0.25f)  //(1.0f/4.0f)

typedef int mType;



extern "C" void knp(
		unsigned char * im1,
		unsigned char * im2,
		float * out
		);
