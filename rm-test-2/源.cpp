#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <math.h>
using namespace std;
using namespace cv;

const double PI = atan(1.)*4.;

Mat g_grayimg, g_dstimg;
int g_ThresholdValue=100;
void fillHole(Mat srcBw, Mat& dstBw)
{
	Size m_Size = srcBw.size();
	Mat Temp = Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcBw.type());//延展图像  
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));
	cv::floodFill(Temp, Point(0, 0), Scalar(255));//填充区域  
	Mat cutImg;//裁剪延展的图像  
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);
	dstBw = srcBw | (~cutImg);
}

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
	minRGB = ((r<g) ? (r) : (g));
	minRGB = (minRGB<b) ? (minRGB) : (b);
	maxRGB = ((r>g) ? (r) : (g));
	maxRGB = (maxRGB>b) ? (maxRGB) : (b);
	sum = r + g + b;
	i = sum / 3.0;
	if (i<0.001 || maxRGB - minRGB<0.001)
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
bool isCircle(const Mat srcBw, Mat& mytemp)//（待改进）  
{//输入的是一个灰度图像  
	Mat temp = Mat::zeros(srcBw.size(), CV_8UC1);;
	bool iscircle = false;
	//获得 srcBw 信息  
	int w = srcBw.cols;
	int h = srcBw.rows;
	int count1 = 0;//各部分的缺失像素计数器  
	int count2 = 0;
	int count3 = 0;
	int count4 = 0;
	//将 srcBw 平均分成四份,进行访问缺失的像素个数、所占比重  
	//先访问左上  
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
	//右上  
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
	//左下  
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
	//右下  
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
	float c1 = (float)count1 / (float)(w*h);//左上  
	float c2 = (float)count2 / (float)(w*h);//右上  
	float c3 = (float)count3 / (float)(w*h);//左下  
	float c4 = (float)count4 / (float)(w*h);//右下  
	cout << "result: " << c1 << "," << c2
		<< "," << c3 << "," << c4 << endl;
	//限定每个比率的差值范围  
	if ((c1>0.037&&c1<0.12) && (c2>0.037&&c2<0.12) && (c2>0.037&&c2<0.12) && (c2>0.037
		&&c2<0.12))
	{
		//限制差值,差值比较容错，相邻块之间差值相近，如左上=右上&&左下=右下或左上=左下&&右上=右下  
		if ((abs(c1 - c2)<0.04&&abs(c3 - c4)<0.04) || (abs(c1 - c3)<0.04&&abs(c2 - c4)<0.04))
		{
			iscircle = true;
		}
	}
	return iscircle;
}
void on_Threshold(int, void*) {
	threshold(g_grayimg, g_dstimg, g_ThresholdValue, 255, THRESH_OTSU);
	imshow("test", g_dstimg);
}

