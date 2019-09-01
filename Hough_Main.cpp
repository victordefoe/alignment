// Alignment.cpp : Defines the entry point for the console application.
//

#include "operation.h" 
#include "EllipseDetectorYaed.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <string>


using namespace std;
using namespace cv;

using namespace align;


align::Params par;
align::Opram OP;

char * get_exe_path( char * buf, int count) { int i; int rslt = readlink("/proc/self/exe", buf, count - 1); if (rslt < 0 || (rslt >= count - 1)) { return NULL; } buf[rslt] = '\0'; for (i = rslt; i >= 0; i--) { if (buf[i] == '/') { buf[i + 1] = '\0'; break; } } return buf; }


int main(int argc, char* argv[])
{
	VideoCapture capture; 
	VideoWriter writer;
	cv::Mat frame, test_tmp, test_lines, edgeline;
	

	//test();
/////////////////////////////////////
	char buffer[200];
	string WD, Video_path;
	get_exe_path(buffer, 200);
	WD = buffer;
	Video_path = "Source_Videos/jd_pl1.avi";
	cout << WD+Video_path <<endl;	
	frame = capture.open(WD+Video_path); 
//////////////////////////////////////////

	writer.open("./edge.avi", 
				CV_FOURCC('D', 'I', 'V', 'X'), 
				5.0,
				Size(720,480),
				false);

	if (!capture.isOpened()) 
	{ 
		printf("can not open ...\n"); 
		return -1; 
	} 
	if (!writer.isOpened())
	{
		printf("can not open writer ...\n");
		return -1;
	}
	
	
	int i=0;

	namedWindow("output", CV_WINDOW_AUTOSIZE); 
	cout <<"FPS: "<< capture.get(CAP_PROP_FPS) << endl;
	while (capture.read(frame))
	{ 
		
		i++;
		frame.copyTo(test_tmp);
		
		
		frame = OP.prepocess(frame); //change it into gray image
		//frame = OP.boader(frame, 1);
		//test_lines = frame.clone();
		//imshow("sobel", frame);
		//waitKey(10);
		
		test_tmp = OP.Hough_detect(frame, test_tmp);

		//////////////////lines test//////////////////
		//cvtColor(test_lines, test_lines, COLOR_RGB2GRAY);
		//adaptiveThreshold(frame, edgeline, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 3, 2);
		//threshold(frame, edgeline, 0, 255, CV_THRESH_OTSU);
		//imshow("frame", frame);
		//waitKey(100);
		Canny(test_tmp, edgeline,2,150,3,true);
		//----------find contours -------
		/*
		vector<vector<Point>> contours;
		findContours(edgeline, contours, noArray(), RETR_LIST, CHAIN_APPROX_SIMPLE);
		edgeline = Scalar::all(0);
		drawContours(edgeline, contours, -1, Scalar::all(255));
		*/


		imshow("Edge", edgeline);
		waitKey(100);
		

		vector<Vec4i> lines;
		//cout << edgeline.type()<<endl;
		HoughLinesP(edgeline, lines, 1, CV_PI/180, 60, 40,1);
		for( size_t i=0; i<lines.size();i++ )
		{
			
			Vec4i l = lines[i];
			line( test_tmp, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 2);
			/*
			float rho = lines[i][0], theta = lines[i][1];
			Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000*(-b));
			pt1.y = cvRound(y0 + 1000*(a));
			pt2.x = cvRound(x0 - 1000*(-b));
			pt2.y = cvRound(y0 - 1000*(a));
			line( test_tmp, pt1, pt2, Scalar(0,0,255), 3, CV_AA);*/
		} 
		////////////////////////////////////
		
		//test_tmp = OP.watershed(test_tmp, frame);
		//frame = OP.seperate(frame, par.CONNECT_K_NOUSE, par.CONNECT_KD_NOUSE);



		//imshow("output", frame); 
		////cout << i << endl;
		//waitKey(10); 
		
		imshow("HoughLines", test_tmp);
		waitKey(10);
		writer.write(edgeline);


	} 
	capture.release();
	writer.release();
	
	return 0;
}




