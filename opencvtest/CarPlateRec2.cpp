#include "StdAfx.h"
#include "CarPlateRec2.h"
#include "cv.h"
#include "CvvImage.h"
#include "highgui.h"
#include <iostream>
#include <vector>
#include <iostream>
#define MINAREA 100.0

//保存识别结果
using namespace std;
using namespace cv;

CarPlateRec2::CarPlateRec2(void)
{
}

CarPlateRec2::~CarPlateRec2(void)
{
}


string result_RES2;
//extern vector <Hei_sta> vt_Connect_use;
extern IplImage* vp_images_sign_ref; //字符分割后的车牌图像
extern IplImage* roiThree;  //原图中裁剪的彩色车牌截图

int compare(const void *a,const void *b)//定义了一个指针a，a可以指向任意类型的值，但它指向的值必须是常量
{
	return ((ROI *)a)->x - ((ROI *)b)->x;
}

//相似度计算，形参为二值化后的图片
double imageCompare(IplImage* dst, IplImage* model)
{
	double sum = 0; //统计相等像素点的个数
	double temp = 0.0;   //保存最大相似度

	uchar* data_dst = (uchar *)dst->imageData;
	uchar* data_model = (uchar *)model->imageData;
	int step_dst = dst->widthStep;
	int step_model = model->widthStep;
	double max =0.0;
	//设置一定的偏移量进行比较
	for(int row = -2; row < 3; row++)
		for(int col = -2; col < 3; col++)
		{
			sum = 0;
			for(int row_dst = (row>0?row:0), row_model = (row<0?(-row):0); row_dst < dst->height && row_model < model->height; row_dst++, row_model++)
			{
				for(int col_dst = (col>0?col:0), col_model = (col<0?(-col):0); col_dst < dst->width && col_model < model->width; col_dst++, col_model++)
					if(data_dst[row_dst * step_dst + col_dst] == data_model[row_model * step_model + col_model])
						sum++;
			}
			temp = sum/((dst->height-abs(row)) * (dst->width - abs(col)));
			if(temp > max)
				max = temp;
		}
		return max;
}

//对ODQ进行二次检测
int againRecognition_ODQ(IplImage* img)
{
	IplImage* turn_image = cvCloneImage(img);
	int key;
	cvFlip(turn_image, NULL, 1);
	if(imageCompare(turn_image, img) > 0.94)			
		key = 0;
	else
	{
		uchar* data = (uchar *)img->imageData;
		int temp = 0;
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				if(data[i*img->widthStep+j] == 255)
					temp++;
		if(temp >= 8)
			key = 13;
		else if(temp < 8)
		{
			temp = 0;
			for(int i = img->height - 4; i <= img->height; i++)
				for(int j = img->width - 4; j < img->width; j++)
					if(data[i*img->widthStep+j] == 255)
						temp++;
			if(temp >= 8)
				key = 26;
			else 
				key = 0;
		}
	}
	return key;
}

//对2Z进行二次检测
int againRecognition_2Z(IplImage* img)
{
	IplImage* turn_image = cvCloneImage(img);
	int key;
	cvFlip(turn_image, NULL, -1);
	if(imageCompare(turn_image, img) > 0.94)			
		key = 35;
	else
	{
		uchar* data = (uchar *)img->imageData;
		int temp = 0;
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				if(data[i * img->widthStep + j] == 255)
					temp++;
		if(temp >= 8)
			key = 35;
		else if(temp < 8)
		{
			temp = 0;
			for(int i = 0; i < 4; i++)
				for(int j = img->width - 4; j < img->width; j++)
					if(data[i * img->widthStep + j] == 255)
						temp++;
			if(temp >= 8)
				key = 35;
			else
				key = 2;
		}
	}
	return key;
}

