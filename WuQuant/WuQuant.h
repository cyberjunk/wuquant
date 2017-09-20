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
   int vwt[TABLELENGTH];
   int vmr[TABLELENGTH];
   int vmg[TABLELENGTH];
   int vmb[TABLELENGTH];
   int vma[TABLELENGTH];
   float m2[TABLELENGTH];
   char  tag[TABLELENGTH];
   int volume[WORKARRAYSIZE];
   int volumeR[WORKARRAYSIZE];
   int volumeG[WORKARRAYSIZE];
   int volumeB[WORKARRAYSIZE];
   int volumeA[WORKARRAYSIZE];
   float volume2[WORKARRAYSIZE];
   int area[INDEXALPHACOUNT];
   int areaR[INDEXALPHACOUNT];
   int areaG[INDEXALPHACOUNT];
   int areaB[INDEXALPHACOUNT];
   int areaA[INDEXALPHACOUNT];
   float area2[INDEXALPHACOUNT];
   float vv[MAXCOLORS];
   Box   cube[MAXCOLORS];
} Quantizer;

EXPORT Quantizer* Create();
EXPORT void Destroy(Quantizer* quantizer);
EXPORT int Quantize(Quantizer* quantizer, unsigned int* image, unsigned int* palette, int* colorCount, int width, int height, char* destPixels, int padMultiple4);

