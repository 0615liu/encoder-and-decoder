#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
typedef unsigned char uint8_t;
#define PI 3.14159265359
typedef struct _BMP_header 
{
    char identifier[2]; // 0x0000
    unsigned int filesize; // 0x0002
    unsigned short reserved; // 0x0006
    unsigned short reserved2;
    unsigned int bitmap_dataoffset; // 0x000A
    unsigned int bitmap_headersize; // 0x000E
    unsigned int width; // 0x0012
    unsigned int height; // 0x0016
    unsigned short planes; // 0x001A
    unsigned short bits_perpixel; // 0x001C
    unsigned int compression; // 0x001E
    unsigned int bitmap_datasize; // 0x0022
    unsigned int hresolution; // 0x0026
    unsigned int vresolution; // 0x002A
    unsigned int usedcolors; // 0x002E
    unsigned int importantcolors; // 0x0032
  //  unsigned int palette; // 0x0036
} BMP_header;

typedef struct RGB
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} ImgRGB;

typedef struct YCbCr
{
	float Y;
	float Cb;
	float Cr;
} ImgYCbCr;

/* structure for BMP file */
typedef struct _BMP_file
{
    char _filename[1024];
    BMP_header header;
    int H;
    int W;
    unsigned char **R;// channel for red
    unsigned char **G;// channel for Green
    unsigned char **B;// channel for Blue
} BMP_file;

/* prototypes */
/* load BMP given with file name (fn) */
int BMP_file_load_fn(char *fn, BMP_file *p_bmp);

/* save BMP as text file */
int BMP_save_text(BMP_file *p_bmp, char *fn_R, char *fn_G, char *fn_B, char *fn_dim);

//myBMPLib.h before main

void usage(FILE *fp)
{
    fprintf(fp, "encoder 0 KimberlyNCat.bmp R.txt G.txt B.txt dim.txt\n");
    return;
}

void RGBtoYCbCr(ImgRGB **RGB, ImgYCbCr **YCbCr);
void DCT(ImgYCbCr  **data, float ****basis_vector, ImgYCbCr ****F);


int demo0(char *fn_bmp, char *fn_R, char *fn_G, char *fn_B, char *fn_dim)
{
    BMP_file bmp;
    if( BMP_file_load_fn(fn_bmp, &bmp)!= 1 ) 
    {
        exit(1);
    }
    // do something with bmp ==> datum for R, G, B, and dim
    
    BMP_save_text(&bmp, fn_R, fn_G, fn_B, fn_dim);

    return 1; //if return 1 means success product txt
}

int H,W;
int main(int argc, char **argv)
{
    int option = 0;// 0 for demo0, 1 for demo1, etc
    BMP_file bmp; // BMP file structure
    if ( argc <=2 ) {
        usage(stderr);
        exit(1);
    }

    option = atoi(argv[1]);

    switch(option)
    {
        case 0:
        demo0(argv[2], argv[3], argv[4], argv[5], argv[6]);
        break;
        // case 1:
        // demo1(argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9], argv[10], argv[11], argv[12]);
        default:
        break;
        // default statements
    }
    BMP_file p_bmp;
    
    BMP_file_load_fn("KimberlyNCat.bmp", &p_bmp);
   
    // FILE *output=fopen("ResKim.bmp","wb");                      //must have wb
    // output_bmp(RGB,  output, p_bmp.header,0);

    return 0; // 0: exit normally 1: exit with error
}


/*read header*/
//Read the header information of the BMP image from the file

//myBMPLib.c after main
void _readheader(FILE* fp, BMP_header *x)
{
    fread(&x->identifier, sizeof(x->identifier), 1, fp);
    fread(&x->filesize, sizeof(x->filesize), 1, fp);
    fread(&x->reserved, sizeof(x->reserved), 1, fp);
    fread(&x->reserved2, sizeof(x->reserved2), 1, fp);
    fread(&x->bitmap_dataoffset, sizeof(x->bitmap_dataoffset), 1, fp);
    fread(&x->bitmap_headersize, sizeof(x->bitmap_headersize), 1, fp);
    fread(&x->width, sizeof(x->width), 1, fp);
    fread(&x->height, sizeof(x->height), 1, fp);
    fread(&x->planes, sizeof(x->planes), 1, fp);
    fread(&x->bits_perpixel, sizeof(x->bits_perpixel), 1, fp);
    fread(&x->compression, sizeof(x->compression), 1, fp);
    fread(&x->bitmap_datasize, sizeof(x->bitmap_datasize), 1, fp);
    fread(&x->hresolution, sizeof(x->hresolution), 1, fp);
    fread(&x->vresolution, sizeof(x->vresolution), 1, fp);
    fread(&x->usedcolors, sizeof(x->usedcolors), 1, fp);
    fread(&x->importantcolors, sizeof(x->importantcolors), 1, fp);
}

