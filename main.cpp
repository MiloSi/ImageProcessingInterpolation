
/**************************************************************
2017.04.29

Created by Milo Si (Called Me C)
I Studied (Exaclty, Learned) Scale Transfrom and Interpolation

I named all function and object(variable) 'PasscalCase' and 'camelCase'

ex)
void DoYourSelf(int a, int b)
{
	char * studyHard;
	int helpMe;
}


**************************************************************/
#pragma warning (disable:4996)
#pragma warning (disable:4819)

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


/**************************************************************
	Func	Mat NearestNeighbourInterpolation(const Mat src, const double scaleWidth, const double scaleHeight)
	
	Param	const Mat src				= Original Image
			cosnt double scaleWidht		= The Variable You Want to Scaling Image Width
			cosnt double scaleHeight	= The Variable You Want to Scaling Image Heihgt

	Return  Mat							= Processed Image ( Usign Nearest Neighbour Interpolation Process)

	Processed Image Width   = origin Image Width  * scaleWidth
	Processed Image Height  = origin Image Height  * scaleWidth

**************************************************************/

Mat NearestNeighbourInterpolation(const Mat src, const double scaleWidth, const double scaleHeight) {

	Mat dst((int) (src.rows * scaleHeight +0.5), (int) (src.cols * scaleHeight +0.5), CV_8UC3);

	for (int y = 0; y < dst.rows; y++)
	{
		Vec3b* dstPtr = dst.ptr<Vec3b>(y);
		int nearestY = (int) floor((y / scaleHeight) +0.5);
		const Vec3b* srcPtr = src.ptr<Vec3b>(nearestY);

		for (int x = 0; x < dst.cols; x++)
		{

			int nearestX =(int) floor((x / scaleWidth) + 0.5);

			dstPtr[x][0] = srcPtr[nearestX][0];
			dstPtr[x][1] = srcPtr[nearestX][1];
			dstPtr[x][2] = srcPtr[nearestX][2];
		}
	}

	return dst;
}


/**************************************************************
	Func	Mat BilinearInterpolation(const Mat src, const double scaleWidth, const double scaleHeight)

	Param	const Mat src				= Original Image
			cosnt double scaleWidht		= The Variable You Want to Scaling Image Width
			cosnt double scaleHeight	= The Variable You Want to Scaling Image Heihgt

	Return  Mat							= Processed Image ( Usign Bilinear Interpolation Process)

	Processed Image Width   = origin Image Width  * scaleWidth
	Processed Image Height  = origin Image Height  * scaleWidth

	r1 =  (1 - s1) Q11 + s1 Q12
	r2 =  (1 - s1) Q21 + s1 Q22
	P  =  (1 - s2) R1  + s2 R2


**************************************************************/

Mat BilinearInterpolation(const Mat src, const double scaleWidth, const double scaleHeight) {

	int x1, x2;
	int y1, y2;
	double rx, ry;
	double s1, s2;

	Vec3b r1, r2;

	Mat dst((int)(src.rows * scaleHeight +0.5), (int)(src.cols * scaleHeight +0.5), CV_8UC3);
	double srcX = 1 / scaleWidth;
	double srcY = 1 / scaleHeight;

	for (int y = 0; y < dst.rows; y++)
	{
		for (int x = 0; x < dst.cols; x++)
		{
			rx = srcX * x;
			ry = srcY * y;
			x1 = (int)floor(rx);
			y1 = (int)floor(ry);
			x2 = x1 + 1; if (x2 == src.cols) x2 = x1;
			y2 = y1 + 1; if (y2 == src.rows) y2 = y1;

			s1 = rx - x1;
			s2 = ry - y1;

			for (int i = 0; i < 3; i++) {
				r1[i] = saturate_cast<uchar>((1 - s1)* src.at<Vec3b>(y1, x1)[i] + s1 * src.at<Vec3b>(y1, x2)[i] + 0.5);
				r2[i] = saturate_cast<uchar>((1 - s1) *src.at<Vec3b>(y2, x1)[i] + s1 * src.at<Vec3b>(y2, x2)[i] + 0.5);
				dst.at<Vec3b>(y, x)[i] = saturate_cast<uchar>((1 - s2) * r1[i] + s2 * r2[i] + 0.5);
			}
		}
	}

	return dst;
}


/**************************************************************
	TODO:

	Func	BicubicInterpolation(const Mat src, const double scaleWidth, const double scaleHeight)

	Param

	Return  Mat							= Processed Image ( Usign Bilinear Interpolation Process)

**************************************************************/
Mat BicubicInterpolation(const Mat src, const double scaleWidth, const double scaleHeight) {
	Mat dst;


	return dst;
}
/**************************************************************
	Func	void Show(String named, const Mat& dst)
	
	Show Image that modifed original Image

	Param	String named		= Window Name and Write File Name
			const  Mat& dst		= Processed Image



**************************************************************/


void Show(String named, const Mat& dst)
{
	namedWindow(named);
	imshow(named, dst);
	waitKey(0);
	destroyWindow(named);

	imwrite(named + ".bmp", dst);

	return;
}
/*
	Default in My Project

	argc  == 4
	argv  project_name.exe lenna.bmp 1.5 1.5
*/

int main(int argc, char** argv) {

	String openImagePath = "lenna.bmp";						// Image Path That You Want To Open.
	double scaleWidth = 1.5;								// scale Width and Height 
	double scaleHeight = 1.5;								
	
	if (argc > 2)
	{
		openImagePath = argv[1];
		scaleWidth = atof(argv[2]);
		scaleHeight = atof(argv[3]);
	}

	Mat src = imread(openImagePath, IMREAD_COLOR);			// Read Image

	if (src.empty())
	{
		cerr << "NO FILE PATH" << endl;
		return 0;
	}

	Show("Nearest Neightbor Interpolation",		NearestNeighbourInterpolation(src, scaleWidth, scaleHeight));
	Show("Bilinear Interpolation",				BilinearInterpolation(src, scaleWidth, scaleHeight));

	return 0;
}