//对5S进行二次检测
int againRecognition_5S(IplImage* img)
{
	IplImage* turn_image = cvCloneImage(img);
	int key;
	cvFlip(turn_image, NULL, -1);
	if(imageCompare(turn_image, img) > 0.94)			
		key = 28;
	else
	{
		uchar* data = (uchar *)img->imageData;
		int temp = 0;
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				if(data[i * img->widthStep + j] == 255)
					temp++;
		if(temp >= 8)
			key = 5;
		else if(temp < 8)
		{
			temp = 0;
			for(int i = 0; i < 4; i++)
				for(int j = img->width - 4; j < img->width; j++)
					if(data[i * img->widthStep + j] == 255)
						temp++;
			if(temp >= 8)
				key = 5;
			else 
				key = 28;
		}
	}
	return key;
}

//对8B进行二次检测
int againRecognition_8B(IplImage* img)
{
	IplImage* turn_image = cvCloneImage(img);
	int key;
	cvFlip(turn_image, NULL, 1);
	if(imageCompare(turn_image, img) > 0.94)			
		key = 8;
	else
	{
		uchar* data = (uchar *)img->imageData;
		int temp = 0;
		for(int i = 0; i < 4; i++)
			for(int j = 0; j < 4; j++)
				if(data[i * img->widthStep + j] == 255)
					temp++;
		for(int i = img->height - 4; i < img->height; i++)
			for(int j = 0; j < 4; j++)
				if(data[i * img->widthStep + j] == 255)
					temp++;
		if(temp >= 10)
			key = 11;
		else 
			key = 8;		
	}
	return key;
}

