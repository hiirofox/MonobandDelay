#pragma once

#include <math.h>

#define MaxDelaySize 96000
#define DelayTimeSmoothSize 4096

typedef struct
{
	float bufl[MaxDelaySize];
	float bufr[MaxDelaySize];
	int pos;
	float time;

	float smoothTime;
	float smoothBuf[DelayTimeSmoothSize];
	int smoothPos;
}Delay;

void DelayInit(Delay* dat);
void DelaySetTime(Delay* dat,float samples);
void DelayProcStereo(Delay* dat, const float* inbufl, const float* inbufr,
	float* outbufl, float* outbufr,int numSamples);