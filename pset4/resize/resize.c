/**
 * Resizes a BMP by given factor.
 */
       
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bmp.h"

bool resize(FILE *in, FILE *out, float f);

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        fprintf(stderr, "Usage: ./resize f infile outfile\n");
        return 1;
    }

    float f = atof(argv[1]);
    if (f <= 0. || f > 100.) {
        fprintf(stderr, "Factor value should be within range (0, 100], given value = %f", f);
        return 2;
    }
    
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 3;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 4;
    }

    // perform resize
    bool result = resize(inptr, outptr, f);

    fclose(outptr);
    fclose(inptr);

    if (!result) {
        fprintf(stderr, "Unsupported file format.\n");
        return 5;  
    };

    return 0;
}

bool resize(FILE *infile, FILE *outfile, float f) {
    
    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, infile);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, infile);
    
    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || bi.biBitCount != 24 || bi.biCompression != 0)
        return false;

    int in_size_w = bi.biWidth;
    int in_size_h = bi.biHeight;
    int out_size_w = in_size_w * f;
    int out_size_h = in_size_h * f;
    
    BITMAPINFOHEADER bi_out = bi;
    BITMAPFILEHEADER bf_out = bf;
    bi_out.biWidth = out_size_w;
    bi_out.biHeight = out_size_h;
    
    // determine padding for scanlines
    int padding_in = (4 - (in_size_w * sizeof(RGBTRIPLE)) % 4) % 4;
    int padding_out = (4 - (out_size_w * sizeof(RGBTRIPLE)) % 4) % 4;
    bi_out.biSizeImage = abs(out_size_h) * (out_size_w * sizeof(RGBTRIPLE) + padding_out);
    
    // calculate total size
    // int size_in = sizeof(bf) + sizeof(bi) + in_size_w * abs(in_size_h) * sizeof(RGBTRIPLE) + padding_in * in_size_w;
    int size_out = sizeof(bf_out) + sizeof(bi_out) + out_size_w * abs(out_size_h) * sizeof(RGBTRIPLE) + padding_out * abs(out_size_h);
    
    // printf("padding_in: %i, bi.biSizeImage = %i\n", padding_in, bi.biSizeImage);

    bf_out.bfSize = size_out;

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf_out, sizeof(BITMAPFILEHEADER), 1, outfile);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi_out, sizeof(BITMAPINFOHEADER), 1, outfile);

    // iterate over out-file's scanlines
    for (int i = 0, height = abs(out_size_h); i < height; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < out_size_w; j++)
        {
            // temporary storage
            RGBTRIPLE triple;
            
            int i_in = i / f;
            int j_in = j / f;
            int position = sizeof(bf) + sizeof(bi) 
                + i_in * ( in_size_w * sizeof(RGBTRIPLE) + padding_in )
                + j_in * sizeof(RGBTRIPLE);
            
            // read RGB triple from infile
            fseek(infile, position, SEEK_SET);
            fread(&triple, sizeof(RGBTRIPLE), 1, infile);

            //printf("x = %i <- %i, y = %i <- %i, pos = %i, RGB = (%i, %i, %i)\n", j, j_in, i, i_in, position, triple.rgbtRed, triple.rgbtGreen, triple.rgbtBlue);
            
            // write RGB triple to outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outfile);
        }

        // skip over padding, if any
        // ** fseek(infile, padding_in, SEEK_CUR);

        // then add it back (to demonstrate how)
        for (int k = 0; k < padding_out; k++)
            fputc(0x00, outfile);

    }    
    
    return true;
}