
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

__forceinline static void Bottom(const Moment* m, 
   const int IDX1, const int IDX2, const int IDX3, const int IDX4, 
   const int IDX5, const int IDX6, const int IDX7, const int IDX8, 
   int* r, int* g, int* b, int* a, int* w)
{
   *r = -m[IDX1].R + m[IDX2].R + m[IDX3].R - m[IDX4].R + m[IDX5].R - m[IDX6].R - m[IDX7].R + m[IDX8].R;
   *g = -m[IDX1].G + m[IDX2].G + m[IDX3].G - m[IDX4].G + m[IDX5].G - m[IDX6].G - m[IDX7].G + m[IDX8].G;
   *b = -m[IDX1].B + m[IDX2].B + m[IDX3].B - m[IDX4].B + m[IDX5].B - m[IDX6].B - m[IDX7].B + m[IDX8].B;
   *a = -m[IDX1].A + m[IDX2].A + m[IDX3].A - m[IDX4].A + m[IDX5].A - m[IDX6].A - m[IDX7].A + m[IDX8].A;
   *w = -m[IDX1].V + m[IDX2].V + m[IDX3].V - m[IDX4].V + m[IDX5].V - m[IDX6].V - m[IDX7].V + m[IDX8].V;
}
__forceinline static void BottomR(const Box* c, const Quantizer* q, int* r, int* g, int* b, int* a, int* w)
{
   const int IDX1 = GetIndex(c->R0, c->G1, c->B1, c->A1);
   const int IDX2 = GetIndex(c->R0, c->G1, c->B1, c->A0);
   const int IDX3 = GetIndex(c->R0, c->G1, c->B0, c->A1);
   const int IDX4 = GetIndex(c->R0, c->G1, c->B0, c->A0);
   const int IDX5 = GetIndex(c->R0, c->G0, c->B1, c->A1);
   const int IDX6 = GetIndex(c->R0, c->G0, c->B1, c->A0);
   const int IDX7 = GetIndex(c->R0, c->G0, c->B0, c->A1);
   const int IDX8 = GetIndex(c->R0, c->G0, c->B0, c->A0);
   Bottom(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, r, g, b, a, w);
}
__forceinline static void BottomG(const Box* c, const Quantizer* q, int* r, int* g, int* b, int* a, int* w)
{
   const int IDX1 = GetIndex(c->R1, c->G0, c->B1, c->A1);
   const int IDX2 = GetIndex(c->R1, c->G0, c->B1, c->A0);
   const int IDX3 = GetIndex(c->R1, c->G0, c->B0, c->A1);
   const int IDX4 = GetIndex(c->R1, c->G0, c->B0, c->A0);
   const int IDX5 = GetIndex(c->R0, c->G0, c->B1, c->A1);
   const int IDX6 = GetIndex(c->R0, c->G0, c->B1, c->A0);
   const int IDX7 = GetIndex(c->R0, c->G0, c->B0, c->A1);
   const int IDX8 = GetIndex(c->R0, c->G0, c->B0, c->A0);
   Bottom(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, r, g, b, a, w);
}
__forceinline static void BottomB(const Box* c, const Quantizer* q, int* r, int* g, int* b, int* a, int* w)
{
   const int IDX1 = GetIndex(c->R1, c->G1, c->B0, c->A1);
   const int IDX2 = GetIndex(c->R1, c->G1, c->B0, c->A0);
   const int IDX3 = GetIndex(c->R1, c->G0, c->B0, c->A1);
   const int IDX4 = GetIndex(c->R1, c->G0, c->B0, c->A0);
   const int IDX5 = GetIndex(c->R0, c->G1, c->B0, c->A1);
   const int IDX6 = GetIndex(c->R0, c->G1, c->B0, c->A0);
   const int IDX7 = GetIndex(c->R0, c->G0, c->B0, c->A1);
   const int IDX8 = GetIndex(c->R0, c->G0, c->B0, c->A0);
   Bottom(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, r, g, b, a, w);
}
__forceinline static void BottomA(const Box* c, const Quantizer* q, int* r, int* g, int* b, int* a, int* w)
{
   const int IDX1 = GetIndex(c->R1, c->G1, c->B1, c->A0);
   const int IDX2 = GetIndex(c->R1, c->G1, c->B0, c->A0);
   const int IDX3 = GetIndex(c->R1, c->G0, c->B1, c->A0);
   const int IDX4 = GetIndex(c->R1, c->G0, c->B0, c->A0);
   const int IDX5 = GetIndex(c->R0, c->G1, c->B1, c->A0);
   const int IDX6 = GetIndex(c->R0, c->G1, c->B0, c->A0);
   const int IDX7 = GetIndex(c->R0, c->G0, c->B1, c->A0);
   const int IDX8 = GetIndex(c->R0, c->G0, c->B0, c->A0);
   Bottom(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, r, g, b, a, w);
}

