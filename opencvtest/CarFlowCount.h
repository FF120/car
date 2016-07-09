#pragma once
#include "cv.h"
#include "cv.hpp"
#include "highgui.h"
#include "resource.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;
//字符结构体
typedef struct Hei_sta{//宽度高度统计，第一列存高，第二列存顶端坐标，第三列底端坐标，,第四列左侧坐标，第五列右侧坐标 第六列为标记值
	int height;
	int width;
	int top;
	int bottom;
	int left;
	int right;
	int sign;
};
class CarFlowCount:CDialogEx
{	
public:
	CarFlowCount(void);
	~CarFlowCount(void);
	

	//IplImage *frame_use; 
	//CString strTmp2_use;//车辆计数
	CString strTmp2;
	
	

	IplImage* count_aga();
	int countCar();
	IplImage* cutRoi(IplImage* img, CvPoint pt1, int width,int height);
	void ShowImage(IplImage *img, UINT ID);
	IplImage* CopyIplImageROI(IplImage * pImageSrc,CvRect rectROI);
	IplImage* cvCopyIplImage(IplImage*  srcImg);
	vector<Hei_sta> find_connected_domain(IplImage* src_image, IplImage* dst_image);
};

