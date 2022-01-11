/*
 * @Description: 
 * @Author: xielock
 * @Date: 2022-01-11 18:42:13
 * @LastEditors: xie
 * @LastEditTime: 2022-01-11 20:31:21
 */
#include<slic.hpp>

using namespace cv;
using namespace std;

void SLIC::SLIC_demo(const std::string & imagePath, cv:: Mat& labelMask, int len,  int m)
{
    this->imagePath = imagePath;
    this->labelMask = labelMask;
    this->len = len;
    this->m = m;
	srcImage = imread(imagePath);


	SLIC_Init();
	//showSLICResult();
	//showSLICResult2();

}


int SLIC::SLIC_Init()
{

	if (srcImage.empty())
	{
		std::cout << "in SLIC the input image is empty!\n";
		return -1;

	}
	int MAXDIS = 999999;
	int height, width;
	height = srcImage.rows;
	width = srcImage.cols;

	//convert color
	cv::cvtColor(srcImage, imageLAB, cv::COLOR_BGR2Lab);

	//get sobel gradient map
	cv::Sobel(imageLAB, sobelImagex, CV_64F, 0, 1, 3);
	cv::Sobel(imageLAB, sobelImagey, CV_64F, 1, 0, 3);
	cv::addWeighted(sobelImagex, 0.5, sobelImagey, 0.5, 0, sobelGradient);//sobel output image type is CV_64F


	// cv::Mat disMask ;
	labelMask = cv::Mat::zeros(cv::Size(width, height), CV_64FC1);

	//initialize centers,  get centers
	initilizeCenters();

	//if the center locates in the edges, fitune it's location
	fituneCenter();

	//update cluster 10 times 
	for (int time = 0; time < 2; time++)
	{
		//clustering
		dst = cv::Mat(height, width, CV_64FC1, cv::Scalar(MAXDIS));
		cluster();
		//update
		updateCenter();
	}
	return 0;
}


void SLIC::initilizeCenters()
{
		center cent;
		int num = 0;
		for (int i = 0; i < imageLAB.rows; i += len)
	{
			//行
			cent.y = i + len / 2;
			if (cent.y >= imageLAB.rows) continue;
		for (int j = 0; j < imageLAB.cols; j += len)
		{
			//列
			cent.x = j + len / 2;
			if ((cent.x >= imageLAB.cols)) continue;

			cent.L = int(imageLAB.at<Vec3b>(cent.y, cent.x)[0]);
			cent.A = int(imageLAB.at<Vec3b>(cent.y, cent.x)[1]);
			cent.B = int(imageLAB.at<Vec3b>(cent.y, cent.x)[2]);
			
			cent.label = ++num;
			centers.push_back(cent);
		}
	}
	
}

void SLIC::fituneCenter()
{
	if (sobelGradient.empty()) return ;
	center cent;
	int w = sobelGradient.cols;
	int h = sobelGradient.rows;
	for (int k = 0; k < centers.size(); k++)
	{
		// 超范围则 继续
		cent = centers[k];
		if (cent.x - 1 < 0 || cent.x + 1 >= w || cent.y - 1 < 0 || cent.y + 1 >= h)
		{
			continue;
		}
		double minGradient = 9999999;
		int tempx = 0, tempy = 0;
		//3*3 范围内调整
		for (int m = -1; m < 2; m++)
		{
			for (int n = -1; n < 2; n++)
			{
				double gradient = pow(imageLAB.at<Vec3b>(cent.y +m, cent.x + n)[0], 2)
												  + pow(imageLAB.at<Vec3b>(cent.y +m, cent.x + n)[1], 2)
												  + pow(imageLAB.at<Vec3b>(cent.y +m, cent.x + n)[2], 2);
				if (gradient < minGradient)
				{
					minGradient = gradient;
					tempy = m;//row 要调整的数目
					tempx = n;//column
				}
			}
		}
		cent.x += tempx;
		cent.y += tempy;

		centers[k].x = cent.x;
		centers[k].y = cent.y;
		centers[k].L = imageLAB.at<Vec3b>(cent.y, cent.x)[0];
		centers[k].A = imageLAB.at<Vec3b>(cent.y, cent.x)[1];
		centers[k].B = imageLAB.at<Vec3b>(cent.y, cent.x)[2];

	}//end for

}


