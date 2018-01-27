#pragma once
#include "circle-pre_img.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <math.h>
using namespace std;
using namespace cv;
const double PI = atan(1.)*4.;


void RGB2HSV(double red, double green, double blue, double& hue, double& saturation,
	double& intensity)
{
	double r, g, b;
	double h, s, i;
	double sum;
	double minRGB, maxRGB;
	double theta;
	r = red / 255.0;
	g = green / 255.0;
	b = blue / 255.0;
	minRGB = ((r < g) ? (r) : (g));
	minRGB = (minRGB < b) ? (minRGB) : (b);
	maxRGB = ((r > g) ? (r) : (g));
	maxRGB = (maxRGB > b) ? (maxRGB) : (b);
	sum = r + g + b;
	i = sum / 3.0;
	if (i < 0.001 || maxRGB - minRGB < 0.001)
	{
		h = 0.0;
		s = 0.0;
	}
	else
	{
		s = 1.0 - 3.0*minRGB / sum;
		theta = sqrt((r - g)*(r - g) + (r - b)*(g - b));
		theta = acos((r - g + r - b)*0.5 / theta);
		if (b <= g)
			h = theta;
		else
			h = 2 * PI - theta;
		if (s <= 0.01)
			h = 0;
	}
	hue = (int)(h * 180 / PI);
	saturation = (int)(s * 100);
	intensity = (int)(i * 100);
}
bool isCircle(const Mat srcBw, Mat& mytemp)//�����Ľ���  
{//�������һ���Ҷ�ͼ��  
	Mat temp = Mat::zeros(srcBw.size(), CV_8UC1);;
	bool iscircle = false;
	//��� srcBw ��Ϣ  
	int w = srcBw.cols;
	int h = srcBw.rows;
	int count1 = 0;//�����ֵ�ȱʧ���ؼ�����  
	int count2 = 0;
	int count3 = 0;
	int count4 = 0;
	//�� srcBw ƽ���ֳ��ķ�,���з���ȱʧ�����ظ�������ռ����  
	//�ȷ�������  
	for (int i = 0; i < h / 2; i++)
	{
		for (int j = 0; j < w / 2; j++)
		{
			if (srcBw.at<uchar>(i, j) == 0)
			{
				temp.at<uchar>(i, j) = 255;
				mytemp.at<uchar>(i, j) = 255;
				count1++;
			}
		}
	}
	//����  
	for (int i = 0; i < h / 2; i++)
	{
		for (int j = w / 2 - 1; j < w; j++)
		{
			if (srcBw.at<uchar>(i, j) == 0)
			{
				temp.at<uchar>(i, j) = 255;
				mytemp.at<uchar>(i, j) = 255;
				count2++;
			}
		}
	}
	//����  
	for (int i = h / 2 - 1; i < h; i++)
	{
		for (int j = 0; j < w / 2; j++)
		{
			if (srcBw.at<uchar>(i, j) == 0)
			{
				temp.at<uchar>(i, j) = 255;
				mytemp.at<uchar>(i, j) = 255;
				count3++;
			}
		}
	}
	//����  
	for (int i = h / 2 - 1; i < h; i++)
	{
		for (int j = w / 2 - 1; j < w; j++)
		{
			if (srcBw.at<uchar>(i, j) == 0)
			{
				temp.at<uchar>(i, j) = 255;
				mytemp.at<uchar>(i, j) = 255;
				count4++;
			}
		}
	}
	float c1 = (float)count1 / (float)(w*h);//����  
	float c2 = (float)count2 / (float)(w*h);//����  
	float c3 = (float)count3 / (float)(w*h);//����  
	float c4 = (float)count4 / (float)(w*h);//����  
	cout << "result: " << c1 << "," << c2
		<< "," << c3 << "," << c4 << endl;
	//�޶�ÿ�����ʵĲ�ֵ��Χ  
	if ((c1 > 0.037&&c1 < 0.12) && (c2 > 0.037&&c2 < 0.12) && (c2 > 0.037&&c2 < 0.12) && (c2 > 0.037
		&&c2 < 0.12))
	{
		//���Ʋ�ֵ,��ֵ�Ƚ��ݴ����ڿ�֮���ֵ�����������=����&&����=���»�����=����&&����=����  
		if ((abs(c1 - c2) < 0.04&&abs(c3 - c4) < 0.04) || (abs(c1 - c3) < 0.04&&abs(c2 - c4) < 0.04))
		{
			iscircle = true;
		}
	}
	return iscircle;
}

bool isSomeColor(Mat colorRoi) {
	bool isSomeColor = false;
	int width = colorRoi.cols;   //ͼ����
	int height = colorRoi.rows;
	double B = 0.0, G = 0.0, R = 0.0, H = 0.0, S = 0.0, V = 0.0;
	Mat Mat_rgb = Mat::zeros(colorRoi.size(), CV_8UC1);
	Mat Mat_rgb_copy;//һ���ݴ浥Ԫ
	int x, y, px, py; //ѭ��
	for (y = 0; y<height; y++)
	{
		for (x = 0; x<width; x++)
		{
			// ��ȡBGRֵ
			B = colorRoi.at<Vec3b>(y, x)[0];
			G = colorRoi.at<Vec3b>(y, x)[1];
			R = colorRoi.at<Vec3b>(y, x)[2];
			RGB2HSV(R, G, B, H, S, V);
			//��ɫ��337-360 ����ɫ
			if ((H >= 337 && H <= 360 || H >= 0 && H <= 10) && S >= 12 && S <= 100 && V > 20 && V < 99)
			{
				Mat_rgb.at<uchar>(y, x) = 255;  //R  
				isSomeColor = true;
			}
		}
	}
	return isSomeColor;
}