#pragma once

#if defined(_MSC_VER)
//  Microsoft
#include <intrin.h>
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#define SIMD_TYPE __declspec(intrin_type) __declspec(align(16))
#elif defined(__GNUC__)
//  GCC
#include <x86intrin.h>
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#define SIMD_TYPE __attribute__((aligned(16)))
#define __forceinline inline __attribute__((always_inline))
#endif


#define MAXCOLORS       256
#define INDEXBITS       6
#define INDEXALPHABITS  3
#define INDEXCOUNT      ((1 << INDEXBITS) + 1)
#define INDEXALPHACOUNT ((1 << INDEXALPHABITS) + 1)
#define WORKARRAYSIZE   (INDEXCOUNT * INDEXALPHACOUNT)
#define TABLELENGTH     (INDEXCOUNT * INDEXCOUNT * INDEXCOUNT * INDEXALPHACOUNT)

typedef union SIMD_TYPE V4i
{
   __m128i SSE;
   struct { int R; int G; int B; int A; };
} V4i;

typedef union SIMD_TYPE V4f
{
   __m128 SSE;
   struct { float R; float G; float B; float A; };
} V4f;

typedef struct
{
   V4i P0;
   V4i P1;
   int Volume;
   float vv;
} Box;

typedef struct
{
   V4i P;
   int V;
   float V2;
} Moment;

typedef struct
{
   Moment v[TABLELENGTH];
   char   tag[TABLELENGTH];
   Box    cube[MAXCOLORS];
   Moment volume[WORKARRAYSIZE];
   Moment area[INDEXALPHACOUNT];
} Quantizer;


EXPORT Quantizer* Create();
EXPORT void Destroy(Quantizer* quantizer);
EXPORT int Quantize(Quantizer* quantizer, unsigned int* image, unsigned int* palette, int* colorCount, int width, int height, char* destPixels, int padMultiple4);

