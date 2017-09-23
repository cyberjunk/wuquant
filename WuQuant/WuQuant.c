
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "WuQuant.h"

#define MASK_AND      _mm_set_epi32(0xFF000000, 0x000000FF, 0x0000FF00, 0x00FF0000)
#define MASK_SHIFT    _mm_set_epi32(24, 0, 8, 16)
#define MASK_ONE      _mm_set1_epi32(1)
#define MASK_SHIFTIDX _mm_set_epi32(8 - INDEXALPHABITS, 8 - INDEXBITS, 8 - INDEXBITS, 8 - INDEXBITS)

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
   V4i* v, int* w)
{
   const __m128i s1 = _mm_setzero_si128();
   const __m128i s2 = _mm_sub_epi32(s1, m[IDX1].P.SSE);
   const __m128i s3 = _mm_add_epi32(s2, m[IDX2].P.SSE);
   const __m128i s4 = _mm_add_epi32(s3, m[IDX3].P.SSE);
   const __m128i s5 = _mm_sub_epi32(s4, m[IDX4].P.SSE);
   const __m128i s6 = _mm_add_epi32(s5, m[IDX5].P.SSE);
   const __m128i s7 = _mm_sub_epi32(s6, m[IDX6].P.SSE);
   const __m128i s8 = _mm_sub_epi32(s7, m[IDX7].P.SSE);
   v->SSE = _mm_add_epi32(s8, m[IDX8].P.SSE);
   *w = -m[IDX1].V + m[IDX2].V + m[IDX3].V - m[IDX4].V + m[IDX5].V - m[IDX6].V - m[IDX7].V + m[IDX8].V;
}
__forceinline static void BottomR(const Box* c, const Quantizer* q, V4i* v, int* w)
{
   const int IDX1 = GetIndex(c->R0, c->G1, c->B1, c->A1);
   const int IDX2 = GetIndex(c->R0, c->G1, c->B1, c->A0);
   const int IDX3 = GetIndex(c->R0, c->G1, c->B0, c->A1);
   const int IDX4 = GetIndex(c->R0, c->G1, c->B0, c->A0);
   const int IDX5 = GetIndex(c->R0, c->G0, c->B1, c->A1);
   const int IDX6 = GetIndex(c->R0, c->G0, c->B1, c->A0);
   const int IDX7 = GetIndex(c->R0, c->G0, c->B0, c->A1);
   const int IDX8 = GetIndex(c->R0, c->G0, c->B0, c->A0);
   Bottom(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}
__forceinline static void BottomG(const Box* c, const Quantizer* q, V4i* v, int* w)
{
   const int IDX1 = GetIndex(c->R1, c->G0, c->B1, c->A1);
   const int IDX2 = GetIndex(c->R1, c->G0, c->B1, c->A0);
   const int IDX3 = GetIndex(c->R1, c->G0, c->B0, c->A1);
   const int IDX4 = GetIndex(c->R1, c->G0, c->B0, c->A0);
   const int IDX5 = GetIndex(c->R0, c->G0, c->B1, c->A1);
   const int IDX6 = GetIndex(c->R0, c->G0, c->B1, c->A0);
   const int IDX7 = GetIndex(c->R0, c->G0, c->B0, c->A1);
   const int IDX8 = GetIndex(c->R0, c->G0, c->B0, c->A0);
   Bottom(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}
__forceinline static void BottomB(const Box* c, const Quantizer* q, V4i* v, int* w)
{
   const int IDX1 = GetIndex(c->R1, c->G1, c->B0, c->A1);
   const int IDX2 = GetIndex(c->R1, c->G1, c->B0, c->A0);
   const int IDX3 = GetIndex(c->R1, c->G0, c->B0, c->A1);
   const int IDX4 = GetIndex(c->R1, c->G0, c->B0, c->A0);
   const int IDX5 = GetIndex(c->R0, c->G1, c->B0, c->A1);
   const int IDX6 = GetIndex(c->R0, c->G1, c->B0, c->A0);
   const int IDX7 = GetIndex(c->R0, c->G0, c->B0, c->A1);
   const int IDX8 = GetIndex(c->R0, c->G0, c->B0, c->A0);
   Bottom(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}
__forceinline static void BottomA(const Box* c, const Quantizer* q, V4i* v, int* w)
{
   const int IDX1 = GetIndex(c->R1, c->G1, c->B1, c->A0);
   const int IDX2 = GetIndex(c->R1, c->G1, c->B0, c->A0);
   const int IDX3 = GetIndex(c->R1, c->G0, c->B1, c->A0);
   const int IDX4 = GetIndex(c->R1, c->G0, c->B0, c->A0);
   const int IDX5 = GetIndex(c->R0, c->G1, c->B1, c->A0);
   const int IDX6 = GetIndex(c->R0, c->G1, c->B0, c->A0);
   const int IDX7 = GetIndex(c->R0, c->G0, c->B1, c->A0);
   const int IDX8 = GetIndex(c->R0, c->G0, c->B0, c->A0);
   Bottom(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}

__forceinline static void Top(const Moment* m,
   const int IDX1, const int IDX2, const int IDX3, const int IDX4,
   const int IDX5, const int IDX6, const int IDX7, const int IDX8,
   V4i* v, int* w)
{
   const __m128i s1 = _mm_sub_epi32(m[IDX1].P.SSE, m[IDX2].P.SSE);
   const __m128i s2 = _mm_sub_epi32(s1, m[IDX3].P.SSE);
   const __m128i s3 = _mm_add_epi32(s2, m[IDX4].P.SSE);
   const __m128i s4 = _mm_sub_epi32(s3, m[IDX5].P.SSE);
   const __m128i s5 = _mm_add_epi32(s4, m[IDX6].P.SSE);
   const __m128i s6 = _mm_add_epi32(s5, m[IDX7].P.SSE);
   v->SSE = _mm_sub_epi32(s6, m[IDX8].P.SSE);
   *w = m[IDX1].V - m[IDX2].V - m[IDX3].V + m[IDX4].V - m[IDX5].V + m[IDX6].V + m[IDX7].V - m[IDX8].V;
}
__forceinline static void TopR(const Box* c, const int position, const Quantizer* q, V4i* v, int* w)
{
   const int IDX1 = GetIndex(position, c->G1, c->B1, c->A1);
   const int IDX2 = GetIndex(position, c->G1, c->B1, c->A0);
   const int IDX3 = GetIndex(position, c->G1, c->B0, c->A1);
   const int IDX4 = GetIndex(position, c->G1, c->B0, c->A0);
   const int IDX5 = GetIndex(position, c->G0, c->B1, c->A1);
   const int IDX6 = GetIndex(position, c->G0, c->B1, c->A0);
   const int IDX7 = GetIndex(position, c->G0, c->B0, c->A1);
   const int IDX8 = GetIndex(position, c->G0, c->B0, c->A0);
   Top(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}
__forceinline static void TopG(const Box* c, const int position, const Quantizer* q, V4i* v, int* w)
{
   const int IDX1 = GetIndex(c->R1, position, c->B1, c->A1);
   const int IDX2 = GetIndex(c->R1, position, c->B1, c->A0);
   const int IDX3 = GetIndex(c->R1, position, c->B0, c->A1);
   const int IDX4 = GetIndex(c->R1, position, c->B0, c->A0);
   const int IDX5 = GetIndex(c->R0, position, c->B1, c->A1);
   const int IDX6 = GetIndex(c->R0, position, c->B1, c->A0);
   const int IDX7 = GetIndex(c->R0, position, c->B0, c->A1);
   const int IDX8 = GetIndex(c->R0, position, c->B0, c->A0);
   Top(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}
__forceinline static void TopB(const Box* c, const int position, const Quantizer* q, V4i* v, int* w)
{
   const int IDX1 = GetIndex(c->R1, c->G1, position, c->A1);
   const int IDX2 = GetIndex(c->R1, c->G1, position, c->A0);
   const int IDX3 = GetIndex(c->R1, c->G0, position, c->A1);
   const int IDX4 = GetIndex(c->R1, c->G0, position, c->A0);
   const int IDX5 = GetIndex(c->R0, c->G1, position, c->A1);
   const int IDX6 = GetIndex(c->R0, c->G1, position, c->A0);
   const int IDX7 = GetIndex(c->R0, c->G0, position, c->A1);
   const int IDX8 = GetIndex(c->R0, c->G0, position, c->A0);
   Top(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}
__forceinline static void TopA(const Box* c, const int position, const Quantizer* q, V4i* v, int* w)
{
   const int IDX1 = GetIndex(c->R1, c->G1, c->B1, position);
   const int IDX2 = GetIndex(c->R1, c->G1, c->B0, position);
   const int IDX3 = GetIndex(c->R1, c->G0, c->B1, position);
   const int IDX4 = GetIndex(c->R1, c->G0, c->B0, position);
   const int IDX5 = GetIndex(c->R0, c->G1, c->B1, position);
   const int IDX6 = GetIndex(c->R0, c->G1, c->B0, position);
   const int IDX7 = GetIndex(c->R0, c->G0, c->B1, position);
   const int IDX8 = GetIndex(c->R0, c->G0, c->B0, position);
   Top(q->v, IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8, v, w);
}

__forceinline static float MaximizeR(Quantizer* quantizer, Box* cube, int first, int last, int* cut, V4f* whole, float wholeW)
{
   V4i base; int baseW;
   BottomR(cube, quantizer, &base, &baseW);
   float max = 0.0;
   *cut = -1;
   for (int i = first; i < last; i++)
   {
      V4i top; int topW; V4f half; float halfW; __m128 temp; float tf;
      TopR(cube, i, quantizer, &top, &topW);
      half.SSE = _mm_cvtepi32_ps(_mm_add_epi32(base.SSE, top.SSE));
      halfW = (float)(baseW + topW);
      if (halfW == 0) continue;
      temp = _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW));
      half.SSE = _mm_sub_ps(whole->SSE, half.SSE);
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp = _mm_add_ss(temp, _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW)));
      tf = temp.m128_f32[0];
      if (tf > max)
      {
         max = tf;
         *cut = i;
      }
   }
   return max;
}
__forceinline static float MaximizeG(Quantizer* quantizer, Box* cube, int first, int last, int* cut, V4f* whole, float wholeW)
{
   V4i base; int baseW;
   BottomG(cube, quantizer, &base, &baseW);
   float max = 0.0;
   *cut = -1;
   for (int i = first; i < last; i++)
   {
      V4i top; int topW; V4f half; float halfW; __m128 temp; float tf;
      TopG(cube, i, quantizer, &top, &topW);
      half.SSE = _mm_cvtepi32_ps(_mm_add_epi32(base.SSE, top.SSE));
      halfW = (float)(baseW + topW);
      if (halfW == 0) continue;
      temp = _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW));
      half.SSE = _mm_sub_ps(whole->SSE, half.SSE);
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp = _mm_add_ss(temp, _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW)));
      tf = temp.m128_f32[0];
      if (tf > max)
      {
         max = tf;
         *cut = i;
      }
   }
   return max;
}
__forceinline static float MaximizeB(Quantizer* quantizer, Box* cube, int first, int last, int* cut, V4f* whole, float wholeW)
{
   V4i base; int baseW;
   BottomB(cube, quantizer, &base, &baseW);
   float max = 0.0;
   *cut = -1;
   for (int i = first; i < last; i++)
   {
      V4i top; int topW; V4f half; float halfW; __m128 temp; float tf;
      TopB(cube, i, quantizer, &top, &topW);
      half.SSE = _mm_cvtepi32_ps(_mm_add_epi32(base.SSE, top.SSE));
      halfW = (float)(baseW + topW);
      if (halfW == 0) continue;
      temp = _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW));
      half.SSE = _mm_sub_ps(whole->SSE, half.SSE);
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp = _mm_add_ss(temp, _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW)));
      tf = temp.m128_f32[0];
      if (tf > max)
      {
         max = tf;
         *cut = i;
      }
   }
   return max;
}
__forceinline static float MaximizeA(Quantizer* quantizer, Box* cube, int first, int last, int* cut, V4f* whole, float wholeW)
{
   V4i base; int baseW; 
   BottomA(cube, quantizer, &base, &baseW);
   float max = 0.0;
   *cut = -1;
   for (int i = first; i < last; i++)
   {
      V4i top; int topW; V4f half; float halfW; __m128 temp; float tf;
      TopA(cube, i, quantizer, &top, &topW);
      half.SSE = _mm_cvtepi32_ps(_mm_add_epi32(base.SSE, top.SSE));
      halfW = (float)(baseW + topW);
      if (halfW == 0) continue;
      temp = _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW));
      half.SSE = _mm_sub_ps(whole->SSE, half.SSE);
      halfW = wholeW - halfW;
      if (halfW == 0) continue;
      temp = _mm_add_ss(temp, _mm_div_ss(_mm_dp_ps(half.SSE, half.SSE, 0xF1), _mm_set_ss(halfW)));
      tf = temp.m128_f32[0];
      if (tf > max)
      {
         max = tf;
         *cut = i;
      }
   }
   return max;
}

