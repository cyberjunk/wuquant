
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "WuQuant.h"

__forceinline static int GetIndex(const int r, const int g, const int b, const int a)
{
   return (r << ((INDEXBITS * 2) + INDEXALPHABITS))
      + (r << (INDEXBITS + INDEXALPHABITS + 1))
      + (g << (INDEXBITS + INDEXALPHABITS))
      + (r << (INDEXBITS * 2))
      + (r << (INDEXBITS + 1))
      + (g << INDEXBITS)
      + ((r + g + b) << INDEXALPHABITS)
      + r + g + b + a;
}

__forceinline static int BottomR(const Box* cube, const int* moment)
{
   return -moment[GetIndex(cube->R0, cube->G1, cube->B1, cube->A1)]
      + moment[GetIndex(cube->R0, cube->G1, cube->B1, cube->A0)]
      + moment[GetIndex(cube->R0, cube->G1, cube->B0, cube->A1)]
      - moment[GetIndex(cube->R0, cube->G1, cube->B0, cube->A0)]
      + moment[GetIndex(cube->R0, cube->G0, cube->B1, cube->A1)]
      - moment[GetIndex(cube->R0, cube->G0, cube->B1, cube->A0)]
      - moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A1)]
      + moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A0)];
}
__forceinline static int BottomG(const Box* cube, const int* moment)
{
   return -moment[GetIndex(cube->R1, cube->G0, cube->B1, cube->A1)]
      + moment[GetIndex(cube->R1, cube->G0, cube->B1, cube->A0)]
      + moment[GetIndex(cube->R1, cube->G0, cube->B0, cube->A1)]
      - moment[GetIndex(cube->R1, cube->G0, cube->B0, cube->A0)]
      + moment[GetIndex(cube->R0, cube->G0, cube->B1, cube->A1)]
      - moment[GetIndex(cube->R0, cube->G0, cube->B1, cube->A0)]
      - moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A1)]
      + moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A0)];
}
__forceinline static int BottomB(const Box* cube, const int* moment)
{
   return -moment[GetIndex(cube->R1, cube->G1, cube->B0, cube->A1)]
      + moment[GetIndex(cube->R1, cube->G1, cube->B0, cube->A0)]
      + moment[GetIndex(cube->R1, cube->G0, cube->B0, cube->A1)]
      - moment[GetIndex(cube->R1, cube->G0, cube->B0, cube->A0)]
      + moment[GetIndex(cube->R0, cube->G1, cube->B0, cube->A1)]
      - moment[GetIndex(cube->R0, cube->G1, cube->B0, cube->A0)]
      - moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A1)]
      + moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A0)];
}
__forceinline static int BottomA(const Box* cube, const int* moment)
{
   return -moment[GetIndex(cube->R1, cube->G1, cube->B1, cube->A0)]
      + moment[GetIndex(cube->R1, cube->G1, cube->B0, cube->A0)]
      + moment[GetIndex(cube->R1, cube->G0, cube->B1, cube->A0)]
      - moment[GetIndex(cube->R1, cube->G0, cube->B0, cube->A0)]
      + moment[GetIndex(cube->R0, cube->G1, cube->B1, cube->A0)]
      - moment[GetIndex(cube->R0, cube->G1, cube->B0, cube->A0)]
      - moment[GetIndex(cube->R0, cube->G0, cube->B1, cube->A0)]
      + moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A0)];
}

