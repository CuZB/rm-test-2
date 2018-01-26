#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <math.h>
#include "circle-pre_img.h"
using namespace std;
using namespace cv;

Mat g_grayimg, g_dstimg;
int g_ThresholdValue = 100;


int main()
{

	//--------------读取原图----------//
	Mat img;
	img = imread("1.jpg");
	Mat copy;
	img.copyTo(copy);//备份


	//--------------色彩分割	-----------------//
	Mat tempImg = Mat::zeros(img.size(), CV_8UC1);
	Mat tempImgCopy;//一个暂存
	cvtColor(img, g_grayimg, COLOR_RGB2GRAY);
	threshold(g_grayimg, tempImg, 120, 255, THRESH_OTSU);
	imshow("threshold", tempImg);

	//------------形态学运算---------------------//
	Mat ELLimg, DILimg;
	Mat elementE = getStructuringElement(MORPH_RECT, Size(2 * 1+ 1, 2 * 1 + 1),Point(1, 1));
	morphologyEx(tempImg, ELLimg, MORPH_ELLIPSE, elementE);
	imshow("ELLimg", ELLimg);
	ELLimg.copyTo(tempImgCopy);
	//-----------找轮廓-----------------//
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(ELLimg, contours, hierarchy, CV_RETR_EXTERNAL,
		CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	Mat drawing1 = Mat::zeros(ELLimg.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(0, 0, 255);
		drawContours(drawing1, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
	}
	imshow("Contours", drawing1);

	//多边形逼近轮廓 + 获取矩形和圆形边界框  
	vector<Rect> boundRect(contours.size());
	vector<Point2f>center(contours.size());
	vector<float>radius(contours.size());
	//得到轮廓矩形框  
	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] = boundingRect(Mat(contours[i]));
		minEnclosingCircle(contours[i], center[i], radius[i]);
		Scalar color = Scalar(0, 255, 0);
	    //绘制轮廓和检测到的轮廓外接矩形  
		rectangle(drawing1, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);

	}
	imshow("rect", drawing1);

	// 画多边形轮廓 + 包围的矩形框  
	Mat drawing = Mat::zeros(tempImg.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		Rect rect = boundRect[i];
		//-------------进行一定的限制，筛选出区域-------------//
		//高宽比限制  
		float ratio = (float)rect.width / (float)rect.height;
		//轮廓面积  
		float Area = (float)rect.width * (float)rect.height;
		float dConArea = (float)contourArea(contours[i]);
		float dConLen = (float)arcLength(contours[i], 1);
		if (dConArea < 250)//ROI 区域面积限制  面积会被距离限制，说明此筛选方法不够完美
			continue;
		if (ratio > 1.5 || ratio < 0.5)//ROI 区域宽高比限制  
			continue;


		//-----------------roi无效区域检测或霍夫圆检测+圆度检测-----------------//
		Mat roiImg;
		tempImgCopy(rect).copyTo(roiImg);
		imshow("testwwwww", roiImg);
		Mat temp;
		copy(rect).copyTo(temp);
		//imshow("test2",temp);
		bool iscircle = isCircle(roiImg, temp);
		cout << "circle:" << iscircle << endl;
		if (!iscircle)
			continue;

		float C = (4 * PI*dConArea) / (dConLen*dConLen);
		if (C < 0.3)//利用圆度初步对形状进行筛选
			continue;

		Scalar color = Scalar(0, 255, 0);
		//绘制轮廓和检测到的轮廓外接矩形  
		drawContours(drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		rectangle(img, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		char tam[100];
		sprintf(tam, "(%0.0f, %0.0f)", center[i].x, center[i].y);
		putText(drawing, tam, center[i], FONT_HERSHEY_SIMPLEX, 0.8, cvScalar(255, 0, 255), 1);
		cout << "center.x = " << center[i].x << endl;
		cout << "center.y = " << center[i].y << endl;
	}
	imshow("drwaing", drawing);
	imshow("img", img);

	char key = (char)waitKey(3000000);
	if (key == 27)
		return 1;

}

