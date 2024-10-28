#pragma once

#include <JuceHeader.h>
#include <math.h>

#define MaxFirCoreLen 2048

typedef struct
{
	float bufl[MaxFirCoreLen];
	float bufr[MaxFirCoreLen];
	float core[MaxFirCoreLen];
	int len = 0;
	int pos = 0;
	float normul = 1.0;
}FirData;

void firInit(FirData* fir);
void firSetCore(FirData* fir, float* core, int coreLen);
void firProc(FirData* fir, float* input, float* output, int numSamples);
void firProcStereo(FirData* fir, const float* inputl, const float* inputr,
	float* outputl, float* outputr, int numSamples);

void firApplyBPF(FirData* fir, float freql, float freqr, float coreLen);//虽然设置bpf但是也要有长度，这样好加窗。freq是归一化的
//不用归一化！！！！！！

void firAutoNormalization(FirData *fir);