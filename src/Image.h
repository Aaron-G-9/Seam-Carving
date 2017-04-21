#ifndef IMAGE_H
#define IMAGE_H
#include <iostream>
#include <limits>



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
