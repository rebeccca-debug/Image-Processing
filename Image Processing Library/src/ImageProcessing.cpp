#include "ImageProcessing.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

ImageProcessing::ImageProcessing(
                                 const char *_inImgName,
                                 const char *_outImgName,
                                 int * _height,
                                 int * _width,
                                 int * _bitDepth,
                                 unsigned char * _header,
                                 unsigned char * _colorTable,
                                 unsigned char * _inBuf,
                                 unsigned char * _outBuf

                                 )
{
    inImgName  = _inImgName;
    outImgName = _outImgName;
    height     = _height;
    width      = _width;
    bitDepth   = _bitDepth;
    header     = _header;
    colorTable = _colorTable;
    inBuf      = _inBuf;
    outBuf     = _outBuf;
}

/**
 * @brief Read in an image from an input file.
 * @returns void
 */
void ImageProcessing::readImage()
{

    int i;
    FILE *streamIn;
    streamIn = fopen(inImgName,"rb");
    if(streamIn ==(FILE *)0)
    {
        cout<<"Unable to open file. Maybe file does not exist"<<endl;
        exit(0);
    }
    for(i=0;i<BMP_HEADER_SIZE;i++)
    {
        header[i] =  getc(streamIn);
    }
    *width = *(int *)&header[18];           //read the width from the image header
    *height = *(int *)&header[22];
    *bitDepth = *(int *)&header[28];

    if(*bitDepth <=8)
    {

        fread(colorTable,sizeof(unsigned char),BMP_COLOR_TABLE_SIZE,streamIn);
    }

    fread(inBuf,sizeof(unsigned char),_512by512_IMG_SIZE,streamIn );

    fclose(streamIn);
}

/**
 * @brief Write an image to a 512x512 .bmp file.
 * @returns void
 */
void ImageProcessing::writeImage(){

    FILE  *fo = fopen(outImgName,"wb");
    fwrite(header,sizeof(unsigned char),BMP_HEADER_SIZE,fo);
    if(*bitDepth <=8)
    {
        fwrite(colorTable,sizeof(unsigned char),BMP_COLOR_TABLE_SIZE,fo);
    }

   fwrite(outBuf, sizeof(unsigned char),_512by512_IMG_SIZE,fo);
   fclose(fo);
}

/**
 * @brief Iterating through the image and copying it to its destination.
 * @param _srcBuf source buffer array pointer of image pixels
 * @param _destBuf destination buffer array pointer of image pixels
 * @param bufSize integer representing size of source array
 * @return void
*/
void ImageProcessing ::copyImgData(unsigned char *_srcBuf, unsigned char *_destBuf, int bufSize)
{
    for(int i =0;i<bufSize;i++)
    {
        _destBuf[i] = _srcBuf[i];
    }
}

/**
 * @brief Iterating through the image and removing color from the image.
 * @param _inImgData - char pointer to input image data
 * @param _outImgData - char pointer to output image data
 * @param imgSize - integer representing size of source array
 * @param threshold - integer representing threshold between what's considered white or black
 * @return void
*/
void ImageProcessing::binarizeImage(unsigned char *_inImgData, unsigned char *_outImgData, int imgSize, int threshold)
{
    for(int i=0;i<imgSize;i++)
    {
        _outImgData[i] =  (_inImgData[i] > threshold) ? WHITE :BLACK;
    }
}

/**
 * @brief Making the image brighter. Iterating through the image, setting a temp variable to the pixel+the brightness. 
 *        If the temporary pixel is more than the maximum color, set the maximum color equal to temp.
 * @param _inImgData - char pointer to input image data
 * @param _outImgData - char pointer to output image data
 * @param imgSize - integer representing size of source array
 * @param brightness - integer representing brightness to up the image.
 * @return void
*/
void ImageProcessing::brightnessUp(unsigned char *_inImgData, unsigned char *_outImgData, int imgSize, int brightness)
{

    for(int i =0;i<imgSize;i++)
    {
        int temp = _inputImgData[i]+ brightness;
        _outImgData[i] =  (temp > MAX_COLOR) ? MAX_COLOR :temp;
    }
}