__forceinline static int TopR(const Box* cube, const int position, const int* moment)
{
   return moment[GetIndex(position, cube->G1, cube->B1, cube->A1)]
      - moment[GetIndex(position, cube->G1, cube->B1, cube->A0)]
      - moment[GetIndex(position, cube->G1, cube->B0, cube->A1)]
      + moment[GetIndex(position, cube->G1, cube->B0, cube->A0)]
      - moment[GetIndex(position, cube->G0, cube->B1, cube->A1)]
      + moment[GetIndex(position, cube->G0, cube->B1, cube->A0)]
      + moment[GetIndex(position, cube->G0, cube->B0, cube->A1)]
      - moment[GetIndex(position, cube->G0, cube->B0, cube->A0)];
}
__forceinline static int TopG(const Box* cube, const int position, const int* moment)
{
   return moment[GetIndex(cube->R1, position, cube->B1, cube->A1)]
      - moment[GetIndex(cube->R1, position, cube->B1, cube->A0)]
      - moment[GetIndex(cube->R1, position, cube->B0, cube->A1)]
      + moment[GetIndex(cube->R1, position, cube->B0, cube->A0)]
      - moment[GetIndex(cube->R0, position, cube->B1, cube->A1)]
      + moment[GetIndex(cube->R0, position, cube->B1, cube->A0)]
      + moment[GetIndex(cube->R0, position, cube->B0, cube->A1)]
      - moment[GetIndex(cube->R0, position, cube->B0, cube->A0)];
}
__forceinline static int TopB(const Box* cube, const int position, const int* moment)
{
   return moment[GetIndex(cube->R1, cube->G1, position, cube->A1)]
      - moment[GetIndex(cube->R1, cube->G1, position, cube->A0)]
      - moment[GetIndex(cube->R1, cube->G0, position, cube->A1)]
      + moment[GetIndex(cube->R1, cube->G0, position, cube->A0)]
      - moment[GetIndex(cube->R0, cube->G1, position, cube->A1)]
      + moment[GetIndex(cube->R0, cube->G1, position, cube->A0)]
      + moment[GetIndex(cube->R0, cube->G0, position, cube->A1)]
      - moment[GetIndex(cube->R0, cube->G0, position, cube->A0)];
}
__forceinline static int TopA(const Box* cube, const int position, const int* moment)
{
   return moment[GetIndex(cube->R1, cube->G1, cube->B1, position)]
      - moment[GetIndex(cube->R1, cube->G1, cube->B0, position)]
      - moment[GetIndex(cube->R1, cube->G0, cube->B1, position)]
      + moment[GetIndex(cube->R1, cube->G0, cube->B0, position)]
      - moment[GetIndex(cube->R0, cube->G1, cube->B1, position)]
      + moment[GetIndex(cube->R0, cube->G1, cube->B0, position)]
      + moment[GetIndex(cube->R0, cube->G0, cube->B1, position)]
      - moment[GetIndex(cube->R0, cube->G0, cube->B0, position)];
}

