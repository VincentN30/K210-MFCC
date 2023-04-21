/*
 * Date           Author       Notes
 * 2020-5-5     liqiwen   first version
 */

#include <stdint.h>
#include <stdio.h>
#include <arm_math.h>
#include <string.h>
#include <stdlib.h>
#include "kws_mfcc.h"
#include "dataset.h"
#include "commands_dt.h"
#include <rtthread.h>

MFCC_OUT_T mfcc_features[NUM_FRAMES*NUM_MFCC_COEFFS]={0.0};

kws_mfcc_t *kws_mfcc;

int kws_init ( void )
{
    kws_mfcc = kws_mfcc_create ( NUM_MFCC_COEFFS, AUDIO_FREQ, 0, FRAME_LEN, MFCC_DEC_BITS );
    return 0;
}

int kws_input ( const int16_t *audio )
{
    memcpy ( mfcc_features, &mfcc_features[NUM_MFCC_COEFFS], sizeof(float)*( NUM_FRAMES - 1 ) *NUM_MFCC_COEFFS );
    kws_mfcc_compute ( kws_mfcc, &audio[0], &mfcc_features[ ( NUM_FRAMES - 1 ) *NUM_MFCC_COEFFS] ); //40MS
    return 0;
}
