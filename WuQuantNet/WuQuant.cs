﻿
namespace WuQuantNet
{
    using System;
    using System.Drawing;
    using System.Drawing.Imaging;
    using System.Runtime.CompilerServices;

    internal struct Box
    {
        public int R0 { get; set; }
        public int R1 { get; set; }
        public int G0 { get; set; }
        public int G1 { get; set; }
        public int B0 { get; set; }
        public int B1 { get; set; }
        public int A0 { get; set; }
        public int A1 { get; set; }
        public int Volume { get; set; }
        public void Clear()
        {
            this.R0 = 0;
            this.R1 = 0;
            this.G0 = 0;
            this.G0 = 0;
            this.B0 = 0;
            this.B0 = 0;
            this.A0 = 0;
            this.A1 = 0;
            this.Volume = 0;
        }
    }

    public static class Data
    {
        public struct Volume
        {
            public int Vol;
            public int VolumeR;
            public int VolumeG;
            public int VolumeB;
            public int VolumeA;
            public float Volume2;
        }
    }
    /// <summary>
    /// A Wu's color quantizer with alpha channel.
    /// </summary>
    /// <remarks>
    /// <para>
    /// Based on C Implementation of Xiaolin Wu's Color Quantizer (v. 2)
    /// (see Graphics Gems volume II, pages 126-133)
    /// (<see href="http://www.ece.mcmaster.ca/~xwu/cq.c"/>).
    /// </para>
    /// </remarks>
    public class WuQuant
    {
        /// <summary>
        /// Maximum supported colors
        /// </summary>
        private const int MAXCOLORS = 256;

        /// <summary>
        /// The index bits.
        /// </summary>
        private const int IndexBits = 6;

        /// <summary>
        /// The index alpha bits.
        /// </summary>
        private const int IndexAlphaBits = 3;

        /// <summary>
        /// The index count.
        /// </summary>
        private const int IndexCount = (1 << WuQuant.IndexBits) + 1;

        /// <summary>
        /// The index alpha count.
        /// </summary>
        private const int IndexAlphaCount = (1 << WuQuant.IndexAlphaBits) + 1;

        /// <summary>
        /// IndexCount * IndexAlphaCount
        /// </summary>
        private const int WorkArraySize = WuQuant.IndexCount * WuQuant.IndexAlphaCount;

        /// <summary>
        /// The table length.
        /// </summary>
        private const int TableLength = WuQuant.IndexCount * WuQuant.IndexCount * WuQuant.IndexCount * WuQuant.IndexAlphaCount;

        /// <summary>
        /// Moment of <c>P(c)</c>.
        /// </summary>
        private readonly int[] vwt = new int[WuQuant.TableLength];

        /// <summary>
        /// Moment of <c>r*P(c)</c>.
        /// </summary>
        private readonly int[] vmr = new int[WuQuant.TableLength];

        /// <summary>
        /// Moment of <c>g*P(c)</c>.
        /// </summary>
        private readonly int[] vmg = new int[WuQuant.TableLength];

        /// <summary>
        /// Moment of <c>b*P(c)</c>.
        /// </summary>
        private readonly int[] vmb = new int[WuQuant.TableLength];

        /// <summary>
        /// Moment of <c>a*P(c)</c>.
        /// </summary>
        private readonly int[] vma = new int[WuQuant.TableLength];

        /// <summary>
        /// Moment of <c>c^2*P(c)</c>.
        /// </summary>
        private readonly float[] m2 = new float[WuQuant.TableLength];

        /// <summary>
        /// Color space tag.
        /// </summary>
        private readonly byte[] tag = new byte[WuQuant.TableLength];

        #region Temporary Arrays for GetMoments3D
        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        private readonly Data.Volume[] volume = new Data.Volume[WuQuant.WorkArraySize];


        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        /*private readonly int[] volume = new int[WuAlphaColorQuantizer.WorkArraySize];

        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        private readonly int[] volumeR = new int[WuAlphaColorQuantizer.WorkArraySize];

        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        private readonly int[] volumeG = new int[WuAlphaColorQuantizer.WorkArraySize];

        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        private readonly int[] volumeB = new int[WuAlphaColorQuantizer.WorkArraySize];

        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        private readonly int[] volumeA = new int[WuAlphaColorQuantizer.WorkArraySize];

        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        private readonly float[] volume2 = new float[WuAlphaColorQuantizer.WorkArraySize];*/

        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        private readonly int[] area = new int[WuQuant.IndexAlphaCount];

        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        private readonly int[] areaR = new int[WuQuant.IndexAlphaCount];

        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        private readonly int[] areaG = new int[WuQuant.IndexAlphaCount];

        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        private readonly int[] areaB = new int[WuQuant.IndexAlphaCount];

        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        private readonly int[] areaA = new int[WuQuant.IndexAlphaCount];

