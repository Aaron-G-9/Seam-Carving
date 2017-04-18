#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

using namespace cv;
using namespace std;
//Global Variables
Mat src, srcGray, srcBlur, srcEdges;
Mat dst;
char* window_name = "Stuff";

static void CannyThreshold(int, void*){
  blur( src, srcBlur, Size(3,3) );
  Canny( srcBlur, srcEdges, 40, 40*3, 3 );
  dst = Scalar::all(0);
  src.copyTo( dst, srcEdges);
}

void MatType( Mat inputMat )
{
    int inttype = inputMat.type();

    string r, a;
    uchar depth = inttype & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (inttype >> CV_CN_SHIFT);
    switch ( depth ) {
        case CV_8U:  r = "8U";   a = "Mat.at<uchar>(y,x)"; break;  
        case CV_8S:  r = "8S";   a = "Mat.at<schar>(y,x)"; break;  
        case CV_16U: r = "16U";  a = "Mat.at<ushort>(y,x)"; break; 
        case CV_16S: r = "16S";  a = "Mat.at<short>(y,x)"; break; 
        case CV_32S: r = "32S";  a = "Mat.at<int>(y,x)"; break; 
        case CV_32F: r = "32F";  a = "Mat.at<float>(y,x)"; break; 
        case CV_64F: r = "64F";  a = "Mat.at<double>(y,x)"; break; 
        default:     r = "User"; a = "Mat.at<UKNOWN>(y,x)"; break; 
    }   
    r += "C";
    r += (chans+'0');
    cout << "Mat is of type " << r << " and should be accessed with " << a << endl;

}



int getMin(int a, int b){
  if (a < b){
    return 0;  
  }else if (a > b){
    return 1;
  }else{
    return 1;
  }
}

int getMin(int a, int b, int c){
  if (a <= b && a <= c){
    return -1;
  }
  if (b <= a && b <= c){
    return 0;
  }
  if (c <= a && c <= b){
    return 1;
  }
}

void greedySeam(Mat& image, Mat& output){
  int height = image.rows;
  int width = image.cols;
  int array[height][width];

  for (int rowNum = 0; height < 50; rowNum++){
    for (int colNum = 0; width < 50; colNum++){
      array[rowNum][colNum] = image.at<float>(rowNum, colNum);  
    }
  }

  int path[height];
  int minNum = 255;
  int minIndex = 0;

  for (int colNum = 0; colNum < width; colNum++){
    if (array[0][colNum] < minNum){
      minIndex = colNum;
      minNum = array[0][colNum];
    }
  }
  path[0] = minIndex;

  for (int rowNum = 1; rowNum < height; rowNum++){
    minIndex = path[rowNum - 1];
    if (path[rowNum - 1] == 0){
      path[rowNum] = minIndex + getMin(array[rowNum][minIndex], array[rowNum][minIndex + 1]);
    }else if (path[rowNum - 1] == width){
      path[rowNum] = minIndex + getMin(array[rowNum][minIndex - 1], array[rowNum][minIndex]);
    }else{
      path[rowNum] = minIndex + getMin(array[rowNum][minIndex - 1], array[rowNum][minIndex], array[rowNum][minIndex + 1]);
    }
  }
  

 // for (int i = 0; i < height; i++){
 //   for (int j = 0; j < width; j++){
 //     if (path[i] == j){
 //       cout << "X" << "\t";
 //     }else{
 //       cout << array[i][j] << "\t";
 //     }
 //   }
 //   cout << endl;
 // }


  for (int i = 0; i < height; i++){
    for (int j = 0; j < width; j++){
      if (j >= path[i]){
        output.at<float>(i, j) = image.at<float>(i, j + 1);
      }else{
        output.at<float>(i, j) = image.at<float>(i, j);
      }
    }
  }
}



int main(int argc, char** argv){
  src = imread( argv[1], 1 );

  if( argc != 2 || !src.data )
  {
    printf( " No image data \n " );
    return -1;
  }

  srcGray;
  cvtColor( src, srcGray, CV_BGR2GRAY );
  CannyThreshold(0,0);

  namedWindow( "Original Image", CV_WINDOW_AUTOSIZE );
  namedWindow( "Gray Image", CV_WINDOW_AUTOSIZE );
  namedWindow( "Blur Image", CV_WINDOW_AUTOSIZE );
  namedWindow( "Edge Image", CV_WINDOW_AUTOSIZE );
  namedWindow( "Color Edges", CV_WINDOW_AUTOSIZE );
  namedWindow( "blob", CV_WINDOW_AUTOSIZE);

  imshow( "Original Image", src );
  imshow( "Gray Image", srcGray );
  imshow( "Blur Image", srcBlur );
  imshow( "Edge Image", srcEdges );
  imshow( "Color Edges", dst);
  MatType(srcEdges);
   
  Mat fMat;
  srcEdges.convertTo(fMat, CV_32F); 
  MatType(fMat);
  imshow( "blob", fMat);
 
  Mat output(fMat.rows, fMat.cols, CV_32F);

  for (int i = 0; i < 100; i++){
    greedySeam(fMat, output);
    fMat = output;
  }

  namedWindow( "blob2", CV_WINDOW_AUTOSIZE);
  imshow( "blob2", output);

  waitKey(0);

  return 0;
}