__forceinline static float MaximizeR(Quantizer* quantizer, Box* cube, int first, int last, int* cut, float wholeR, float wholeG, float wholeB, float wholeA, float wholeW)
{
   int baseR = BottomR(cube, quantizer->vmr);
   int baseG = BottomR(cube, quantizer->vmg);
   int baseB = BottomR(cube, quantizer->vmb);
   int baseA = BottomR(cube, quantizer->vma);
   int baseW = BottomR(cube, quantizer->vwt);
   float max = 0.0;
   *cut = -1;
   for (int i = first; i < last; i++)
   {
      float halfR = (float)(baseR + TopR(cube, i, quantizer->vmr));
      float halfG = (float)(baseG + TopR(cube, i, quantizer->vmg));
      float halfB = (float)(baseB + TopR(cube, i, quantizer->vmb));
      float halfA = (float)(baseA + TopR(cube, i, quantizer->vma));
      float halfW = (float)(baseW + TopR(cube, i, quantizer->vwt));
      if (halfW == 0) continue;
      float temp = ((halfR * halfR) + (halfG * halfG) + (halfB * halfB) + (halfA * halfA)) / halfW;
      halfR = wholeR - halfR;
      halfG = wholeG - halfG;
      halfB = wholeB - halfB;
      halfA = wholeA - halfA;
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp += ((halfR * halfR) + (halfG * halfG) + (halfB * halfB) + (halfA * halfA)) / halfW;
      if (temp > max)
      {
         max = temp;
         *cut = i;
      }
   }
   return max;
}
__forceinline static float MaximizeG(Quantizer* quantizer, Box* cube, int first, int last, int* cut, float wholeR, float wholeG, float wholeB, float wholeA, float wholeW)
{
   int baseR = BottomG(cube, quantizer->vmr);
   int baseG = BottomG(cube, quantizer->vmg);
   int baseB = BottomG(cube, quantizer->vmb);
   int baseA = BottomG(cube, quantizer->vma);
   int baseW = BottomG(cube, quantizer->vwt);
   float max = 0.0;
   *cut = -1;
   for (int i = first; i < last; i++)
   {
      float halfR = (float)(baseR + TopG(cube, i, quantizer->vmr));
      float halfG = (float)(baseG + TopG(cube, i, quantizer->vmg));
      float halfB = (float)(baseB + TopG(cube, i, quantizer->vmb));
      float halfA = (float)(baseA + TopG(cube, i, quantizer->vma));
      float halfW = (float)(baseW + TopG(cube, i, quantizer->vwt));
      if (halfW == 0) continue;
      float temp = ((halfR * halfR) + (halfG * halfG) + (halfB * halfB) + (halfA * halfA)) / halfW;
      halfR = wholeR - halfR;
      halfG = wholeG - halfG;
      halfB = wholeB - halfB;
      halfA = wholeA - halfA;
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp += ((halfR * halfR) + (halfG * halfG) + (halfB * halfB) + (halfA * halfA)) / halfW;
      if (temp > max)
      {
         max = temp;
         *cut = i;
      }
   }
   return max;
}
__forceinline static float MaximizeB(Quantizer* quantizer, Box* cube, int first, int last, int* cut, float wholeR, float wholeG, float wholeB, float wholeA, float wholeW)
{
   int baseR = BottomB(cube, quantizer->vmr);
   int baseG = BottomB(cube, quantizer->vmg);
   int baseB = BottomB(cube, quantizer->vmb);
   int baseA = BottomB(cube, quantizer->vma);
   int baseW = BottomB(cube, quantizer->vwt);
   float max = 0.0;
   *cut = -1;
   for (int i = first; i < last; i++)
   {
      float halfR = (float)(baseR + TopB(cube, i, quantizer->vmr));
      float halfG = (float)(baseG + TopB(cube, i, quantizer->vmg));
      float halfB = (float)(baseB + TopB(cube, i, quantizer->vmb));
      float halfA = (float)(baseA + TopB(cube, i, quantizer->vma));
      float halfW = (float)(baseW + TopB(cube, i, quantizer->vwt));
      if (halfW == 0) continue;
      float temp = ((halfR * halfR) + (halfG * halfG) + (halfB * halfB) + (halfA * halfA)) / halfW;
      halfR = wholeR - halfR;
      halfG = wholeG - halfG;
      halfB = wholeB - halfB;
      halfA = wholeA - halfA;
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp += ((halfR * halfR) + (halfG * halfG) + (halfB * halfB) + (halfA * halfA)) / halfW;
      if (temp > max)
      {
         max = temp;
         *cut = i;
      }
   }
   return max;
}
__forceinline static float MaximizeA(Quantizer* quantizer, Box* cube, int first, int last, int* cut, float wholeR, float wholeG, float wholeB, float wholeA, float wholeW)
{
   int baseR = BottomA(cube, quantizer->vmr);
   int baseG = BottomA(cube, quantizer->vmg);
   int baseB = BottomA(cube, quantizer->vmb);
   int baseA = BottomA(cube, quantizer->vma);
   int baseW = BottomA(cube, quantizer->vwt);
   float max = 0.0;
   *cut = -1;
   for (int i = first; i < last; i++)
   {
      float halfR = (float)(baseR + TopA(cube, i, quantizer->vmr));
      float halfG = (float)(baseG + TopA(cube, i, quantizer->vmg));
      float halfB = (float)(baseB + TopA(cube, i, quantizer->vmb));
      float halfA = (float)(baseA + TopA(cube, i, quantizer->vma));
      float halfW = (float)(baseW + TopA(cube, i, quantizer->vwt));
      if (halfW == 0) continue;
      float temp = ((halfR * halfR) + (halfG * halfG) + (halfB * halfB) + (halfA * halfA)) / halfW;
      halfR = wholeR - halfR;
      halfG = wholeG - halfG;
      halfB = wholeB - halfB;
      halfA = wholeA - halfA;
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp += ((halfR * halfR) + (halfG * halfG) + (halfB * halfB) + (halfA * halfA)) / halfW;
      if (temp > max)
      {
         max = temp;
         *cut = i;
      }
   }
   return max;
}

