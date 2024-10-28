#pragma once

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "fft.h"

#define FFTFilterSize 2048

typedef struct
{
	float inbufl[FFTFilterSize];
	float outbufl[FFTFilterSize * 2];
	float inbufr[FFTFilterSize];
	float outbufr[FFTFilterSize * 2];
	complex_f32_t fftdatl[FFTFilterSize];
	complex_f32_t fftdatr[FFTFilterSize];
	complex_f32_t core[FFTFilterSize / 2];
	float* window;
	int posout;
	int pos;
}FFTFilter;

void FFTFilterInit(FFTFilter* dat, float* window);
void FFTFilterProc(FFTFilter* dat, const float* in, float* out, float numSamples);
void FFTFilterProcStereo(FFTFilter* dat, const float* inl, const float* inr,
	float* outl, float* outr, float numSamples);
