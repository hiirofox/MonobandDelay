#include "fir.h"

#define M_PI 3.1415926535897932384626

void firInit(FirData* fir)
{
	for (int i = 0; i < MaxFirCoreLen; ++i)
	{
		fir->bufl[i] = 0;
		fir->bufr[i] = 0;
		fir->core[i] = 0;
	}
	fir->len = 0;
	fir->pos = 0;
}

void firSetCore(FirData* fir, float* core, int coreLen)
{
	if (coreLen > MaxFirCoreLen)coreLen = MaxFirCoreLen;
	for (int i = 0; i < coreLen; ++i)
	{
		fir->core[i] = core[i];
	}
	fir->len = coreLen;
}

void firProc(FirData* fir, float* input, float* output, int numSamples)
{
	for (int i = 0; i < numSamples; ++i)
	{
		fir->bufl[fir->pos] = input[i];

		float suml = 0;
		suml = std::inner_product(&fir->core[fir->len - 1 - fir->pos], &fir->core[fir->len - 1], &fir->bufl[0], suml);
		suml = std::inner_product(&fir->core[0], &fir->core[fir->len - 1 - fir->pos], &fir->bufl[fir->pos], suml);

		fir->pos++;
		if (fir->pos >= fir->len)fir->pos = 0;

		output[i] = suml;
	}
}

void firProcStereo(FirData* fir, const float* inputl, const float* inputr, float* outputl, float* outputr, int numSamples)
{
	auto& pos = fir->pos;
	auto& len = fir->len;
	auto& bufl = fir->bufl;
	auto& bufr = fir->bufr;
	auto& core = fir->core;
	auto& normul = fir->normul;
	for (int i = 0; i < numSamples; ++i)
	{
		bufl[pos] = inputl[i];
		bufr[pos] = inputr[i];

		float suml = 0;
		float sumr = 0;

		int start = (MaxFirCoreLen + pos - len + 1) % MaxFirCoreLen;
		for (int j = 0; j < len; ++j)
		{
			suml += bufl[start] * core[j];
			sumr += bufr[start] * core[j];
			start = (start + 1) % MaxFirCoreLen;
		}

		pos++;
		if (pos >= MaxFirCoreLen)pos = 0;
		outputl[i] = suml;
		outputr[i] = sumr;
	}
}

float sincf(float x)
{
	return (x != 0.0) ? (sinf(x * M_PI) / (x * M_PI)) : 1.0;
}
void firApplyBPF(FirData* fir, float freqLow, float freqHigh, float coreLen)
{
	if (coreLen > MaxFirCoreLen)coreLen = MaxFirCoreLen;
	int halflen = coreLen / 2;
	for (int i = 1; i < halflen; ++i)
	{
		float x = (float)i;
		float y = sincf(x * freqHigh) * freqHigh - sincf(x * freqLow) * freqLow;
		y *= cosf(x * M_PI / coreLen) * 0.46 + 0.54;//hamming
		fir->core[halflen - 1 - i] = y;
		fir->core[halflen - 1 + i] = y;
	}
	//归一化已在其他地方处理
	fir->core[halflen - 1] = freqHigh - freqLow;//防爆
	fir->len = coreLen;
}

void firAutoNormalization(FirData* fir)//bpf不用
{
	auto& len = fir->len;
	auto& core = fir->core;
	auto& normul = fir->normul;
	for (int i = 0; i < len; ++i)
	{
		normul += core[i] * core[i];
	}
	if (normul != 0.0) normul = 1.0 / sqrtf(normul);

	for (int i = 0; i < len; ++i)
	{
		core[i] *= normul;
	}
}