__forceinline static void VolumeRGBA(const Box* cube, const Quantizer* quantizer, float* vR, float* vG, float* vB, float* vA)
{
   const int IDX1 = GetIndex(cube->R1, cube->G1, cube->B1, cube->A1);
   const int IDX2 = GetIndex(cube->R1, cube->G1, cube->B1, cube->A0);
   const int IDX3 = GetIndex(cube->R1, cube->G1, cube->B0, cube->A1);
   const int IDX4 = GetIndex(cube->R1, cube->G1, cube->B0, cube->A0);
   const int IDX5 = GetIndex(cube->R1, cube->G0, cube->B1, cube->A1);
   const int IDX6 = GetIndex(cube->R1, cube->G0, cube->B1, cube->A0);
   const int IDX7 = GetIndex(cube->R1, cube->G0, cube->B0, cube->A1);
   const int IDX8 = GetIndex(cube->R1, cube->G0, cube->B0, cube->A0);
   const int IDX9 = GetIndex(cube->R0, cube->G1, cube->B1, cube->A1);
   const int IDX10 = GetIndex(cube->R0, cube->G1, cube->B1, cube->A0);
   const int IDX11 = GetIndex(cube->R0, cube->G1, cube->B0, cube->A1);
   const int IDX12 = GetIndex(cube->R0, cube->G1, cube->B0, cube->A0);
   const int IDX13 = GetIndex(cube->R0, cube->G0, cube->B1, cube->A1);
   const int IDX14 = GetIndex(cube->R0, cube->G0, cube->B1, cube->A0);
   const int IDX15 = GetIndex(cube->R0, cube->G0, cube->B0, cube->A1);
   const int IDX16 = GetIndex(cube->R0, cube->G0, cube->B0, cube->A0);

   const int* vmr = quantizer->vmr;
   const int* vmg = quantizer->vmg;
   const int* vmb = quantizer->vmb;
   const int* vma = quantizer->vma;

   *vR = (float)(
      vmr[IDX1] - vmr[IDX2] - vmr[IDX3] + vmr[IDX4] -
      vmr[IDX5] + vmr[IDX6] + vmr[IDX7] - vmr[IDX8] -
      vmr[IDX9] + vmr[IDX10] + vmr[IDX11] - vmr[IDX12] +
      vmr[IDX13] - vmr[IDX14] - vmr[IDX15] + vmr[IDX16]);

   *vG = (float)(
      vmg[IDX1] - vmg[IDX2] - vmg[IDX3] + vmg[IDX4] -
      vmg[IDX5] + vmg[IDX6] + vmg[IDX7] - vmg[IDX8] -
      vmg[IDX9] + vmg[IDX10] + vmg[IDX11] - vmg[IDX12] +
      vmg[IDX13] - vmg[IDX14] - vmg[IDX15] + vmg[IDX16]);

   *vB = (float)(
      vmb[IDX1] - vmb[IDX2] - vmb[IDX3] + vmb[IDX4] -
      vmb[IDX5] + vmb[IDX6] + vmb[IDX7] - vmb[IDX8] -
      vmb[IDX9] + vmb[IDX10] + vmb[IDX11] - vmb[IDX12] +
      vmb[IDX13] - vmb[IDX14] - vmb[IDX15] + vmb[IDX16]);

   *vA = (float)(
      vma[IDX1] - vma[IDX2] - vma[IDX3] + vma[IDX4] -
      vma[IDX5] + vma[IDX6] + vma[IDX7] - vma[IDX8] -
      vma[IDX9] + vma[IDX10] + vma[IDX11] - vma[IDX12] +
      vma[IDX13] - vma[IDX14] - vma[IDX15] + vma[IDX16]);
}