__forceinline static void _Volume(const Moment* m,
   const int IDX1, const int IDX2, const int IDX3, const int IDX4,
   const int IDX5, const int IDX6, const int IDX7, const int IDX8,
   const int IDX9, const int IDX10, const int IDX11, const int IDX12,
   const int IDX13, const int IDX14, const int IDX15, const int IDX16,
   V4f* v, float* w)
{
   __m128i s01 = _mm_sub_epi32(m[IDX1].P.SSE, m[IDX2].P.SSE);
   __m128i s02 = _mm_sub_epi32(s01, m[IDX3].P.SSE);
   __m128i s03 = _mm_add_epi32(s02, m[IDX4].P.SSE);
   __m128i s04 = _mm_sub_epi32(s03, m[IDX5].P.SSE);
   __m128i s05 = _mm_add_epi32(s04, m[IDX6].P.SSE);
   __m128i s06 = _mm_add_epi32(s05, m[IDX7].P.SSE);
   __m128i s07 = _mm_sub_epi32(s06, m[IDX8].P.SSE);
   __m128i s08 = _mm_sub_epi32(s07, m[IDX9].P.SSE);
   __m128i s09 = _mm_add_epi32(s08, m[IDX10].P.SSE);
   __m128i s10 = _mm_add_epi32(s09, m[IDX11].P.SSE);
   __m128i s11 = _mm_sub_epi32(s10, m[IDX12].P.SSE);
   __m128i s12 = _mm_add_epi32(s11, m[IDX13].P.SSE);
   __m128i s13 = _mm_sub_epi32(s12, m[IDX14].P.SSE);
   __m128i s14 = _mm_sub_epi32(s13, m[IDX15].P.SSE);
   __m128i s15 = _mm_add_epi32(s14, m[IDX16].P.SSE);
   v->SSE = _mm_cvtepi32_ps(s15);

   *w = (float)(
      m[IDX1].V  - m[IDX2].V  - m[IDX3].V  + m[IDX4].V  -
      m[IDX5].V  + m[IDX6].V  + m[IDX7].V  - m[IDX8].V  -
      m[IDX9].V  + m[IDX10].V + m[IDX11].V - m[IDX12].V +
      m[IDX13].V - m[IDX14].V - m[IDX15].V + m[IDX16].V);
}

