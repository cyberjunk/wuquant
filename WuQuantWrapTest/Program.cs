using System;

namespace WuQuantWrapTest
{
    using WuQuantWrap;

    class Program
    {
        static unsafe void Main(string[] args)
        {
            const int WIDTH = 2;
            const int HEIGHT = 2;
            const int PIXELS = WIDTH * HEIGHT;

            byte[] pixels = new byte[PIXELS];
            uint[] img = new uint[PIXELS];
            uint[] pal = new uint[256];

            img[0] = 0xFFFF0000;
            img[1] = 0xFF00FF00;
            img[2] = 0xFF0000FF;
            img[3] = 0xFFFFFF00;

            IntPtr inst = WuQuant.Create();
            int colors = 256;

            fixed (byte* ptrPixels = pixels)
                fixed (uint* ptrImage = img)
                    fixed (uint* ptrPalette = pal)
                        WuQuant.Quantize(inst, ptrImage, ptrPalette, &colors, WIDTH, HEIGHT, ptrPixels, 0);

            WuQuant.Destroy(inst);

            Console.WriteLine("SUCCESS");

            while(true) { }
        }
    }
}