__forceinline static float VolumeVWT(const Box* cube, const Quantizer* quantizer)
{
   const int IDX1 = GetIndex(cube->R1, cube->G1, cube->B1, cube->A1);
   const int IDX2 = GetIndex(cube->R1, cube->G1, cube->B1, cube->A0);
   const int IDX3 = GetIndex(cube->R1, cube->G1, cube->B0, cube->A1);
   const int IDX4 = GetIndex(cube->R1, cube->G1, cube->B0, cube->A0);
   const int IDX5 = GetIndex(cube->R1, cube->G0, cube->B1, cube->A1);
   const int IDX6 = GetIndex(cube->R1, cube->G0, cube->B1, cube->A0);
   const int IDX7 = GetIndex(cube->R1, cube->G0, cube->B0, cube->A1);
   const int IDX8 = GetIndex(cube->R1, cube->G0, cube->B0, cube->A0);
   const int IDX9 = GetIndex(cube->R0, cube->G1, cube->B1, cube->A1);
   const int IDX10 = GetIndex(cube->R0, cube->G1, cube->B1, cube->A0);
   const int IDX11 = GetIndex(cube->R0, cube->G1, cube->B0, cube->A1);
   const int IDX12 = GetIndex(cube->R0, cube->G1, cube->B0, cube->A0);
   const int IDX13 = GetIndex(cube->R0, cube->G0, cube->B1, cube->A1);
   const int IDX14 = GetIndex(cube->R0, cube->G0, cube->B1, cube->A0);
   const int IDX15 = GetIndex(cube->R0, cube->G0, cube->B0, cube->A1);
   const int IDX16 = GetIndex(cube->R0, cube->G0, cube->B0, cube->A0);

   const int* vwt = quantizer->vwt;

   return (float)(
      vwt[IDX1] - vwt[IDX2] - vwt[IDX3] + vwt[IDX4] -
      vwt[IDX5] + vwt[IDX6] + vwt[IDX7] - vwt[IDX8] -
      vwt[IDX9] + vwt[IDX10] + vwt[IDX11] - vwt[IDX12] +
      vwt[IDX13] - vwt[IDX14] - vwt[IDX15] + vwt[IDX16]);
}

__forceinline static float Variance(const Quantizer* quantizer, const Box* cube)
{
   float dr, dg, db, da;
   VolumeRGBA(cube, quantizer, &dr, &dg, &db, &da);

   float xx =
      quantizer->m2[GetIndex(cube->R1, cube->G1, cube->B1, cube->A1)]
      - quantizer->m2[GetIndex(cube->R1, cube->G1, cube->B1, cube->A0)]
      - quantizer->m2[GetIndex(cube->R1, cube->G1, cube->B0, cube->A1)]
      + quantizer->m2[GetIndex(cube->R1, cube->G1, cube->B0, cube->A0)]
      - quantizer->m2[GetIndex(cube->R1, cube->G0, cube->B1, cube->A1)]
      + quantizer->m2[GetIndex(cube->R1, cube->G0, cube->B1, cube->A0)]
      + quantizer->m2[GetIndex(cube->R1, cube->G0, cube->B0, cube->A1)]
      - quantizer->m2[GetIndex(cube->R1, cube->G0, cube->B0, cube->A0)]
      - quantizer->m2[GetIndex(cube->R0, cube->G1, cube->B1, cube->A1)]
      + quantizer->m2[GetIndex(cube->R0, cube->G1, cube->B1, cube->A0)]
      + quantizer->m2[GetIndex(cube->R0, cube->G1, cube->B0, cube->A1)]
      - quantizer->m2[GetIndex(cube->R0, cube->G1, cube->B0, cube->A0)]
      + quantizer->m2[GetIndex(cube->R0, cube->G0, cube->B1, cube->A1)]
      - quantizer->m2[GetIndex(cube->R0, cube->G0, cube->B1, cube->A0)]
      - quantizer->m2[GetIndex(cube->R0, cube->G0, cube->B0, cube->A1)]
      + quantizer->m2[GetIndex(cube->R0, cube->G0, cube->B0, cube->A0)];

   return xx - (((dr * dr) + (dg * dg) + (db * db) + (da * da)) / VolumeVWT(cube, quantizer));
}