        /// <summary>
        /// Temporary Array Data used in Get3DMoments()
        /// </summary>
        private readonly float[] area2 = new float[WuQuant.IndexAlphaCount];
        #endregion

        /// <summary>
        /// Temporary Data in BuildCube()
        /// </summary>
        private readonly float[] vv = new float[WuQuant.MAXCOLORS];

        /// <summary>
        /// Temporary Data
        /// </summary>
        private readonly Box[] cube = new Box[WuQuant.MAXCOLORS];

        /// <summary>
        /// Quantizes an image.
        /// </summary>
        /// <param name="image">The image (ARGB).</param>
        /// <param name="colorCount">The color count.</param>
        /// <param name="width">Width of image</param>
        /// <param name="height">Height of image</param>
        /// <param name="destPixels">Indexed pixelData will be written there</param>
        /// <param name="padMultiple4">True to pad rows to multiple of 4</param>
        /// <returns>Palette with ARGB colors</returns>
        public unsafe uint[] Quantize(uint* image, int colorCount, int width, int height, byte* destPixels, bool padMultiple4)
        {
            if (image == null)
            {
                throw new ArgumentNullException("image");
            }

            if (destPixels == null)
            {
                throw new ArgumentNullException("destPixels");
            }

            if (colorCount < 1 || colorCount > WuQuant.MAXCOLORS)
            {
                throw new ArgumentOutOfRangeException("colorCount");
            }

            this.Clear();
            this.Build3DHistogram(image, width, height);
            this.Get3DMoments();
            this.BuildCube(ref colorCount);

            return this.GenerateResult(image, colorCount, width, height, destPixels, padMultiple4);
        }

        /// <summary>
        /// Quantizes an image.
        /// </summary>
        /// <param name="image">The image (ARGB).</param>
        /// <param name="colorCount">The color count.</param>
        /// <returns>Bitmap with indexed colors</returns>
        public unsafe Bitmap Quantize(Bitmap image, int colorCount)
        {
            if (image == null)
            {
                throw new ArgumentNullException("image");
            }

            if (colorCount < 1 || colorCount > WuQuant.MAXCOLORS)
            {
                throw new ArgumentOutOfRangeException("colorCount");
            }

            Bitmap bmp = new Bitmap(image.Width, image.Height, PixelFormat.Format8bppIndexed);

            BitmapData imgdata = image.LockBits(
                Rectangle.FromLTRB(0, 0, image.Width, image.Height),
                ImageLockMode.ReadOnly,
                image.PixelFormat);

            BitmapData bmpdata = bmp.LockBits(
                Rectangle.FromLTRB(0, 0, bmp.Width, bmp.Height),
                ImageLockMode.WriteOnly,
                bmp.PixelFormat);

            uint[] res = this.Quantize(
               (uint*)imgdata.Scan0.ToPointer(),
               colorCount,
               image.Width,
               image.Height,
               (byte*)bmpdata.Scan0.ToPointer(),
               true);

            ColorPalette pal = bmp.Palette;
            for (int i = 0; i < res.Length; i++)
            {
                pal.Entries[i] = Color.FromArgb((int)res[i]);
            }

            for (int i = res.Length; i < 256; i++)
            {
                pal.Entries[i] = Color.FromArgb(0);
            }

            bmp.Palette = pal;

            image.UnlockBits(imgdata);
            bmp.UnlockBits(bmpdata);

            return bmp;
        }

        /// <summary>
        /// Quantizes an image.
        /// </summary>
        /// <param name="image">The image (ARGB).</param>
        /// <param name="colorCount">The color count.</param>
        /// <param name="destPixels">Indexed pixelData will be written there</param>
        /// <param name="padMultiple4">True to pad rows to multiple of 4</param>
        /// <returns>Bitmap with indexed colors</returns>
        public unsafe uint[] Quantize(Bitmap image, int colorCount, byte[] destPixels, bool padMultiple4)
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

