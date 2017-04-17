#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

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

static void findPaths(Mat img){
  int rows = img.rows;
  int cols = img.cols; 
  int min = 255;
  int minIndex = 1;
  
  for (int i = 1; i < cols -1; i++){
    if (img.at<float>(Point(i, 0)) < min){
      min = img.at<float>(Point(i, 0));
      minIndex = i;
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
  cout << fMat.at<float>(Point(141, 143)) << endl;
  //findPaths(fmat);

  waitKey(0);

  return 0;
}
