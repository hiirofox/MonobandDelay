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
}

void MultibandProcStereo(Multiband* dat, const float* inbufl, const float* inbufr, float* outbufl, float* outbufr, int numSamples)
{
	auto& filt = dat->filt;
	auto& delay = dat->delay;
	auto& num = dat->num;
	auto& tmpbufl = dat->tmpbufl;
	auto& tmpbufr = dat->tmpbufr;

	for (int i = 0; i < numSamples; ++i)
	{
		outbufl[i] = 0;
		outbufr[i] = 0;
	}
	for (int i = 0; i < num; ++i)
	{
		FFTFilterProcStereo(&filt[i], inbufl, inbufr, tmpbufl, tmpbufr, numSamples);
		//DelayProcStereo(&delay[i], tmpbufl, tmpbufr, tmpbufl, tmpbufr, numSamples);
		for (int j = 0; j < numSamples; ++j)
		{
			outbufl[j] += tmpbufl[j];//!
			outbufr[j] += tmpbufr[j];
		}
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
	for (int i = 0; i < num; ++i)
	{
		FFTFilterApplyBPF(&filt[i], (float)i / num, (float)(i + 1) / num);
	}
}
