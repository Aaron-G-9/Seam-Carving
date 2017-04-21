#ifndef IMAGE_H
#define IMAGE_H
#include <iostream>
#include <limits>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>


using namespace cv;
using namespace std;

//Utility method to return which of the three integers
//was the minimum integer
int getMin(int a, int b, int c){
  if (a <= b && a <= c){
    return -1;
  }else if (b <= a && b <= c){
    return 0;
  }else if (c <= a && c <= b){
    return 1;
  }else{
    cout << "Some error occured" << endl;
  }
}


class Image {
  private:
    Mat src;
    Mat edges;
    vector<int> seam;
  public:
    Image(Mat arg){
      src = arg;
    }
    void showGray(){
      Mat temp;
      cvtColor(src, temp, CV_BGR2GRAY);
      namedWindow("GrayScale Image");
      imshow("GrayScale Image", temp);
      temp.release();
    }
    void showEdges(){
      namedWindow("Edge Image");
      imshow("Edge Image", edges);
    }

    void getSobelEnergy(){
      int scale = 1;
      int delta = 0;
      int ddepth = CV_16S;
      Mat temp1, grad_x, grad_y, abs_grad_x, abs_grad_y;
      cvtColor(src, temp1, CV_BGR2GRAY);

      /// Gradient X
      Sobel( temp1, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
      convertScaleAbs( grad_x, abs_grad_x );

      /// Gradient Y
      Sobel( temp1, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
      convertScaleAbs( grad_y, abs_grad_y );

      /// Total Gradient (approximate)
      addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edges );
    }
    void getCannyEnergy(int precision){
      Mat temp1, temp2, temp3;
      cvtColor(src, temp1, CV_BGR2GRAY);
      blur(temp1, temp2, Size(3,3));
      Canny(temp2, edges, precision, precision*3, 3 );
      temp1.release();
      temp2.release();
      temp3.release();
    }

    /*A dynamic programming approach to seam carving as described in:
      https://en.wikipedia.org/wiki/Seam_carving#Algorithm
      This is one of the more reliable methods for carving (among my implementations)
    */
    void dynamicSeam(){
      Mat temp;
      edges.copyTo(temp);

      int height = temp.rows;
      int width = temp.cols;
      int array[height][width];
      
      seam.resize(height + 1);

      int veryLarge = numeric_limits<int>::max();
      //Creates 2D array of seams using dynamic programming methods
      for (int rowNum = 0; rowNum < height; rowNum++){
        for (int colNum = 0; colNum < width; colNum++){
          if (colNum == 0){
            array[rowNum][colNum] = temp.at<float>(rowNum, colNum) + min({veryLarge, array[rowNum -1][colNum], array[rowNum -1][colNum + 1]}); 
          }else if (colNum == width -1){
            array[rowNum][colNum] = temp.at<float>(rowNum, colNum) + min({array[rowNum -1][colNum -1], array[rowNum -1][colNum], veryLarge}); 
          }else{
            array[rowNum][colNum] = temp.at<float>(rowNum, colNum) + 
              min({array[rowNum -1][colNum -1], array[rowNum -1][colNum], array[rowNum -1][colNum + 1]}); 
          }
        }
      }

      float minValue = numeric_limits<float>::max();
      int minIndex = 0;
      //Weird stuff might happen with .at(height) look here for segfaults
      for (int colNum = 0; colNum < width; colNum++){
        if (array[height][colNum] < minValue){
          minValue = array[height][colNum];
          minIndex = colNum;
        }
      }

      seam.at(height) = minIndex;

      //minIndex is still the colNum
      
      for (int rowNum = height - 1; rowNum >= 0; rowNum--){
        seam.at(rowNum) = minIndex + getMin(array[rowNum][minIndex - 1], array[rowNum][minIndex], array[rowNum][minIndex + 1]);
        minIndex = seam.at(rowNum);
      }

      temp.release();

    }

    /*This function was initially going to take 10 random seams using a greedy algorithm
      appraoch, but as I got carried away, it basically takes a brute force approach to 
      seam carving. This isn't a very efficient or well-made algorithm, hence the bad
      variable names ;)
    */
    void randomFindSeam(){
      struct save{
        int index;
        int energy;
      } seams[edges.rows];

      Mat temp2;
      edges.copyTo(temp2);

      int height = temp2.rows;
      int width = temp2.cols;
      int array[height][width];
      
      seam.resize(height);

      for (int rowNum = 0; rowNum < height; rowNum++){
        for (int colNum = 0; colNum < width; colNum++){
          array[rowNum][colNum] = temp2.at<float>(rowNum, colNum);  
        }
      }

      for (int bad = 0; bad < height; bad++){
        vector<float> bleam(height);
        int minNum = 255;
        int minIndex = 0;

        bleam.at(0) = bad;

        for (int rowNum = 1; rowNum < height; rowNum++){
          minIndex = bleam.at(rowNum - 1);
          if (bleam.at(rowNum - 1) == 0){
            bleam.at(rowNum) = minIndex + getMin(256, array[rowNum][minIndex], array[rowNum][minIndex + 1]);
          }else if (bleam.at(rowNum - 1) == width){
            bleam.at(rowNum) = minIndex + getMin(array[rowNum][minIndex - 1], array[rowNum][minIndex], 256);
          }else{
            bleam.at(rowNum) = minIndex + getMin(array[rowNum][minIndex - 1], array[rowNum][minIndex], array[rowNum][minIndex + 1]);
          }
        }
        float sum;
        for (auto x : bleam){
          sum += x;
        }
        seams[bad].index = bleam.at(0);
        seams[bad].energy = sum;
      }
      
      float min = FLT_MAX;
      float minInd = 0;
      for (int x = 0; x < height; x++){
        if (seams[x].energy < min){
          seams[x].energy = min;
          minInd = x;
        }
      }

      int minNum = 255;
      seam.at(0) = static_cast<int>(minInd);
      int minIndex = 0;

      for (int rowNum = 1; rowNum < height; rowNum++){
        minIndex = seam.at(rowNum - 1);
        if (seam.at(rowNum - 1) == 0){
          seam.at(rowNum) = minIndex + getMin(256, array[rowNum][minIndex], array[rowNum][minIndex + 1]);
        }else if (seam.at(rowNum - 1) == width){
          seam.at(rowNum) = minIndex + getMin(array[rowNum][minIndex - 1], array[rowNum][minIndex], 256);
        }else{
          seam.at(rowNum) = minIndex + getMin(array[rowNum][minIndex - 1], array[rowNum][minIndex], array[rowNum][minIndex + 1]);
        }
      }
      temp2.release();

    }

    /*This was my initial try - it uses a greedy algorithm to find seams, starting from the pixel with
      the lowest energy in the top row and working on a seam after that. It tends to cut from left to 
      right too much for my taste. But it's probably the quickest way to find a seam
    */
    void findSeam(){
      Mat temp2;
      edges.copyTo(temp2);

      int height = temp2.rows;
      int width = temp2.cols;
      int array[height][width];
      
      seam.resize(height);

      for (int rowNum = 0; rowNum < height; rowNum++){
        for (int colNum = 0; colNum < width; colNum++){
          array[rowNum][colNum] = temp2.at<float>(rowNum, colNum);  
        }
      }

      int minNum = 255;
      int minIndex = 0;

      for (int colNum = 0; colNum < width; colNum++){
        if (array[0][colNum] < minNum){
          minIndex = colNum;
          minNum = array[0][colNum];
        }
      }
      seam.at(0) = minIndex;

      for (int rowNum = 1; rowNum < height; rowNum++){
        minIndex = seam.at(rowNum - 1);
        if (seam.at(rowNum - 1) == 0){
          seam.at(rowNum) = minIndex + getMin(256, array[rowNum][minIndex], array[rowNum][minIndex + 1]);
        }else if (seam.at(rowNum - 1) == width){
          seam.at(rowNum) = minIndex + getMin(array[rowNum][minIndex - 1], array[rowNum][minIndex], 256);
        }else{
          seam.at(rowNum) = minIndex + getMin(array[rowNum][minIndex - 1], array[rowNum][minIndex], array[rowNum][minIndex + 1]);
        }
      }

      temp2.release();
    }
    /*Originally a utility method for me, I figure I'll leave it in in case
      anyone ever wants to see what a seam looks like. Also, many other people
      in their implementations leave a method like this. It just looks kinda cool
    */
    void showSeam(){
      Mat temp;
      src.copyTo(temp);
      for (int i = 0; i < temp.rows; i++){
        for (int j = 0; j < temp.cols; j++){
          if (seam.at(i) == j){
            temp.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
          }
        }
      }
      namedWindow("SEAM");
      imshow("SEAM", temp);
      temp.release();
    }

    /*This method actually removes the seam from the picture. Although one would think this was a simple method,
      I've had drastically different outcomes depending on how I implement this. This is an issue I'll keep looking
      at
    */
    void removeSeam(){
      //Move all the pixels to the right of the seam, one pixel to the left
      
      for (int rowNum = 0; rowNum < src.rows; rowNum++){
        for (int colNum = 0; colNum < src.cols -1; colNum++){
          if (seam.at(rowNum) >= colNum){
            src.at<Vec3b>(rowNum, colNum) = src.at<Vec3b>(rowNum, colNum +1);
          }else{
            src.at<Vec3b>(rowNum, colNum) = src.at<Vec3b>(rowNum, colNum);
          }
        }
      }

      //Resize the image to remove the last column
      src = src(Rect(0, 0, src.cols-1, src.rows)); 

      namedWindow("REMOVED");
      imshow("REMOVED", src);
    }

    float getEdgeValue(int a, int b){
      return edges.at<float>(a, b);
    }
};
#endif
