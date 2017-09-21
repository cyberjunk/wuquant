
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "WuQuant.h"

inline static int GetIndex(int r, int g, int b, int a)
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

inline static float Volume(Box* cube, int* moment)
{
   return (float)(moment[GetIndex(cube->R1, cube->G1, cube->B1, cube->A1)]
      - moment[GetIndex(cube->R1, cube->G1, cube->B1, cube->A0)]
      - moment[GetIndex(cube->R1, cube->G1, cube->B0, cube->A1)]
      + moment[GetIndex(cube->R1, cube->G1, cube->B0, cube->A0)]
      - moment[GetIndex(cube->R1, cube->G0, cube->B1, cube->A1)]
      + moment[GetIndex(cube->R1, cube->G0, cube->B1, cube->A0)]
      + moment[GetIndex(cube->R1, cube->G0, cube->B0, cube->A1)]
      - moment[GetIndex(cube->R1, cube->G0, cube->B0, cube->A0)]
      - moment[GetIndex(cube->R0, cube->G1, cube->B1, cube->A1)]
      + moment[GetIndex(cube->R0, cube->G1, cube->B1, cube->A0)]
      + moment[GetIndex(cube->R0, cube->G1, cube->B0, cube->A1)]
      - moment[GetIndex(cube->R0, cube->G1, cube->B0, cube->A0)]
      + moment[GetIndex(cube->R0, cube->G0, cube->B1, cube->A1)]
      - moment[GetIndex(cube->R0, cube->G0, cube->B1, cube->A0)]
      - moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A1)]
      + moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A0)]);
}

inline static int Bottom(Box* cube, int direction, int* moment)
{
   switch (direction)
   {
      // Red
   case 3:
      return -moment[GetIndex(cube->R0, cube->G1, cube->B1, cube->A1)]
         + moment[GetIndex(cube->R0, cube->G1, cube->B1, cube->A0)]
         + moment[GetIndex(cube->R0, cube->G1, cube->B0, cube->A1)]
         - moment[GetIndex(cube->R0, cube->G1, cube->B0, cube->A0)]
         + moment[GetIndex(cube->R0, cube->G0, cube->B1, cube->A1)]
         - moment[GetIndex(cube->R0, cube->G0, cube->B1, cube->A0)]
         - moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A1)]
         + moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A0)];

      // Green
   case 2:
      return -moment[GetIndex(cube->R1, cube->G0, cube->B1, cube->A1)]
         + moment[GetIndex(cube->R1, cube->G0, cube->B1, cube->A0)]
         + moment[GetIndex(cube->R1, cube->G0, cube->B0, cube->A1)]
         - moment[GetIndex(cube->R1, cube->G0, cube->B0, cube->A0)]
         + moment[GetIndex(cube->R0, cube->G0, cube->B1, cube->A1)]
         - moment[GetIndex(cube->R0, cube->G0, cube->B1, cube->A0)]
         - moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A1)]
         + moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A0)];

      // Blue
   case 1:
      return -moment[GetIndex(cube->R1, cube->G1, cube->B0, cube->A1)]
         + moment[GetIndex(cube->R1, cube->G1, cube->B0, cube->A0)]
         + moment[GetIndex(cube->R1, cube->G0, cube->B0, cube->A1)]
         - moment[GetIndex(cube->R1, cube->G0, cube->B0, cube->A0)]
         + moment[GetIndex(cube->R0, cube->G1, cube->B0, cube->A1)]
         - moment[GetIndex(cube->R0, cube->G1, cube->B0, cube->A0)]
         - moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A1)]
         + moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A0)];

      // Alpha
   case 0:
      return -moment[GetIndex(cube->R1, cube->G1, cube->B1, cube->A0)]
         + moment[GetIndex(cube->R1, cube->G1, cube->B0, cube->A0)]
         + moment[GetIndex(cube->R1, cube->G0, cube->B1, cube->A0)]
         - moment[GetIndex(cube->R1, cube->G0, cube->B0, cube->A0)]
         + moment[GetIndex(cube->R0, cube->G1, cube->B1, cube->A0)]
         - moment[GetIndex(cube->R0, cube->G1, cube->B0, cube->A0)]
         - moment[GetIndex(cube->R0, cube->G0, cube->B1, cube->A0)]
         + moment[GetIndex(cube->R0, cube->G0, cube->B0, cube->A0)];

   default:
      return 0;
   }
}

