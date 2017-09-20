#pragma once

#if defined(_MSC_VER)
//  Microsoft 
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
//  do nothing and hope for the best?
#define EXPORT
#define IMPORT
#pragma warning Unknown dynamic link import/export semantics.
#endif

#define MAXCOLORS       256
#define INDEXBITS       6
#define INDEXALPHABITS  3
#define INDEXCOUNT      ((1 << INDEXBITS) + 1)
#define INDEXALPHACOUNT ((1 << INDEXALPHABITS) + 1)
#define WORKARRAYSIZE   (INDEXCOUNT * INDEXALPHACOUNT)
#define TABLELENGTH     (INDEXCOUNT * INDEXCOUNT * INDEXCOUNT * INDEXALPHACOUNT)

typedef struct
{
   int R0;
   int R1;
   int G0;
   int G1;
   int B0;
   int B1;
   int A0;
   int A1;
   int Volume;
} Box;

typedef struct
{
   long long vwt[TABLELENGTH];
   long long vmr[TABLELENGTH];
   long long vmg[TABLELENGTH];
   long long vmb[TABLELENGTH];
   long long vma[TABLELENGTH];
   double    m2[TABLELENGTH];
   char      tag[TABLELENGTH];
   long long volume[WORKARRAYSIZE];
   long long volumeR[WORKARRAYSIZE];
   long long volumeG[WORKARRAYSIZE];
   long long volumeB[WORKARRAYSIZE];
   long long volumeA[WORKARRAYSIZE];
   double    volume2[WORKARRAYSIZE];
   long long area[INDEXALPHACOUNT];
   long long areaR[INDEXALPHACOUNT];
   long long areaG[INDEXALPHACOUNT];
   long long areaB[INDEXALPHACOUNT];
   long long areaA[INDEXALPHACOUNT];
   double    area2[INDEXALPHACOUNT];
   double    vv[MAXCOLORS];
   Box       cube[MAXCOLORS];
} Quantizer;

EXPORT Quantizer* Create();
EXPORT void Destroy(Quantizer* quantizer);
EXPORT int Quantize(Quantizer* quantizer, unsigned int* image, unsigned int* palette, int* colorCount, int width, int height, char* destPixels, int padMultiple4);