__forceinline static void Volume(const Box* cube, const Moment* m, V4f* col, float* w)
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

   _Volume(m, 
      IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8,
      IDX9, IDX10, IDX11, IDX12, IDX13, IDX14, IDX15, IDX16,
      col, w);
}

__forceinline static float Variance(const Moment* m, const Box* c)
{
   const int IDX1 = GetIndex(c->R1, c->G1, c->B1, c->A1);
   const int IDX2 = GetIndex(c->R1, c->G1, c->B1, c->A0);
   const int IDX3 = GetIndex(c->R1, c->G1, c->B0, c->A1);
   const int IDX4 = GetIndex(c->R1, c->G1, c->B0, c->A0);
   const int IDX5 = GetIndex(c->R1, c->G0, c->B1, c->A1);
   const int IDX6 = GetIndex(c->R1, c->G0, c->B1, c->A0);
   const int IDX7 = GetIndex(c->R1, c->G0, c->B0, c->A1);
   const int IDX8 = GetIndex(c->R1, c->G0, c->B0, c->A0);
   const int IDX9 = GetIndex(c->R0, c->G1, c->B1, c->A1);
   const int IDX10 = GetIndex(c->R0, c->G1, c->B1, c->A0);
   const int IDX11 = GetIndex(c->R0, c->G1, c->B0, c->A1);
   const int IDX12 = GetIndex(c->R0, c->G1, c->B0, c->A0);
   const int IDX13 = GetIndex(c->R0, c->G0, c->B1, c->A1);
   const int IDX14 = GetIndex(c->R0, c->G0, c->B1, c->A0);
   const int IDX15 = GetIndex(c->R0, c->G0, c->B0, c->A1);
   const int IDX16 = GetIndex(c->R0, c->G0, c->B0, c->A0);

   //const Moment* m = quantizer->v;

   const float xx =
      + m[IDX1].V2  - m[IDX2].V2  - m[IDX3].V2  + m[IDX4].V2
      - m[IDX5].V2  + m[IDX6].V2  + m[IDX7].V2  - m[IDX8].V2
      - m[IDX9].V2  + m[IDX10].V2 + m[IDX11].V2 - m[IDX12].V2
      + m[IDX13].V2 - m[IDX14].V2 - m[IDX15].V2 + m[IDX16].V2;

   V4f d; 
   float vol;

   _Volume(m,
      IDX1, IDX2, IDX3, IDX4, IDX5, IDX6, IDX7, IDX8,
      IDX9, IDX10, IDX11, IDX12, IDX13, IDX14, IDX15, IDX16,
      &d, &vol);

   const __m128 sub = _mm_div_ss(_mm_dp_ps(d.SSE, d.SSE, 0xF1), _mm_set_ss(vol));

   return xx - sub.m128_f32[0];
}

