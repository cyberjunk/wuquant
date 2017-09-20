
#include <stdio.h>
#include <tchar.h>
#include <SDKDDKVer.h>
#include <WuQuant.h>

#define WIDTH 2
#define HEIGHT 2
#define PIXELS (WIDTH*HEIGHT)

char pixels[PIXELS];
unsigned int img[PIXELS];
unsigned int pal[256];

int main()
{
   memset(pixels, 0, sizeof(pixels));
   memset(pal, 0, sizeof(pal));

   img[0] = 0xFFFF0000;
   img[1] = 0xFF00FF00;
   img[2] = 0xFF0000FF;
   img[3] = 0xFFFFFF00;

   /*img[0] = 0x00FF00FF;
   img[1] = 0x0000FFFF;
   img[2] = 0xFF0000FF;
   img[3] = 0x00FFFFFF;*/


   int colors = 256;
   //memset(&quant, 0, sizeof(quant));

   Quantizer* quantizer = Create();
   Quantize(quantizer, img, pal, &colors, WIDTH, HEIGHT, pixels, 0);
   Destroy(quantizer);
   
   while (1) {}
   return 0;
}