//装载标准字符库
void loadImageLib(IplImage *model[])
{
	//装载数字图片库
	model[0] = cvLoadImage("D:\\lib\\num\\0.jpg", CV_LOAD_IMAGE_GRAYSCALE); 
	model[1] = cvLoadImage("D:\\lib\\num\\1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[2] = cvLoadImage("D:\\lib\\num\\2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[3] = cvLoadImage("D:\\lib\\num\\3.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[4] = cvLoadImage("D:\\lib\\num\\4.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[5] = cvLoadImage("D:\\lib\\num\\5.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[6] = cvLoadImage("D:\\lib\\num\\6.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[7] = cvLoadImage("D:\\lib\\num\\7.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[8] = cvLoadImage("D:\\lib\\num\\8.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[9] = cvLoadImage("D:\\lib\\num\\9.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//装载字母图片库
	model[10] = cvLoadImage("D:\\lib\\alphabet\\38.jpg", CV_LOAD_IMAGE_GRAYSCALE); 
	model[11] = cvLoadImage("D:\\lib\\alphabet\\39.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[12] = cvLoadImage("D:\\lib\\alphabet\\40.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[13] = cvLoadImage("D:\\lib\\alphabet\\41.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[14] = cvLoadImage("D:\\lib\\alphabet\\42.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[15] = cvLoadImage("D:\\lib\\alphabet\\43.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[16] = cvLoadImage("D:\\lib\\alphabet\\44.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[17] = cvLoadImage("D:\\lib\\alphabet\\45.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[18] = cvLoadImage("D:\\lib\\alphabet\\46.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[19] = cvLoadImage("D:\\lib\\alphabet\\47.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[20] = cvLoadImage("D:\\lib\\alphabet\\48.jpg", CV_LOAD_IMAGE_GRAYSCALE); 
	model[21] = cvLoadImage("D:\\lib\\alphabet\\49.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[22] = cvLoadImage("D:\\lib\\alphabet\\50.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[23] = cvLoadImage("D:\\lib\\alphabet\\51.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[24] = cvLoadImage("D:\\lib\\alphabet\\52.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[25] = cvLoadImage("D:\\lib\\alphabet\\53.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[26] = cvLoadImage("D:\\lib\\alphabet\\54.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[27] = cvLoadImage("D:\\lib\\alphabet\\55.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[28] = cvLoadImage("D:\\lib\\alphabet\\56.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[29] = cvLoadImage("D:\\lib\\alphabet\\57.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[30] = cvLoadImage("D:\\lib\\alphabet\\58.jpg", CV_LOAD_IMAGE_GRAYSCALE); 
	model[31] = cvLoadImage("D:\\lib\\alphabet\\59.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[32] = cvLoadImage("D:\\lib\\alphabet\\60.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[33] = cvLoadImage("D:\\lib\\alphabet\\61.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[34] = cvLoadImage("D:\\lib\\alphabet\\62.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[35] = cvLoadImage("D:\\lib\\alphabet\\63.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	//装载汉字图片库
	model[36] = cvLoadImage("D:\\lib\\chinese\\1.jpg", CV_LOAD_IMAGE_GRAYSCALE); 
	model[37] = cvLoadImage("D:\\lib\\chinese\\2.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[38] = cvLoadImage("D:\\lib\\chinese\\3.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[39] = cvLoadImage("D:\\lib\\chinese\\4.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[40] = cvLoadImage("D:\\lib\\chinese\\5.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[41] = cvLoadImage("D:\\lib\\chinese\\6.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[42] = cvLoadImage("D:\\lib\\chinese\\7.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[43] = cvLoadImage("D:\\lib\\chinese\\8.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[44] = cvLoadImage("D:\\lib\\chinese\\9.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[45] = cvLoadImage("D:\\lib\\chinese\\10.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[46] = cvLoadImage("D:\\lib\\chinese\\11.jpg", CV_LOAD_IMAGE_GRAYSCALE); 
	model[47] = cvLoadImage("D:\\lib\\chinese\\12.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[48] = cvLoadImage("D:\\lib\\chinese\\13.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[49] = cvLoadImage("D:\\lib\\chinese\\14.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[50] = cvLoadImage("D:\\lib\\chinese\\15.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[51] = cvLoadImage("D:\\lib\\chinese\\16.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[52] = cvLoadImage("D:\\lib\\chinese\\17.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[53] = cvLoadImage("D:\\lib\\chinese\\18.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[54] = cvLoadImage("D:\\lib\\chinese\\19.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[55] = cvLoadImage("D:\\lib\\chinese\\20.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[56] = cvLoadImage("D:\\lib\\chinese\\21.jpg", CV_LOAD_IMAGE_GRAYSCALE); 
	model[57] = cvLoadImage("D:\\lib\\chinese\\22.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[58] = cvLoadImage("D:\\lib\\chinese\\23.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[59] = cvLoadImage("D:\\lib\\chinese\\24.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[60] = cvLoadImage("D:\\lib\\chinese\\25.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[61] = cvLoadImage("D:\\lib\\chinese\\26.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[62] = cvLoadImage("D:\\lib\\chinese\\27.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[63] = cvLoadImage("D:\\lib\\chinese\\28.jpg", CV_LOAD_IMAGE_GRAYSCALE); 
	model[64] = cvLoadImage("D:\\lib\\chinese\\29.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[65] = cvLoadImage("D:\\lib\\chinese\\30.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[66] = cvLoadImage("D:\\lib\\chinese\\31.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	model[67] = cvLoadImage("D:\\lib\\chinese\\32.jpg", CV_LOAD_IMAGE_GRAYSCALE);
}

void representChinese(int k)
{
	switch(k)
	{
	case 36:
		result_RES2 += "京";
		break;
	case 37:
		result_RES2 += "津";
		break;
	case 38:
		result_RES2 += "冀";
		break;
	case 39:
		result_RES2 += "晋";
		break;
	case 40:
		result_RES2 += "蒙";
		break;
	case 41:
		result_RES2 += "辽";
		break;
	case 42:
		result_RES2 += "吉";
		break;
	case 43:
		result_RES2 += "黑";
		break;
	case 44:
		result_RES2 += "沪";
		break;
	case 45:
		result_RES2 += "苏";
		break;
	case 46:
		result_RES2 += "浙";
		break;
	case 47:
		result_RES2 += "皖";
		break;
	case 48:
		result_RES2 += "闽";
		break;
	case 49:
		result_RES2 += "赣";
		break;
	case 50:
		result_RES2 += "鲁";
		break;
	case 51:
		result_RES2 += "豫";
		break;
	case 52:
		result_RES2 += "鄂";
		break;
	case 53:
		result_RES2 += "湘";
		break;
	case 54:
		result_RES2 += "粤";
		break;
	case 55:
		result_RES2 += "桂";
		break;
	case 56:
		result_RES2 += "琼";
		break;
	case 57:
		result_RES2 += "渝";
		break;
	case 58:
		result_RES2 += "川";
		break;
	case 59:
		result_RES2 += "贵";
		break;
	case 60:
		result_RES2 += "云";
		break;
	case 61:
		result_RES2 += "藏";
		break;
	case 62:
		result_RES2 += "陕";
		break;
	case 63:
		result_RES2 += "甘";
		break;
	case 64:
		result_RES2 += "青";
		break;
	case 65:
		result_RES2 += "宁";
		break;
	case 66:
		result_RES2 += "港";
		break;
	case 67:
		result_RES2 += "澳";
		break;
	}
}


void imageRecognition(IplImage *img, int m, int n)
{
	IplImage *model[68];  //保存模板图片
	double key[68];		  //保存与各模板的相似度
	IplImage* dst;

	//根据分割出字符的宽度，可以直接判断是否为字符1
	 	if(img->width >= 10 && img->width <= 25)
		{
			int a = 1;
			result_RES2 += (char)(a + '0');
			return ;
		}

	loadImageLib(model);  //载入模板字符库
	for(int i = 0; i < 68; i++)
		key[i] = 0.0;

	//将二值化后的图片分别与每幅模板比较，得出相似度
	for(int i = m; i <= n; i++)
	{
		int width = model[i]->width;
		int height = model[i]->height;

		//只是定义了一张图像啊
		IplImage* temp = cvCreateImage(cvSize(width, height), 8, 1);
		dst = cvCreateImage(cvSize(width, height), 8, 1);
		IplImage* mod = cvCreateImage(cvSize(width, height), 8, 1);

		//将分割出的字符调整到与模板字符一样大小
		cvResize(img, temp, CV_INTER_NN);


		//void cvResize( const CvArr* src, CvArr* dst, int interpolation=CV_INTER_LINEAR );
		//函数cvResize 重新调整图像src（或它的ROI），使它精确匹配目标dst（或其ROI）。
		//cvNamedWindow("调整大小后",CV_WINDOW_AUTOSIZE);     
		//cvShowImage("调整大小后", temp);  
		//void cvThreshold( const CvArr* src, CvArr* dst, double threshold, double max_value, int threshold_type );
		//对temp图像进行二值化操作，得到dst图像，阈值为110
		cvThreshold(temp, dst, 110, 255, CV_THRESH_OTSU);//二值化

		//对模板进行二值化操作,二值结果存入到mod图像中
		cvThreshold(model[i], mod, 128, 255, CV_THRESH_BINARY_INV);//二值化


		key[i] = imageCompare(dst, mod);
	}

	//找出最大的相似度
	double max = 0;
	int k = -1;
	for(int i = m; i <= n; i++)
	{
		if(key[i] > max)
		{
			max = key[i];
			k = i;
		}
	}

	//如果判断为ODQ之一，则进行二次检测
	if((k == 0 || k == 13 || k == 26) && m == 0 && n == 35)
		k = againRecognition_ODQ(dst);

	//如果判断为2Z之一，则进行二次检测
	if((k == 2 || k == 35) && m == 0 && n == 35)
		k = againRecognition_2Z(dst);

	//如果判断为5S之一，则进行二次检测
	if((k == 5 || k == 28) && m == 0 && n == 35)
		k = againRecognition_5S(dst);

	//如果判断为8B之一，则进行二次检测
	if((k == 8 || k == 11) && m == 0 && n == 35)
		k = againRecognition_8B(dst);

	//输出识别字符
	if(k >= 10 && k <= 35)
		result_RES2 += (char)(k+55);
	//printf("%c", k+55);
	else if(k < 10)
		result_RES2 += (char)(k+'0');
	//printf("%d", k);
	else
		representChinese(k);
	return ;
}

void segmentRecognition()
{
	IplImage* roiThree_copy = cvCreateImage(cvGetSize(roiThree),roiThree->depth,roiThree->nChannels);
	cvCopy(roiThree,roiThree_copy);

	IplImage *src = cvCreateImage(cvGetSize(roiThree_copy),roiThree_copy->depth,1);//灰度图
	IplImage* img_8uc1 = cvCreateImage(cvSize(409, 90), 8, 1);
	IplImage* img_edge = cvCreateImage(cvSize(409, 90), 8, 1);
	IplImage* img_8uc3 = cvCreateImage(cvSize(409, 90), 8, 3);
	cvCvtColor(roiThree_copy, src,CV_BGR2GRAY);
	//ShowImage(src, IDC_STATIC_GRAY);  //显示灰度图

	cvResize(src,img_8uc1,CV_INTER_LINEAR); //线性插值，变换图片为常规大小
	//cvShowImage("灰度化", img_8uc1);

	cvSmooth(img_8uc1,img_8uc1,CV_GAUSSIAN,3,0,0); //高斯滤波，去噪
	//ShowImage(img_8uc1, IDC_STATIC_GUASS);  //显示高斯滤波图像

	cvThreshold(img_8uc1, img_edge, 200, 255, CV_THRESH_OTSU);//二值化
	//cvShowImage("去噪", img_8uc1);
	//cvShowImage("二值化", img_edge);
	//ShowImage(img_edge, IDC_STATIC_THRESH);  //显示高斯滤波图像

	//截取字符
	IplImage *pImgCharOne=NULL;
	IplImage *pImgCharTwo=NULL;
	IplImage *pImgCharThree=NULL;
	IplImage *pImgCharFour=NULL;
	IplImage *pImgCharFive=NULL;
	IplImage *pImgCharSix=NULL;
	IplImage *pImgCharSeven=NULL;

	//记录各个字符的位置信息
	ROI cROI[7]; 

	//用来创建一个内存存储器，来统一管理各种动态对象的内存。
	//函数返回一个新创建的内存存储器指针
	CvMemStorage * storage = cvCreateMemStorage(0);
	CvSeq * first_contour = NULL;//CvSeq：一种动态的数据结构

	cvCvtColor(img_8uc1, img_8uc3, CV_GRAY2BGR); //转换为彩图，便于标记连通域
	//删除较小的连通域
	IplImage* img_clone=cvCloneImage(img_edge); 
	cvFindContours( img_clone, storage, &first_contour, sizeof(CvContour), CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE); 
	for(CvSeq * del = first_contour; del != NULL; del = del->h_next)
	{
		cvDrawContours(img_8uc3, del, CV_RGB(255,0,0), CV_RGB(0, 0, 255), 0, 2, 8);
		//根据面积大小判断小连通域
		double area = fabs(cvContourArea(del));
		printf("%f\n", area);
		CvRect delRect = cvBoundingRect(del,0);
		if(area < MINAREA)
		{
			//将img_edge中对应的小连通域变为黑色
			uchar *p;       
			for(int y = delRect.y; y<delRect.y+delRect.height; y++)         
				for(int x =delRect.x; x<delRect.x+delRect.width; x++)
				{
					p = (uchar*)(img_edge->imageData + img_edge->widthStep * y + x);
					p[0] = 0;
				}
				continue;
		}
	}

	 	//在去除了小连通域的二值图像img_edge中查找连通域，返回值为连通域的个数
	 	int Nc = cvFindContours( img_edge, storage, &first_contour, sizeof(CvContour), CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	 	int n = 0;           //标记各个字符的下标
	 	int minleft = 409;	//记录最左边的连通域横坐标
	 
	 	for(CvSeq * c = first_contour; c != NULL; c = c->h_next)
	 	{
	 		CvRect rect;//定义一个左上角坐标为（x,y），长度为width，宽度为height的矩形
	 		rect = cvBoundingRect(c,0); //返回字符边界的矩形//计算点集最外面的矩形边界
	 		printf("\n#%d #%d *%d *%d\n\n",rect.x, rect.y, rect.width, rect.height );
	 		//判断是否为左边框
	 		if(!((rect.x < 25 && rect.height > 80) || (rect.x < 25 && rect.width > 60) || (rect.x < 15 && rect.width < 8 /*11*/)))
	 		{
	 			//粗定位第一个字符的左边界
	 			if(minleft > rect.x)
	 				minleft = rect.x;
	 		}
	 
	 		//按连通域大小搜索字符
	 		if(rect.height >= 47 && rect.width >= 8 && rect.width <= 54 && rect.x > 45 && rect.x < 398)
	 		{
	 			cROI[n].x = rect.x;
	 			cROI[n].y = rect.y;
	 			cROI[n].width = rect.width;
	 			cROI[n].height = rect.height;
	 			n++;
	 		}
	 	}
	 
	 	if(n == 6)  //顺利分割出除第一个字符以外的后6个字符
	 	{
	 		//将字符按坐标排序
	 		qsort(cROI, 6, sizeof(ROI),compare);
	 
	 		//确定第一个字符的区域
	 		if(minleft + cROI[0].width + 20 < cROI[0].x)
	 		{
	 			cROI[6].x = cROI[0].x - 12 - cROI[0].width;
	 			if(cROI[6].x < 0)
	 				cROI[6].x = minleft;
	 		}
	 		else
	 			cROI[6].x = minleft;
	 
	 		//第二个和第六个不参与求均值是为了最小化车钉的影响
	 		cROI[6].y = (cROI[1].y + cROI[2].y + cROI[3].y + cROI[5].y)/4; 
	 		cROI[6].width = cROI[0].x - cROI[6].x - 5;
	 		cROI[6].height = (cROI[0].height + cROI[1].height + cROI[2].height + cROI[3].height + cROI[4].height + cROI[5].height)/6;
	 	}
	 	else    //未能分出后6个字符时，根据分割出的某个字符获取其他字符的位置，从而得到6个字符
	 	{
	 		//printf("wrong = %d\n", n);
	 		int x;
	 		int y;
	 		int width;
	 		int height;
	 
	 		if(n != 0 && minleft < 40)
	 		{
	 			int sum_height = 0;
	 			int sum_y = 0;
	 			for(int i = 0; i < n; i++)
	 			{
	 				sum_height += cROI[i].height;
	 				sum_y += cROI[i].y;
	 			}
	 			x = minleft;
	 			y = sum_y/n;
	 			width = cROI[0].width;
	 			height = sum_height/n;		
	 
	 			CvRect rect_ROI[6];
	 
	 			//第一个字符位置
	 			cROI[6].x = minleft;
	 			cROI[6].y = y;
	 			cROI[6].width = width;
	 			cROI[6].height = height;
	 
	 			for(int i = 0; i < 6; i++)
	 			{
	 				if(i == 0)
	 				{
	 					rect_ROI[i].x = minleft + (i + 1) * width;
	 					rect_ROI[i].y = y;
						rect_ROI[i].width = width + 10;
	 					rect_ROI[i].height = height;
	 				}
	 				else
	 				{
	 					rect_ROI[i].x = minleft + (i + 1) * (width + 12) + 10;
	 					rect_ROI[i].y = y;
	 					rect_ROI[i].width = width + 20; /*15*/
	 					if(rect_ROI[i].x + rect_ROI[i].width > 409)
	 						rect_ROI[i].width = 409 - rect_ROI[i].x;
	 					rect_ROI[i].height = height;
	 				}
	 
	 				IplImage* temp_img = cvCreateImage(cvSize(rect_ROI[i].width, rect_ROI[i].height),IPL_DEPTH_8U,1);
	 				IplImage* temp_img3 = cvCreateImage(cvSize(rect_ROI[i].width, rect_ROI[i].height),IPL_DEPTH_8U,3);
	 
	 				cvSetImageROI(img_8uc1, rect_ROI[i]);
	 				cvCopy(img_8uc1, temp_img, NULL); 
					cvResetImageROI(img_8uc1);
	
	 				cvThreshold(temp_img, temp_img, 128, 255, CV_THRESH_OTSU);//二值化
					CvMemStorage * storage_ROI = cvCreateMemStorage(0);
	 				CvSeq * first_contourROI = NULL;
	 
	 				int N = cvFindContours( temp_img, storage_ROI, &first_contourROI, sizeof(CvContour), CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	 
	 				cvCvtColor(temp_img, temp_img3, CV_GRAY2BGR); //转换为彩图，便于标记连通域
	 
					double contour_area = 0.0;
				CvRect MaxRect;
					for(CvSeq * contour = first_contourROI; contour != NULL; contour = contour->h_next)
					{
						cvDrawContours(temp_img3, contour, CV_RGB(255,0,0), CV_RGB(0, 0, 255), 0, 2, 8);
	 					//cvShowImage("temp_img3", temp_img3);
	
						double temp_area = fabs(cvContourArea(contour));
						if(contour_area < temp_area)
						{
 						contour_area = temp_area;
							MaxRect = cvBoundingRect(contour, 0);
						//printf("%d %d\n", MaxRect.x, MaxRect.y);
						}
	 				}
	
				//横、纵坐标的实际位置应该为切割出的子图在原图的坐标加上在子图中的相对位移
	 				cROI[i].x = MaxRect.x + rect_ROI[i].x;
	 				cROI[i].y = MaxRect.y + rect_ROI[i].y;
					cROI[i].width = MaxRect.width;
	 				cROI[i].height = MaxRect.height;
	 			}
			}
	 		else
	 		{
	 			result_RES2 += "字符分割失败";
	 //			SetDlgItemText(IDC_EDIT1, result);
	 			return;
	 		}
	 	}


	pImgCharOne=cvCreateImage(cvSize(cROI[6].width, cROI[6].height),IPL_DEPTH_8U,1);
	pImgCharTwo=cvCreateImage(cvSize(cROI[0].width, cROI[0].height),IPL_DEPTH_8U,1);
	pImgCharThree=cvCreateImage(cvSize(cROI[1].width, cROI[1].height),IPL_DEPTH_8U,1);
	pImgCharFour=cvCreateImage(cvSize(cROI[2].width, cROI[2].height),IPL_DEPTH_8U,1);
	pImgCharFive=cvCreateImage(cvSize(cROI[3].width, cROI[3].height),IPL_DEPTH_8U,1);
	pImgCharSix=cvCreateImage(cvSize(cROI[4].width, cROI[4].height),IPL_DEPTH_8U,1);
	pImgCharSeven=cvCreateImage(cvSize(cROI[5].width, cROI[5].height),IPL_DEPTH_8U,1);

	//用于确定字符区域
	CvRect ROI_rect1;


	//分割第一个字符
	 	ROI_rect1.x=cROI[6].x;
	 	ROI_rect1.y=cROI[6].y;
	 	ROI_rect1.width=cROI[6].width;
	 	ROI_rect1.height=cROI[6].height;
		cvSetImageROI(img_8uc1,ROI_rect1);
	 	cvCopy(img_8uc1,pImgCharOne,NULL); 
		cvResetImageROI(img_8uc1);


	//分割第二个字符
	 	ROI_rect1.x=cROI[0].x;
	 	ROI_rect1.y=cROI[0].y;
	 	ROI_rect1.width=cROI[0].width;
	 	ROI_rect1.height=cROI[0].height;
	 	cvSetImageROI(img_8uc1,ROI_rect1);
	 	cvCopy(img_8uc1,pImgCharTwo,NULL); 
	 	cvResetImageROI(img_8uc1);


	//分割第三个字符
	 	ROI_rect1.x=cROI[1].x;
	 	ROI_rect1.y=cROI[1].y;
		ROI_rect1.width=cROI[1].width;
	 	ROI_rect1.height=cROI[1].height;
	 	cvSetImageROI(img_8uc1,ROI_rect1);
	 	cvCopy(img_8uc1,pImgCharThree,NULL); 
	 	cvResetImageROI(img_8uc1);


	//分割第四个字符
	 	ROI_rect1.x=cROI[2].x;
	 	ROI_rect1.y=cROI[2].y;
	 	ROI_rect1.width=cROI[2].width;
	 	ROI_rect1.height=cROI[2].height;
	 	cvSetImageROI(img_8uc1,ROI_rect1);
	 	cvCopy(img_8uc1,pImgCharFour,NULL); 
	 	cvResetImageROI(img_8uc1);


	//分割第五个字符
		ROI_rect1.x=cROI[3].x;
	 	ROI_rect1.y=cROI[3].y;
	 	ROI_rect1.width=cROI[3].width;
	 	ROI_rect1.height=cROI[3].height;
	 	cvSetImageROI(img_8uc1,ROI_rect1);
	 	cvCopy(img_8uc1,pImgCharFive,NULL); 
	 	cvResetImageROI(img_8uc1);


	//分割第六个字符
	 	ROI_rect1.x=cROI[4].x;
	 	ROI_rect1.y=cROI[4].y;
	 	ROI_rect1.width=cROI[4].width;
	 	ROI_rect1.height=cROI[4].height;
	 	cvSetImageROI(img_8uc1,ROI_rect1);
	 	cvCopy(img_8uc1,pImgCharSix,NULL); 
	 	cvResetImageROI(img_8uc1);



	//分割第七个字符
	 	ROI_rect1.x=cROI[5].x;
	 	ROI_rect1.y=cROI[5].y;
	 	ROI_rect1.width=cROI[5].width;
	 	ROI_rect1.height=cROI[5].height;
	 	cvSetImageROI(img_8uc1,ROI_rect1);
	 	cvCopy(img_8uc1,pImgCharSeven,NULL); 
	 	cvResetImageROI(img_8uc1);


	/*----------------------对分割出的字符进行检测----------------------*/
	//检测第一个字符
	imageRecognition(pImgCharOne, 36, 67);

	//检测第二个字符
	imageRecognition(pImgCharTwo, 10, 35);

	//检测第三个字符
	imageRecognition(pImgCharThree, 0, 35);

	//检测第四个字符
	imageRecognition(pImgCharFour, 0, 35);

	//检测第五个字符
	imageRecognition(pImgCharFive, 0, 9);

	//检测第六个字符
	imageRecognition(pImgCharSix, 0, 9);

	//检测第七个字符
	imageRecognition(pImgCharSeven, 0, 9);

	//显示识别结果
	//SetDlgItemText(IDC_EDIT1, result);

	//使用完释放各种临时变量
	cvReleaseImage(&pImgCharOne);
	cvReleaseImage(&pImgCharTwo);
	cvReleaseImage(&pImgCharThree);
	cvReleaseImage(&pImgCharFour);
	cvReleaseImage(&pImgCharFive);
	cvReleaseImage(&pImgCharSix);
	cvReleaseImage(&pImgCharSeven);
	cvReleaseImage(&img_8uc1);
	cvReleaseImage(&roiThree_copy);
}