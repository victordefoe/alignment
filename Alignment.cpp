// Alignment.cpp : Defines the entry point for the console application.
//

#include "operation.h" 
#include "EllipseDetectorYaed.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;

using namespace align;


align::Params par;
align::Opram OP;
/*
string num2str(double i)
{
	stringstream ss;
	ss << i;
	return ss.str();

}

double distance(Point A, Point B)
{
	double distance;
	distance = powf((A.x - B.x), 2) + powf((A.y - B.y), 2);
	distance = sqrtf(distance);
	return distance;
}
*/
int main(int argc, char* argv[])
{
	double distance(Point, Point);
	string num2str(double);
	VideoCapture capture; 
	VideoWriter writer;
	cv::Mat frame, test_tmp;
	string video_name = argv[1];	

	//test();
	
	frame = capture.open("./Source_Videos/" + video_name); 
	writer.open("./write_" + video_name, 
				CV_FOURCC('D', 'I', 'V', 'X'), 
				5.0,
				Size(720,480),
				true);

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
		imshow("sobel", frame);
		waitKey(10);
		
		//test_tmp = OP.Hough_detect(frame, test_tmp);
		//test_tmp = OP.watershed(test_tmp, frame);
		//frame = OP.seperate(frame, par.CONNECT_K_NOUSE, par.CONNECT_KD_NOUSE);

		////////////////////////// this is for eclipse detect only /////////
		//set parameters (Sect. 4.2)
		Size sz = frame.size();
		int	iThLength = 10;
		float	fThObb = 3.0f;
		float	fThPos = 1.0f;
		float	fTaoCenters = 0.1f;
		int 	iNs = 16;
		float	fMaxCenterDistance = sqrt(float(sz.width*sz.width + sz.height*sz.height)) * fTaoCenters;

		float	fThScoreScore = 0.99f;

		// Other constant parameters settings. 

		// Gaussian filter parameters, in pre-processing
		Size	szPreProcessingGaussKernelSize = Size(5, 5);
		double	dPreProcessingGaussSigma = 1.0;

		float	fDistanceToEllipseContour = 0.3f;	// (Sect. 3.3.1 - Validation)
		float	fMinReliability = 0.6f;	// Const parameters to discard bad ellipses

		int top_k = 5; //select to show results at most

		// Initialize Detector with selected parameters
		CEllipseDetectorYaed* yaed = new CEllipseDetectorYaed();
		yaed->SetParameters(szPreProcessingGaussKernelSize,
			dPreProcessingGaussSigma,
			fThPos,
			fMaxCenterDistance,
			iThLength,
			fThObb,
			fDistanceToEllipseContour,
			fThScoreScore,
			fMinReliability,
			iNs
			);

		Mat1b gray = frame.clone(), temp;
		
		/////////////Creating ROI region ////
		/*
		Mat1b roi = Mat::zeros(gray.cols, gray.rows, CV_8UC1);
		//roi.setTo(Scalar(0,0,0));
		Rect roi_rect = Rect(100,60,300, 200);
		temp = gray(roi_rect);
		temp.copyTo(roi);
		imshow("ROI", roi);
		waitKey(10);
		cout << roi.size() << endl;*/
		/////////////ROI finish
		// Detect
		vector<Ellipse> ellsYaed;
		yaed->Detect(gray, ellsYaed);
		//cout << ellsYaed[0]._xc<<endl;
		vector<double> times = yaed->GetTimes();
		cout << "--------------------------------" << endl;
		cout << "Execution Time: " << endl;
		cout << "Edge Detection: \t" << times[0] << endl;
		cout << "Pre processing: \t" << times[1] << endl;
		cout << "Grouping:       \t" << times[2] << endl;
		cout << "Estimation:     \t" << times[3] << endl;
		cout << "Validation:     \t" << times[4] << endl;
		cout << "Clustering:     \t" << times[5] << endl;
		cout << "--------------------------------" << endl;
		cout << "Total:	         \t" << yaed->GetExecTime() << endl;
		cout << "--------------------------------" << endl;
		
		Mat3b result = test_tmp.clone();
		float max=0, min=10000;
		float error;
		string error_str;
		int idx_max=0, idx_min=0;
		if(ellsYaed.size() >= 2)
		{
			top_k = ellsYaed.size();
			vector<Ellipse> ellsout(ellsYaed.begin(),ellsYaed.begin()+top_k);
			if(top_k>=2)
			{
				for (int j=0; j<top_k; j++)
				{
					float sum = ellsout[j]._a;
					if(sum>max) 
					{
						max = sum;
						idx_max = j;
					}
					if(sum<min)
					{
						min = sum;
						idx_min = j;
					}
				}
			}
			//yaed->DrawDetectedEllipses(result, ellshow);
			Point centre_out = Point(ellsout[idx_max]._xc, ellsout[idx_max]._yc);
			circle(result, centre_out, 2, Scalar(0,255,0));
			Point centre_in = Point(ellsout[idx_min]._xc, ellsout[idx_min]._yc);
			circle(result, centre_in, 2, Scalar(0,0,255));
			error = distance(centre_out, centre_in);
			error_str = num2str(error);
			ellsout[idx_max].Draw(result, Scalar(0,255,0),2);
			ellsout[idx_min].Draw(result,Scalar(0,0,255),2);
			if(error > 100)
				cv::putText(result, "Too much shift!", Point(30,30), 
						cv::FONT_HERSHEY_TRIPLEX, 0.8, CV_RGB(34,193,65),2,CV_AA);
			else
				cv::putText(result, "Center Shift:"+error_str, Point(30,30), 
					cv::FONT_HERSHEY_TRIPLEX, 0.8, CV_RGB(245,12,65),2,CV_AA);

				
			cout<< "长轴:"<<cvRound(ellsout[idx_max]._a)<<"  短轴:"<<cvRound(ellsout[idx_max]._b)<<endl;
			float rate_out = ellsout[idx_max]._b / (ellsout[idx_max]._a + 0.0001);
			float rate_in = ellsout[idx_min]._b / (ellsout[idx_min]._a + 0.0001);
			float rads_shift = ellsout[idx_min]._rad - ellsout[idx_max]._rad;
			string rate_error_str = num2str(rate_out - rate_in);
			string rad_error_str = num2str(rads_shift);
			cv::putText(result, "Rate Shift:"+ rate_error_str, Point(30,60), 
				cv::FONT_HERSHEY_TRIPLEX, 0.8, CV_RGB(245,12,65),2,CV_AA);
			cv::putText(result, "Rads Shift:"+ rad_error_str, Point(30,90), 
				cv::FONT_HERSHEY_TRIPLEX, 0.8, CV_RGB(56,12,195),2,CV_AA);
			
		}
		else
		{
			vector<Ellipse> ellsout(ellsYaed.begin(),ellsYaed.end());
			cv::putText(result, "Loss_detect", Point(30,90), 
				cv::FONT_HERSHEY_TRIPLEX, 0.8, CV_RGB(56,12,195),2,CV_AA);

			yaed->DrawDetectedEllipses(result, ellsout);
		}


		///////////////////////// detect end //////////////////////////////
		


		imshow("output", result); 
		////cout << i << endl;
		waitKey(10); 

		writer.write(result);

	} 
	capture.release();
	writer.release();
	
	return 0;
}




