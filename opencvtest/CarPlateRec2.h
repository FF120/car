#pragma once
#include "stdafx.h"
#include "CarPlatePosition.h"
#include "CarFlowCount.h"
#include "SetPreferencesDlg.h"

#include "cv.h"
#include "cvaux.h"
#include "CvvImage.h"
#include "cxcore.h"
#include "highgui.h"
#include <iostream>

//记录分割连通域的位置信息
typedef struct ROI
{
	int x;
	int y;
	int width;
	int height;
}ROI;

int compare(const void *a,const void *b);
//装载标准字符库
void loadImageLib(IplImage *model[]);
//相似度计算，形参为二值化后的图片
double imageCompare(IplImage* dst, IplImage* model);
//字符检测函数
void imageRecognition(IplImage *img, int m, int n);
//对ODQ进行二次检测
int againRecognition_ODQ(IplImage* img);
//对2Z进行二次检测
int againRecognition_2Z(IplImage* img);
//对5S进行二次检测
int againRecognition_5S(IplImage* img);
//对8B进行二次检测
int againRecognition_8B(IplImage* img);
//对应输出识别的汉字
void representChinese(int k);
//识别函数
void segmentRecognition();

class CarPlateRec2
{
public:
	CarPlateRec2(void);
	~CarPlateRec2(void);
};