            uint[] res;
            fixed (byte* p = destPixels)
            {
                res = this.Quantize(
                   (uint*)imgdata.Scan0.ToPointer(),
                   colorCount,
                   image.Width,
                   image.Height,
                   p,
                   padMultiple4);
            }

            image.UnlockBits(imgdata);

            return res;
        }

        /// <summary>
        /// Gets an index.
        /// </summary>
        /// <param name="r">The red value.</param>
        /// <param name="g">The green value.</param>
        /// <param name="b">The blue value.</param>
        /// <param name="a">The alpha value.</param>
        /// <returns>The index.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private static int GetIndex(int r, int g, int b, int a)
        {
            return (r << ((WuQuant.IndexBits * 2) + WuQuant.IndexAlphaBits))
                + (r << (WuQuant.IndexBits + WuQuant.IndexAlphaBits + 1))
                + (g << (WuQuant.IndexBits + WuQuant.IndexAlphaBits))
                + (r << (WuQuant.IndexBits * 2))
                + (r << (WuQuant.IndexBits + 1))
                + (g << WuQuant.IndexBits)
                + ((r + g + b) << WuQuant.IndexAlphaBits)
                + r + g + b + a;
        }

        /// <summary>
        /// Computes sum over a box of any given statistic.
        /// </summary>
        /// <param name="cube">The cube.</param>
        /// <param name="moment">The moment.</param>
        /// <returns>The result.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private static float Volume(ref Box cube, int[] moment)
        {
            return (float)(moment[WuQuant.GetIndex(cube.R1, cube.G1, cube.B1, cube.A1)]
                - moment[WuQuant.GetIndex(cube.R1, cube.G1, cube.B1, cube.A0)]
                - moment[WuQuant.GetIndex(cube.R1, cube.G1, cube.B0, cube.A1)]
                + moment[WuQuant.GetIndex(cube.R1, cube.G1, cube.B0, cube.A0)]
                - moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B1, cube.A1)]
                + moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B1, cube.A0)]
                + moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B0, cube.A1)]
                - moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B0, cube.A0)]
                - moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B1, cube.A1)]
                + moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B1, cube.A0)]
                + moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B0, cube.A1)]
                - moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B0, cube.A0)]
                + moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B1, cube.A1)]
                - moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B1, cube.A0)]
                - moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B0, cube.A1)]
                + moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B0, cube.A0)]);
        }

        /// <summary>
        /// Computes part of Volume(cube, moment) that doesn't depend on r1, g1, or b1 (depending on direction).
        /// </summary>
        /// <param name="cube">The cube.</param>
        /// <param name="direction">The direction.</param>
        /// <param name="moment">The moment.</param>
        /// <returns>The result.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private static int Bottom(ref Box cube, int direction, int[] moment)
        {
            switch (direction)
            {
                // Red
                case 3:
                    return -moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B1, cube.A1)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B1, cube.A0)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B0, cube.A1)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B0, cube.A0)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B1, cube.A1)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B1, cube.A0)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B0, cube.A1)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B0, cube.A0)];

                // Green
                case 2:
                    return -moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B1, cube.A1)]
                        + moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B1, cube.A0)]
                        + moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B0, cube.A1)]
                        - moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B0, cube.A0)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B1, cube.A1)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B1, cube.A0)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B0, cube.A1)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B0, cube.A0)];

                // Blue
                case 1:
                    return -moment[WuQuant.GetIndex(cube.R1, cube.G1, cube.B0, cube.A1)]
                        + moment[WuQuant.GetIndex(cube.R1, cube.G1, cube.B0, cube.A0)]
                        + moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B0, cube.A1)]
                        - moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B0, cube.A0)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B0, cube.A1)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B0, cube.A0)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B0, cube.A1)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B0, cube.A0)];

                // Alpha
                case 0:
                    return -moment[WuQuant.GetIndex(cube.R1, cube.G1, cube.B1, cube.A0)]
                        + moment[WuQuant.GetIndex(cube.R1, cube.G1, cube.B0, cube.A0)]
                        + moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B1, cube.A0)]
                        - moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B0, cube.A0)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B1, cube.A0)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B0, cube.A0)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B1, cube.A0)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B0, cube.A0)];

                default:
                    throw new ArgumentOutOfRangeException("direction");
            }
        }

        /// <summary>
        /// Computes remainder of Volume(cube, moment), substituting position for r1, g1, or b1 (depending on direction).
        /// </summary>
        /// <param name="cube">The cube.</param>
        /// <param name="direction">The direction.</param>
        /// <param name="position">The position.</param>
        /// <param name="moment">The moment.</param>
        /// <returns>The result.</returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private static int Top(ref Box cube, int direction, int position, int[] moment)
        {
            switch (direction)
            {
                // Red
                case 3:
                    return moment[WuQuant.GetIndex(position, cube.G1, cube.B1, cube.A1)]
                        - moment[WuQuant.GetIndex(position, cube.G1, cube.B1, cube.A0)]
                        - moment[WuQuant.GetIndex(position, cube.G1, cube.B0, cube.A1)]
                        + moment[WuQuant.GetIndex(position, cube.G1, cube.B0, cube.A0)]
                        - moment[WuQuant.GetIndex(position, cube.G0, cube.B1, cube.A1)]
                        + moment[WuQuant.GetIndex(position, cube.G0, cube.B1, cube.A0)]
                        + moment[WuQuant.GetIndex(position, cube.G0, cube.B0, cube.A1)]
                        - moment[WuQuant.GetIndex(position, cube.G0, cube.B0, cube.A0)];

                // Green
                case 2:
                    return moment[WuQuant.GetIndex(cube.R1, position, cube.B1, cube.A1)]
                        - moment[WuQuant.GetIndex(cube.R1, position, cube.B1, cube.A0)]
                        - moment[WuQuant.GetIndex(cube.R1, position, cube.B0, cube.A1)]
                        + moment[WuQuant.GetIndex(cube.R1, position, cube.B0, cube.A0)]
                        - moment[WuQuant.GetIndex(cube.R0, position, cube.B1, cube.A1)]
                        + moment[WuQuant.GetIndex(cube.R0, position, cube.B1, cube.A0)]
                        + moment[WuQuant.GetIndex(cube.R0, position, cube.B0, cube.A1)]
                        - moment[WuQuant.GetIndex(cube.R0, position, cube.B0, cube.A0)];

                // Blue
                case 1:
                    return moment[WuQuant.GetIndex(cube.R1, cube.G1, position, cube.A1)]
                        - moment[WuQuant.GetIndex(cube.R1, cube.G1, position, cube.A0)]
                        - moment[WuQuant.GetIndex(cube.R1, cube.G0, position, cube.A1)]
                        + moment[WuQuant.GetIndex(cube.R1, cube.G0, position, cube.A0)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G1, position, cube.A1)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G1, position, cube.A0)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G0, position, cube.A1)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G0, position, cube.A0)];

                // Alpha
                case 0:
                    return moment[WuQuant.GetIndex(cube.R1, cube.G1, cube.B1, position)]
                        - moment[WuQuant.GetIndex(cube.R1, cube.G1, cube.B0, position)]
                        - moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B1, position)]
                        + moment[WuQuant.GetIndex(cube.R1, cube.G0, cube.B0, position)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B1, position)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G1, cube.B0, position)]
                        + moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B1, position)]
                        - moment[WuQuant.GetIndex(cube.R0, cube.G0, cube.B0, position)];

                default:
                    throw new ArgumentOutOfRangeException("direction");
            }
        }

        /// <summary>
        /// Clears the tables.
        /// </summary>
        private void Clear()
        {
            Array.Clear(this.vwt, 0, WuQuant.TableLength);
            Array.Clear(this.vmr, 0, WuQuant.TableLength);
            Array.Clear(this.vmg, 0, WuQuant.TableLength);
            Array.Clear(this.vmb, 0, WuQuant.TableLength);
            Array.Clear(this.vma, 0, WuQuant.TableLength);
            Array.Clear(this.m2, 0, WuQuant.TableLength);
            Array.Clear(this.tag, 0, WuQuant.TableLength);
        }

        /// <summary>
        /// Builds a 3-D color histogram of <c>counts, r/g/b, c^2</c>.
        /// </summary>
        /// <param name="image">The image.</param>
        private void Build3DHistogram(byte[] image)
        {
            for (int i = 0; i < image.Length; i += 4)
            {
                int a = image[i + 3];
                int r = image[i + 2];
                int g = image[i + 1];
                int b = image[i];

                int inr = r >> (8 - WuQuant.IndexBits);
                int ing = g >> (8 - WuQuant.IndexBits);
                int inb = b >> (8 - WuQuant.IndexBits);
                int ina = a >> (8 - WuQuant.IndexAlphaBits);

                int ind = WuQuant.GetIndex(inr + 1, ing + 1, inb + 1, ina + 1);

                this.vwt[ind]++;
                this.vmr[ind] += r;
                this.vmg[ind] += g;
                this.vmb[ind] += b;
                this.vma[ind] += a;
                this.m2[ind] += (r * r) + (g * g) + (b * b) + (a * a);
            }
        }

        /// <summary>
        /// Builds a 3-D color histogram of <c>counts, r/g/b, c^2</c>.
        /// </summary>
        /// <param name="image">The image.</param>
        /// <param name="width">Width of image</param>
        /// <param name="height">Height of image</param>
        private unsafe void Build3DHistogram(uint* image, int width, int height)
        {
            int pixels = width * height;

            for (int i = 0; i < pixels; i++)
            {
                uint pix = image[i];
                int a = (int)((pix & 0xFF000000) >> 24);
                int r = (int)((pix & 0x00FF0000) >> 16);
                int g = (int)((pix & 0x0000FF00) >> 8);
                int b = (int)(pix & 0x000000FF);

                int inr = r >> (8 - WuQuant.IndexBits);
                int ing = g >> (8 - WuQuant.IndexBits);
                int inb = b >> (8 - WuQuant.IndexBits);
                int ina = a >> (8 - WuQuant.IndexAlphaBits);

                int ind = WuQuant.GetIndex((int)inr + 1, (int)ing + 1, (int)inb + 1, (int)ina + 1);

                this.vwt[ind]++;
                this.vmr[ind] += r;
                this.vmg[ind] += g;
                this.vmb[ind] += b;
                this.vma[ind] += a;
                this.m2[ind] += (r * r) + (g * g) + (b * b) + (a * a);
            }
        }

        /// <summary>
        /// Converts the histogram into moments so that we can rapidly calculate
        /// the sums of the above quantities over any desired box.
        /// </summary>
        private void Get3DMoments()
        {
            for (int r = 1; r < WuQuant.IndexCount; r++)
            {
                /*Array.Clear(this.volume, 0, WuAlphaColorQuantizer.IndexCount * WuAlphaColorQuantizer.IndexAlphaCount);
                Array.Clear(this.volumeR, 0, WuAlphaColorQuantizer.IndexCount * WuAlphaColorQuantizer.IndexAlphaCount);
                Array.Clear(this.volumeG, 0, WuAlphaColorQuantizer.IndexCount * WuAlphaColorQuantizer.IndexAlphaCount);
                Array.Clear(this.volumeB, 0, WuAlphaColorQuantizer.IndexCount * WuAlphaColorQuantizer.IndexAlphaCount);
                Array.Clear(this.volumeA, 0, WuAlphaColorQuantizer.IndexCount * WuAlphaColorQuantizer.IndexAlphaCount);
                Array.Clear(this.volume2, 0, WuAlphaColorQuantizer.IndexCount * WuAlphaColorQuantizer.IndexAlphaCount);*/
                Array.Clear(this.volume, 0, this.volume.Length);

                for (int g = 1; g < WuQuant.IndexCount; g++)
                {
                    Array.Clear(this.area, 0, WuQuant.IndexAlphaCount);
                    Array.Clear(this.areaR, 0, WuQuant.IndexAlphaCount);
                    Array.Clear(this.areaG, 0, WuQuant.IndexAlphaCount);
                    Array.Clear(this.areaB, 0, WuQuant.IndexAlphaCount);
                    Array.Clear(this.areaA, 0, WuQuant.IndexAlphaCount);
                    Array.Clear(this.area2, 0, WuQuant.IndexAlphaCount);

                    for (int b = 1; b < WuQuant.IndexCount; b++)
                    {
                        int line = 0;
                        int lineR = 0;
                        int lineG = 0;
                        int lineB = 0;
                        int lineA = 0;
                        float line2 = 0;

                        for (int a = 1; a < WuQuant.IndexAlphaCount; a++)
                        {
                            int ind1 = WuQuant.GetIndex(r, g, b, a);

                            line += this.vwt[ind1];
                            lineR += this.vmr[ind1];
                            lineG += this.vmg[ind1];
                            lineB += this.vmb[ind1];
                            lineA += this.vma[ind1];
                            line2 += this.m2[ind1];

                            this.area[a] += line;
                            this.areaR[a] += lineR;
                            this.areaG[a] += lineG;
                            this.areaB[a] += lineB;
                            this.areaA[a] += lineA;
                            this.area2[a] += line2;

                            int inv = (b * WuQuant.IndexAlphaCount) + a;

                            this.volume[inv].Vol += this.area[a];
                            this.volume[inv].VolumeR += this.areaR[a];
                            this.volume[inv].VolumeG += this.areaG[a];
                            this.volume[inv].VolumeB += this.areaB[a];
                            this.volume[inv].VolumeA += this.areaA[a];
                            this.volume[inv].Volume2 += this.area2[a];

                            int ind2 = ind1 - WuQuant.GetIndex(1, 0, 0, 0);

                            this.vwt[ind1] = this.vwt[ind2] + this.volume[inv].Vol;
                            this.vmr[ind1] = this.vmr[ind2] + this.volume[inv].VolumeR;
                            this.vmg[ind1] = this.vmg[ind2] + this.volume[inv].VolumeG;
                            this.vmb[ind1] = this.vmb[ind2] + this.volume[inv].VolumeB;
                            this.vma[ind1] = this.vma[ind2] + this.volume[inv].VolumeA;
                            this.m2[ind1] = this.m2[ind2] + this.volume[inv].Volume2;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Computes the weighted variance of a box.
        /// </summary>
        /// <param name="c">The cube.</param>
        /// <returns>The result.</returns>
        private float Variance(ref Box c)
        {
            float dr = WuQuant.Volume(ref c, this.vmr);
            float dg = WuQuant.Volume(ref c, this.vmg);
            float db = WuQuant.Volume(ref c, this.vmb);
            float da = WuQuant.Volume(ref c, this.vma);

            float xx =
                this.m2[WuQuant.GetIndex(c.R1, c.G1, c.B1, c.A1)]
                - this.m2[WuQuant.GetIndex(c.R1, c.G1, c.B1, c.A0)]
                - this.m2[WuQuant.GetIndex(c.R1, c.G1, c.B0, c.A1)]
                + this.m2[WuQuant.GetIndex(c.R1, c.G1, c.B0, c.A0)]
                - this.m2[WuQuant.GetIndex(c.R1, c.G0, c.B1, c.A1)]
                + this.m2[WuQuant.GetIndex(c.R1, c.G0, c.B1, c.A0)]
                + this.m2[WuQuant.GetIndex(c.R1, c.G0, c.B0, c.A1)]
                - this.m2[WuQuant.GetIndex(c.R1, c.G0, c.B0, c.A0)]
                - this.m2[WuQuant.GetIndex(c.R0, c.G1, c.B1, c.A1)]
                + this.m2[WuQuant.GetIndex(c.R0, c.G1, c.B1, c.A0)]
                + this.m2[WuQuant.GetIndex(c.R0, c.G1, c.B0, c.A1)]
                - this.m2[WuQuant.GetIndex(c.R0, c.G1, c.B0, c.A0)]
                + this.m2[WuQuant.GetIndex(c.R0, c.G0, c.B1, c.A1)]
                - this.m2[WuQuant.GetIndex(c.R0, c.G0, c.B1, c.A0)]
                - this.m2[WuQuant.GetIndex(c.R0, c.G0, c.B0, c.A1)]
                + this.m2[WuQuant.GetIndex(c.R0, c.G0, c.B0, c.A0)];

            return xx - (((dr * dr) + (dg * dg) + (db * db) + (da * da)) / WuQuant.Volume(ref c, this.vwt));
        }

        /// <summary>
        /// We want to minimize the sum of the variances of two sub-boxes.
        /// The sum(c^2) terms can be ignored since their sum over both sub-boxes
        /// is the same (the sum for the whole box) no matter where we split.
        /// The remaining terms have a minus sign in the variance formula,
        /// so we drop the minus sign and maximize the sum of the two terms.
        /// </summary>
        /// <param name="c">The cube.</param>
        /// <param name="direction">The direction.</param>
        /// <param name="first">The first position.</param>
        /// <param name="last">The last position.</param>
        /// <param name="cut">The cutting point.</param>
        /// <param name="wholeR">The whole red.</param>
        /// <param name="wholeG">The whole green.</param>
        /// <param name="wholeB">The whole blue.</param>
        /// <param name="wholeA">The whole alpha.</param>
        /// <param name="wholeW">The whole weight.</param>
        /// <returns>The result.</returns>
        private float Maximize(ref Box c, int direction, int first, int last, out int cut, float wholeR, float wholeG, float wholeB, float wholeA, float wholeW)
        {
            int baseR = Bottom(ref c, direction, this.vmr);
            int baseG = WuQuant.Bottom(ref c, direction, this.vmg);
            int baseB = WuQuant.Bottom(ref c, direction, this.vmb);
            int baseA = WuQuant.Bottom(ref c, direction, this.vma);
            int baseW = WuQuant.Bottom(ref c, direction, this.vwt);

            float max = 0.0f;
            cut = -1;

            for (int i = first; i < last; i++)
            {
                float halfR = baseR + WuQuant.Top(ref c, direction, i, this.vmr);
                float halfG = baseG + WuQuant.Top(ref c, direction, i, this.vmg);
                float halfB = baseB + WuQuant.Top(ref c, direction, i, this.vmb);
                float halfA = baseA + WuQuant.Top(ref c, direction, i, this.vma);
                float halfW = baseW + WuQuant.Top(ref c, direction, i, this.vwt);

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
                    cut = i;
                }
            }

            return max;
        }

        /// <summary>
        /// Cuts a box.
        /// </summary>
        /// <param name="set1">The first set.</param>
        /// <param name="set2">The second set.</param>
        /// <returns>Returns a value indicating whether the box has been split.</returns>
        private bool Cut(ref Box set1, ref Box set2)
        {
            float wholeR = WuQuant.Volume(ref set1, this.vmr);
            float wholeG = WuQuant.Volume(ref set1, this.vmg);
            float wholeB = WuQuant.Volume(ref set1, this.vmb);
            float wholeA = WuQuant.Volume(ref set1, this.vma);
            float wholeW = WuQuant.Volume(ref set1, this.vwt);

            int cutr;
            int cutg;
            int cutb;
            int cuta;

            float maxr = this.Maximize(ref set1, 3, set1.R0 + 1, set1.R1, out cutr, wholeR, wholeG, wholeB, wholeA, wholeW);
            float maxg = this.Maximize(ref set1, 2, set1.G0 + 1, set1.G1, out cutg, wholeR, wholeG, wholeB, wholeA, wholeW);
            float maxb = this.Maximize(ref set1, 1, set1.B0 + 1, set1.B1, out cutb, wholeR, wholeG, wholeB, wholeA, wholeW);
            float maxa = this.Maximize(ref set1, 0, set1.A0 + 1, set1.A1, out cuta, wholeR, wholeG, wholeB, wholeA, wholeW);

            int dir;

            if ((maxr >= maxg) && (maxr >= maxb) && (maxr >= maxa))
            {
                dir = 3;

                if (cutr < 0)
                {
                    return false;
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

            set2.R1 = set1.R1;
            set2.G1 = set1.G1;
            set2.B1 = set1.B1;
            set2.A1 = set1.A1;

            switch (dir)
            {
                // Red
                case 3:
                    set2.R0 = set1.R1 = cutr;
                    set2.G0 = set1.G0;
                    set2.B0 = set1.B0;
                    set2.A0 = set1.A0;
                    break;

                // Green
                case 2:
                    set2.G0 = set1.G1 = cutg;
                    set2.R0 = set1.R0;
                    set2.B0 = set1.B0;
                    set2.A0 = set1.A0;
                    break;

                // Blue
                case 1:
                    set2.B0 = set1.B1 = cutb;
                    set2.R0 = set1.R0;
                    set2.G0 = set1.G0;
                    set2.A0 = set1.A0;
                    break;

                // Alpha
                case 0:
                    set2.A0 = set1.A1 = cuta;
                    set2.R0 = set1.R0;
                    set2.G0 = set1.G0;
                    set2.B0 = set1.B0;
                    break;
            }

            set1.Volume = (set1.R1 - set1.R0) * (set1.G1 - set1.G0) * (set1.B1 - set1.B0) * (set1.A1 - set1.A0);
            set2.Volume = (set2.R1 - set2.R0) * (set2.G1 - set2.G0) * (set2.B1 - set2.B0) * (set2.A1 - set2.A0);

            return true;
        }

        /// <summary>
        /// Marks a color space tag.
        /// </summary>
        /// <param name="c">The cube.</param>
        /// <param name="label">A label.</param>
        private void Mark(ref Box c, byte label)
        {
            for (int r = c.R0 + 1; r <= c.R1; r++)
            {
                for (int g = c.G0 + 1; g <= c.G1; g++)
                {
                    for (int b = c.B0 + 1; b <= c.B1; b++)
                    {
                        for (int a = c.A0 + 1; a <= c.A1; a++)
                        {
                            this.tag[WuQuant.GetIndex(r, g, b, a)] = label;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Builds the cube.
        /// </summary>
        /// <param name="colorCount">The color count.</param>
        private void BuildCube(ref int colorCount)
        {
            for (int i = 0; i < colorCount; i++)
            {
                this.cube[i].Clear();
            }

            // clear temporary vv memory
            Array.Clear(this.vv, 0, this.vv.Length);

            this.cube[0].R0 = this.cube[0].G0 = this.cube[0].B0 = this.cube[0].A0 = 0;
            this.cube[0].R1 = this.cube[0].G1 = this.cube[0].B1 = WuQuant.IndexCount - 1;
            this.cube[0].A1 = WuQuant.IndexAlphaCount - 1;

            int next = 0;

            for (int i = 1; i < colorCount; i++)
            {
                if (this.Cut(ref this.cube[next], ref this.cube[i]))
                {
                    this.vv[next] = this.cube[next].Volume > 1 ? this.Variance(ref this.cube[next]) : 0.0f;
                    this.vv[i] = this.cube[i].Volume > 1 ? this.Variance(ref this.cube[i]) : 0.0f;
                }
                else
                {
                    this.vv[next] = 0.0f;
                    i--;
                }

                next = 0;

                float temp = this.vv[0];
                for (int k = 1; k <= i; k++)
                {
                    if (this.vv[k] > temp)
                    {
                        temp = this.vv[k];
                        next = k;
                    }
                }

                if (temp <= 0.0)
                {
                    colorCount = i + 1;
                    break;
                }
            }
        }

        /// <summary>
        /// Generates the quantized result.
        /// </summary>
        /// <param name="image">The image.</param>
        /// <param name="colorCount">The color count.</param>
        /// <param name="width">Width of image</param>
        /// <param name="height">Height of image</param>
        /// <param name="destPixels">Pixel values are written here. Must provide width*height memory.</param>
        /// <param name="padMultiple4">True to write zero padding to make row bytes multiple of 4</param>
        /// <returns>Receives colors</returns>
        private unsafe uint[] GenerateResult(uint* image, int colorCount, int width, int height, byte* destPixels, bool padMultiple4)
        {
            uint[] palette = new uint[colorCount];

            // rows must be a multiple of 4, hence padding up to 3 bytes for 8-bit indexed pixels
            int widthMod4 = width % 4;
            int widthZeros = widthMod4 != 0 ? 4 - widthMod4 : 0;

            for (int k = 0; k < colorCount; k++)
            {
                this.Mark(ref this.cube[k], (byte)k);

                float weight = WuQuant.Volume(ref this.cube[k], this.vwt);

                if (weight != 0)
                {
                    uint a = (byte)(WuQuant.Volume(ref this.cube[k], this.vma) / weight);
                    uint r = (byte)(WuQuant.Volume(ref this.cube[k], this.vmr) / weight);
                    uint g = (byte)(WuQuant.Volume(ref this.cube[k], this.vmg) / weight);
                    uint b = (byte)(WuQuant.Volume(ref this.cube[k], this.vmb) / weight);

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
                    uint pix = image[0];

                    uint a = ((pix & 0xFF000000) >> 24) >> (8 - WuQuant.IndexAlphaBits);
                    uint r = ((pix & 0x00FF0000) >> 16) >> (8 - WuQuant.IndexBits);
                    uint g = ((pix & 0x0000FF00) >> 8) >> (8 - WuQuant.IndexBits);
                    uint b = (pix & 0x000000FF) >> (8 - WuQuant.IndexBits);

                    int ind = WuQuant.GetIndex((int)r + 1, (int)g + 1, (int)b + 1, (int)a + 1);

                    destPixels[0] = this.tag[ind];
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

            return palette;
        }

    }
}
