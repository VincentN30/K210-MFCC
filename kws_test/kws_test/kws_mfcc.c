#include <stdint.h>
#include <stdlib.h>
#include <kws_mfcc.h>
#include "float.h"
#include <math.h>

#define NUM_FBANK_BINS 40
#define MEL_LOW_FREQ 20
#define MEL_HIGH_FREQ 4000

#define M_2PI 6.283185307179586476925286766559005

#define M_PI 3.14159265358979323846264338327950288

float *kws_create_dct_matrix(int32_t input_length, int32_t coefficient_count);
float **kws_create_mel_fbank(kws_mfcc_t *mfcc);

static inline float InverseMelScale(float mel_freq)
{
	return 700.0f * (expf(mel_freq / 1127.0f) - 1.0f);
}

static inline float MelScale(float freq)
{
	return 1127.0f * logf(1.0f + freq / 700.0f);
}

static void *mfcc_malloc(size_t size)
{
	void *p = malloc(size);

	if (p == NULL)
		return NULL;

	memset(p, 0, size);
	return p;
}

static void mfcc_free(void *p)
{
	if (p != NULL)
		free(p);
}

kws_mfcc_t *kws_mfcc_create(int num_mfcc_features, int sample_freq, int feature_offset, int frame_len, int mfcc_dec_bits)
{
	kws_mfcc_t *mfcc;
	int i;
	mfcc = mfcc_malloc(sizeof(kws_mfcc_t));
	mfcc->sample_freq = sample_freq;
	mfcc->num_mfcc_features = num_mfcc_features;//10
	mfcc->num_features_offset = feature_offset;//1
	mfcc->frame_len = frame_len;//640
	mfcc->mfcc_dec_bits = mfcc_dec_bits;
	// Round-up to nearest power of 2.
	mfcc->frame_len_padded = powf(2, ceilf((logf(frame_len) / logf(2)))); //1024
	mfcc->frame = mfcc_malloc(sizeof(float) * mfcc->frame_len_padded);
	mfcc->buffer = mfcc_malloc(sizeof(float) * mfcc->frame_len_padded);
	mfcc->mel_energies = mfcc_malloc(sizeof(float) * NUM_FBANK_BINS);
	//create window function, hanning
	mfcc->window_func = mfcc_malloc(sizeof(float) * frame_len);

	for (i = 0; i < frame_len; i++)
		mfcc->window_func[i] = 0.5f - 0.5f * cosf((float)M_2PI * ((float)i) / (frame_len));

	//create mel filterbank
	mfcc->fbank_filter_first = mfcc_malloc(sizeof(int32_t) * NUM_FBANK_BINS);
	mfcc->fbank_filter_last = mfcc_malloc(sizeof(int32_t) * NUM_FBANK_BINS);
	mfcc->mel_fbank = kws_create_mel_fbank(mfcc);
	//create DCT matrix
	mfcc->dct_matrix = kws_create_dct_matrix(NUM_FBANK_BINS, num_mfcc_features);
	//initialize FFT
	mfcc->rfft = mfcc_malloc(sizeof(arm_rfft_fast_instance_f32));
	arm_rfft_fast_init_f32(mfcc->rfft, mfcc->frame_len_padded);
	return mfcc;
}


void kws_mfcc_delete(kws_mfcc_t *mfcc)
{
	int  i;
	mfcc_free(mfcc->frame);
	mfcc_free(mfcc->buffer);
	mfcc_free(mfcc->mel_energies);
	mfcc_free(mfcc->window_func);
	mfcc_free(mfcc->fbank_filter_first);
	mfcc_free(mfcc->fbank_filter_last);
	mfcc_free(mfcc->dct_matrix);
	mfcc_free(mfcc->rfft);

	for (i = 0; i < NUM_FBANK_BINS; i++)
		mfcc_free(mfcc->mel_fbank[i]);

	mfcc_free(mfcc->mel_fbank);
}

float *kws_create_dct_matrix(int32_t input_length, int32_t coefficient_count)
{
	int32_t k, n;
	float *M = mfcc_malloc(sizeof(float) * input_length * coefficient_count);
	float normalizer;
	arm_sqrt_f32(2.0f / (float)input_length, &normalizer);

	for (k = 0; k < coefficient_count; k++)
	{
		for (n = 0; n < input_length; n++)
		{
			M[k * input_length + n] = normalizer * cosf(((float)M_PI) / input_length * (n + 0.5f) * k);
		}
	}

	return M;
}

