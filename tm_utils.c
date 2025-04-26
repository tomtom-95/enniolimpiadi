#ifndef TM_UTILS_C
#define TM_UTILS_C

#include <stdint.h>

#define KiloByte(x) (((uint64_t)(1024ULL))*x)
#define MegaByte(x) (((uint64_t)(1024ULL*1024ULL))*x)
#define GigaByte(x) (((uint64_t)(1024ULL*1024ULL*1024ULL))*x)

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef float    f32;
typedef double   f64;

#endif // TM_UTILS_C
