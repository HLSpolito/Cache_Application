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



#include "cache_only.h"
typedef CACHE_ONLY::Cache<unsigned char, 2, 7> CacheM;
typedef CACHE_ONLY::Cache<unsigned char, 0, 7> CacheN;
typedef CACHE_ONLY::Cache<float, 0, 4> CacheF;

extern "C" void knp(CacheM::DataType *im1,
		CacheN::DataType *im2,
		CacheF::DataType *  out
		);