inline static int Top(Box* cube, int direction, int position, int* moment)
{
   switch (direction)
   {
      // Red
   case 3:
      return moment[GetIndex(position, cube->G1, cube->B1, cube->A1)]
         - moment[GetIndex(position, cube->G1, cube->B1, cube->A0)]
         - moment[GetIndex(position, cube->G1, cube->B0, cube->A1)]
         + moment[GetIndex(position, cube->G1, cube->B0, cube->A0)]
         - moment[GetIndex(position, cube->G0, cube->B1, cube->A1)]
         + moment[GetIndex(position, cube->G0, cube->B1, cube->A0)]
         + moment[GetIndex(position, cube->G0, cube->B0, cube->A1)]
         - moment[GetIndex(position, cube->G0, cube->B0, cube->A0)];

      // Green
   case 2:
      return moment[GetIndex(cube->R1, position, cube->B1, cube->A1)]
         - moment[GetIndex(cube->R1, position, cube->B1, cube->A0)]
         - moment[GetIndex(cube->R1, position, cube->B0, cube->A1)]
         + moment[GetIndex(cube->R1, position, cube->B0, cube->A0)]
         - moment[GetIndex(cube->R0, position, cube->B1, cube->A1)]
         + moment[GetIndex(cube->R0, position, cube->B1, cube->A0)]
         + moment[GetIndex(cube->R0, position, cube->B0, cube->A1)]
         - moment[GetIndex(cube->R0, position, cube->B0, cube->A0)];

      // Blue
   case 1:
      return moment[GetIndex(cube->R1, cube->G1, position, cube->A1)]
         - moment[GetIndex(cube->R1, cube->G1, position, cube->A0)]
         - moment[GetIndex(cube->R1, cube->G0, position, cube->A1)]
         + moment[GetIndex(cube->R1, cube->G0, position, cube->A0)]
         - moment[GetIndex(cube->R0, cube->G1, position, cube->A1)]
         + moment[GetIndex(cube->R0, cube->G1, position, cube->A0)]
         + moment[GetIndex(cube->R0, cube->G0, position, cube->A1)]
         - moment[GetIndex(cube->R0, cube->G0, position, cube->A0)];

      // Alpha
   case 0:
      return moment[GetIndex(cube->R1, cube->G1, cube->B1, position)]
         - moment[GetIndex(cube->R1, cube->G1, cube->B0, position)]
         - moment[GetIndex(cube->R1, cube->G0, cube->B1, position)]
         + moment[GetIndex(cube->R1, cube->G0, cube->B0, position)]
         - moment[GetIndex(cube->R0, cube->G1, cube->B1, position)]
         + moment[GetIndex(cube->R0, cube->G1, cube->B0, position)]
         + moment[GetIndex(cube->R0, cube->G0, cube->B1, position)]
         - moment[GetIndex(cube->R0, cube->G0, cube->B0, position)];

   default:
      return 0;
   }
}

inline static float Variance(Quantizer* quantizer, Box* cube)
{
   float dr = Volume(cube, quantizer->vmr);
   float dg = Volume(cube, quantizer->vmg);
   float db = Volume(cube, quantizer->vmb);
   float da = Volume(cube, quantizer->vma);

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

   return xx - (((dr * dr) + (dg * dg) + (db * db) + (da * da)) / Volume(cube, quantizer->vwt));
}

inline static void ClearBox(Box* box)
{
   box->R0 = 0;
   box->R1 = 0;
   box->G0 = 0;
   box->G1 = 0;
   box->B0 = 0;
   box->B1 = 0;
   box->A0 = 0;
   box->A1 = 0;
   box->Volume = 0;
}

