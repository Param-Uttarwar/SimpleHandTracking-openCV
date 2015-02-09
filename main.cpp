#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


using namespace cv;
using namespace std;

Mat src, src_gray;
Mat dst, detected_edges, drawingtemp;
vector<vector<Point> > contours, largestcon;

vector<Vec4i> hierarchy;

int edgeThresh = 1;
int lowThreshold = 50;
int const max_lowThreshold = 100;
int ratio = 3;
int stateL = 0, prev1 = 0;
int kernel_size = 3;

int main(int argc, char** argv)
{
	VideoCapture cap(0); //capture the video from webcam

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}



	
	//define useful variables

	Mat imgTmp;
	cap.read(imgTmp);   //storing frame info in a temporary matrix
	Mat lines = Mat::zeros(imgTmp.size(), CV_8UC3);  //matrix for  storing tracking lines (cyan color)


	Mat imgOriginal, imgOriginal1, frame;
	Mat imgGray;
	int Hmin = 143, Smin = 70, Vmin = 0;  //
	int Hmax = 179, Smax = 180, Vmax = 164;
	int iLastX = 0, iLastY = 0;

	namedWindow("Control", CV_WINDOW_AUTOSIZE);
	createTrackbar("Hue_LowThreshold", "Control", &Hmin, 360); 
	createTrackbar("Hue_HighThreshold", "Control", &Hmax, 360);
	createTrackbar("Sat_LowThreshold", "Control", &Smin, 360);
	createTrackbar("Sat_HighThreshold", "Control", &Smax, 360);
	createTrackbar("Val_LowThreshold", "Control", &Vmin, 360);
	createTrackbar("Val_HighThreshold", "Control", &Vmax, 360);


	namedWindow("Blurred",CV_WINDOW_AUTOSIZE);  //displaying blurred segmentated and canny edge images
	namedWindow("Canny",CV_WINDOW_AUTOSIZE);
	namedWindow("EdgeMap", CV_WINDOW_AUTOSIZE);

	while (true)
	{


		bool bSuccess = cap.read(frame);// read a new frame from video
		imgOriginal1 = frame;


		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		cvtColor(imgOriginal1, imgOriginal, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		inRange(imgOriginal, cv::Scalar(Hmin, Smin, Vmin), cv::Scalar(Hmax, Smax, Vmax), imgOriginal); // HSV space thresholding

 		 //opening and closing operations for eliminating small holes
		dilate(imgOriginal, imgOriginal, getStructuringElement(MORPH_ELLIPSE, Size(7, 7))); 
		erode(imgOriginal, imgOriginal, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
		erode(imgOriginal, imgOriginal, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));
		dilate(imgOriginal, detected_edges, getStructuringElement(MORPH_ELLIPSE, Size(7, 7)));

		//preprocessing for canny (noise reduction by blur)	
		blur(detected_edges, detected_edges, Size(5, 5));
		imshow("Blurred", detected_edges);
		/// Canny detector
		Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
		dilate(detected_edges, detected_edges, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		imshow("Canny", detected_edges);

		
		dst.create(detected_edges.size(), detected_edges.type());
		dst = Scalar::all(0); //empty image 



		imgOriginal.copyTo(dst, detected_edges); //edges copy to dst matrix
		dilate(dst, dst, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));

		//finding contour (largest contour is taken as hand)
		findContours(detected_edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		vector<Rect> boundRect(contours.size());
		int area[400];
		Mat drawing = Mat::zeros(detected_edges.size(), CV_8UC3);
		int largest = 0;
		for (int i = 0; i < contours.size(); i++)
		{


			approxPolyDP(contours[i], contours[i], 3, false);
			Scalar color = Scalar(5 * i, 50 + 3 * i, 250);
			//drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
			boundRect[i] = boundingRect(Mat(contours[i]));
			area[i] = boundRect[i].width*boundRect[i].height;
			if (i > 1)
			{
				if (area[i] >= area[largest])
					largest = i;
			}



		

		}
		drawContours(drawing, contours, largest, Scalar(0, 0, 255), 2, 8, hierarchy, 0, Point());

		cvtColor(drawing, drawingtemp, COLOR_BGR2GRAY);

		//Find moments for centroid of contour as hand position
		Moments oMoments = moments(drawingtemp);
		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		int posX = dM10 / dArea;
		int posY = dM01 / dArea;

		line(lines, Point(posX, posY), Point(iLastX, iLastY), Scalar(255, 255, 0), 2);

		iLastX = posX;
		iLastY = posY;
	

		cout << area[largest] << "  "; //area of largest contour in display
		add(drawing, frame, drawing);
		add(drawing, lines, drawing);
		imshow("EdgeMap", drawing);
		if (waitKey(10) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	return 0;
}