__forceinline static void Top(const Moment* m,
   const int IDX1, const int IDX2, const int IDX3, const int IDX4,
   const int IDX5, const int IDX6, const int IDX7, const int IDX8,
   int* r, int* g, int* b, int* a, int* w)
{
   *r = m[IDX1].R - m[IDX2].R - m[IDX3].R + m[IDX4].R - m[IDX5].R + m[IDX6].R + m[IDX7].R - m[IDX8].R;
   *g = m[IDX1].G - m[IDX2].G - m[IDX3].G + m[IDX4].G - m[IDX5].G + m[IDX6].G + m[IDX7].G - m[IDX8].G;
   *b = m[IDX1].B - m[IDX2].B - m[IDX3].B + m[IDX4].B - m[IDX5].B + m[IDX6].B + m[IDX7].B - m[IDX8].B;
   *a = m[IDX1].A - m[IDX2].A - m[IDX3].A + m[IDX4].A - m[IDX5].A + m[IDX6].A + m[IDX7].A - m[IDX8].A;
   *w = m[IDX1].V - m[IDX2].V - m[IDX3].V + m[IDX4].V - m[IDX5].V + m[IDX6].V + m[IDX7].V - m[IDX8].V;
}
__forceinline static void TopR(const Box* c, const int position, const Quantizer* q, int* r, int* g, int* b, int* a, int* w)
{
   const int IDX1 = GetIndex(position, c->G1, c->B1, c->A1);
   const int IDX2 = GetIndex(position, c->G1, c->B1, c->A0);
   const int IDX3 = GetIndex(position, c->G1, c->B0, c->A1);
   const int IDX4 = GetIndex(position, c->G1, c->B0, c->A0);
   const int IDX5 = GetIndex(position, c->G0, c->B1, c->A1);
   const int IDX6 = GetIndex(position, c->G0, c->B1, c->A0);
   const int IDX7 = GetIndex(position, c->G0, c->B0, c->A1);
   const int IDX8 = GetIndex(position, c->G0, c->B0, c->A0);
   Top(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, r, g, b, a, w);
}
__forceinline static void TopG(const Box* c, const int position, const Quantizer* q, int* r, int* g, int* b, int* a, int* w)
{
   const int IDX1 = GetIndex(c->R1, position, c->B1, c->A1);
   const int IDX2 = GetIndex(c->R1, position, c->B1, c->A0);
   const int IDX3 = GetIndex(c->R1, position, c->B0, c->A1);
   const int IDX4 = GetIndex(c->R1, position, c->B0, c->A0);
   const int IDX5 = GetIndex(c->R0, position, c->B1, c->A1);
   const int IDX6 = GetIndex(c->R0, position, c->B1, c->A0);
   const int IDX7 = GetIndex(c->R0, position, c->B0, c->A1);
   const int IDX8 = GetIndex(c->R0, position, c->B0, c->A0);
   Top(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, r, g, b, a, w);
}
__forceinline static void TopB(const Box* c, const int position, const Quantizer* q, int* r, int* g, int* b, int* a, int* w)
{
   const int IDX1 = GetIndex(c->R1, c->G1, position, c->A1);
   const int IDX2 = GetIndex(c->R1, c->G1, position, c->A0);
   const int IDX3 = GetIndex(c->R1, c->G0, position, c->A1);
   const int IDX4 = GetIndex(c->R1, c->G0, position, c->A0);
   const int IDX5 = GetIndex(c->R0, c->G1, position, c->A1);
   const int IDX6 = GetIndex(c->R0, c->G1, position, c->A0);
   const int IDX7 = GetIndex(c->R0, c->G0, position, c->A1);
   const int IDX8 = GetIndex(c->R0, c->G0, position, c->A0);
   Top(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, r, g, b, a, w);
}
__forceinline static void TopA(const Box* c, const int position, const Quantizer* q, int* r, int* g, int* b, int* a, int* w)
{
   const int IDX1 = GetIndex(c->R1, c->G1, c->B1, position);
   const int IDX2 = GetIndex(c->R1, c->G1, c->B0, position);
   const int IDX3 = GetIndex(c->R1, c->G0, c->B1, position);
   const int IDX4 = GetIndex(c->R1, c->G0, c->B0, position);
   const int IDX5 = GetIndex(c->R0, c->G1, c->B1, position);
   const int IDX6 = GetIndex(c->R0, c->G1, c->B0, position);
   const int IDX7 = GetIndex(c->R0, c->G0, c->B1, position);
   const int IDX8 = GetIndex(c->R0, c->G0, c->B0, position);
   Top(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, r, g, b, a, w);
}

