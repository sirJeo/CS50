/**
 * resize.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize infile outfile\n");
        return 1;
    }

    // remember filenames
    int rate = atoi(argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];

    // printf("Rate %i\n", rate);

    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    int width = bi.biWidth;
    int height = bi.biHeight; 

    bi.biWidth = bi.biWidth * rate;
    bi.biHeight = bi.biHeight * rate;

    int padding =  (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;
    int newPadding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    bi.biSizeImage = (bi.biWidth * sizeof(RGBTRIPLE) + newPadding) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + 54;
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);


    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(height); i < biHeight; i++)
    {
        RGBTRIPLE line[bi.biWidth];
        int li = 0;
        // iterate over pixels in scanline
        for (int j = 0; j < width; j++)
        {
            RGBTRIPLE triple;

            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
            // read RGB triple from infile
            for (int r = 0; r < rate; r++) {
                memcpy(&line[li++], &triple, sizeof(RGBTRIPLE));
                // line[li++] = triple;
            }
        }

        for (int ri = 0; ri < rate; ri++) {
            for (li = 0; li < bi.biWidth; li++) {
                // write RGB triple to outfile
                fwrite(&line[li], sizeof(RGBTRIPLE), 1, outptr);
            }

            // then add it back (to demonstrate how)
            for (int k = 0; k < newPadding; k++)
            {
                fputc(0x00, outptr);
            }
        }

        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);        
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}