inline static void Clear(Quantizer* quantizer)
{
   memset(quantizer->vwt, 0, sizeof(quantizer->vwt));
   memset(quantizer->vmr, 0, sizeof(quantizer->vmr));
   memset(quantizer->vmg, 0, sizeof(quantizer->vmg));
   memset(quantizer->vmb, 0, sizeof(quantizer->vmb));
   memset(quantizer->vma, 0, sizeof(quantizer->vma));
   memset(quantizer->m2, 0, sizeof(quantizer->m2));
   memset(quantizer->tag, 0, sizeof(quantizer->tag));
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
      memset(quantizer->volume,  0, sizeof(quantizer->volume));
      memset(quantizer->volumeR, 0, sizeof(quantizer->volumeR));
      memset(quantizer->volumeG, 0, sizeof(quantizer->volumeG));
      memset(quantizer->volumeB, 0, sizeof(quantizer->volumeB));
      memset(quantizer->volumeA, 0, sizeof(quantizer->volumeA));
      memset(quantizer->volume2, 0, sizeof(quantizer->volume2));

      for (int g = 1; g < INDEXCOUNT; g++)
      {
         memset(quantizer->area,  0, sizeof(quantizer->area));
         memset(quantizer->areaR, 0, sizeof(quantizer->areaR));
         memset(quantizer->areaG, 0, sizeof(quantizer->areaG));
         memset(quantizer->areaB, 0, sizeof(quantizer->areaB));
         memset(quantizer->areaA, 0, sizeof(quantizer->areaA));
         memset(quantizer->area2, 0, sizeof(quantizer->area2));

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

               quantizer->area[a] += line;
               quantizer->areaR[a] += lineR;
               quantizer->areaG[a] += lineG;
               quantizer->areaB[a] += lineB;
               quantizer->areaA[a] += lineA;
               quantizer->area2[a] += line2;

               int inv = (b * INDEXALPHACOUNT) + a;

               quantizer->volume[inv] += quantizer->area[a];
               quantizer->volumeR[inv] += quantizer->areaR[a];
               quantizer->volumeG[inv] += quantizer->areaG[a];
               quantizer->volumeB[inv] += quantizer->areaB[a];
               quantizer->volumeA[inv] += quantizer->areaA[a];
               quantizer->volume2[inv] += quantizer->area2[a];

               int ind2 = ind1 - GetIndex(1, 0, 0, 0);

               quantizer->vwt[ind1] = quantizer->vwt[ind2] + quantizer->volume[inv];
               quantizer->vmr[ind1] = quantizer->vmr[ind2] + quantizer->volumeR[inv];
               quantizer->vmg[ind1] = quantizer->vmg[ind2] + quantizer->volumeG[inv];
               quantizer->vmb[ind1] = quantizer->vmb[ind2] + quantizer->volumeB[inv];
               quantizer->vma[ind1] = quantizer->vma[ind2] + quantizer->volumeA[inv];
               quantizer->m2[ind1] = quantizer->m2[ind2] + quantizer->volume2[inv];
            }
         }
      }
   }
}

static float Maximize(Quantizer* quantizer, Box* cube, int direction, int first, int last, int* cut, float wholeR, float wholeG, float wholeB, float wholeA, float wholeW)
{
   int baseR = Bottom(cube, direction, quantizer->vmr);
   int baseG = Bottom(cube, direction, quantizer->vmg);
   int baseB = Bottom(cube, direction, quantizer->vmb);
   int baseA = Bottom(cube, direction, quantizer->vma);
   int baseW = Bottom(cube, direction, quantizer->vwt);

   float max = 0.0;
   *cut = -1;

   for (int i = first; i < last; i++)
   {
      float halfR = (float)(baseR + Top(cube, direction, i, quantizer->vmr));
      float halfG = (float)(baseG + Top(cube, direction, i, quantizer->vmg));
      float halfB = (float)(baseB + Top(cube, direction, i, quantizer->vmb));
      float halfA = (float)(baseA + Top(cube, direction, i, quantizer->vma));
      float halfW = (float)(baseW + Top(cube, direction, i, quantizer->vwt));

      if (halfW == 0)
      {
         continue;
      }

      float temp = ((halfR * halfR) + (halfG * halfG) + (halfB * halfB) + (halfA * halfA)) / halfW;

      halfR = wholeR - halfR;
      halfG = wholeG - halfG;
      halfB = wholeB - halfB;
      halfA = wholeA - halfA;
      halfW = wholeW - halfW;

      if (halfW == 0)
      {
         continue;
      }

      temp += ((halfR * halfR) + (halfG * halfG) + (halfB * halfB) + (halfA * halfA)) / halfW;

      if (temp > max)
      {
         max = temp;
         *cut = i;
      }
   }

   return max;
}

