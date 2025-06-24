#ifndef UTILS_C
#define UTILS_C

#include <stdint.h>

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

#define KiloByte(n) (((u64)(n)) << 10)
#define MegaByte(n) (((u64)(n)) << 20)
#define GigaByte(n) (((u64)(n)) << 30)
#define TeraByte(n) (((u64)(n)) << 40)

#define Thousand(n) ((n)*1000)
#define Million(n)  ((n)*1000000)
#define Billion(n)  ((n)*1000000000)

#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))

#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))

#endif // UTILS_C
