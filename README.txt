SIMPLE HAND TRACKING in OpenCV

Tested on  Microsoft Visual Studio (Windows32) and Cmake(Ubuntu 14.04)

HSV segmentation for skin color 
  trackbar provided for altering min,max values of HSV taking care
  of illumination changes


Algorithm (sequential from top to bottom as implimented in code)

HSV segmentation
Opening and Closing
Canny Edge 
Contour Detection
Finding Largest contour (assumed as Hand)
Calculate centroid of Hand Contour (moments)
Track its position through frames 


 