static int Cut(Quantizer* quantizer, Box* set1, Box* set2)
{
   float wholeR = Volume(set1, quantizer->vmr);
   float wholeG = Volume(set1, quantizer->vmg);
   float wholeB = Volume(set1, quantizer->vmb);
   float wholeA = Volume(set1, quantizer->vma);
   float wholeW = Volume(set1, quantizer->vwt);

   int cutr;
   int cutg;
   int cutb;
   int cuta;

   float maxr = Maximize(quantizer, set1, 3, set1->R0 + 1, set1->R1, &cutr, wholeR, wholeG, wholeB, wholeA, wholeW);
   float maxg = Maximize(quantizer, set1, 2, set1->G0 + 1, set1->G1, &cutg, wholeR, wholeG, wholeB, wholeA, wholeW);
   float maxb = Maximize(quantizer, set1, 1, set1->B0 + 1, set1->B1, &cutb, wholeR, wholeG, wholeB, wholeA, wholeW);
   float maxa = Maximize(quantizer, set1, 0, set1->A0 + 1, set1->A1, &cuta, wholeR, wholeG, wholeB, wholeA, wholeW);

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

static void Mark(Quantizer* quantizer, Box* cube, char label)
{
   for (int r = cube->R0 + 1; r <= cube->R1; r++)
      for (int g = cube->G0 + 1; g <= cube->G1; g++)
         for (int b = cube->B0 + 1; b <= cube->B1; b++)
            for (int a = cube->A0 + 1; a <= cube->A1; a++)
               quantizer->tag[GetIndex(r, g, b, a)] = label;
}

static void BuildCube(Quantizer* quantizer, int* colorCount)
{
   //clear boxes
   for (int i = 0; i < *colorCount; i++)
      ClearBox(&quantizer->cube[i]);

   memset(quantizer->vv, 0, sizeof(quantizer->vv));

   quantizer->cube[0].R0 = quantizer->cube[0].G0 = quantizer->cube[0].B0 = quantizer->cube[0].A0 = 0;
   quantizer->cube[0].R1 = quantizer->cube[0].G1 = quantizer->cube[0].B1 = INDEXCOUNT - 1;
   quantizer->cube[0].A1 = INDEXALPHACOUNT - 1;

   int next = 0;

   for (int i = 1; i < *colorCount; i++)
   {
      if (Cut(quantizer, &quantizer->cube[next], &quantizer->cube[i]))
      {
         quantizer->vv[next] = quantizer->cube[next].Volume > 1 ? Variance(quantizer, &quantizer->cube[next]) : 0.0f;
         quantizer->vv[i] = quantizer->cube[i].Volume > 1 ? Variance(quantizer, &quantizer->cube[i]) : 0.0f;
      }
      else
      {
         quantizer->vv[next] = 0.0f;
         i--;
      }

      next = 0;

      float temp = quantizer->vv[0];
      for (int k = 1; k <= i; k++)
      {
         if (quantizer->vv[k] > temp)
         {
            temp = quantizer->vv[k];
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

      float weight = Volume(&quantizer->cube[k], quantizer->vwt);

      if (weight > 0.01 || weight < -0.01)
      {
         float da = Volume(&quantizer->cube[k], quantizer->vma) / weight;
         float dr = Volume(&quantizer->cube[k], quantizer->vmr) / weight;
         float dg = Volume(&quantizer->cube[k], quantizer->vmg) / weight;
         float db = Volume(&quantizer->cube[k], quantizer->vmb) / weight;

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
   Quantizer* quantizer = (Quantizer*)malloc(sizeof(Quantizer));
   memset(quantizer, 0, sizeof(Quantizer));
   return quantizer;
}

void Destroy(Quantizer* quantizer)
{
   free(quantizer);
}

int Quantize(
   Quantizer* quantizer, 
   unsigned int* image, 
   unsigned int* palette, 
   int* colorCount, 
   int width, 
   int height, 
   char* destPixels, 
   int padMultiple4)
{
   if (quantizer == 0)
   {
      return 0;
   }

   if (image == 0 || destPixels == 0 || palette == 0 || colorCount == 0)
   {
      return 0;
   }

   if (*colorCount < 1 || *colorCount > MAXCOLORS)
   {
      return 0;
   }

   Clear(quantizer);
   Build3DHistogram(quantizer, image, width, height);
   Get3DMoments(quantizer);
   BuildCube(quantizer, colorCount);
   GenerateResult(quantizer, image, palette, *colorCount, width, height, destPixels, padMultiple4);

   return 1;
}