/* load BMP given with file name (fn) */
int BMP_file_load_fn(char *fn, BMP_file *p_bmp)
{
    char tmp_data = 0;
    size_t i = 0, j = 0;
    int skip = 0;
    char skip_buf[3] = { 0, 0, 0 };                 //
    FILE *fp = fopen(fn, "rb");// open file
    /* check if the file exist?*/
    if ( !fp ) {
       // fprintf(stderr, "cannot open %s\n", fn);
        return 0;
    }

    strcpy(p_bmp->_filename, fn);                        //
    
    // read header
    _readheader(fp, &(p_bmp->header));
    
    // data
    p_bmp->W = p_bmp->header.width;
    p_bmp->H = p_bmp->header.height;
    
    p_bmp->B = (unsigned char **) calloc(p_bmp->H, sizeof(unsigned char *));
    for ( i=0; i<p_bmp->H; i++) 
    {
        p_bmp->B[i] = (unsigned char *) calloc(p_bmp->W, sizeof(unsigned char));
    }
    p_bmp->G = (unsigned char **) calloc(p_bmp->H, sizeof(unsigned char *));
    for ( i=0; i<p_bmp->H; i++) 
    {
        p_bmp->G[i] = (unsigned char *) calloc(p_bmp->W, sizeof(unsigned char));
    }
    p_bmp->R = (unsigned char **) calloc(p_bmp->H, sizeof(unsigned char *));
    for ( i=0; i<p_bmp->H; i++) 
    {
        p_bmp->R[i] = (unsigned char *) calloc(p_bmp->W, sizeof(unsigned char));
    }

//Each row of pixel data is a multiple of 4
    skip = (4 - (p_bmp->W * 3) % 4);
    if (skip == 4) skip = 0; //4 divide by 4 reamains 0 

    for (i = 0; i<p_bmp->H; i++){
        for (j = 0; j<p_bmp->W; j++){
            fread(&(p_bmp->B[i][j]), sizeof(unsigned char), 1, fp);
            fread(&(p_bmp->G[i][j]), sizeof(unsigned char), 1, fp);
            fread(&(p_bmp->R[i][j]), sizeof(unsigned char), 1, fp);
        }
        if (skip != 0) fread(skip_buf, skip, 1, fp);
    } 

    fclose(fp);
    return 1;
}


int BMP_save_text(BMP_file *p_bmp, char *fn_R, char *fn_G, char *fn_B, char *fn_dim)
{
    //   p_bmp:  BMP file structure pointer, contain image data and information, will point to H W
    FILE *fp_R = fopen(fn_R, "w");
    int i, j;
    for( i=0;i<p_bmp->H; i++) {
        for( j=0;j<p_bmp->W;j++) {
            fprintf(fp_R, "%d\t", p_bmp->R[i][j]);
        }
        fprintf(fp_R, "\n");
    }

    fclose(fp_R);

    FILE *fp_G = fopen(fn_G, "w");
    for( i=0;i<p_bmp->H; i++) {
        for( j=0;j<p_bmp->W;j++) {
            fprintf(fp_G, "%d\t", p_bmp->G[i][j]);
        }
        fprintf(fp_G, "\n");
    }

    fclose(fp_G);

    FILE *fp_B = fopen(fn_B, "w");
    for( i=0;i<p_bmp->H; i++) {
        for( j=0;j<p_bmp->W;j++) {
            fprintf(fp_B, "%d\t", p_bmp->B[i][j]);
        }
        fprintf(fp_B, "\n");
    }

    fclose(fp_B);

    int lengthcalculate=0;
    int widcalculate=0;
    FILE *fp_dim = fopen(fn_dim, "w"); //Open write-only file
    for( i=0;i<p_bmp->H; i++) 
    {
      lengthcalculate+=1;
    }
    for( j=0;j<p_bmp->W;j++) 
    {
      widcalculate+=1;
    }
     fprintf(fp_dim,"%d\n",widcalculate);
     fprintf(fp_dim,"%d\n",lengthcalculate);

    return 1;

    //Write the data in txt (4 txt) and record the R G B dim value
}


/*new*/
void RGBtoYCbCr(ImgRGB **RGB, ImgYCbCr **YCbCr)
{
	for(int i = 0; i < H; i++)
    {
		for(int j = 0; j < W; j++)
        {
			YCbCr[i][j].Y = 0.299*RGB[i][j].R + 0.587*RGB[i][j].G + 0.114*RGB[i][j].B;
			YCbCr[i][j].Cb = - 0.1689*RGB[i][j].R - 0.3313*RGB[i][j].G + 0.5*RGB[i][j].B +128;
			YCbCr[i][j].Cr =  0.5*RGB[i][j].R - 0.4187*RGB[i][j].G - 0.081*RGB[i][j].B +128;
		}
	}
}


/*new*/
void basis_8to8_vector(float ****basis_vector) //four dimension for u v r c
{
    const int size = 8;
    const float factor = PI / (2 * size);

    for (int u = 0; u < size; u++) 
    {
        for (int v = 0; v < size; v++) 
        {
            for (int r = size - 1; r >= 0; r--) 
            {
                for (int c = 0; c < size; c++) 
                {
                    float u_term = cos(factor * u * (2 * r + 1));
                    float v_term = cos(factor * v * (2 * c + 1));
                    basis_vector[u][v][r][c] = u_term * v_term;
                }
            }
        }
    }
}