/**
 * @brief Making the image darker. Iterating through the image, setting a temp variable to the pixel+the darkness. 
 *        If the temporary pixel is less than the minimum color, set the minimum color equal to temp.
 * @param _inImgData - char pointer to input image data
 * @param _outImgData - char pointer to output image data
 * @param imgSize - integer representing size of source array
 * @param brightness - integer representing brightness to up the image.
 * @return void
*/
void ImageProcessing::brightnessDown(unsigned char *_inputImgData, unsigned char *_outImgData, int imgSize, int darkness)
{
     for(int i =0;i<imgSize;i++)
     {
         int temp = _inputImgData[i] - darkness;
         _outImgData[i] = (temp<MIN_COLOR) ? MIN_COLOR :temp;
     }
}

/**
 * @brief Computing the histogram representation of the image given.
 * @param _imgData - char pointer to image data
 * @param imgRows - integer representing number of image rows
 * @param imgCols - integer representing number of image cols
 * @param hist - float array representing histogram
 * @return void
*/
void ImageProcessing::computeHistogram(unsigned char * _imgData, int imgRows, int imgCols, float hist[])
{
    FILE *fptr;
    fptr =fopen("image_hist.txt","w");

    int x,y,i,j;
    // create temporary histogram, sum variable
    long int ihist[255],sum;
    //fill temporary histogram with zeros
    for(i =0;i<=255;i++)
    {
        ihist[i] =0;
    }
    // sum of pixels
    sum=0;
    for(x=0;x<imgRows;y++)
    {
        for(y=0;y<imgCols;x++)
        {
            j = *(_imgData+y+x*imgCols);
            ihist[j] = ihist[j] +1;
            sum = sum+1;
        }

    }
    // create the histogram of pixel colors
    for(i=0;i<255;i++)
        hist[i] =  (float)ihist[i]/(float)sum;
    // print the histogram
    for(int i=0;i<255;i++)
    {
        fprintf(fptr,"\n%f",hist[i]);
    }
    fclose(fptr);
}

/**
 * @brief Equalize the histogram computed by computeHistogram. Equalizing the histogram entails adding more contrast to the image.
 * @param _inputImgData - char pointer to input image data
 * @param _outputImgData - char pointer to output image data
 * @param imgRows - integer representing image rows
 * @param imgCols - integer representing image columns
 * @return void
*/
void ImageProcessing::equalizeHistogram(unsigned char * _inputImgData, unsigned char * _outputImgData, int imgRows, int imgCols)
{
    int x,y,i,j;
    int histeq[256];
    float hist[256];
    float sum;

    // initial histogram
    const char initHist[] ="init_hist.txt";
    // final histogram
    const char finalHist[] = "final_hist.txt";

    // Compute the initial histogram
    computeHistogram(&_inputImgData[0],imgRows,imgCols,&hist[0],initHist);
  
    // Iterate through histogram
    for(i=0;i<=255;i++)
    {
        sum =0.0;
        for(j=0;j<=i;j++){
            //Add each pixel to sum
            sum = sum+hist[j];
        }
        histeq[i] = (int)(255*sum+0.5);

    }
  
    // Iterate through rows and columns
    for(y=0;y<imgRows;y++)
    {
        for(x=0;x<imgCols;x++)
        {
            //Equalize the array of the equalized histogram
            *(_outputImgData+x+y*imgCols) = histeq[*(_inputImgData+x+y*imgCols)];
        }
    }
    //compute final histogram for the output image
    computeHistogram(&_outputImgData[0], imgRows,imgCols,&hist[0],finalHist);
}

/**
 * @brief Get the negative version of the image.
 * @param _inImgData - char pointer to input image data
 * @param _ouImgData - char pointer to output image data
 * @param imgWidth - integer representing image rows
 * @param imgHeight - integer representing image columns
 * @return void
*/
void ImageProcessing::getImageNegative(unsigned char *_inImgData, unsigned char * _outImgData,int imgWidth,int imgHeight)
{

    for(int i=0;i<imgHeight;i++)
    {
        for(int j=0;j<imgWidth;j++){
             //Subtract each pixel from 255
             _outImgData[i*imgWidth+j ]  = 255 - _inImgData[i*imgWidth+j];
        }
    }
}