__forceinline static int Cut(Quantizer* quantizer, Box* set1, Box* set2)
{
   float wholeR, wholeG, wholeB, wholeA;
   VolumeRGBA(set1, quantizer, &wholeR, &wholeG, &wholeB, &wholeA);

   float wholeW = VolumeVWT(set1, quantizer);

   int cutr;
   int cutg;
   int cutb;
   int cuta;

   float maxr = MaximizeR(quantizer, set1, set1->R0 + 1, set1->R1, &cutr, wholeR, wholeG, wholeB, wholeA, wholeW);
   float maxg = MaximizeG(quantizer, set1, set1->G0 + 1, set1->G1, &cutg, wholeR, wholeG, wholeB, wholeA, wholeW);
   float maxb = MaximizeB(quantizer, set1, set1->B0 + 1, set1->B1, &cutb, wholeR, wholeG, wholeB, wholeA, wholeW);
   float maxa = MaximizeA(quantizer, set1, set1->A0 + 1, set1->A1, &cuta, wholeR, wholeG, wholeB, wholeA, wholeW);

   int dir;

   if ((maxr >= maxg) && (maxr >= maxb) && (maxr >= maxa))
   {
      dir = 3;

      if (cutr < 0)
      {
         return 0;
      }
   }
   else if ((maxg >= maxr) && (maxg >= maxb) && (maxg >= maxa))
   {
      dir = 2;
   }
   else if ((maxb >= maxr) && (maxb >= maxg) && (maxb >= maxa))
   {
      dir = 1;
   }
   else
   {
      dir = 0;
   }

   set2->R1 = set1->R1;
   set2->G1 = set1->G1;
   set2->B1 = set1->B1;
   set2->A1 = set1->A1;

   switch (dir)
   {
      // Red
   case 3:
      set2->R0 = set1->R1 = cutr;
      set2->G0 = set1->G0;
      set2->B0 = set1->B0;
      set2->A0 = set1->A0;
      break;

      // Green
   case 2:
      set2->G0 = set1->G1 = cutg;
      set2->R0 = set1->R0;
      set2->B0 = set1->B0;
      set2->A0 = set1->A0;
      break;

      // Blue
   case 1:
      set2->B0 = set1->B1 = cutb;
      set2->R0 = set1->R0;
      set2->G0 = set1->G0;
      set2->A0 = set1->A0;
      break;

      // Alpha
   case 0:
      set2->A0 = set1->A1 = cuta;
      set2->R0 = set1->R0;
      set2->G0 = set1->G0;
      set2->B0 = set1->B0;
      break;
   }

   set1->Volume = (set1->R1 - set1->R0) * (set1->G1 - set1->G0) * (set1->B1 - set1->B0) * (set1->A1 - set1->A0);
   set2->Volume = (set2->R1 - set2->R0) * (set2->G1 - set2->G0) * (set2->B1 - set2->B0) * (set2->A1 - set2->A0);

   return 1;
}