float **kws_create_mel_fbank(kws_mfcc_t *mfcc)
{
	int32_t bin, i;
	int32_t num_fft_bins = mfcc->frame_len_padded / 2;
	float fft_bin_width = ((float)mfcc->sample_freq) / mfcc->frame_len_padded;
	float mel_low_freq = MelScale(MEL_LOW_FREQ);
	float mel_high_freq = MelScale(MEL_HIGH_FREQ);
	float mel_freq_delta = (mel_high_freq - mel_low_freq) / (NUM_FBANK_BINS + 1);
	float *this_bin = mfcc_malloc(sizeof(float) * num_fft_bins);
	float **mel_fbank = mfcc_malloc(sizeof(float) * NUM_FBANK_BINS);

	for (bin = 0; bin < NUM_FBANK_BINS; bin++)
	{
		float left_mel = mel_low_freq + bin * mel_freq_delta;
		float center_mel = mel_low_freq + (bin + 1) * mel_freq_delta;
		float right_mel = mel_low_freq + (bin + 2) * mel_freq_delta;
		int32_t first_index = -1, last_index = -1;

		for (i = 0; i < num_fft_bins; i++)
		{
			float freq = (fft_bin_width * i); // center freq of this fft bin.
			float mel = MelScale(freq);
			this_bin[i] = 0.0;

			if (mel > left_mel && mel < right_mel)
			{
				float weight;

				if (mel <= center_mel)
				{
					weight = (mel - left_mel) / (center_mel - left_mel);
				}
				else
				{
					weight = (right_mel - mel) / (right_mel - center_mel);
				}

				this_bin[i] = weight;

				if (first_index == -1)
					first_index = i;

				last_index = i;
			}
		}

		mfcc->fbank_filter_first[bin] = first_index;
		mfcc->fbank_filter_last[bin] = last_index;
		mel_fbank[bin] = mfcc_malloc(sizeof(float) * (last_index - first_index + 1));
		int32_t j = 0;

		//copy the part we care about
		for (i = first_index; i <= last_index; i++)
		{
			mel_fbank[bin][j++] = this_bin[i];
		}
	}

	free(this_bin);
	return mel_fbank;
}

void kws_mfcc_compute(kws_mfcc_t *mfcc, const int16_t *audio_data, MFCC_OUT_T *mfcc_out)
{
	int32_t i, j, bin;

	//1. TensorFlow way of normalizing .wav data to (-1,1) and 2. do pre-emphasis.
	for (i = 0; i < mfcc->frame_len; i++)
	{
		mfcc->frame[i] = (float)audio_data[i] / (1 << 15); // original
	}

	//Fill up remaining with zeros
	memset(&mfcc->frame[mfcc->frame_len], 0, sizeof(float) * (mfcc->frame_len_padded - mfcc->frame_len));

	// windows filter
	for (i = 0; i < mfcc->frame_len; i++)
	{
		mfcc->frame[i] *= mfcc->window_func[i];
	}

	//Compute FFT
	arm_rfft_fast_f32(mfcc->rfft, mfcc->frame, mfcc->buffer, 0);
	//Convert to power spectrum
	//frame is stored as [real0, realN/2-1, real1, im1, real2, im2, ...]
	int32_t half_dim = mfcc->frame_len_padded / 2;
	float first_energy = mfcc->buffer[0] * mfcc->buffer[0];
	float last_energy = mfcc->buffer[1] * mfcc->buffer[1];  // handle this special case

	for (i = 1; i < half_dim; i++)
	{
		float real = mfcc->buffer[i * 2];
		float im = mfcc->buffer[i * 2 + 1];
		mfcc->buffer[i] = real * real + im * im;
	}

	mfcc->buffer[0] = first_energy;
	mfcc->buffer[half_dim] = last_energy;
	float sqmfcc_data;

	//Apply mel filterbanks
	for (bin = 0; bin < NUM_FBANK_BINS; bin++)
	{
		j = 0;
		float mel_energy = 0;
		int32_t first_index = mfcc->fbank_filter_first[bin];
		int32_t last_index = mfcc->fbank_filter_last[bin];

		for (i = first_index; i <= last_index; i++)
		{
			arm_sqrt_f32(mfcc->buffer[i], &sqmfcc_data);
			mel_energy += (sqmfcc_data)* mfcc->mel_fbank[bin][j++];
		}

		mfcc->mel_energies[bin] = mel_energy;

		//avoid log of zero
		if (mel_energy == 0.0f)
			mfcc->mel_energies[bin] = FLT_MIN;
	}

	//Take log
	for (bin = 0; bin < NUM_FBANK_BINS; bin++)
		mfcc->mel_energies[bin] = logf(mfcc->mel_energies[bin]);

	//Take DCT. Uses matrix mul.
	int out_index = 0;

	for (i = mfcc->num_features_offset; i < mfcc->num_mfcc_features; i++)
	{
		float sum = 0.0;

		for (j = 0; j < NUM_FBANK_BINS; j++)
		{
			sum += mfcc->dct_matrix[i * NUM_FBANK_BINS + j] * mfcc->mel_energies[j];
		}
#if MFCC_OUT_FMT == Q7_T
		//Input is Qx.mfcc_dec_bits (from quantization step)
		sum *= (0x1 << mfcc->mfcc_dec_bits);
		sum = round(sum);

		if (sum >= 127)
			mfcc_out[out_index] = 127;
		else
			if (sum <= -128)
				mfcc_out[out_index] = -128;
			else
				mfcc_out[out_index] = sum;
#elif MFCC_OUT_FMT == FLOAT
		mfcc_out[out_index] = sum;
#else
	#error error for MFCC OUT FMT
#endif
		out_index++;
	}
}
