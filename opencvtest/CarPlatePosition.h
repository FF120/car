#pragma once
#include "stdafx.h"
#include "cv.h"
#include "cvaux.h"
#include "CvvImage.h"
#include "cxcore.h"
#include "highgui.h"
#include "resource.h"
#include "CarFlowCount.h"
#include<vector>
#include<iostream>

using namespace std;
using namespace cv;

typedef struct VP{    
	int initiapos_row;//起始行的行数值
	int initiapos_col;//起始列的列数值
	int end_row;//最终行的行值
	int end_col;//最终列的列值
	int count;//像素数量
};

//字符结构体


class CarPlatePosition
{
public:
	CarPlatePosition(void);
	~CarPlatePosition(void);
	
	/***************工具函数********************************/
	//void DrawPicToHDC(IplImage *img, UINT ID);
	//void OpenPicture();
	IplImage* cut_Roi_single(IplImage* img, CvPoint pt1, int width,int height);
	IplImage* cut_Roi_three(IplImage* img, CvPoint pt1, int width,int height);
	IplImage* CopyIplImage(IplImage*  srcImg);
	IplImage* CopyIplImageROI(IplImage* m_img_src,CvRect selection);

	/******************核心算法函数*******************************/
	//IplImage* cutRoi(IplImage* pimg, CvPoint pt1, int width,int height);
	IplImage* texture(IplImage *img_src) ;
	void vp_blue_white( int istart_row, int istart_col, int iend_row, int iend_col); //若为蓝底白字车牌，提取纹理特征
	void Color_Trans(IplImage *img_src);
	void projection(IplImage *img, int istart_row, int iend_row, int istart_col, int iend_col, char s); //按行和列投影，重新估计上下以及左右边缘
	VP vp_detect(int W_Width, int W_Height);
	VP plate_segment(IplImage *src_img);
	//vector<Hei_sta> find_connected_domain(IplImage* src_image, IplImage* dst_image);	
	bool pre_check(vector <Hei_sta> vt_Connect,IplImage* src_image, IplImage* image_sign, IplImage* dst_image, IplImage* image_sign_ref);
	void  PlateCharSegment(IplImage *pImagePlate);
	void plateSegment();//车牌定位单独处理函数
	void plateSegmentOne();//车牌定位一键处理函数
};

