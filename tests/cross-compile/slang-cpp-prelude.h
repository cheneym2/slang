#ifndef SLANG_CPP_PRELUDE_H
#define SLANG_CPP_PRELUDE_H

#include <inttypes.h>
#include <math.h>
#include <inttypes.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#ifndef SLANG_FORCE_INLINE
#   define SLANG_FORCE_INLINE inline
#endif

#ifndef SLANG_PRELUDE_PI
#   define SLANG_PRELUDE_PI           3.14159265358979323846
#endif

#if defined(_MSC_VER)
#   define SLANG_PRELUDE_SHARED_LIB_EXPORT __declspec(dllexport)
#else
#   define SLANG_PRELUDE_SHARED_LIB_EXPORT __attribute__((__visibility__("default")))
//#   define SLANG_PRELUDE_SHARED_LIB_EXPORT __attribute__ ((dllexport)) __attribute__((__visibility__("default")))
#endif    

#ifdef __cplusplus    
#   define SLANG_PRELUDE_EXTERN_C extern "C"
#else
#   define SLANG_PRELUDE_EXTERN_C 
#endif    

#define SLANG_PRELUDE_EXPORT SLANG_PRELUDE_EXTERN_C SLANG_PRELUDE_SHARED_LIB_EXPORT

#ifdef SLANG_PRELUDE_NAMESPACE
namespace SLANG_PRELUDE_NAMESPACE {
#endif

template <typename T, size_t SIZE>
struct FixedArray
{
    const T& operator[](size_t index) const { assert(index < SIZE); return m_data[index]; }
    T& operator[](size_t index) { assert(index < SIZE); return m_data[index]; }
    
    T m_data[SIZE];
};

template <typename T>
struct RWStructuredBuffer
{
    T& operator[](size_t index) const { return data[index]; }
    
