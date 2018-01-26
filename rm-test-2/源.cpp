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

	//--------------��ȡԭͼ----------//
	Mat img;
	img = imread("1.jpg");
	Mat copy;
	img.copyTo(copy);//����


	//--------------ɫ�ʷָ�	-----------------//
	Mat tempImg = Mat::zeros(img.size(), CV_8UC1);
	Mat tempImgCopy;//һ���ݴ�
	cvtColor(img, g_grayimg, COLOR_RGB2GRAY);
	threshold(g_grayimg, tempImg, 120, 255, THRESH_OTSU);
	imshow("threshold", tempImg);

	//------------��̬ѧ����---------------------//
	Mat ELLimg, DILimg;
	Mat elementE = getStructuringElement(MORPH_RECT, Size(2 * 1+ 1, 2 * 1 + 1),Point(1, 1));
	morphologyEx(tempImg, ELLimg, MORPH_ELLIPSE, elementE);
	imshow("ELLimg", ELLimg);
	ELLimg.copyTo(tempImgCopy);
	//-----------������-----------------//
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

	//����αƽ����� + ��ȡ���κ�Բ�α߽��  
	vector<Rect> boundRect(contours.size());
	vector<Point2f>center(contours.size());
	vector<float>radius(contours.size());
	//�õ��������ο�  
	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] = boundingRect(Mat(contours[i]));
		minEnclosingCircle(contours[i], center[i], radius[i]);
		Scalar color = Scalar(0, 255, 0);
	    //���������ͼ�⵽��������Ӿ���  
		rectangle(drawing1, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);

	}
	imshow("rect", drawing1);

	// ����������� + ��Χ�ľ��ο�  
	Mat drawing = Mat::zeros(tempImg.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		Rect rect = boundRect[i];
		//-------------����һ�������ƣ�ɸѡ������-------------//
		//�߿������  
		float ratio = (float)rect.width / (float)rect.height;
		//�������  
		float Area = (float)rect.width * (float)rect.height;
		float dConArea = (float)contourArea(contours[i]);
		float dConLen = (float)arcLength(contours[i], 1);
		if (dConArea < 250)//ROI �����������  ����ᱻ�������ƣ�˵����ɸѡ������������
			continue;
		if (ratio > 1.5 || ratio < 0.5)//ROI �����߱�����  
			continue;


		//-----------------roi��Ч����������Բ���+Բ�ȼ��-----------------//
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
		if (C < 0.3)//����Բ�ȳ�������״����ɸѡ
			continue;

		Scalar color = Scalar(0, 255, 0);
		//���������ͼ�⵽��������Ӿ���  
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

