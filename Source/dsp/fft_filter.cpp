#include "fft_filter.h"
#define M_PI 3.1415926535897932384626

void FFTFilterInit(FFTFilter* dat, float* window)
{
	dat->window = window;
	dat->pos = 0;
	dat->posout = 0;
	memset(dat->inbuf, 0, sizeof(float) * FFTFilterSize);
	memset(dat->outbuf, 0, sizeof(float) * FFTFilterSize * 2);
	memset(dat->fftdat, 0, sizeof(complex_f32_t) * FFTFilterSize);
	memset(dat->core, 0, sizeof(complex_f32_t) * FFTFilterSize / 2);
}

void FFTFilterProc(FFTFilter* dat, const float* in, float* out, float numSamples)
{
	auto& inbuf = dat->inbuf;
	auto& outbuf = dat->outbuf;
	auto& core = dat->core;
	auto& window = dat->window;
	auto& fftdat = dat->fftdat;
	auto& pos = dat->pos;
	auto& posout = dat->posout;
	for (int i = 0; i < numSamples; ++i)
	{
		inbuf[pos] = in[i];
		out[i] = outbuf[posout];
		pos++;
		posout++;
		if (pos >= FFTFilterSize)pos = 0;
		if (posout >= FFTFilterSize * 2)posout = 0;
		if (pos % (FFTFilterSize / 4) == 0)//������ˣ�
		{//Ȼ�����fftdat����࣬Ȼ��Ӵ���Ȼ��fft��Ȼ���Ƶ�죬Ȼ��ifft��Ȼ��Ӵ���Ȼ����ӻ�outbuf
			for (int j = 0; j < FFTFilterSize; ++j)//��
			{
				fftdat[j].re = inbuf[(pos + j) % FFTFilterSize] * window[j];
				fftdat[j].im = 0;
			}
			fft_f32(fftdat, FFTFilterSize, 1);//fft
			for (int j = 0; j < FFTFilterSize / 2; ++j)
			{
				fftdat[j] = cmul(fftdat[j], core[j]);
				fftdat[FFTFilterSize - 1 - j] = cmul(fftdat[FFTFilterSize - 1 - j], core[j]);
			}
			fft_f32(fftdat, FFTFilterSize, -1);//ifft
			for (int j = 0; j < FFTFilterSize; ++j)//�Ӵ�
			{
				fftdat[j].re *= window[j];
			}
			int pos2 = posout;
			for (int j = 0; j < FFTFilterSize; ++j)
			{
				pos2++;
				outbuf[pos2 % (FFTFilterSize * 2)] += fftdat[j].re / 1.5 / FFTFilterSize;//�Ҽǵ�������1.5
			}
			for (int j = 0; j < FFTFilterSize / 4; ++j)
			{
				pos2++;
				outbuf[pos2 % (FFTFilterSize * 2)] = 0;//����Ҫ����һ�£�Ҫ��Ȼ����һֱ����
			}
		}
	}
}