__forceinline static void Mark(Quantizer* quantizer, Box* cube, char label)
{
   for (int r = cube->R0 + 1; r <= cube->R1; r++)
      for (int g = cube->G0 + 1; g <= cube->G1; g++)
         for (int b = cube->B0 + 1; b <= cube->B1; b++)
            for (int a = cube->A0 + 1; a <= cube->A1; a++)
               quantizer->tag[GetIndex(r, g, b, a)] = label;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTIONS

static void Clear(Quantizer* quantizer)
{
   memset(quantizer->vwt, 0, sizeof(quantizer->vwt));
   memset(quantizer->vmr, 0, sizeof(quantizer->vmr));
   memset(quantizer->vmg, 0, sizeof(quantizer->vmg));
   memset(quantizer->vmb, 0, sizeof(quantizer->vmb));
   memset(quantizer->vma, 0, sizeof(quantizer->vma));
   memset(quantizer->m2, 0, sizeof(quantizer->m2));
   memset(quantizer->tag, 0, sizeof(quantizer->tag));
   memset(quantizer->cube, 0, sizeof(quantizer->cube));
}

static void Build3DHistogram(Quantizer* quantizer, unsigned int* image, int width, int height)
{
   const int pixels = width * height;
   for (int i = 0; i < pixels; i++)
   {
      unsigned int pix = image[i];
      unsigned int a = (pix & 0xFF000000) >> 24;
      unsigned int r = (pix & 0x00FF0000) >> 16;
      unsigned int g = (pix & 0x0000FF00) >> 8;
      unsigned int b = pix & 0x000000FF;

      unsigned int inr = r >> (8 - INDEXBITS);
      unsigned int ing = g >> (8 - INDEXBITS);
      unsigned int inb = b >> (8 - INDEXBITS);
      unsigned int ina = a >> (8 - INDEXALPHABITS);

      int ind = GetIndex((int)inr + 1, (int)ing + 1, (int)inb + 1, (int)ina + 1);

      quantizer->vwt[ind]++;
      quantizer->vmr[ind] += r;
      quantizer->vmg[ind] += g;
      quantizer->vmb[ind] += b;
      quantizer->vma[ind] += a;
      quantizer->m2[ind] += (r * r) + (g * g) + (b * b) + (a * a);
   }
}

static void Get3DMoments(Quantizer* quantizer)
{
   for (int r = 1; r < INDEXCOUNT; r++)
   {
      memset(quantizer->volume, 0, sizeof(quantizer->volume));

      for (int g = 1; g < INDEXCOUNT; g++)
      {
         memset(quantizer->area, 0, sizeof(quantizer->area));

         for (int b = 1; b < INDEXCOUNT; b++)
         {
            int line = 0;
            int lineR = 0;
            int lineG = 0;
            int lineB = 0;
            int lineA = 0;
            float line2 = 0;

            for (int a = 1; a < INDEXALPHACOUNT; a++)
            {
               int ind1 = GetIndex(r, g, b, a);

               line += quantizer->vwt[ind1];
               lineR += quantizer->vmr[ind1];
               lineG += quantizer->vmg[ind1];
               lineB += quantizer->vmb[ind1];
               lineA += quantizer->vma[ind1];
               line2 += quantizer->m2[ind1];

               quantizer->area[a].V += line;
               quantizer->area[a].R += lineR;
               quantizer->area[a].G += lineG;
               quantizer->area[a].B += lineB;
               quantizer->area[a].A += lineA;
               quantizer->area[a].V2 += line2;

               int inv = (b * INDEXALPHACOUNT) + a;

               quantizer->volume[inv].V += quantizer->area[a].V;
               quantizer->volume[inv].R += quantizer->area[a].R;
               quantizer->volume[inv].G += quantizer->area[a].G;
               quantizer->volume[inv].B += quantizer->area[a].B;
               quantizer->volume[inv].A += quantizer->area[a].A;
               quantizer->volume[inv].V2 += quantizer->area[a].V2;

               int ind2 = ind1 - GetIndex(1, 0, 0, 0);

               quantizer->vwt[ind1] = quantizer->vwt[ind2] + quantizer->volume[inv].V;
               quantizer->vmr[ind1] = quantizer->vmr[ind2] + quantizer->volume[inv].R;
               quantizer->vmg[ind1] = quantizer->vmg[ind2] + quantizer->volume[inv].G;
               quantizer->vmb[ind1] = quantizer->vmb[ind2] + quantizer->volume[inv].B;
               quantizer->vma[ind1] = quantizer->vma[ind2] + quantizer->volume[inv].A;
               quantizer->m2[ind1] = quantizer->m2[ind2] + quantizer->volume[inv].V2;
            }
         }
      }
   }
}

static void BuildCube(Quantizer* quantizer, int* colorCount)
{
   quantizer->cube[0].R0 = quantizer->cube[0].G0 = quantizer->cube[0].B0 = quantizer->cube[0].A0 = 0;
   quantizer->cube[0].R1 = quantizer->cube[0].G1 = quantizer->cube[0].B1 = INDEXCOUNT - 1;
   quantizer->cube[0].A1 = INDEXALPHACOUNT - 1;

   int next = 0;

   for (int i = 1; i < *colorCount; i++)
   {
      if (Cut(quantizer, &quantizer->cube[next], &quantizer->cube[i]))
      {
         quantizer->cube[next].vv = quantizer->cube[next].Volume > 1 ? Variance(quantizer, &quantizer->cube[next]) : 0.0f;
         quantizer->cube[i].vv = quantizer->cube[i].Volume > 1 ? Variance(quantizer, &quantizer->cube[i]) : 0.0f;
      }
      else
      {
         quantizer->cube[next].vv = 0.0f;
         i--;
      }

      next = 0;

      float temp = quantizer->cube[0].vv;
      for (int k = 1; k <= i; k++)
      {
         if (quantizer->cube[k].vv > temp)
         {
            temp = quantizer->cube[k].vv;
            next = k;
         }
      }

      if (temp <= 0.0)
      {
         *colorCount = i + 1;
         break;
      }
   }
}

static void GenerateResult(Quantizer* quantizer, unsigned int* image, unsigned int* palette, int colorCount, int width, int height, char* destPixels, int padMultiple4)
{
   // rows must be a multiple of 4, hence padding up to 3 bytes for 8-bit indexed pixels
   int widthMod4 = width % 4;
   int widthZeros = widthMod4 != 0 ? 4 - widthMod4 : 0;

   for (int k = 0; k < colorCount; k++)
   {
      Mark(quantizer, &quantizer->cube[k], (char)k);

      float weight = VolumeVWT(&quantizer->cube[k], quantizer);

      if (weight > 0.01 || weight < -0.01)
      {
         float dr, dg, db, da;
         VolumeRGBA(&quantizer->cube[k], quantizer, &dr, &dg, &db, &da);
         dr /= weight;
         dg /= weight;
         db /= weight;
         da /= weight;

         unsigned int a = (unsigned int)da;
         unsigned int r = (unsigned int)dr;
         unsigned int g = (unsigned int)dg;
         unsigned int b = (unsigned int)db;

         palette[k] = (a << 24) | (r << 16) | (g << 8) | b;
      }
      else
      {
         palette[k] = 0xFF000000;
      }
   }

   for (int ri = 0; ri < height; ri++)
   {
      for (int ci = 0; ci < width; ci++)
      {
         unsigned int pix = image[0];

         unsigned int a = ((pix & 0xFF000000) >> 24) >> (8 - INDEXALPHABITS);
         unsigned int r = ((pix & 0x00FF0000) >> 16) >> (8 - INDEXBITS);
         unsigned int g = ((pix & 0x0000FF00) >> 8) >> (8 - INDEXBITS);
         unsigned int b = (pix & 0x000000FF) >> (8 - INDEXBITS);

         int ind = GetIndex((int)r + 1, (int)g + 1, (int)b + 1, (int)a + 1);

         destPixels[0] = quantizer->tag[ind];
         destPixels++;
         image++;
      }

      // write additional zero bytes if requested
      if (padMultiple4)
      {
         for (int c = 0; c < widthZeros; c++)
         {
            destPixels[0] = 0x00;
            destPixels++;
         }
      }
   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC

Quantizer* Create()
{
   // allocate
   Quantizer* quantizer = (Quantizer*)malloc(sizeof(Quantizer));

   // zero by default
   memset(quantizer, 0, sizeof(Quantizer));

   // return
   return quantizer;
}

void Destroy(Quantizer* quantizer)
{
   free(quantizer);
}

int Quantize(
   Quantizer*    quantizer, 
   unsigned int* image, 
   unsigned int* palette, 
   int*          colorCount, 
   int           width, 
   int           height, 
   char*         destPixels, 
   int           padMultiple4)
{
   // nullptr check
   if ((quantizer == 0) | (image == 0) | (palette == 0) | (colorCount == 0) | (destPixels == 0))
      return 0;

   // range check
   const int COLCOUNT = *colorCount;
   if ((COLCOUNT < 1) | (COLCOUNT > MAXCOLORS))
      return 0;

   // execute
   Clear(quantizer);
   Build3DHistogram(quantizer, image, width, height);
   Get3DMoments(quantizer);
   BuildCube(quantizer, colorCount);
   GenerateResult(quantizer, image, palette, *colorCount, width, height, destPixels, padMultiple4);

   return 1;
}