void SLIC::cluster()
{
		if (imageLAB.empty())
	{
		std::cout << "clustering :the input image is empty!\n";
		return;
	}


	//disc = std::sqrt(pow(L - cL, 2)+pow(A - cA, 2)+pow(B - cB,2))
	//diss = std::sqrt(pow(x-cx,2) + pow(y-cy,2));
	//dis = sqrt(disc^2 + (diss/len)^2 * m^2)
	double dis = 0, disc = 0, diss = 0;
	//cluster center's cx, cy,cL,cA,cB;
	int cx, cy, cL, cA, cB, clabel;
	//imageLAB's  x, y, L,A,B
	int x, y, L, A, B;

	//注：这里的图像坐标以左上角为原点，水平向右为x正方向,水平向下为y正方向，与opencv保持一致
	//      从矩阵行列角度看，i表示行，j表示列，即(i,j) = (y,x)
	for (int ck = 0; ck < centers.size(); ++ck)
	{
		cx = centers[ck].x;
		cy = centers[ck].y;
		cL = centers[ck].L;
		cA = centers[ck].A;
		cB = centers[ck].B;
		clabel = centers[ck].label;

		for (int i = cy - len; i < cy + len; i++)
		{
				if (i < 0 | i >= imageLAB.rows) continue;
			for (int j = cx - len; j < cx + len; j++)
			{
				if (j < 0 | j >= imageLAB.cols) continue;

				L = imageLAB.at<Vec3b>(i, j)[0];
				A = imageLAB.at<Vec3b>(i, j)[1];
				B = imageLAB.at<Vec3b>(i, j)[2];


				disc = std::sqrt(pow(L - cL, 2) + pow(A - cA, 2) + pow(B - cB, 2));
				diss = std::sqrt(pow(j - cx, 2) + pow(i - cy, 2));
				dis = sqrt(pow(disc, 2) + m * pow(diss, 2));
				if (dis < dst.at<double>(i,j))
				{
					dst.at<double>(i,j)  = dis;
					labelMask.at<double>(i,j)  = clabel;
				}
			}
		}
	}
}


void SLIC::updateCenter()
{
	int cx, cy;
	for (int ck = 0; ck < centers.size(); ++ck)
	{
		double sumx = 0, sumy = 0, sumL = 0, sumA = 0, sumB = 0, sumNum = 0;
		cx = centers[ck].x;
		cy = centers[ck].y;
		for (int i = cy - len; i < cy + len; i++)
		{
				if (i < 0 | i >= imageLAB.rows) continue;
				//pointer point to the ith row
			for (int j = cx - len; j < cx + len; j++)
			{
				if (j < 0 | j >= imageLAB.cols) continue;

				if (labelMask.at<double>(i,j) == centers[ck].label)
				{
					sumL += imageLAB.at<Vec3b>(i,j)[0];
					sumA += imageLAB.at<Vec3b>(i,j)[1];
					sumB += imageLAB.at<Vec3b>(i,j)[2];
					sumx += j;
					sumy += i;
					sumNum += 1;
				}//end if
			}
		}
		//update center
		if (sumNum == 0) sumNum = 0.000000001;
		centers[ck].x = sumx / sumNum;
		centers[ck].y = sumy / sumNum;
		centers[ck].L = sumL / sumNum;
		centers[ck].A = sumA / sumNum;
		centers[ck].B = sumB / sumNum;

	}
	
}


void SLIC::showSLICResult()
{
	cv::Mat dst = srcImage.clone();
	cv::cvtColor(dst, dst, cv::COLOR_BGR2Lab);

	int cx, cy;
	double sumx = 0, sumy = 0, sumL = 0, sumA = 0, sumB = 0, sumNum = 0.00000001;
	for (int ck = 0; ck < centers.size(); ++ck)
	{
		cx = centers[ck].x;
		cy = centers[ck].y;

		for (int i = cy - len; i < cy + len; i++)
		{
			if (i < 0 | i >= srcImage.rows) continue;
			//pointer point to the ith row
			for (int j = cx - len; j < cx + len; j++)
			{
				if (j < 0 | j >= srcImage.cols) continue;

				if (labelMask.at<double>(i,j) == centers[ck].label)
				{
					dst.at<Vec3b>(i,j)[0] = centers[ck].L;
					dst.at<Vec3b>(i,j)[1]= centers[ck].A;
					dst.at<Vec3b>(i,j)[2] = centers[ck].B;
				}//end if
			}
		}
	}//end for

	cv::cvtColor(dst, dst, cv::COLOR_Lab2BGR);
	cv::namedWindow("showSLIC", 0);
	cv::imshow("showSLIC", dst);
	cv::waitKey(1);

}


void SLIC::showSLICResult2()
{
	cv::Mat dst = srcImage.clone();
	for (int i = 0; i < labelMask.rows - 1; i++)
	{
		for (int j = 0; j < labelMask.cols - 1; j++)
		{
			//if left pixel's label is different from the right's  找到不同超像素的跳变点->|->左右
			if (labelMask.at<double>(i,j) != labelMask.at<double>(i,j + 1))
			{
				dst.at<Vec3b>(i,j)[0] = 0;
				dst.at<Vec3b>(i,j)[1] = 0;
				dst.at<Vec3b>(i,j)[2] = 0;

			}
			//if the upper pixel's label is different from the bottom's _|上下
			if ( labelMask.at<double>(i+1,j) != labelMask.at<double>(i, j))
			{
				dst.at<Vec3b>(i,j)[0] = 0;
				dst.at<Vec3b>(i,j)[1] = 0;
				dst.at<Vec3b>(i,j)[2] = 0;
			}
		}
	}

	//show center
	for (int ck = 0; ck < centers.size(); ck++)
	{
		dst.at<Vec3b>(centers[ck].y, centers[ck].x)[0] = 100;
		dst.at<Vec3b>(centers[ck].y, centers[ck].x)[1] = 100;
		dst.at<Vec3b>(centers[ck].y, centers[ck].x)[2] = 10;
	}
	cv::namedWindow("showSLIC2", 0);
	cv::imshow("showSLIC2", dst);
	// imwrite(imagePath+ to_string(1) + ".jpg", dst);
	cv::waitKey(1);
}


void SLIC::clear()
{
	centers.clear();
}