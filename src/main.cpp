#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "Image.h"

using namespace cv;
using namespace std;
//Global Variables

Mat original;
int main(int argc, char** argv){
  original = imread( argv[1], 1 );

  if( argc != 2 || !original.data ){
    printf( " No image data \n " );
    return -1;
  }

  imshow("ORIGINAL", original);
  Image picture(original);

  int newWidth, newHeight;
  cout << "Enter new width for image" << endl;
  cin >> newWidth;

  while (newWidth > original.cols){
    cout << "Sorry, please enter a width smaller than " << original.cols << endl;
    cin >> newWidth;
  }

  cout << "Enter new height for image" << endl;
  cin >> newHeight;

  while (newHeight > original.rows){
    cout << "Sorry, please enter a height smaller than " << original.rows << endl;
    cin >> newHeight;
  }

  newWidth = original.cols - newWidth;
  newHeight = original.rows - newHeight;

  for (int i = 0; i < newWidth; i++){
    picture.getSobelEnergy();
    picture.dynamicVertSeam();
    //picture.showSeam();
    picture.removeVertSeam();
  }
  for (int i = 0; i < newHeight; i++){
    picture.getSobelEnergy();
    picture.dynamicHorizSeam();
    //picture.showSeam();
    picture.removeHorizSeam();
  }

  waitKey(0);

  return 0;
}
