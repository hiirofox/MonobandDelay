#pragma once

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "fft.h"

#define FFTFilterSize 2048

typedef struct
{
	float inbuf[FFTFilterSize];
	float outbuf[FFTFilterSize * 2];
	complex_f32_t fftdat[FFTFilterSize];
	complex_f32_t core[FFTFilterSize / 2];
	float* window;
	int posout;
	int pos;
}FFTFilter;

void FFTFilterInit(FFTFilter* dat,float *window);
void FFTFilterProc(FFTFilter* dat, const float* in, float* out, float numSamples);