/**
 * @brief 2-D Discrete Convolution algorithm. Convolve a mask with the input image to invert the colors and make the edges pronounced.
 * @param imgRows - integer representing image rows
 * @param imgCols - integer representing image columns
 * @param myMask - Mask struct pointer
 * @param input_buf - input image data
 * @param output_buf - output image data
 * @return void
*/
void ImageProcessing::Convolve2D(int imgRows, int imgCols, struct Mask *myMask, unsigned char *input_buf, unsigned char *output_buf)
{
    // indexes
    long i,j,m,n,idx,jdx;
    //mask pixel, image pixel, convolved value
    int ms,im,val;
    unsigned char *tmp;

    // Iterating through rows and columns of the image
    for(i=0;i<imgRows;++i){
      for(j=0;j<imgCols;++j){
        val =0;
        // Iterate through rows and columns of the mask
        for(m=0;m<myMask->Rows;m++){
          for(n=0;n<myMask->Cols;n++){
            ms = (signed char)*(myMask->Data + m*myMask->Rows + n);
            idx = i-m;
            jdx = j-n;
            // if the index is more than 0 after subtracting the mask's index from the image index, the pixel is dark
            if(idx>=0 && jdx >=0){
              im = *(input_buf+idx*imgRows+jdx);
            }
            val += ms*im;
          }
          if(val>255) val=255;
          if(val<0) val=0;
          tmp = output_buf + i*imgRows + j;
          // Set the temp pointer equal to the convolved value
          *tmp = (unsigned char)val;
       }
    }
}

/**
 * @brief Detect lines in an image. Then, invert each line.
 * @param _inputImgData - char pointer to input image
 * @param _outputImgData - char pointer to output image
 * @param imgCols - integer representing image columns
 * @param imgRows - integer representing image rows
 * @param Mask - 2d integer array representing line detector mask
 * @return void
*/
void  ImageProcessing::detectLine(unsigned char *_inputImgData, unsigned char *_outputImgData, int imgCols, int imgRows, const int MASK[][3])
{
    int x,y,i,j,sum;
  
    // Iterate through image, invert the lines in the image
    for(x=1;x<=imgRows-1;x++)
    {
        for(y=1;y<=imgCols;y++)
        {
            sum = 0;
            for(i=-1;i<=1;i++)
            {
                for(j=-1;j<=1;j++)
                {
                    // Add each inverted pixel to the sum
                    sum = sum + *(_inputImgData+y+i+(long)(x+j)*imgCols)*MASK[i+1][j+1];
                }
            }
            // Make sure sum is in 8-bit value range
            if(sum>255) sum = 255;
            if(sum<0) sum = 0;
          
            *(_outputImgData+y+(long)x*imgCols) = sum;
        }
    }
}


/**
 * @brief Set the desired mask onto the image.
 * @param mskRows - integer representing mask rows
 * @param mskCols - integer representing mask columns
 * @param mskData - integer array representing mask data
 * @return void
*/
void ImageProcessing::setMask(int mskRows, int mskCols, const int mskData[])
{
  signed char * tmp;
  int requiredSize;

  myMask.Rows  = mskRows;
  myMask.Cols  = mskCols;
  requiredSize =  myMask.Rows * myMask.Cols;
  myMask.Data =  (unsigned char *)malloc(requiredSize);

  // Temp = pointer to mask data in header file
  tmp = (signed char *)myMask.Data;

  // Iterate through mask data array and set it equal to temp
  for(int i = 0;i<requiredSize;i++)
  {
      *tmp =  mskData[i];
      ++tmp;
  }

}
  
/**
 * @brief Generating Gaussian noise from an image. Getting rid of contrast in photos.
 * @param _inputImgData - char pointer to input image
 * @param imgCols - integer representing image columns
 * @param imgRows - integer representing image rows
 * @param var - integer array representing mask data
 * @param mean - integer array representing mask data
 * @return void
*/
void ImageProcessing::generateGaussNoise(unsigned char* _inputImgData, int imgCols, int imgRows, float var, float mean)
{
    int x,y;
    float theta,noise;
    for(x=0;x<imgRows;x++)
        for(y=0;y<imgCols;y++)
    {
        noise  = sqrt(-2*var*log(1.0-(float)rand()/32767.1));
        theta  =  (float)rand()* 1.9175345e-4 - 3.14159265;
        noise = noise * cos(theta);
        noise =  noise + mean;
        if(noise > 255) noise = 255;
        if(noise< 0) noise =0;
        *(_inputImgData+y+(long)x*imgCols) = (unsigned char)(noise +0.5);
    }
}

/**
 * @brief Adding salt and pepper noise to an image. Each pixel is set to either black or white depending on the vale of the probability variable.
 * @param _inputImgData - char pointer to input image
 * @param imgCols - integer representing image columns
 * @param imgRows - integer representing image rows
 * @param prob - float representing probability 
 * @return void
*/
void ImageProcessing::saltAndPepper(unsigned char *_inputImgData, int imgCols, int imgRows, float prob) {
    int x,y,data1,data2,data;
    data = (int)(prob*32768/2);
    data1 = data + 16384;
    data2 =  16384 - data;

    // Iterate through photo
    for(x=0;x<imgRows;x++){
        for(y=0;y<imgCols;y++) {
            data = rand();
            // Set pixel to black
            if(data >= 16384 && data< data1)
                *(_inputImgData+y+(long)x*imgCols) =0;
            // Set pixel to white
            if(data>=data2&& data<16384)
                *(_inputImgData+y+(long)x*imgCols) =255;
        }
    }
}

/**
 * @brief Adding a  maximum filter to the photo. The photo is sharpened.
 * @param _inputImgData - char pointer to input image
 * @param _outputImgData - char pointer to output image
 * @param imgCols - integer representing image columns
 * @param imgRows - integer representing image rows 
 * @return void
*/
void ImageProcessing::maximumFilter(unsigned char *_inputImgData, unsigned char *_outputImgData, int imgCols,int imgRows)
{
   int x,y,i,j,smax,n;
   int a[11][11];
   n=3;
   n=3;
   for(y=n/2;y<imgCols-n/2;y++)
   {
       for(x=n/2;x<imgCols-n/2;x++)
       {
           smax =0;
           for(j=-n/2;j<=n/2;j++){
               for(i=-n/2;i<=n/2;i++){
                   a[i+n/2][j+n/2] = *(_inputImgData+x+i+(long)(y+j)*imgCols);
               }
           }
           for(j=0;j<=n-1;j++)
           {
               for(i =0;i<=n-1;i++)
               {

                   if(a[i][j] > smax)
                    smax = a[i][j];
               }
           }
           *(_outputImgData+x+(long)y*imgCols) = smax;
       }

   }


}
void ImageProcessing::medianFilter(unsigned char *_inputImgData, unsigned char *_outputImgData, int imgCols, int imgRows)
{
   int x, y,i,j,z;
   int n, ar[121],a;
   n=7;

   for(y=n/2;y<imgRows-n/2;y++)
        for(x =n/2;x<imgCols-n/2;x++)
   {
       z =0;
       for(j=-n/2;j<=n/2;j++)
        for(i=-n/2;i<=n/2;i++)
       {
           ar[z] =  *(_inputImgData+x+i+(long)(y+j)*imgCols);
           z++;
       }

       for(j=1;j<=n*n-1;j++)
       {
           a = ar[j];
           i=j-1;
           while(i>=0&&ar[i] >a)
           {

               ar[i+1] =  ar[i];
               i =i-1;
           }
           ar[i+1] = a;
       }
       *(_outputImgData+x+(long)y*imgCols) =  ar[n*n/2];
   }

}

void ImageProcessing::minimumFilter(unsigned char *_inputImgData, unsigned char *_outputImgData, int imgCols, int imgRows)
{
    int x, y,i,j,smin,n,a[11][11];
    n =5;
    for(y =n/2;y<imgRows-n/2;y++)
    {
        for(x =n/2;x<imgCols-n/2;x++)
        {
            smin =255;
            for(j=-n/2;j<=n/2;j++)
                for(i=-n/2;i<=n/2;i++)
            {
                a[i+n/2][j+n/2] =  *(_inputImgData+x+i+(long)(y+j)*imgCols);

            }
            for(j=0;j<=n-1;j++)
            {
                for(i=0;i<=n-1;i++)
                {
                    if(a[i][j]<smin)smin = a[i][j];
                }
            }
            *(_outputImgData+x+(long)y*imgCols) = smin;
        }
    }
}

ImageProcessing::~ImageProcessing()
{
    //dtor
}

