#include "multiband.h"

void MultibandInit(Multiband* dat)
{
	for (int i = 0; i < FFTFilterSize; ++i)//´°º¯Êý
	{
		dat->window[i] = 0.5 - 0.5 * cosf((float)i / FFTFilterSize * 2.0 * M_PI);
	}
	for (int i = 0; i < MultibandNum; ++i)
	{
		FFTFilterInit(&dat->filt[i], dat->window);
		DelayInit(&dat->delay[i]);
	}
	dat->num = MultibandNum;
	for (int i = 0; i < 16384; ++i)
	{
		dat->tmpbufl[i] = 0;
		dat->tmpbufr[i] = 0;
		dat->tmpbufl2[i] = 0;
		dat->tmpbufr2[i] = 0;
	}
}

void MultibandProcStereo(Multiband* dat, const float* inbufl, const float* inbufr, float* outbufl, float* outbufr, int numSamples)
{
	auto& filt = dat->filt;
	auto& delay = dat->delay;
	auto& num = dat->num;
	auto& tmpbufl = dat->tmpbufl;
	auto& tmpbufr = dat->tmpbufr;
	auto& tmpbufl2 = dat->tmpbufl2;
	auto& tmpbufr2 = dat->tmpbufr2;

	for (int i = 0; i < numSamples; ++i)
	{
		tmpbufl2[i] = 0;
		tmpbufr2[i] = 0;
	}
	for (int i = 0; i < num; ++i)
	{
		FFTFilterProcStereo(&filt[i], inbufl, inbufr, tmpbufl, tmpbufr, numSamples);
		DelayProcStereo(&delay[i], tmpbufl, tmpbufr, tmpbufl, tmpbufr, numSamples);
		for (int j = 0; j < numSamples; ++j)
		{
			tmpbufl2[j] += tmpbufl[j];//!
			tmpbufr2[j] += tmpbufr[j];
		}
	}
	for (int i = 0; i < numSamples; ++i)
	{
		outbufl[i] = tmpbufl2[i];
		outbufr[i] = tmpbufr2[i];
	}
}

void MultibandSetLinearArrangement(Multiband* dat, float minSamples, float maxSamples)
{
	auto& filt = dat->filt;
	auto& delay = dat->delay;
	auto& num = dat->num;
	auto& tmpbufl = dat->tmpbufl;
	auto& tmpbufr = dat->tmpbufr;
	for (int i = 0; i < num; ++i)
	{
		DelaySetTime(&delay[i], minSamples + (maxSamples - minSamples) * i / (num - 1));
	}
}

void MultibandSetFilterFreq(Multiband* dat)
{
	auto& filt = dat->filt;
	auto& delay = dat->delay;
	auto& num = dat->num;
	auto& tmpbufl = dat->tmpbufl;
	auto& tmpbufr = dat->tmpbufr;
	for (int i = 1; i < num; ++i)
	{
		float l = (float)(i - 1) / (num - 1);
		float r = (float)(i) / (num - 1);
		l = expf((l - 1.0) * 7.0);
		r = expf((r - 1.0) * 7.0);
		if (i > num / 3) l += 1.0 / FFTFilterSize * 2.0;

		FFTFilterApplyBPF(&filt[i], l, r);
	}
	FFTFilterApplyBPF(&filt[0], 0, expf(-7.0));
}