int main()
{
	bool isCircle(const Mat srcBw, Mat& mytemp);
	void fillHole(Mat srcBw, Mat& dstBw);

	//--------------读取原图----------//
	Mat img;
	img=imread("2.jpg");
	Mat copy;
	img.copyTo(copy);//备份

	//---------转换成hsv 再直方图均衡化用函数彩色分割-------------//
	/*Mat hsvImg;
	vector<Mat> hsvSplit;
	cvtColor(img, hsvImg, COLOR_BGR2HSV);
	split(hsvImg , hsvSplit);
	equalizeHist(hsvSplit[2], hsvSplit[2]);
	merge(hsvSplit, hsvImg);
	imshow("hsv", hsvImg);
*/
    /*Mat tempImg;
    inRange(hsvImg, Scalar(0, 0, 80), Scalar(180, 30,220), tempImg);*/


	//--------------色彩分割	-----------------//
	//待改进，号码牌颜色这样调太没有应用性
	//int width = img.cols;   //图像宽度
	//int height = img.rows;   //图像高度					 
	//double B = 0.0, G = 0.0, R = 0.0, H = 0.0, S = 0.0, V = 0.0;
	Mat tempImg = Mat::zeros(img.size(), CV_8UC1);
	Mat tempImgCopy;//一个暂存
	//
	//int x, y, px, py; //循环
	//for (y = 0; y<height; y++)
	//{
	//	for (x = 0; x<width; x++)
	//	{
	//		// 获取BGR值
	//		B = img.at<Vec3b>(y, x)[0];
	//		G = img.at<Vec3b>(y, x)[1];
	//		R = img.at<Vec3b>(y, x)[2];
	//		RGB2HSV(R, G, B, H, S, V);
	//		//灰白色
	//		if (H >= 0 && H <= 360 && S >= 0 && S <= 20 && V>40 && V<50)
	//		{
	//			tempImg.at<uchar>(y, x) = 255;  //R     
	//		}
	//	}
	//}
	cvtColor(img, g_grayimg, COLOR_RGB2GRAY);
	//threshold(tempImg, tempImg, 5, 255, THRESH_BINARY);
	//imshow("tempImg", tempImg);
	///medianBlur(tempImg, tempImg, 3);//去掉椒盐噪声
	threshold(g_grayimg, tempImg, 120, 255, THRESH_OTSU);
	imshow("threshold",tempImg);
	/*namedWindow("test", WINDOW_AUTOSIZE);
	createTrackbar("参考值", "test", &g_ThresholdValue, 255,on_Threshold);
	on_Threshold(0, 0);*/

	//------------形态学运算---------------------//
	//Mat ELLimg, DILimg;
	//Mat elementE = getStructuringElement(MORPH_RECT, Size(2 * 1 + 1, 2 * 1 + 1),
	//	Point(1, 1));
	//Mat elementD = getStructuringElement(MORPH_RECT, Size(2 * 3 + 1, 2 * 3 + 1),
	//	Point(3, 3));
	//morphologyEx(tempImg, ELLimg, MORPH_ELLIPSE, elementE);
	//morphologyEx(ELLimg, DILimg, MORPH_DILATE, elementD);
	//imshow("DILimg", DILimg);
	//Mat bw;
	//fillHole(DILimg, bw);//填补空洞
	//imshow("bw", bw);
	//bw.copyTo(tempImgCopy);
	tempImg.copyTo(tempImgCopy);
	//-----------找轮廓-----------------//
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(tempImg, contours, hierarchy, CV_RETR_EXTERNAL,
		CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	Mat drawing1 = Mat::zeros(tempImg.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(0, 0, 255);
		//drawContours(drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		drawContours(drawing1, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
	}
	imshow("Contours",drawing1);
	
	//多边形逼近轮廓 + 获取矩形和圆形边界框  
	//vector<vector<Point> > contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f>center(contours.size());
	vector<float>radius(contours.size());
	//得到轮廓矩形框  
	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] =boundingRect(Mat(contours[i]));
		//boundRect[i] = boundingRect(Mat(contours_poly[i]));
		minEnclosingCircle(contours[i], center[i], radius[i]);
	}

	
	// 画多边形轮廓 + 包围的矩形框  
	Mat drawing = Mat::zeros(tempImg.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		Rect rect = boundRect[i];
		//-------------进行一定的限制，筛选出区域-------------//
		//高宽比限制  
		float ratio = (float)rect.width / (float)rect.height;
		//轮廓面积  
		float Area = (float)rect.width * (float)rect.height;
		float dConArea = (float)contourArea(contours[i]);
		float dConLen = (float)arcLength(contours[i], 1);
		if (dConArea < 500)//ROI 区域面积限制  
			continue;
		if (ratio>1.5 || ratio<0.5)//ROI 区域宽高比限制  
			continue;


		//-----------------roi无效区域检测或霍夫圆检测+圆度检测-----------------//
		Mat roiImg;
		tempImgCopy(rect).copyTo(roiImg);
		imshow("testwwwww",roiImg);
		//Mat temp;
		//copy(rect).copyTo(temp);
		////imshow("test2",temp);
		//bool iscircle = isCircle(roiImg, temp);
		//cout << "circle:" << iscircle << endl;
		//if (!iscircle)
		//	continue;
		//
		//float C = (4 * PI*dConArea) / (dConLen*dConLen);
		//if (C < 0.3)//利用圆度初步对形状进行筛选
		//	continue;
		////copy(rect).copyTo(roiImg);


		//Scalar color = Scalar(0, 255, 0);
		////绘制轮廓和检测到的轮廓外接矩形  
		//drawContours(drawing, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		//rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		//rectangle(img, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		//char tam[100];
		//sprintf(tam, "(%0.0f, %0.0f)", center[i].x,center[i].y);
		//putText(drawing, tam, center[i], FONT_HERSHEY_SIMPLEX, 0.8, cvScalar(255, 0, 255), 1);
		//cout << "center.x = " << center[i].x << endl;
		//cout << "center.y = " << center[i].y << endl;
	}
	imshow("drwaing", drawing);
	imshow("img", img);

		char key = (char)waitKey(3000000);
		if (key == 27)
			return 1;
	
}

