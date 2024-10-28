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

void firApplyBPF(FirData* fir, float freql, float freqr, float coreLen);//��Ȼ����bpf����ҲҪ�г��ȣ������üӴ���freq�ǹ�һ����
//���ù�һ��������������

void firAutoNormalization(FirData *fir);