    T* data;
    size_t count;
};

template <typename T, int COUNT>
struct Vector;

template <typename T>
struct Vector<T, 1>
{
    T x;
};

template <typename T>
struct Vector<T, 2>
{
    T x, y;
};

template <typename T>
struct Vector<T, 3>
{
    T x, y, z;
};

template <typename T>
struct Vector<T, 4>
{
    T x, y, z, w;
};

template <typename T, int ROWS, int COLS>
struct Matrix
{
    Vector<T, COLS> rows[ROWS];
};

// ----------------------------- F32 -----------------------------------------

union Union32 
{
    uint32_t u;
    int32_t i;
    float f;
};

// Helpers
SLANG_FORCE_INLINE float F32_calcSafeRadians(float radians)
{
	float a = radians * (1.0f /  float(SLANG_PRELUDE_PI));
	a = (a < 0.0f) ? (::ceilf(a) - a) : (a - ::floorf(a));
	return (a * float(SLANG_PRELUDE_PI));
}

// Unary 
SLANG_FORCE_INLINE float F32_ceil(float f) { return ::ceilf(f); }
SLANG_FORCE_INLINE float F32_floor(float f) { return ::floorf(f); }
SLANG_FORCE_INLINE float F32_sin(float f) { return ::sinf(F32_calcSafeRadians(f)); }
SLANG_FORCE_INLINE float F32_cos(float f) { return ::cosf(F32_calcSafeRadians(f)); }
SLANG_FORCE_INLINE float F32_tan(float f) { return ::tanf(f); }
SLANG_FORCE_INLINE float F32_asin(float f) { return ::asinf(f); }
SLANG_FORCE_INLINE float F32_acos(float f) { return ::acosf(f); }
SLANG_FORCE_INLINE float F32_atan(float f) { return ::atanf(f); }
SLANG_FORCE_INLINE float F32_log2(float f) { return ::log2f(f); }
SLANG_FORCE_INLINE float F32_exp2(float f) { return ::exp2f(f); }
SLANG_FORCE_INLINE float F32_exp(float f) { return ::expf(f); }
SLANG_FORCE_INLINE float F32_abs(float f) { return ::fabsf(f); }
SLANG_FORCE_INLINE float F32_trunc(float f) { return ::truncf(f); }
SLANG_FORCE_INLINE float F32_sqrt(float f) { return ::sqrtf(f); }
SLANG_FORCE_INLINE float F32_rsqrt(float f) { return 1.0f / F32_sqrt(f); }
SLANG_FORCE_INLINE float F32_rcp(float f) { return 1.0f / f; }
SLANG_FORCE_INLINE float F32_sign(float f) { return ( f == 0.0f) ? f : (( f < 0.0f) ? -1.0f : 1.0f); } 
SLANG_FORCE_INLINE float F32_saturate(float f) { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; }
SLANG_FORCE_INLINE float F32_frac(float f) { return f - F32_floor(f); }
SLANG_FORCE_INLINE float F32_radians(float f) { return f * 0.01745329222f; }

// Binary
SLANG_FORCE_INLINE float F32_min(float a, float b) { return a < b ? a : b; }
SLANG_FORCE_INLINE float F32_max(float a, float b) { return a > b ? a : b; }
SLANG_FORCE_INLINE float F32_pow(float a, float b) { return ::powf(a, b); }
SLANG_FORCE_INLINE float F32_fmod(float a, float b) { return ::fmodf(a, b); }
SLANG_FORCE_INLINE float F32_step(float a, float b) { return float(a >= b); }
SLANG_FORCE_INLINE float F32_atan2(float a, float b) { return float(atan2(a, b)); }

// Ternary 
SLANG_FORCE_INLINE float F32_smoothstep(float min, float max, float x) { return x < min ? min : ((x > max) ? max : x / (max - min)); }
SLANG_FORCE_INLINE float F32_lerp(float x, float y, float s) { return x + s * (y - x); }
SLANG_FORCE_INLINE float F32_clamp(float x, float min, float max) { return ( x < min) ? min : ((x > max) ? max : x); }
SLANG_FORCE_INLINE void F32_sincos(float f, float& outSin, float& outCos) { outSin = F32_sin(f); outCos = F32_cos(f); }

SLANG_FORCE_INLINE uint32_t F32_asuint(float f) { Union32 u; u.f = f; return u.u; }
SLANG_FORCE_INLINE int32_t F32_asint(float f) { Union32 u; u.f = f; return u.i; }

// ----------------------------- F64 -----------------------------------------

SLANG_FORCE_INLINE double F64_calcSafeRadians(double radians)
{
    double a = radians * (1.0 / SLANG_PRELUDE_PI);
    a = (a < 0.0) ? (::ceil(a) - a) : (a - ::floor(a));
    return (a * SLANG_PRELUDE_PI);
}

// Unary 
SLANG_FORCE_INLINE double F64_ceil(double f) { return ::ceil(f); }
SLANG_FORCE_INLINE double F64_floor(double f) { return ::floor(f); }
SLANG_FORCE_INLINE double F64_sin(double f) { return ::sin(F64_calcSafeRadians(f)); }
SLANG_FORCE_INLINE double F64_cos(double f) { return ::cos(F64_calcSafeRadians(f)); }
SLANG_FORCE_INLINE double F64_tan(double f) { return ::tan(f); }
SLANG_FORCE_INLINE double F64_asin(double f) { return ::asin(f); }
SLANG_FORCE_INLINE double F64_acos(double f) { return ::acos(f); }
SLANG_FORCE_INLINE double F64_atan(double f) { return ::atan(f); }
SLANG_FORCE_INLINE double F64_log2(double f) { return ::log2(f); }
SLANG_FORCE_INLINE double F64_exp2(double f) { return ::exp2(f); }
SLANG_FORCE_INLINE double F64_exp(double f) { return ::exp(f); }
SLANG_FORCE_INLINE double F64_abs(double f) { return ::fabs(f); }
SLANG_FORCE_INLINE double F64_trunc(double f) { return ::trunc(f); }
SLANG_FORCE_INLINE double F64_sqrt(double f) { return ::sqrt(f); }
SLANG_FORCE_INLINE double F64_rsqrt(double f) { return 1.0 / F64_sqrt(f); }
SLANG_FORCE_INLINE double F64_rcp(double f) { return 1.0 / f; }
SLANG_FORCE_INLINE double F64_sign(double f) { return (f == 0.0) ? f : ((f < 0.0) ? -1.0 : 1.0); }
SLANG_FORCE_INLINE double F64_saturate(double f) { return (f < 0.0) ? 0.0 : (f > 1.0) ? 1.0 : f; }
SLANG_FORCE_INLINE double F64_frac(double f) { return f - F64_floor(f); }
SLANG_FORCE_INLINE double F64_radians(double f) { return f * 0.01745329222; }

// Binary
SLANG_FORCE_INLINE double F64_min(double a, double b) { return a < b ? a : b; }
SLANG_FORCE_INLINE double F64_max(double a, double b) { return a > b ? a : b; }
SLANG_FORCE_INLINE double F64_pow(double a, double b) { return ::pow(a, b); }
SLANG_FORCE_INLINE double F64_fmod(double a, double b) { return ::fmod(a, b); }
SLANG_FORCE_INLINE double F64_step(double a, double b) { return double(a >= b); }
SLANG_FORCE_INLINE double F64_atan2(double a, double b) { return atan2(a, b); }

// Ternary 
SLANG_FORCE_INLINE double F64_smoothstep(double min, double max, double x) { return x < min ? min : ((x > max) ? max : x / (max - min)); }
SLANG_FORCE_INLINE double F64_lerp(double x, double y, double s) { return x + s * (y - x); }
SLANG_FORCE_INLINE double F64_clamp(double x, double min, double max) { return (x < min) ? min : ((x > max) ? max : x); }
SLANG_FORCE_INLINE void F64_sincos(double f, double& outSin, double& outCos) { outSin = F64_sin(f); outCos = F64_cos(f); }

// TODO!
//uint32_t F64_asuint(float f);
//int32_t F64_asint(float f);

// ----------------------------- I32 -----------------------------------------

SLANG_FORCE_INLINE int32_t I32_abs(int32_t f) { return (f < 0) ? -f : f; }

SLANG_FORCE_INLINE int32_t I32_min(int32_t a, int32_t b) { return a < b ? a : b; }
SLANG_FORCE_INLINE int32_t I32_max(int32_t a, int32_t b) { return a > b ? a : b; }

SLANG_FORCE_INLINE int32_t I32_clamp(int32_t x, int32_t min, int32_t max) { return ( x < min) ? min : ((x > max) ? max : x); }

SLANG_FORCE_INLINE float I32_asfloat(int32_t x) { Union32 u; u.i = x; return u.f; }

// ----------------------------- U32 -----------------------------------------

SLANG_FORCE_INLINE uint32_t U32_abs(uint32_t f) { return f; }

SLANG_FORCE_INLINE uint32_t U32_min(uint32_t a, uint32_t b) { return a < b ? a : b; }
SLANG_FORCE_INLINE uint32_t U32_max(uint32_t a, uint32_t b) { return a > b ? a : b; }

SLANG_FORCE_INLINE uint32_t U32_clamp(uint32_t x, uint32_t min, uint32_t max) { return ( x < min) ? min : ((x > max) ? max : x); }

SLANG_FORCE_INLINE float U32_asfloat(uint32_t x) { Union32 u; u.u = x; return u.f; }

#ifdef SLANG_PRELUDE_NAMESPACE
} 
#endif

#endif
