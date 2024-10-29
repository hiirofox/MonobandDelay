#include "delay.h"

void DelayInit(Delay* dat)
{
	for (int i = 0; i < MaxDelaySize; ++i)
	{
		dat->bufl[i] = 0;
		dat->bufr[i] = 0;
	}
	for (int i = 0; i < DelayTimeSmoothSize; ++i)
	{
		dat->smoothBuf[i] = 0;
	}
	dat->pos = 0;
	dat->smoothTime = 0;
	dat->smoothPos = 0;
	dat->time = 0;
}

void DelaySetTime(Delay* dat, float samples)
{
	dat->time = samples;
}

void DelayProcStereo(Delay* dat, const float* inbufl, const float* inbufr, float* outbufl, float* outbufr, int numSamples)
{
	auto& bufl = dat->bufl;
	auto& bufr = dat->bufr;
	auto& pos = dat->pos;
	auto& time = dat->time;
	auto& smoothBuf = dat->smoothBuf;
	auto& smoothTime = dat->smoothTime;
	auto& smoothPos = dat->smoothPos;

	for (int i = 0; i < numSamples; ++i)
	{
		int inpos = (int)(pos + smoothTime / DelayTimeSmoothSize) % MaxDelaySize;
		bufl[inpos] = inbufl[i];
		bufr[inpos] = inbufr[i];
		int outpos = pos % MaxDelaySize;
		outbufl[i] = bufl[outpos];
		outbufr[i] = bufr[outpos];
		pos++;

		float oldtime = smoothBuf[smoothPos];
		smoothBuf[smoothPos] = time;
		smoothPos = (smoothPos + 1) % DelayTimeSmoothSize;
		smoothTime += time - oldtime;
	}
}



