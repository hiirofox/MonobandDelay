#pragma once

#include <math.h>
#include "fft_filter.h"
#include "delay.h"

#define M_PI 3.1415926535897932384626
#define MultibandNum 48

typedef struct
{
	FFTFilter filt[MultibandNum];
	Delay delay[MultibandNum];
	float window[FFTFilterSize];//��������ֱ��������ʵ����
	int num;

	float tmpbufl[16384];//����������ŪСһ��
	float tmpbufr[16384];
	float tmpbufl2[16384];
	float tmpbufr2[16384];
}Multiband;

void MultibandInit(Multiband* dat);
void MultibandProcStereo(Multiband* dat, const float* inbufl, const float* inbufr,
	float* outbufl, float* outbufr, int numSamples);

//test:
void MultibandSetLinearArrangement(Multiband* dat, float minSamples, float maxSamples);//��С���ӳ����Ե������ӳ�
void MultibandSetFilterFreq(Multiband* dat);