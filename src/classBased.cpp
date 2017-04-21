#include <iostream>
#include <limits>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/imgcodecs.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

using namespace cv;
using namespace std;


int getMin(int a, int b, int c){
  if (a <= b && a <= c){
    return -1;
  }
  if (b <= a && b <= c){
    return 0;
  }
  if (c <= a && c <= b){
    int randNum = (rand() % 3) + 1;
    if (randNum == 1){
      return -1;
    }else if (randNum == 2){
      return 0;
    }else{
      return 1;
    }
  }
}

float returnMin(float a, float b, float c){
  if (a <= b && a <= c){
    return a;
  }
  if (b <= a && b <= c){
    return b;
  }
    //cout << a << "\t" << b << "\t" << c << endl;
    return c;
  
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
    void getEnergy(){
      int scale = 1;
      int delta = 0;
      int ddepth = CV_16S;
      Mat temp1;
      cvtColor(src, temp1, CV_BGR2GRAY);
      /// Generate grad_x and grad_y
      Mat grad_x, grad_y;
      Mat abs_grad_x, abs_grad_y;

      /// Gradient X
      //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
      Sobel( temp1, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
      convertScaleAbs( grad_x, abs_grad_x );

      /// Gradient Y
      //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
      Sobel( temp1, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
      convertScaleAbs( grad_y, abs_grad_y );

      /// Total Gradient (approximate)
      addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, edges );
      namedWindow("EDGES");
      imshow( "EDGES", edges );
    //  Mat temp1, temp2, temp3;
    //  cvtColor(src, temp1, CV_BGR2GRAY);
    //  blur(temp1, temp2, Size(3,3));
    //  Canny(temp2, edges, 50, 50*3, 3 );
    //  temp1.release();
    //  temp2.release();
    //  temp3.release();
    }

    void dynamicSeam(){
      Mat temp;
      edges.copyTo(temp);

      int height = temp.rows;
      int width = temp.cols;
      int array[height][width];
      
      seam.resize(height + 1);

      float veryLarge = numeric_limits<float>::max();
      //Creates 2D array of seams using dynamic programming methods
      for (int rowNum = 0; rowNum < height; rowNum++){
        for (int colNum = 0; colNum < width; colNum++){
          if (rowNum == 0){
            array[0][colNum] = temp.at<float>(0, colNum);
          }else{
            if (colNum == 0){
              array[rowNum][colNum] = temp.at<float>(rowNum, colNum) + returnMin(veryLarge, temp.at<float>(rowNum -1, colNum), temp.at<float>(rowNum -1, colNum + 1)); 
            }else if (colNum == width -1){
              array[rowNum][colNum] = temp.at<float>(rowNum, colNum) + returnMin(temp.at<float>(rowNum -1, colNum -1), temp.at<float>(rowNum -1, colNum), veryLarge); 
            }else{
              array[rowNum][colNum] = temp.at<float>(rowNum, colNum) + 
                returnMin(temp.at<float>(rowNum -1, colNum -1), temp.at<float>(rowNum -1, colNum), temp.at<float>(rowNum -1, colNum + 1)); 
            }
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
    void otherFindSeam(){
      int H = edges.rows, W = edges.cols;

        int dp[H][W];
        for(int c = 0; c < W; c++){
            dp[0][c] = (int)edges.at<uchar>(0,c);
        }

        for(int r = 1; r < H; r++){
            for(int c = 0; c < W; c++){
                if (c == 0)
                    dp[r][c] = min(dp[r-1][c+1], dp[r-1][c]);
                else if (c == W-1)
                    dp[r][c] = min(dp[r-1][c-1], dp[r-1][c]);
                else
                    dp[r][c] = min({dp[r-1][c-1], dp[r-1][c], dp[r-1][c+1]});
                dp[r][c] += (int)edges.at<uchar>(r,c);
            }
        }

        int min_value = 2147483647; //infinity
        int min_index = -1;
        for(int c = 0; c < W; c++)
            if (dp[H-1][c] < min_value) {
                min_value = dp[H - 1][c];
                min_index = c;
            }

        int path[H];
        Point pos(H-1,min_index);
        path[pos.x] = pos.y;

        while (pos.x != 0){
            int value = dp[pos.x][pos.y] - (int)edges.at<uchar>(pos.x,pos.y);
            int r = pos.x, c = pos.y;
            if (c == 0){
                if (value == dp[r-1][c+1])
                    pos = Point(r-1,c+1);
                else
                    pos = Point(r-1,c);
            }
            else if (c == W-1){
                if (value == dp[r-1][c-1])
                    pos = Point(r-1,c-1);
                else
                    pos = Point(r-1,c);
            }
            else{
                if (value == dp[r-1][c-1])
                    pos = Point(r-1,c-1);
                else if (value == dp[r-1][c+1])
                    pos = Point(r-1,c+1);
                else
                    pos = Point(r-1,c);
            }
            path[pos.x] = pos.y;
            copy(&path[0], &path[H], back_inserter(seam)); 
        }
    
    }
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
    void showSeams(){
      for (int i = 0; i < src.rows; i++){
        for (int j = 0; j < src.cols; j++){
          if (seam.at(i) == j){
            src.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
          }
        }
      }
      namedWindow("SEAM");
      imshow("SEAM", src);
    }
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
    int getEdgesCol(){
      return edges.cols;
    }
    int getEdgesRows(){
      return edges.rows;
    }
    float pixelValue(int a, int b){
      return edges.at<float>(a, b);
    }
    
};


Mat src;
int main(int argc, char** argv){
  src = imread( argv[1], 1 );

  if( argc != 2 || !src.data ){
    printf( " No image data \n " );
    return -1;
  }
  imshow("ORIGINAL", src);
  Image picture(src);
  
  for (int i = 0; i < 100; i++){
    picture.getEnergy();
    picture.dynamicSeam();
    //picture.showSeam();
    picture.removeSeam();
  }

  waitKey(0);
  return 0;
}
