#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <cstdio>

typedef int LONG;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef unsigned short WORD;

typedef struct tagBITMAPFILEHEADER 
{
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} __attribute__((packed)) BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER 
{
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompress;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} __attribute__((packed)) BITMAPINFOHEADER;

struct pixel 
{
    unsigned char b, g, r;
};

int compare(pixel p, pixel p2) 
{
    if (p.b == p2.b && p.g == p2.g && p.r == p2.r)
        return 1;

    return 0;
}

void zip_img(char file_path[]) 
{
    unsigned char *vetc;
    int *vet;
    int size = 0, bc, x;
    int count, i, j = 0;
    pixel prev, curr;
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    FILE *img, *zip;

    img = fopen(file_path, "rb+");

    fread(&bf, sizeof (BITMAPFILEHEADER), 1, img);
    fread(&bi, sizeof (BITMAPINFOHEADER), 1, img);

    printf("\n\nFile size: %d bytes", bf.bfSize);
    printf("\nImage width: %d", bi.biWidth);
    printf("\nImage height: %d", bi.biHeight);
    printf("\nColor pattern: %d bits", bi.biBitCount);
    printf("\nImage size: %d bits\n", bi.biSizeImage);

    bc = (bi.biSizeImage - (bi.biWidth * bi.biHeight * 3)) / bi.biHeight; //padding


    fprintf(stderr, "\nFINISHED\n\n");

    x = 0;
    count = 1;
    vet = new int[bi.biWidth * bi.biHeight * 4];
    vetc = new unsigned char[bi.biWidth * bi.biHeight * 4];

    fread(&prev, sizeof (pixel), 1, img);
    while (!feof(img)) 
    {
        x++;
        if (x >= bi.biWidth) 
        {
            x = 0;
            fseek(img, bc, 1);
        }

        fread(&curr, sizeof (pixel), 1, img);
        if (!compare(prev, curr) || feof(img)) 
        {
            vet[size++] = count;
            vetc[j++] = prev.b;
            vetc[j++] = prev.g;
            vetc[j++] = prev.r;
            count = 1;
        } 
        else
            count++;

        prev = curr;
    }
    fclose(img);

    sprintf(file_path, "comp.ed", file_path);

    zip = fopen(file_path, "wb");

    fwrite(&bf, sizeof (BITMAPFILEHEADER), 1, zip);
    fwrite(&bi, sizeof (BITMAPINFOHEADER), 1, zip);

    for (i = 0, j = 0; i < size; i++) 
    {
        fwrite(&vet[i], sizeof (int), 1, zip);
        fwrite(&vetc[j++], sizeof (unsigned char), 1, zip);
        fwrite(&vetc[j++], sizeof (unsigned char), 1, zip);
        fwrite(&vetc[j++], sizeof (unsigned char), 1, zip);
    }

    fclose(zip);
}

void unzip_img(char file_path[]) 
{
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    FILE *img, *unzip;
    int x, bc, qty;
    unsigned char r, g, b, p = 0;

    img = fopen(file_path, "rb");

    fread(&bf, sizeof (BITMAPFILEHEADER), 1, img);
    fread(&bi, sizeof (BITMAPINFOHEADER), 1, img);

    sprintf(file_path, "unzipped.bmp", file_path);

    unzip = fopen(file_path, "wb+");

    fwrite(&bf, sizeof (BITMAPFILEHEADER), 1, unzip);
    fwrite(&bi, sizeof (BITMAPINFOHEADER), 1, unzip);

    bc = (bi.biSizeImage - (bi.biWidth * bi.biHeight * 3)) / bi.biHeight;

    x = 0;
    while (!feof(img)) 
    {
        fread(&qty, sizeof (int), 1, img);
        fread(&b, sizeof (unsigned char), 1, img);
        fread(&g, sizeof (unsigned char), 1, img);
        fread(&r, sizeof (unsigned char), 1, img);

        for (int i = 0; i < qty; i++) 
        {
            fwrite(&b, sizeof (unsigned char), 1, unzip);
            fwrite(&g, sizeof (unsigned char), 1, unzip);
            fwrite(&r, sizeof (unsigned char), 1, unzip);

            x++;
            if (x >= bi.biWidth) 
            {
                x = 0;
                fwrite(&p, sizeof (unsigned char), bc, unzip);
            }
        }
    }

    fclose(img);
    fclose(unzip);
}

int main(int argc, char **argv) 
{
    char *path, *comm;

    path = argv[1];
    comm = argv[2];

    if (strcmp(comm, "zip") == 0)
        zip_img(path);
    if (strcmp(comm, "unzip") == 0)
        unzip_img(path);
}