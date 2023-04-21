#ifndef _ARM_MATH_H
#define _ARM_MATH_H
#ifdef   __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <math.h>
#include "cmsis_compiler.h"
#if  defined ( __GNUC__ )    
	#define  __ALIGNED(x) __attribute__((aligned(x)))
#else
	#define  __ALIGNED(x) 
#endif
/**
* @brief 8-bit fractional data type in 1.7 format.
*/
typedef int8_t q7_t;

/**
* @brief 16-bit fractional data type in 1.15 format.
*/
typedef int16_t q15_t;

/**
* @brief 32-bit fractional data type in 1.31 format.
*/
typedef int32_t q31_t;

/**
* @brief 64-bit fractional data type in 1.63 format.
*/
typedef int64_t q63_t;

/**
* @brief 32-bit floating-point type definition.
*/
typedef float float32_t;

/**
* @brief 64-bit floating-point type definition.
*/
typedef double float64_t;



typedef enum
{
    ARM_MATH_SUCCESS        =  0,        /**< No error */
    ARM_MATH_ARGUMENT_ERROR = -1,        /**< One or more arguments are incorrect */
    ARM_MATH_LENGTH_ERROR   = -2,        /**< Length of data buffer is incorrect */
    ARM_MATH_SIZE_MISMATCH  = -3,        /**< Size of matrices is not compatible with the operation */
    ARM_MATH_NANINF         = -4,        /**< Not-a-number (NaN) or infinity is generated */
    ARM_MATH_SINGULAR       = -5,        /**< Input matrix is singular and cannot be inverted */
    ARM_MATH_TEST_FAILURE   = -6         /**< Test Failed */
} arm_status;

__STATIC_FORCEINLINE arm_status arm_sqrt_f32(
	float32_t in,
	float32_t * pOut)
{
	if (in >= 0.0f)
	{
#if defined ( __CC_ARM )
#if defined __TARGET_FPU_VFP
		*pOut = __sqrtf(in);
#else
		*pOut = sqrtf(in);
#endif

#elif defined ( __ICCARM__ )
#if defined __ARMVFP__
		__ASM("VSQRT.F32 %0,%1" : "=t"(*pOut) : "t"(in));
#else
		*pOut = sqrtf(in);
#endif

#else
		*pOut = sqrtf(in);
#endif

		return (ARM_MATH_SUCCESS);
	}
	else
	{
		*pOut = 0.0f;
		return (ARM_MATH_ARGUMENT_ERROR);
	}
}
/**
\brief   Signed Saturate
\details Saturates a signed value.
\param [in]  value  Value to be saturated
\param [in]    sat  Bit position to saturate to (1..32)
\return             Saturated value
*/
__STATIC_FORCEINLINE int32_t __SSAT(int32_t val, uint32_t sat)
{
    if ((sat >= 1U) && (sat <= 32U))
    {
        const int32_t max = (int32_t)((1U << (sat - 1U)) - 1U);
        const int32_t min = -1 - max ;
        if (val > max)
        {
            return max;
        }
        else if (val < min)
        {
            return min;
        }
    }
    return val;
}

__STATIC_FORCEINLINE uint32_t __USAT(int32_t val, uint32_t sat)
{
    if (sat <= 31U)
    {
        const uint32_t max = ((1U << sat) - 1U);
        if (val > (int32_t)max)
        {
            return max;
        }
        else if (val < 0)
        {
            return 0U;
        }
    }
    return (uint32_t)val;
}


/**
* @brief Macros required for SINE and COSINE Fast math approximations
*/

#define FAST_MATH_TABLE_SIZE  512
#define FAST_MATH_Q31_SHIFT   (32 - 10)
#define FAST_MATH_Q15_SHIFT   (16 - 10)
#define CONTROLLER_Q31_SHIFT  (32 - 9)
#define TABLE_SPACING_Q31     0x400000
#define TABLE_SPACING_Q15     0x80


/**
 * @brief Instance structure for the floating-point CFFT/CIFFT function.
 */