__forceinline static int Cut(Quantizer* quantizer, Box* set1, Box* set2)
{
   V4f whole; float wholeW;
   Volume(set1, quantizer->v, &whole, &wholeW);

   int cutr;
   int cutg;
   int cutb;
   int cuta;

   float maxr = MaximizeR(quantizer, set1, set1->R0 + 1, set1->R1, &cutr, &whole, wholeW);
   float maxg = MaximizeG(quantizer, set1, set1->G0 + 1, set1->G1, &cutg, &whole, wholeW);
   float maxb = MaximizeB(quantizer, set1, set1->B0 + 1, set1->B1, &cutb, &whole, wholeW);
   float maxa = MaximizeA(quantizer, set1, set1->A0 + 1, set1->A1, &cuta, &whole, wholeW);

   set2->R1 = set1->R1;
   set2->G1 = set1->G1;
   set2->B1 = set1->B1;
   set2->A1 = set1->A1;

   // RED
   if ((maxr >= maxg) && (maxr >= maxb) && (maxr >= maxa))
   {
      if (cutr < 0)
         return 0;

      set2->R0 = set1->R1 = cutr;
      set2->G0 = set1->G0;
      set2->B0 = set1->B0;
      set2->A0 = set1->A0;
   }

   // GREEN
   else if ((maxg >= maxr) && (maxg >= maxb) && (maxg >= maxa))
   {
      set2->G0 = set1->G1 = cutg;
      set2->R0 = set1->R0;
      set2->B0 = set1->B0;
      set2->A0 = set1->A0;
   }

   // BLUE
   else if ((maxb >= maxr) && (maxb >= maxg) && (maxb >= maxa))
   {
      set2->B0 = set1->B1 = cutb;
      set2->R0 = set1->R0;
      set2->G0 = set1->G0;
      set2->A0 = set1->A0;
   }

   // ALPHA
   else
   {
      set2->A0 = set1->A1 = cuta;
      set2->R0 = set1->R0;
      set2->G0 = set1->G0;
      set2->B0 = set1->B0;
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

static void Build3DHistogram(Moment* m, unsigned int* image, int width, int height)
{
   const unsigned int PIXELS = width * height;
   for (unsigned int i = 0; i < PIXELS; i++)
   {
      V4i p, in;
      p.SSE  = _mm_srlv_epi32(_mm_and_si128(_mm_set1_epi32(image[i]), MASK_AND), MASK_SHIFT);
      in.SSE = _mm_add_epi32(_mm_srlv_epi32(p.SSE, MASK_SHIFTIDX), MASK_ONE);
      const int IDX = GetIndex(in.R, in.G, in.B, in.A);
      m[IDX].P.SSE = _mm_add_epi32(m[IDX].P.SSE, p.SSE);
      m[IDX].V++;
      m[IDX].V2 += (p.R * p.R) + (p.G * p.G) + (p.B * p.B) + (p.A * p.A);
   }
}

static void Get3DMoments(Quantizer* quantizer)
{
   V4i l;

   for (int r = 1; r < INDEXCOUNT; r++)
   {
      memset(quantizer->volume, 0, sizeof(quantizer->volume));

      for (int g = 1; g < INDEXCOUNT; g++)
      {
         memset(quantizer->area, 0, sizeof(quantizer->area));

         for (int b = 1; b < INDEXCOUNT; b++)
         {
            l.SSE = _mm_setzero_si128();

            int v = 0;
            float v2 = 0;

            for (int a = 1; a < INDEXALPHACOUNT; a++)
            {
               const int ind1 = GetIndex(r, g, b, a);

               l.SSE = _mm_add_epi32(l.SSE, quantizer->v[ind1].P.SSE);
               v  += quantizer->v[ind1].V;
               v2 += quantizer->v[ind1].V2;

               quantizer->area[a].P.SSE = _mm_add_epi32(quantizer->area[a].P.SSE, l.SSE);
               quantizer->area[a].V  += v;
               quantizer->area[a].V2 += v2;

               const int inv = (b * INDEXALPHACOUNT) + a;

               quantizer->volume[inv].P.SSE = _mm_add_epi32(quantizer->volume[inv].P.SSE, quantizer->area[a].P.SSE);
               quantizer->volume[inv].V  += quantizer->area[a].V;
               quantizer->volume[inv].V2 += quantizer->area[a].V2;

               const int ind2 = ind1 - GetIndex(1, 0, 0, 0);

               quantizer->v[ind1].P.SSE = _mm_add_epi32(quantizer->v[ind2].P.SSE, quantizer->volume[inv].P.SSE);
               quantizer->v[ind1].V  = quantizer->v[ind2].V + quantizer->volume[inv].V;
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
         quantizer->cube[next].vv = quantizer->cube[next].Volume > 1 ? Variance(quantizer->v, &quantizer->cube[next]) : 0.0f;
         quantizer->cube[i].vv = quantizer->cube[i].Volume > 1 ? Variance(quantizer->v, &quantizer->cube[i]) : 0.0f;
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
   V4f d; float weight;
   V4i di; V4i in;

   // rows must be a multiple of 4, hence padding up to 3 bytes for 8-bit indexed pixels
   const int WIDTHMOD4 = width % 4;
   const int WIDTHZEROS = WIDTHMOD4 != 0 ? 4 - WIDTHMOD4 : 0;
   
   // create palette
   for (int k = 0; k < colorCount; k++)
   {
      Mark(quantizer, &quantizer->cube[k], (char)k);
      Volume(&quantizer->cube[k], quantizer->v, &d, &weight);

      if (weight > 0.01 || weight < -0.01)
      {
         di.SSE = _mm_cvtps_epi32(_mm_mul_ps(d.SSE, _mm_set1_ps(1.0f / weight)));
         palette[k] = (di.A << 24) | (di.R << 16) | (di.G << 8) | di.B;
      }
      else
      {
         palette[k] = 0xFF000000;
      }
   }

   // create pixels
   for (int ri = 0; ri < height; ri++)
   {
      for (int ci = 0; ci < width; ci++)
      {
         in.SSE = _mm_add_epi32(_mm_srlv_epi32(_mm_srlv_epi32(_mm_and_si128(
            _mm_set1_epi32(image[0]), MASK_AND), MASK_SHIFT), MASK_SHIFTIDX), MASK_ONE);
         
         const int IDX = GetIndex(in.R, in.G, in.B, in.A);

         destPixels[0] = quantizer->tag[IDX];
         destPixels++;
         image++;
      }

      // write additional zero bytes if requested
      if (padMultiple4)
      {
         for (int c = 0; c < WIDTHZEROS; c++)
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
   Build3DHistogram(quantizer->v, image, width, height);
   Get3DMoments(quantizer);
   BuildCube(quantizer, colorCount);
   GenerateResult(quantizer, image, palette, *colorCount, width, height, destPixels, padMultiple4);

   return 1;
}
