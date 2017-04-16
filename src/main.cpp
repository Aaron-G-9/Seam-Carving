#include <iostream>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;
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

 imshow( "Original Image", src );
 imshow( "Gray Image", srcGray );
 imshow( "Blur Image", srcBlur );
 imshow( "Edge Image", srcEdges );
 imshow( "Color Edges", dst);
 waitKey(0);

 return 0;
}
