#ifndef __CMSIS_GCC_H
#define __CMSIS_GCC_H

#ifndef   __INLINE
  #define __INLINE                               inline
#endif
#ifndef   __STATIC_INLINE
  #define __STATIC_INLINE                        static inline
#endif

#if  defined ( __GNUC__ )               
  #define __STATIC_FORCEINLINE                   __attribute__((always_inline)) static inline
#else
#define __STATIC_FORCEINLINE					__STATIC_INLINE
#endif

#endif //__CMSIS_GCC_H
