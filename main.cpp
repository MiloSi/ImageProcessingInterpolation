
/**************************************************************
2017.04.29

Created by Milo Si (Call me C)
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
		if (nearestY == src.rows) nearestY = src.rows - 1;
		const Vec3b* srcPtr = src.ptr<Vec3b>(nearestY);

		for (int x = 0; x < dst.cols; x++)
		{

			int nearestX =(int) floor((x / scaleWidth) + 0.5);
			if (nearestX == src.cols) nearestX = src.cols - 1;

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

#if 0
			//boudary handling using Padding
			x2 = x1 + 1; if (x2 == src.cols) x2 = x1;
			y2 = y1 + 1; if (y2 == src.rows) y2 = y1;
#else

			//boudary handling Using Mirroring
			x2 = x1 + 1; if (x2 == src.cols) x2 = x1 - 1;
			y2 = y1 + 1; if (y2 == src.rows) y2 = y1 - 1;
#endif

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

	Param	const Mat src				= Original Image
			cosnt double scaleWidht		= The Variable You Want to Scaling Image Width
			cosnt double scaleHeight	= The Variable You Want to Scaling Image Heihgt

	Return  Mat							= Processed Image ( Usign Bilinear Interpolation Process)




	F(x) = { (a + 2) * abs( pow(x ,3)) - (a + 3) * abs( pow(x ,3)) + 1	// abs(x) <= 1
	a * abs( pow(x ,3)) -	5 a * abs( pow(x ,3)) + 8 a abs(x) - 4 a	// 1 < abs(x) <= 2
	0	}																// 2 < abs(x)

	We used a = - 0.5

	F(x) = { 1.5 * abs( pow(x ,3)) - 2.5 * abs( pow(x ,3)) + 1			// abs(x) <= 1
	-0.5 * abs( pow(x ,3))  + 2.5 * abs( pow(x ,3)) - 4 abs(x) + 2		// 1 < abs(x) <= 2
	0	}																// 2 < abs(x)



**************************************************************/

Mat BicubicInterpolation(const Mat src, const double scaleWidth, const double scaleHeight) {
	Mat dst((int)(src.rows * scaleHeight + 0.5), (int)(src.cols * scaleHeight + 0.5), CV_8UC3);

	int px[4];		//   x1, x2 are  abs(x) <= 1 , x0, x3 are 1 < abx(x) <=2 
	int py[4];		//	 y1, y2 are  abs(y) <= 1 , y0, y3 are 1 < abx(y) <=2 


	Vec3b r[4];

	double rx, ry;
	double sx, sy;

	double ux[4];
	double uy[4];

	double srcX = 1 / scaleWidth;
	double srcY = 1 / scaleHeight;

	for (int y = 0; y < dst.rows; y++)
	{
		for (int x = 0; x < dst.cols; x++)
		{
			rx = x * srcX;
			ry = y * srcY;

#if 0
			//Boundary Handling using Padding

			px[1] = cvFloor(rx);
			px[0] = px[1] - 1;	if (px[0] < 0) px[0] = 0;
			px[2] = px[1] + 1;	if (px[2] == src.cols) px[2] = src.cols - 1;
			px[3] = px[1] + 2;	if (px[3] >= src.cols) px[3] = src.cols - 1;

			py[1] = cvFloor(ry);
			py[0] = py[1] - 1;	if (py[0] < 0) py[0] = 0;
			py[2] = py[1] + 1;	if (py[2] == src.rows) py[2] = src.rows - 1;
			py[3] = py[1] + 2;	if (py[3] >= src.rows) py[3] = src.rows - 1;

#else
			//Boundary Handling using Mirroring
			px[1] = cvFloor(rx);
			px[0] = px[1] - 1;	if (px[0] < 0) px[0] = 1;
			px[2] = px[1] + 1;	if (px[2] == src.cols) px[2] = src.cols - 2;
			px[3] = px[1] + 2;	if (px[3] == src.cols) px[3] = src.cols - 2; else if (px[3] == src.cols + 1) px[3] = src.cols - 3;

			py[1] = cvFloor(ry);
			py[0] = py[1] - 1;	if (py[0] < 0) py[0] = 1;
			py[2] = py[1] + 1;	if (py[2] == src.rows) py[2] = src.rows - 2;
			py[3] = py[1] + 2;	if (py[3] == src.rows) py[3] = src.rows - 2; else if (py[3] == src.rows + 1) py[3] = src.rows - 3;
#endif

			sx = rx - px[1];
			sy = ry - py[1];

			ux[1] = 1.5 * pow(sx, 3) - 2.5 * pow(sx, 2) + 1;
			ux[2] = 1.5 * pow(1 - sx, 3) - 2.5 * pow(1 - sx, 2) + 1;
			ux[0] = -0.5 * pow(1 + sx, 3) + 2.5 * pow(1 + sx, 2) - 4 * (1 + sx) + 2;
			ux[3] = -0.5 * pow(2 - sx, 3) + 2.5 * pow(2 - sx, 2) - 4 * (2 - sx) + 2;


			uy[1] = 1.5 * pow(sy, 3) - 2.5 * pow(sy, 2) + 1;
			uy[2] = 1.5 * pow(1 - sy, 3) - 2.5 * pow(1 - sy, 2) + 1;
			uy[0] = -0.5 * pow(1 + sy, 3) + 2.5 * pow(1 + sy, 2) - 4 * (1 + sy) + 2;
			uy[3] = -0.5 * pow(2 - sy, 3) + 2.5 * pow(2 - sy, 2) - 4 * (2 - sy) + 2;


			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 4; j++) {
					r[j][i] = saturate_cast<uchar>(
						ux[0] * src.at<Vec3b>(py[j], px[0])[i] +
						ux[1] * src.at<Vec3b>(py[j], px[1])[i] +
						ux[2] * src.at<Vec3b>(py[j], px[2])[i] +
						ux[3] * src.at<Vec3b>(py[j], px[3])[i] + 0.5);
				}

				dst.at<Vec3b>(y, x)[i] = saturate_cast<uchar>(
					uy[0] * r[0][i] +
					uy[1] * r[1][i] +
					uy[2] * r[2][i] +
					uy[3] * r[3][i] + 0.5);
			}
		}
	}


	return dst;
}
/**************************************************************
	Func	void Show(String named, const Mat& dst)
	
	Show Image that modifed original Image

	Param	String named		= Window Name and Write File Name
			const  Mat& dst		= Processed Image


**************************************************************/

int createdWindowX;

void Show(String named, const Mat& dst)
{
	namedWindow(named);
	moveWindow(named, createdWindowX, 0);

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
	String originalName = "Original Image";

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
	createdWindowX = src.cols;


	namedWindow(originalName);
	moveWindow(originalName, 0, 0);
	imshow(originalName, src);





	Show("Nearest Neightbor Interpolation",		NearestNeighbourInterpolation(src, scaleWidth, scaleHeight));
	Show("Bilinear Interpolation",				BilinearInterpolation(src, scaleWidth, scaleHeight));
	Show("Bicubic Interpolation",				BicubicInterpolation(src, scaleWidth, scaleHeight));


	destroyAllWindows();
	return 0;
}