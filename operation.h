#pragma once




#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


using namespace cv;

namespace align
{

	class Opram
	{
	public:
		Mat prepocess(Mat);
		Mat boader(Mat, int);
		Mat seperate(Mat, int k = 0, int kd = 3);
		Mat k_means(Mat, int);
		Mat watershed(Mat, Mat);

		Mat Hough_detect(Mat, Mat);
	};


	class Params
	{
	public:
		std::string videos_directory = "./";
		int CONNECT_KD_NOUSE = 0,CONNECT_K_NOUSE = 0;
		Params();
		
	};
}





