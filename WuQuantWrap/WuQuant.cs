using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WuQuantWrap
{
    public static class WuQuant
    {
        const string LIBNAME = @"WuQuant.dll";

        [DllImport(LIBNAME, CallingConvention=CallingConvention.Cdecl)]
        public static extern IntPtr Create();

        [DllImport(LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        public static extern void Destroy(IntPtr quantizer);

        [DllImport(LIBNAME, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern int Quantize(IntPtr quantizer, uint* image, uint* palette, int* colorCount, int width, int height, byte* destPixels, int padMultiple4);

        public static unsafe uint[] Quantize(IntPtr quantizer, Bitmap image, ref int colorCount, byte[] destPixels, int padMultiple4)
        {
            if (image == null)
            {
                throw new ArgumentNullException("image");
            }

            if (destPixels == null)
            {
                throw new ArgumentNullException("destPixels");
            }

            if (colorCount < 1 || colorCount > 256)
            {
                throw new ArgumentOutOfRangeException("colorCount");
            }

            BitmapData imgdata = image.LockBits(
                Rectangle.FromLTRB(0, 0, image.Width, image.Height),
                ImageLockMode.ReadOnly,
                image.PixelFormat);

            uint[] pal = new uint[colorCount];
            fixed (byte* ptrOutput = destPixels)
            {
                fixed (uint* ptrPal = pal)
                {
                    fixed (int* ptrColors = &colorCount)
                    {
                        Quantize(
                           quantizer,
                           (uint*)imgdata.Scan0.ToPointer(),
                           ptrPal,
                           ptrColors,
                           image.Width,
                           image.Height,
                           ptrOutput,
                           padMultiple4);
                    }
                }
            }

            image.UnlockBits(imgdata);

            return pal;
        }
    }
}