__forceinline static float MaximizeR(Quantizer* quantizer, Box* cube, int first, int last, int* cut, float wholeR, float wholeG, float wholeB, float wholeA, float wholeW)
{
   int baseR, baseG, baseB, baseA, baseW;
   BottomR(cube, quantizer, &baseR, &baseG, &baseB, &baseA, &baseW);
   float max = 0.0;
   *cut = -1;
   for (int i = first; i < last; i++)
   {
      int topR, topG, topB, topA, topW;
      TopR(cube, i, quantizer, &topR, &topG, &topB, &topA, &topW);
      float halfR = (float)(baseR + topR);
      float halfG = (float)(baseG + topG);
      float halfB = (float)(baseB + topB);
      float halfA = (float)(baseA + topA);
      float halfW = (float)(baseW + topW);
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
   int baseR, baseG, baseB, baseA, baseW;
   BottomG(cube, quantizer, &baseR, &baseG, &baseB, &baseA, &baseW);
   float max = 0.0;
   *cut = -1;
   for (int i = first; i < last; i++)
   {
      int topR, topG, topB, topA, topW;
      TopG(cube, i, quantizer, &topR, &topG, &topB, &topA, &topW);
      float halfR = (float)(baseR + topR);
      float halfG = (float)(baseG + topG);
      float halfB = (float)(baseB + topB);
      float halfA = (float)(baseA + topA);
      float halfW = (float)(baseW + topW);
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
   int baseR, baseG, baseB, baseA, baseW;
   BottomB(cube, quantizer, &baseR, &baseG, &baseB, &baseA, &baseW);
   float max = 0.0;
   *cut = -1;
   for (int i = first; i < last; i++)
   {
      int topR, topG, topB, topA, topW;
      TopB(cube, i, quantizer, &topR, &topG, &topB, &topA, &topW);
      float halfR = (float)(baseR + topR);
      float halfG = (float)(baseG + topG);
      float halfB = (float)(baseB + topB);
      float halfA = (float)(baseA + topA);
      float halfW = (float)(baseW + topW);
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
   int baseR, baseG, baseB, baseA, baseW;
   BottomA(cube, quantizer, &baseR, &baseG, &baseB, &baseA, &baseW);
   float max = 0.0;
   *cut = -1;
   for (int i = first; i < last; i++)
   {
      int topR, topG, topB, topA, topW;
      TopA(cube, i, quantizer, &topR, &topG, &topB, &topA, &topW);
      float halfR = (float)(baseR + topR);
      float halfG = (float)(baseG + topG);
      float halfB = (float)(baseB + topB);
      float halfA = (float)(baseA + topA);
      float halfW = (float)(baseW + topW);
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

   const Moment* v = quantizer->v;

   *vR = (float)(
      v[IDX1].R  - v[IDX2].R  - v[IDX3].R  + v[IDX4].R  -
      v[IDX5].R  + v[IDX6].R  + v[IDX7].R  - v[IDX8].R  -
      v[IDX9].R  + v[IDX10].R + v[IDX11].R - v[IDX12].R +
      v[IDX13].R - v[IDX14].R - v[IDX15].R + v[IDX16].R);

   *vG = (float)(
      v[IDX1].G  - v[IDX2].G  - v[IDX3].G  + v[IDX4].G  -
      v[IDX5].G  + v[IDX6].G  + v[IDX7].G  - v[IDX8].G  -
      v[IDX9].G  + v[IDX10].G + v[IDX11].G - v[IDX12].G +
      v[IDX13].G - v[IDX14].G - v[IDX15].G + v[IDX16].G);

   *vB = (float)(
      v[IDX1].B  - v[IDX2].B  - v[IDX3].B  + v[IDX4].B  -
      v[IDX5].B  + v[IDX6].B  + v[IDX7].B  - v[IDX8].B  -
      v[IDX9].B  + v[IDX10].B + v[IDX11].B - v[IDX12].B +
      v[IDX13].B - v[IDX14].B - v[IDX15].B + v[IDX16].B);

   *vA = (float)(
      v[IDX1].A  - v[IDX2].A  - v[IDX3].A  + v[IDX4].A  -
      v[IDX5].A  + v[IDX6].A  + v[IDX7].A  - v[IDX8].A  -
      v[IDX9].A  + v[IDX10].A + v[IDX11].A - v[IDX12].A +
      v[IDX13].A - v[IDX14].A - v[IDX15].A + v[IDX16].A);
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

   const Moment* v = quantizer->v;

   return (float)(
      v[IDX1].V  - v[IDX2].V  - v[IDX3].V  + v[IDX4].V  -
      v[IDX5].V  + v[IDX6].V  + v[IDX7].V  - v[IDX8].V  -
      v[IDX9].V  + v[IDX10].V + v[IDX11].V - v[IDX12].V +
      v[IDX13].V - v[IDX14].V - v[IDX15].V + v[IDX16].V);
}

__forceinline static float Variance(const Quantizer* quantizer, const Box* cube)
{
   float dr, dg, db, da;
   VolumeRGBA(cube, quantizer, &dr, &dg, &db, &da);

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

   const Moment* v = quantizer->v;

   float xx =
      + v[IDX1].V2  - v[IDX2].V2  - v[IDX3].V2  + v[IDX4].V2
      - v[IDX5].V2  + v[IDX6].V2  + v[IDX7].V2  - v[IDX8].V2
      - v[IDX9].V2  + v[IDX10].V2 + v[IDX11].V2 - v[IDX12].V2
      + v[IDX13].V2 - v[IDX14].V2 - v[IDX15].V2 + v[IDX16].V2;

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
   memset(quantizer->v, 0, sizeof(quantizer->v));
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

      quantizer->v[ind].V++;
      quantizer->v[ind].R += r;
      quantizer->v[ind].G += g;
      quantizer->v[ind].B += b;
      quantizer->v[ind].A += a;
      quantizer->v[ind].V2 += (r * r) + (g * g) + (b * b) + (a * a);
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

               line += quantizer->v[ind1].V;
               lineR += quantizer->v[ind1].R;
               lineG += quantizer->v[ind1].G;
               lineB += quantizer->v[ind1].B;
               lineA += quantizer->v[ind1].A;
               line2 += quantizer->v[ind1].V2;

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

               quantizer->v[ind1].V = quantizer->v[ind2].V + quantizer->volume[inv].V;
               quantizer->v[ind1].R = quantizer->v[ind2].R + quantizer->volume[inv].R;
               quantizer->v[ind1].G = quantizer->v[ind2].G + quantizer->volume[inv].G;
               quantizer->v[ind1].B = quantizer->v[ind2].B + quantizer->volume[inv].B;
               quantizer->v[ind1].A = quantizer->v[ind2].A + quantizer->volume[inv].A;
               quantizer->v[ind1].V2 = quantizer->v[ind2].V2 + quantizer->volume[inv].V2;
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
