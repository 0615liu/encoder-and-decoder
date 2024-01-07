#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uint8_t;

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
    unsigned int palette; // 0x0036
} BMP_header;

typedef struct RGB
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
} ImgRGB;


/* structure for BMP file */
typedef struct _BMP_file 
{
    char _filename[1024];
    BMP_header header;
    int H;
    int W;
    unsigned char **B; // channel for Blue
    unsigned char **G; // channel for Green
    unsigned char **R;
} BMP_file;


void usage(FILE *fp)
{
    fprintf(fp, "encoder 0 KimberlyNCat.bmp R.txt G.txt B.txt dim.txt\n");
    return;
}

void output_bmp(ImgRGB **RGB, FILE* outfile,  BMP_header bmpheader, int skip);
void function0(char* argv2, char* argv3, char* argv4, char* argv5, char* argv6);
ImgRGB** malloc_2D(int row, int col)
{
    ImgRGB **Array, *Data;
    Array = (ImgRGB**)malloc(row*sizeof(ImgRGB *));
    Data = (ImgRGB*)malloc(row*col*sizeof(ImgRGB));

    for (int i = 0; i<row; i++, Data += col)     //Point each element of the array to a different row of the one-dim array
    {
        Array[i] = Data;
    }
    return Array;
}


int main(int argc, char *argv[]) 
{    
    if(strcmp(argv[1], "0") == 0)
    {
        function0(argv[2], argv[3], argv[4], argv[5], argv[6]);
    }
    
    printf("finish");

    return 0; // 0: exit normally 1: exit with error
}


void function0(char* argv2, char* argv3, char* argv4, char* argv5, char* argv6)
{
	FILE *fp_out;
	FILE *fp_R;
	FILE *fp_G;
	FILE *fp_B;
	FILE *fp_dim;
	int W, H, M, N; 
    fp_R = fopen(argv3, "r");
    fp_G = fopen(argv4, "r");
    fp_B = fopen(argv5, "r");
    fp_dim = fopen(argv6, "r");
	//read "INPUT.bmp" to get the information of bmp header
	    BMP_header bmpheader;
        bmpheader.identifier[0] = 'B';
        bmpheader.identifier[1] = 'M';
        bmpheader.filesize = 3296094; //3296040 //36578358
        bmpheader.reserved = 0;
        bmpheader.reserved2 = 0;
        bmpheader.bitmap_dataoffset = 54;
        bmpheader.bitmap_headersize = 40;
        bmpheader.width = 908;   //908  //3024
        bmpheader.height = 1210;  //1210 //4032
        bmpheader.planes = 1;
        bmpheader.bits_perpixel = 24;
        bmpheader.compression = 0;
        bmpheader.bitmap_datasize = 3296040;
        bmpheader.hresolution = 2835;
        bmpheader.vresolution = 2835;
        bmpheader.usedcolors = 0;
        bmpheader.importantcolors = 0;
      // bmpheader.palette = 1997860675;
       
       fscanf(fp_dim, "%d\n", &W);
       fscanf(fp_dim, "%d\n", &H);
    ImgRGB **Data_RGB = malloc_2D(H, W);
    
	for (int i = 0; i < H; i++)
    {
        for (int j = 0; j < W; j++)
        {
                
            fscanf(fp_R, "%d", &Data_RGB[i][j].R);
            fscanf(fp_G, "%d", &Data_RGB[i][j].G);
            fscanf(fp_B, "%d", &Data_RGB[i][j].B);
        }
    }

    fp_out = fopen(argv2, "wb");

    output_bmp(Data_RGB,fp_out,bmpheader,0);

}

 /*output header and data*/
void output_bmp(ImgRGB **RGB, FILE* outfile,  BMP_header bmpheader, int skip)
{
    char skip_buf[3] = { 0, 0, 0 };
    int x, y;
    fwrite(&bmpheader.identifier, sizeof(short), 1, outfile);
    fwrite(&bmpheader.filesize, sizeof(int), 1, outfile);
    fwrite(&bmpheader.reserved, sizeof(short), 1, outfile);
    fwrite(&bmpheader.reserved2, sizeof(short), 1, outfile);
    fwrite(&bmpheader.bitmap_dataoffset, sizeof(int), 1, outfile);
    fwrite(&bmpheader.bitmap_headersize, sizeof(int), 1, outfile);
    fwrite(&bmpheader.width, sizeof(int), 1, outfile);
    fwrite(&bmpheader.height, sizeof(int), 1, outfile);
    fwrite(&bmpheader.planes, sizeof(short), 1, outfile);
    fwrite(&bmpheader.bits_perpixel, sizeof(short), 1, outfile);
    fwrite(&bmpheader.compression, sizeof(int), 1, outfile);
    fwrite(&bmpheader.bitmap_datasize, sizeof(int), 1, outfile);
    fwrite(&bmpheader.hresolution, sizeof(int), 1, outfile);
    fwrite(&bmpheader.vresolution, sizeof(int), 1, outfile);
    fwrite(&bmpheader.usedcolors, sizeof(int), 1, outfile);
    fwrite(&bmpheader.importantcolors, sizeof(int), 1, outfile);

    for (x = 0; x<bmpheader.height; x++){
        for (y = 0; y<bmpheader.width; y++){
            
            fwrite(&RGB[x][y].B, sizeof(char), 1, outfile);
            fwrite(&RGB[x][y].G, sizeof(char), 1, outfile);
            fwrite(&RGB[x][y].R, sizeof(char), 1, outfile);
            
        }
        if (skip != 0) { fwrite(skip_buf, skip, 1, outfile); }
       
    }
}
