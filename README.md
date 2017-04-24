# Seam-Carving
A content-aware image resizing program, based on the work done by Shai Avidan and Ariel Shamir, implemented in C++

### About
This project was made for an intro C++ class, therefore it is by no means a perfect implementation of the seam carving algorithm. I intend to keep working on the project, as soon as I have some free time. As it currently stands, there are three different implementations of the seam carving algorithm for vertical seams. There is the algorithm that finds the lowest energy pixel in the first row, and goes down from there, there is a brute force algorithm, and a dynamic programming algorithm. The default is the dynamic programming method. I learned several things through the course of this project, about OpenCV, image manipulation, and design and implementation of algorithms. For example, I created this Image class with seperate methods for removing horizontal and vertical seams, when an easier approach would be to flip the image as necessary. In the future, this will be implemented.

###Screenshots
An original, unaltered image: 
![Unaltered sea](Seam-Carving/pictures/sea-thai.jpg) 
An example seam  
![sea with seam](https://github.com/Aaron-G-9/Seam-Carving/pictures/SEAM_sea.png) 
That image now carved  
![carved sea](https://github.com/Aaron-G-9/Seam-Carving/pictures/sea-carved.png) 


The implementations of the algorithm here don't take faces into account when carving, making resizing of people very problematic. The idea of seam carving is to find the 'least important' part of an image. However, there's not really a least important part of a face - if you mess with any of it too much it'll look off. Similarly, if an object takes up the majority of the picture, the algorithm will have a hard time dealing with it. I will demonstrate this with a picture from the original paper of some plant. Theirs became distorted when carved, mine becomes chopped off :/  


![Original Plant](https://github.com/Aaron-G-9/Seam-Carving/pictures/bad-plant.png) 
![Carved Plant](https://github.com/Aaron-G-9/Seam-Carving/pictures/plant-carved.png) 
