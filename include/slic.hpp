/*
 * @Description: 
 * @Author: xielock
 * @Date: 2022-01-11 16:33:22
 * @LastEditors: xie
 * @LastEditTime: 2022-01-11 19:27:31
 */
#include<iostream>
#include "opencv2/opencv.hpp"
#include<stdlib.h>
#include<time.h>
#include<vector>

class SLIC{

    private:
    struct center
    {
        int x;//column
        int y;//row
        int L;
        int A;
        int B;
        int label;
    };
    std::string imagePath;  //the path of the srcImage
    cv::Mat srcImage;
    cv::Mat imageLAB;
    cv::Mat sobelImagex, sobelImagey, sobelGradient;
    cv::Mat labelMask;  //save every pixel's label
    cv::Mat dst; //save the shortest distance to the nearest centers
    std::vector<center>centers;  //save the center of every superpixels 
    int len; //the scale of the superpixel, len*len 
    int m;   //a parameter which adjust the weights of spacial distance and the color space distance;

    public:
    SLIC(std::string & imagePath, cv:: Mat& labelMask, int len,  int m)
    {
        //赋值
        this->imagePath = imagePath;
        this->labelMask = labelMask;
        this->len = len;
        this->m = m;
        SLIC_demo();

    }
    ~SLIC(){}
    void SLIC_demo();
    int  SLIC_Init();
    void initilizeCenters();
    void fituneCenter();
    void cluster();
    void updateCenter();
    
    void showSLICResult();
    void showSLICResult2();



};
