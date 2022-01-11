/*
 * @Description: 
 * @Author: xielock
 * @Date: 2022-01-11 19:19:47
 * @LastEditors: xie
 * @LastEditTime: 2022-01-11 20:32:22
 */
#include"slic.hpp"

using namespace std;

int main()
{
	double start = cv::getTickCount();
	
	std::string imagePath = "./mao.jpg";
	cv::Mat labelMask;
	int len = 25;
	int m = 10;
	
	SLIC slic; // 不再通过构造函数传递参数  
	for(int i = 0; i< 50; i++)
	{
	slic.SLIC_demo(imagePath, labelMask, len, m);
	slic.clear();
	}
	double end = cv::getTickCount();
	double dura = (end - start)/cv::getTickFrequency();
	std::cout<<dura<<std::endl;

	cv::waitKey(0);
	return 0;
}