typedef struct
{
    uint16_t fftLen;                   /**< length of the FFT. */
    const float32_t *pTwiddle;         /**< points to the Twiddle factor table. */
    const uint16_t *pBitRevTable;      /**< points to the bit reversal table. */
    uint16_t bitRevLength;             /**< bit reversal table length. */
#if defined(ARM_MATH_MVEF) && !defined(ARM_MATH_AUTOVECTORIZE)
    const uint32_t *rearranged_twiddle_tab_stride1_arr;        /**< Per stage reordered twiddle pointer (offset 1) */                                                       \
    const uint32_t *rearranged_twiddle_tab_stride2_arr;        /**< Per stage reordered twiddle pointer (offset 2) */                                                       \
    const uint32_t *rearranged_twiddle_tab_stride3_arr;        /**< Per stage reordered twiddle pointer (offset 3) */                                                       \
    const float32_t *rearranged_twiddle_stride1; /**< reordered twiddle offset 1 storage */                                                                   \
    const float32_t *rearranged_twiddle_stride2; /**< reordered twiddle offset 2 storage */                                                                   \
    const float32_t *rearranged_twiddle_stride3;
#endif
} arm_cfft_instance_f32;

  /**
   * @brief Instance structure for the floating-point CFFT/CIFFT function.
   */
  typedef struct
  {
          uint16_t fftLen;                   /**< length of the FFT. */
          uint8_t ifftFlag;                  /**< flag that selects forward (ifftFlag=0) or inverse (ifftFlag=1) transform. */
          uint8_t bitReverseFlag;            /**< flag that enables (bitReverseFlag=1) or disables (bitReverseFlag=0) bit reversal of output. */
    const float32_t *pTwiddle;               /**< points to the Twiddle factor table. */
    const uint16_t *pBitRevTable;            /**< points to the bit reversal table. */
          uint16_t twidCoefModifier;         /**< twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. */
          uint16_t bitRevFactor;             /**< bit reversal modifier that supports different size FFTs with the same bit reversal table. */
          float32_t onebyfftLen;             /**< value of 1/fftLen. */
  } arm_cfft_radix4_instance_f32;

/**
* @brief Instance structure for the floating-point RFFT/RIFFT function.
*/
typedef struct
{
    uint32_t fftLenReal;                        /**< length of the real FFT. */
    uint16_t fftLenBy2;                         /**< length of the complex FFT. */
    uint8_t ifftFlagR;                          /**< flag that selects forward (ifftFlagR=0) or inverse (ifftFlagR=1) transform. */
    uint8_t bitReverseFlagR;                    /**< flag that enables (bitReverseFlagR=1) or disables (bitReverseFlagR=0) bit reversal of output. */
    uint32_t twidCoefRModifier;                     /**< twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. */
    const float32_t *pTwiddleAReal;                   /**< points to the real twiddle factor table. */
    const float32_t *pTwiddleBReal;                   /**< points to the imag twiddle factor table. */
    arm_cfft_radix4_instance_f32 *pCfft;        /**< points to the complex FFT instance. */
} arm_rfft_instance_f32;

/**
 * @brief Instance structure for the floating-point RFFT/RIFFT function.
 */
typedef struct
{
    arm_cfft_instance_f32 Sint;      /**< Internal CFFT structure. */
    uint16_t fftLenRFFT;             /**< length of the real sequence */
    const float32_t * pTwiddleRFFT;        /**< Twiddle factors real stage  */
} arm_rfft_fast_instance_f32 ;

arm_status arm_rfft_fast_init_f32 (
    arm_rfft_fast_instance_f32 * S,
    uint16_t fftLen);


void arm_rfft_fast_f32(
    const arm_rfft_fast_instance_f32 * S,
    float32_t * p, float32_t * pOut,
    uint8_t ifftFlag);

      /**
   * @brief Instance structure for the Double Precision Floating-point CFFT/CIFFT function.
   */
  typedef struct
  {
          uint16_t fftLen;                   /**< length of the FFT. */
    const float64_t *pTwiddle;         /**< points to the Twiddle factor table. */
    const uint16_t *pBitRevTable;      /**< points to the bit reversal table. */
          uint16_t bitRevLength;             /**< bit reversal table length. */
  } arm_cfft_instance_f64;

#ifdef   __cplusplus
}
#endif

#endif //_ARM_MATH_H
