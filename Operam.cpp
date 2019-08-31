
//这个文件对用到的各种处理操作进行定义

#include "operation.h" 
#include <algorithm>
//#include <set>

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <sstream>
#include <string>
#include <cmath>

# define SOBEL 1

using namespace cv;
using namespace std;
using namespace align;

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





//void k_means(Mat & );//input a binary array 

Mat Opram::prepocess(Mat in_)
{
	Mat kmeans_img;
	Size k_s = {5,5};
	//fastNlMeansDenoisingColored(in_image, in_image);//该算法过于耗费时间
//	kmeans_img = this->k_means(in_, 1);
	cvtColor(in_, in_, COLOR_RGB2GRAY);
	//fastNlMeansDenoising(in_image, in_image);

	GaussianBlur(in_, in_, k_s, 4); 
//	GaussianBlur(kmeans_img, in_, k_s, 4);
	
	return in_;
}

Mat Opram::boader(Mat in_, int type)
{
	///after boader detect ,output binary image
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y, dst;
	
	
	if (type == SOBEL)
	{
		//求x方向梯度
		Sobel(in_, grad_x, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
		convertScaleAbs(grad_x, abs_grad_x);
		//imshow("x方向soble", abs_grad_x);

		//求y方向梯度
		Sobel(in_, grad_y, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
		convertScaleAbs(grad_y, abs_grad_y);
		//imshow("y向soble", abs_grad_y);

		//合并梯度
		addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);
	}

	return dst;
}




Mat Opram::k_means(Mat in_, int show=0)
{
	///聚类（输入3通道的彩色图像，对像素值聚类）
	///返回聚类以后的labels
	///show不为0时候，生成kmeans以后的灰度图并显示

	bool REIMG_opt = true; //如果为true，返回kmeans以后的灰度图，否则返回label_reshape（原图对应像素类别标签）

	Mat points(in_.cols*in_.rows, 1, CV_32FC3);
	Mat labels, center;
	Mat label_reshape(in_.rows, in_.cols, CV_32FC1);

	int K = 5;
	Mat IMG_gray;

	uchar* p;
	int k = 0;
	for (int i = 0; i < in_.rows; i++)
	{
		p = in_.ptr<uchar>(i);
		for (int j = 0; j< in_.cols; j++)
		{
			points.at<Vec3f>(k, 0)[0] = float(p[j*3]);
			points.at<Vec3f>(k, 0)[1] = float(p[j * 3 + 1]);
			points.at<Vec3f>(k, 0)[2] = float(p[j * 3 + 2]);
			k++;
		}
	}
	
	kmeans(
		points,
		K,
		labels,
		cv::TermCriteria(
		cv::TermCriteria::EPS | cv::TermCriteria::COUNT,
		10,
		1.0
		),
		3,
		cv::KMEANS_PP_CENTERS,
		center);
	
	for (int i = 0; i < in_.rows; i++)
	{
		for (int j = 0; j < in_.cols; j++)
		{
			label_reshape.at<float>(i, j) = labels.at<float>(i*in_.cols + j, 0);
		}
	}

	if (show != 0)
	{
		Mat img(in_.rows, in_.cols, CV_8UC1);
		
		float step = 255 / (K - 1);
		for (int i = 0; i < img.rows; i++)
		{
			p = img.ptr<uchar>(i);
			for (int j = 0; j< img.cols; j++)
			{
				int tt = int(label_reshape.at<int>(i,j));
				p[j] = 255 - tt*step;
				/*if (tt == 3)
					p[j] = 255;
				else
					p[j] = 0;*/
				//cout << tt;

			}
		}
		//namedWindow("K-Means分割效果", 0);
		imshow("K-Means分割效果", img);
		waitKey(10);
		IMG_gray = img;
	
	}

	if (REIMG_opt)
		return IMG_gray;
	else
		return labels;

}





struct top_k
{
	int top_area_k;
	int top_id_k;
};

bool comp(const top_k& a, const top_k& b)
{
	return a.top_area_k < b.top_area_k;
}


Mat Opram::seperate(Mat in_, int k, int kd)
{
	/*	k: 选择区域最大的前k个连通域 if is 0, means no_use;
		kd: 排除小于kd的连通域;
	*/
	
	Mat kmeans_label;
	Mat labels, stats, cent, img_color;
	//adaptiveThreshold(in_, in_, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 3, 2);

	if (k > 0 && kd > 0)
	{
		int i, nccomps = connectedComponentsWithStats(in_, labels, stats, cent);
		//GaussianBlur(in_, in_, Size(5,5), 4);
		//adaptiveThreshold(in_, in_, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 3, 2);
		//cout << nccomps << endl;

		vector<cv::Vec3b> colors(nccomps + 1);
		colors[0] = cv::Vec3b(0, 0, 0); // background pixels remain black.


		top_k init_topk = { 0, 0 };
		vector<top_k> comp_k;
		for (int idx = 0; idx < k; idx++)
			comp_k.push_back(init_topk);

		for (i = 1; i <= nccomps; i++) {


			//cout << stats.at<int>(i - 1, cv::CC_STAT_AREA) << endl;


			if (k <= 0)
			{
				colors[i] = cv::Vec3b(rand() % 256, rand() % 256, rand() % 256);
				if (stats.at<int>(i - 1, cv::CC_STAT_AREA) < kd)
					colors[i] = cv::Vec3b(0, 0, 0); // small regions are painted with black too.
			}
			else
			{
				colors[i] = cv::Vec3b(0, 0, 0);
				sort(comp_k.begin(), comp_k.end(), comp);
				if (stats.at<int>(i - 1, cv::CC_STAT_AREA) >= comp_k[0].top_area_k)
					comp_k[0] = { stats.at<int>(i - 1, cv::CC_STAT_AREA), i };
			}



		}
		if (k > 0)
		{
			for (int idx = 0; idx < comp_k.size(); idx++)
				colors[comp_k[idx].top_id_k] = cv::Vec3b(rand() % 256, rand() % 256, rand() % 256);
		}

		img_color = cv::Mat::zeros(in_.size(), CV_8UC3);
		for (int y = 0; y < img_color.rows; y++)
			for (int x = 0; x < img_color.cols; x++)
			{
				int label = labels.at<int>(y, x);
				CV_Assert(0 <= label && label <= nccomps);
				img_color.at<cv::Vec3b>(y, x) = colors[label];
			}

		cvtColor(img_color, in_, COLOR_RGB2GRAY);
		
	}

	adaptiveThreshold(in_, in_, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY_INV, 3, 2);
	
	

	
	return in_;
}


Vec3b RandomColor(int value) //生成随机颜色函数</span>  
{
	value = value % 255;  //生成0~255的随机数  
	RNG rng;
	int aa = rng.uniform(0, value);
	int bb = rng.uniform(0, value);
	int cc = rng.uniform(0, value);
	return Vec3b(aa, bb, cc);
}


Mat Opram::watershed(Mat in_, Mat aftercounter) ///注意，这里我定义的函数名字和opencv的函数名一样
/////有bug， 还未完成！！
{
	///输入in_: 8bit 3通道图
	//输入aftercounter： 一般是经过边缘检测后的二值图
	Mat marks(in_.size(), CV_32S); //32-bit float, single channel //Opencv分水岭第二个矩阵参数  
	marks = Scalar::all(0); 

	////提取轮廓矩阵
	vector<Vec4i> hierarchy;
	vector<vector<Point>> contours;
	findContours(aftercounter, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

	Mat imageContours = Mat::zeros(in_.size(), CV_8UC1);  //轮廓     

	int index = 0;
	int compCount = 0;
	for (; index >= 0; index = hierarchy[index][0], compCount++)
	{
		//对marks进行标记，对不同区域的轮廓进行编号，相当于设置注水点，有多少轮廓，就有多少注水点  
		drawContours(marks, contours, index, Scalar::all(compCount + 1), 1, 8, hierarchy);
		drawContours(imageContours, contours, index, Scalar(255), 1, 8, hierarchy);
	}




	cv::watershed(in_, marks);
	Mat PerspectiveImage = Mat::zeros(in_.size(), CV_8UC3);
	for (int i = 0; i<marks.rows; i++)
	{
		for (int j = 0; j<marks.cols; j++)
		{
			int index = marks.at<int>(i, j);
			if (marks.at<int>(i, j) == -1)
			{
				PerspectiveImage.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			}
			else
			{
				PerspectiveImage.at<Vec3b>(i, j) = RandomColor(index);
			}
		}
	}
	imshow("After ColorFill", PerspectiveImage);
	waitKey(10);
	return PerspectiveImage;

}


Mat Opram::Hough_detect(Mat in_, Mat src_image)
{
	///输入8bit 单通道
	vector<cv::Vec3f> circles; 
	vector<cv::Vec3f> big_circles;
	Mat1b src_gray;
	imshow("Before Hough Circles", in_);
	waitKey(10);
	cvtColor(src_image, src_gray,CV_RGB2GRAY);
	cv::HoughCircles(in_, circles, cv::HOUGH_GRADIENT, 2, in_.cols/10 ,100,100,20,100);
	cv::HoughCircles(in_, big_circles, cv::HOUGH_GRADIENT, 2, in_.cols/12 ,100,100,120,150);
	

	for (size_t i = 0; i < circles.size(); ++i) {
		cv::circle(src_image,
			cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1])),
			cvRound(circles[i][2]),
			CV_RGB(255,0,0),
			2,
			cv::LINE_AA);
		cv::circle(src_image,	
			cv::Point(cvRound(circles[i][0]), cvRound(circles[i][1])),
			2,
			CV_RGB(255,0,0),
			-1);
	}

	for (size_t i = 0; i < big_circles.size(); ++i) {
		cv::circle(src_image,
			cv::Point(cvRound(big_circles[i][0]), cvRound(big_circles[i][1])),
			cvRound(big_circles[i][2]),
			CV_RGB(0,255,0),
			2,
			cv::LINE_AA);
		cv::circle(src_image,	
			cv::Point(cvRound(big_circles[i][0]), cvRound(big_circles[i][1])),
			2,
			CV_RGB(0,255,0),
			-1);
	}
	
	if(circles.size() != 0 && big_circles.size() != 0){
		string error_str, pre_str="Error : ";
		double error;
		Point opt_p_s, opt_p_b;
		opt_p_s = Point(circles[0][0], circles[0][1]);
		opt_p_b = Point(big_circles[0][0], big_circles[0][1]);
		error = distance(opt_p_s, opt_p_b);
	
		if (error > 300 && circles.size() > 1)
		{
			opt_p_s = Point(circles[1][0], circles[1][1]);
			error = distance(opt_p_s, opt_p_b);
		
		}
		error_str = pre_str + num2str(error);
	
		cv::putText(src_image, error_str, Point(30,30), 
				cv::FONT_HERSHEY_TRIPLEX, 0.8, CV_RGB(245,12,65),2,CV_AA);
	}
	imshow("Hough Circles", src_image);
	waitKey(10);

	return src_image;

}













