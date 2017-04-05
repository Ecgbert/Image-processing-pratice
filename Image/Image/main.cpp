#include <opencv2\opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
using namespace cv;
using namespace std;

int main()
{
	unsigned seed;
	seed = (unsigned)time(NULL); // 取得時間序列
	srand(seed); 
	Mat image = imread("image.jpg", CV_LOAD_IMAGE_COLOR);
	const int cols = image.cols;//寬
	const int rows = image.rows;//高
	//double sum = 0;
	double min, max;
	double random;
	Mat gray_image = Mat(rows, cols, CV_8UC1);

	//灰階
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			float b = image.at<Vec3b>(i, j)[0];
			float g = image.at<Vec3b>(i, j)[1];
			float r = image.at<Vec3b>(i, j)[2];
			double gray = 0.299 * r + 0.587 * g + 0.114 * b;
			gray_image.at<unsigned char>(i, j) = gray;
		}
	}
	minMaxLoc(gray_image, &min, &max);
	imwrite("gray.jpg", gray_image);

	//負片
	Mat Negative = gray_image.clone();
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			float b = image.at<Vec3b>(i, j)[0];
			float g = image.at<Vec3b>(i, j)[1];
			float r = image.at<Vec3b>(i, j)[2];
			double gray = 0.299 * r + 0.587 * g + 0.114 * b;
			Negative.at<unsigned char>(i, j) = 255 - gray;
		}
	}
	imwrite("Negative.jpg", Negative);
	//gamma 0.5
	Mat gamma_image = Mat(rows, cols, CV_8UC1);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			float b = image.at<Vec3b>(i, j)[0];
			float g = image.at<Vec3b>(i, j)[1];
			float r = image.at<Vec3b>(i, j)[2];
			double gray = 0.299 * r + 0.587 * g + 0.114 * b;
			double gamma = pow((gray - min) / (max - min), 0.5) * 255;
			gamma_image.at<unsigned char>(i, j) = gamma;
		}
	}
	imwrite("gamma-0.5.jpg", gamma_image);

	//gamma 3
	Mat gamma_image1 = Mat(rows, cols, CV_8UC1);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			float b = image.at<Vec3b>(i, j)[0];
			float g = image.at<Vec3b>(i, j)[1];
			float r = image.at<Vec3b>(i, j)[2];
			double gray = 0.299 * r + 0.587 * g + 0.114 * b;
			double gamma = pow((gray - min) / (max - min), 3) * 255;
			gamma_image1.at<unsigned char>(i, j) = gamma;
		}
	}
	imwrite("gamma-3.jpg", gamma_image1);
	// 對比拉開 gamma 1
	Mat contrast_image = Mat(rows, cols, CV_8UC1);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			float b = image.at<Vec3b>(i, j)[0];
			float g = image.at<Vec3b>(i, j)[1];
			float r = image.at<Vec3b>(i, j)[2];
			double gray = 0.299 * r + 0.587 * g + 0.114 * b;
			double contrast = ((gray - min) / (max - min)) * 255;
			contrast_image.at<unsigned char>(i, j) = contrast;
		}
	}
	imwrite("contrast.jpg", contrast_image);
	// 胡椒鹽
	Mat pepper = Mat(rows, cols, CV_8UC1);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			random = (double)rand() / 32767; //0-1之間
			float b = image.at<Vec3b>(i, j)[0];
			float g = image.at<Vec3b>(i, j)[1];
			float r = image.at<Vec3b>(i, j)[2];
			double gray = 0.299 * r + 0.587 * g + 0.114 * b;
			double contrast = ((gray - min) / (max - min)) * 255;
			pepper.at<unsigned char>(i, j) = contrast;
			if (random > 0.9)
			{
				//sum = min + random * (max - min);
				pepper.at<unsigned char>(i, j) = 255;
			}
			else if (random > 0.8 && random < 0.9)
			{
				pepper.at<unsigned char>(i, j) = 0;
			}
			/*else if (random == 1)
			{
				pepper.at<unsigned char>(i, j) = 255;
			}*/
		}
	}
	imwrite("pepper.jpg", pepper);
	//中值濾波
	Mat pepper2 = pepper.clone();
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			int k = 0;
			int window[9];
			for (int x = i - 1; x < i + 2; ++x)
				for (int y = j - 1; y < j + 2; ++y)
					window[k++] = pepper2.at<unsigned char>(x, y);
			//排序
			for (int m = 0; m < 5; ++m)
			{
				int min = m;
				for (int n = m + 1; n < 9; ++n)
					if (window[n] < window[min])
						min = n;
				//最小值
				int temp = window[m];
				window[m] = window[min];
				window[min] = temp;
			}
			pepper2.at<unsigned char>(i, j) = window[4];
		}
	}
	imwrite("median.jpg", pepper2);
	//平均濾波
	Mat pepper3 = pepper.clone();
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			if ((i != 0) & (i != rows - 1) & (j != 0) & (j != cols - 1))     
			{
				pepper3.at<unsigned char>(i, j) = (pepper3.at<unsigned char>(i - 1, j - 1) + pepper3.at<unsigned char>(i - 1, j) +
				pepper3.at<unsigned char>(i - 1, j + 1) + pepper3.at<unsigned char>(i, j - 1) + pepper3.at<unsigned char>(i, j) +
				pepper3.at<unsigned char>(i, j + 1) + pepper3.at<unsigned char>(i + 1, j - 1) + pepper3.at<unsigned char>(i + 1, j) +
				pepper3.at<unsigned char>(i + 1, j + 1)) / 9;
			}	
		}
	}
	imwrite("average.jpg", pepper3);
	//二值化
	Mat binarization = gamma_image1.clone();
	double avg = 0;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			avg = binarization.at<unsigned char>(i, j) + avg;
		}
	}
	double otsu = avg / (cols*rows);//平均門檻值
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			if (binarization.at<unsigned char>(i, j) > otsu)
				binarization.at<unsigned char>(i, j) = 255;
			else if (binarization.at<unsigned char>(i, j) < otsu)
			{
				binarization.at<unsigned char>(i, j) = 0;
			}
		}
	}
	imwrite("binarization.jpg", binarization);

	//sobel
	int sum, gx, gy;
	Mat sobel = contrast_image.clone();
	Mat sobel2 = Mat(rows, cols, CV_8UC1);
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			if ((i != 0) && (i != rows - 1) && (j != 0) && (j != cols - 1))
			{
				//水平梯度
				gy = sobel.at<unsigned char>(i - 1, j + 1) + 2 * sobel.at<unsigned char>(i, j+1) +
					sobel.at<unsigned char>(i + 1, j + 1) - sobel.at<unsigned char>(i - 1, j -1) -
					2 * sobel.at<unsigned char>(i, j-1 ) - sobel.at<unsigned char>(i + 1, j - 1);
				//cout << gy << endl;
				//垂直梯度
				gx = sobel.at<unsigned char>(i + 1, j - 1) + 2 * sobel.at<unsigned char>(i + 1, j) +
					sobel.at<unsigned char>(i+1 , j +1) - sobel.at<unsigned char>(i-1 , j - 1) -
					2 * sobel.at<unsigned char>(i - 1, j) - sobel.at<unsigned char>(i - 1, j + 1);
				//cout << gx << endl;
				sum = sqrt(gx*gx + gy*gy);
				sobel2.at<unsigned char>(i, j) = sum;
			}
		}
	}
	imwrite("sobel.jpg", sobel2);
	//system("pause");
	return 0;
}