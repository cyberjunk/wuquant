
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

__forceinline static void VolumeRGBA(const Box* cube, const Quantizer* quantizer, V4f* col)
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

   __m128i s01 = _mm_sub_epi32(v[IDX1].P.SSE, v[IDX2].P.SSE);
   __m128i s02 = _mm_sub_epi32(s01, v[IDX3].P.SSE);
   __m128i s03 = _mm_add_epi32(s02, v[IDX4].P.SSE);
   __m128i s04 = _mm_sub_epi32(s03, v[IDX5].P.SSE);
   __m128i s05 = _mm_add_epi32(s04, v[IDX6].P.SSE);
   __m128i s06 = _mm_add_epi32(s05, v[IDX7].P.SSE);
   __m128i s07 = _mm_sub_epi32(s06, v[IDX8].P.SSE);
   __m128i s08 = _mm_sub_epi32(s07, v[IDX9].P.SSE);
   __m128i s09 = _mm_add_epi32(s08, v[IDX10].P.SSE);
   __m128i s10 = _mm_add_epi32(s09, v[IDX11].P.SSE);
   __m128i s11 = _mm_sub_epi32(s10, v[IDX12].P.SSE);
   __m128i s12 = _mm_add_epi32(s11, v[IDX13].P.SSE);
   __m128i s13 = _mm_sub_epi32(s12, v[IDX14].P.SSE);
   __m128i s14 = _mm_sub_epi32(s13, v[IDX15].P.SSE);
   __m128i s15 = _mm_add_epi32(s14, v[IDX16].P.SSE);
   col->SSE = _mm_cvtepi32_ps(s15);
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
   V4f d;
   VolumeRGBA(cube, quantizer, &d);

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

   return xx - (((d.R * d.R) + (d.G * d.G) + (d.B * d.B) + (d.A * d.A)) / VolumeVWT(cube, quantizer));
}

__forceinline static int Cut(Quantizer* quantizer, Box* set1, Box* set2)
{
   V4f whole;
   VolumeRGBA(set1, quantizer, &whole);

   float wholeW = VolumeVWT(set1, quantizer);

   int cutr;
   int cutg;
   int cutb;
   int cuta;

   float maxr = MaximizeR(quantizer, set1, set1->R0 + 1, set1->R1, &cutr, &whole, wholeW);
   float maxg = MaximizeG(quantizer, set1, set1->G0 + 1, set1->G1, &cutg, &whole, wholeW);
   float maxb = MaximizeB(quantizer, set1, set1->B0 + 1, set1->B1, &cutb, &whole, wholeW);
   float maxa = MaximizeA(quantizer, set1, set1->A0 + 1, set1->A1, &cuta, &whole, wholeW);

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

   const __m128i AND = _mm_set_epi32(0xFF000000, 0x000000FF, 0x0000FF00, 0x00FF0000);
   const __m128i SHIFT = _mm_set_epi32(24, 0, 8, 16);
   const __m128i ONE = _mm_set1_epi32(1);
   const __m128i SHIFTIDX = _mm_set_epi32(
      8 - INDEXALPHABITS,
      8 - INDEXBITS,
      8 - INDEXBITS,
      8 - INDEXBITS);

   V4i p, in;
   for (int i = 0; i < pixels; i++)
   {
      p.SSE  = _mm_srlv_epi32(_mm_and_si128(_mm_set1_epi32(image[i]), AND), SHIFT);
      in.SSE = _mm_add_epi32(_mm_srlv_epi32(p.SSE, SHIFTIDX), ONE);

      const int IDX = GetIndex(in.R, in.G, in.B, in.A);

      quantizer->v[IDX].P.SSE = _mm_add_epi32(quantizer->v[IDX].P.SSE, p.SSE);
      quantizer->v[IDX].V++;
      quantizer->v[IDX].V2 += (p.R * p.R) + (p.G * p.G) + (p.B * p.B) + (p.A * p.A);
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
         V4f d;
         VolumeRGBA(&quantizer->cube[k], quantizer, &d);
         d.R /= weight;
         d.G /= weight;
         d.B /= weight;
         d.A /= weight;

         unsigned int a = (unsigned int)d.A;
         unsigned int r = (unsigned int)d.R;
         unsigned int g = (unsigned int)d.G;
         unsigned int b = (unsigned int)d.B;

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
