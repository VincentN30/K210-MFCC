#ifndef MFCC_H
#define MFCC_H

#include <arm_math.h>
#include "string.h"

#define MFCC_OUT_FMT FLOAT
#define MFCC_OUT_T	float

#define AUDIO_FREQ 16000

#define NUM_MFCC_COEFFS 10
#define FRAME_LEN 640
#define MFCC_DEC_BITS 1
#define NUM_FRAMES 49
#ifdef   __cplusplus
extern "C"
{
#endif
//40ms/frame, sample_freq=16K,frame_len=640,
typedef struct _mfcc_t{
    int num_mfcc_features;  //verctor dim
    int sample_freq;		//sampling frequency
	int num_features_offset;
    int frame_len;			//?
    int frame_len_padded;
    int mfcc_dec_bits;		// Q-format
	float preempha;
    float * frame;
    float * buffer;
    float * mel_energies;
    float * window_func;
    int32_t * fbank_filter_first;
    int32_t * fbank_filter_last;
    float ** mel_fbank;
    float * dct_matrix;
    arm_rfft_fast_instance_f32 * rfft;
} kws_mfcc_t;

kws_mfcc_t* kws_mfcc_create(int num_mfcc_features, int sample_freq,int feature_offset, int frame_len, int mfcc_dec_bits) ;
void kws_mfcc_delete(kws_mfcc_t* mfcc);
void kws_mfcc_compute(kws_mfcc_t *mfcc, const int16_t * audio_data, MFCC_OUT_T* mfcc_out);
//void get_mfcc_from_file(kws_mfcc_t *kws_mfcc,int16_t* audio, int fd, q7_t* mfcc_features);
#ifdef   __cplusplus
}
#endif

#endif
