#include <stdio.h>
#include <stdlib.h>

int rotateImage(FILE *fIn, FILE *fOut);

int main()
{
   // Create input and output files
   FILE *fIn= fopen("cameraman.bmp","rb");
   FILE *fOut =fopen("cameraman_rotated3.bmp","wb");
   
   rotateImage(fIn, fOut);
   return 0;
}

/**
 * @brief Rotate image. User inputs their desired direction. Options: left, right, 180.
 * @param fIn - Input image data
 * @param fOut - Output image data
 * @return int
*/
int rotateImage(FILE *fIn, FILE *fOut) {
   // Selected rotation direction
   int selected;

   unsigned char imgHeader[54];
   unsigned char colorTable[1024];

   if(fIn == NULL)
   {
       printf("Unable to open file\n");
   }

   // Copy the header from the input file to imgHeader
   for(int i =0;i<54;i++)
   {
       imgHeader[i] = getc(fIn);
   }
   fwrite(imgHeader,sizeof(unsigned char),54,fOut);
   int height = *(int*)&imgHeader[22];
   int width  =  *(int *)&imgHeader[18];
   int bitDepth = *(int *)&imgHeader[28];

   // Read from the colorTable in the input, write to the colorTable in the output file
   if(bitDepth <=8)
   {
       fread(colorTable,sizeof(unsigned char),1024,fIn);
       fwrite(colorTable,sizeof(unsigned char),1024,fOut);

   }

   int imgSize = height * width;

   unsigned char buffer[width][height];
   unsigned char out_buffer[width][height];

   // Read from the buffer in the input file
   fread(buffer,sizeof(unsigned char),imgSize,fIn);

   // User selects the direction they want the image to rotate in
   printf("Enter rotation direction : \n");
   printf("1  : Rotate right \n");
   printf("2  : Rotate left \n");
   printf("3  : Rotate 180  \n");

   scanf("%d",&selected);

   switch(selected)
   {
   // Rotate right
   case 1:
    for(int i =0;i<width;i++)
    {
        for(int j =0;j<height;j++)
        {
            out_buffer[j][height-1-i] = buffer[i][j];
        }
    }
       break;

   // Rotate left
   case 2:
    for(int i =0;i<width;i++)
    {
        for(int j =0;j<height;j++)
        {
            out_buffer[j][i]=buffer[i][j];
        }
    }
    break;
         
   // Rotate 180
   case 3:
     for(int i=0;i<width;i++)
     {
         for(int j=0;j<height;j++)
         {
             out_buffer[width-i][j] =  buffer[i][j];
         }
     }
     break;
   
   default:
    break;
   }

   //Write image to the output file buffer
   fwrite(out_buffer,sizeof(unsigned char),imgSize,fOut);
   printf("Success !\n");

   fclose(fIn);
   fclose(fOut);
   return 0;
}
