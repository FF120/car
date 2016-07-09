#include "StdAfx.h"
#include "CarFlowCount.h"
#include "cv.h" //  OpenCV 的基本函数头文件  
#include"highgui.h" //OpenCV 的图像显示函数头文件  
#include "CvvImage.h" 
#include "vector"
#include "CarPlatePosition.h"
#include <math.h>
#include<fstream>
#include<iostream>
#include<cmath>
#include "SetPreferencesDlg.h"
#include "opencvtestDlg.h"
#include <vector>
#include "resource.h"
using namespace cv;
using namespace std;

extern int nnPosNum;
extern int thickness_scale2;
//extern BOOL ONE_FLOW;
CarPlatePosition Cpp;
string dir_path;
Directory dir;
CString strTmp2;
char img_name[100];  

int flag=0;
/************************文件中的参数读取到DLG中的参数定义，所有全局变量*************************/
//ComBox内容
string inlaneText2;//车道字符串
string inareaText2;//车牌地区字符串
string insearchboxText2;//粗定位搜索框字符串

//CStrilBar内容
string insprpiexlText2;//像素差
string inzoomText2;//缩放倍数
string inthresholdText2;//阈值
string innocarpiexlText2;//无车像素值
string inplatethresholdText2;//车牌二值化阈值

//自定义路径内容
string inplatepathText2;//车牌路径
string inresultpathText2;//结果路径
string incutpicpathText2;//截图路径


//--------------------------------------变量定义----------------------------------------
float scale = 0.0;  //缩放倍数为0.5倍  
int scale_scale;
int threshold_num=0;//二值化时候的阈值
int diff=0;//相邻两帧之间的像素差值，
int noCar=0;//平均值低于这个值时，认为没有车辆通过
float height_scale=0.5;//控制检测线的位置
CvPoint lefttop;//检测区域的左上角
int cardoor=2;//车道数目
int s0=0,s1=0,s2=0 ;//用来记录平均像素相邻的两个值。
int s3=0,s4=0,s5=0;
int s6=0,s7=0,s8=0;
int s9=0,s10=0,s11=0;
//------------------------------------------------------------------------------------------
//CvCapture* capture = NULL;
//当前帧
IplImage* pFrImg = NULL;  
IplImage* pBkImg = NULL;
CvMat* pFrameMat = NULL;  
CvMat* pFrMat = NULL;  
CvMat* pBkMat = NULL;
int nFrmNum = 0;  //当前的帧数目
int vfps=0;
IplImage *frame; //当前帧
IplImage* big_frame;
CvCapture *capture ;
static int car_num=0;
IplImage* pPic;
IplImage* exPic;
IplImage* zonePic;//存放画线部分截取出来的图片
CarPlatePosition cpl;
//CarPlatePosition cpl;
extern CvCapture* pCapture;
//struct Hei_sta car[4]={{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0}};
//struct Hei_sta car_temp[4] = {{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0}};
		 
CarFlowCount::CarFlowCount(void)
{
	   
}

CarFlowCount::~CarFlowCount(void)
{
}

//初始化参数
void InitText()
{
	//文件中参数读入
	ifstream inf("./out.txt");
	inf>>inlaneText2>>inareaText2>>insearchboxText2>>insprpiexlText2>>inzoomText2>>inthresholdText2>>innocarpiexlText2>>inplatethresholdText2
		>>inplatepathText2>>inresultpathText2>>incutpicpathText2;

	cardoor = atoi(inlaneText2.c_str());
	diff=atoi(insprpiexlText2.c_str());
	scale_scale=atof(inzoomText2.c_str());
	scale = scale_scale/100.0;
	threshold_num=atoi(inthresholdText2.c_str());
	noCar=atoi(innocarpiexlText2.c_str());
}
//车流统计函数
void carDoor1(){
	CarFlowCount cfc;
	lefttop.x = 0;
	height_scale = pFrImg->height*(nnPosNum/100.0);
	lefttop.y = height_scale;
    int thickness=(int)(pFrImg->height*(thickness_scale2/400.0) );//细条粗细
	int sum = 0;//区域像素和
	int count = 0;//区域像素个数

	int avg = 0;//区域像素平均值
	
	uchar tmp=0;

	IplImage* img=cvCreateImage(cvSize(pFrImg->width,pFrImg->height),IPL_DEPTH_8U,1);
	uchar* data=(uchar *)pFrImg->imageData;
	int step = pFrImg->widthStep/sizeof(uchar);

	for(int i=lefttop.y;i<(lefttop.y+thickness);i++)
	{
		for(int j=0;j<pFrImg->width;j++)
		{ 
			tmp=data[i*step+j];
			sum = sum + tmp;
			count++;
		}
	}
	if(count!=0){	
		avg = sum /count;
	}

	s2 = avg;
	if(s2>diff){//可能有车经过
		if(s2-s1>diff&&s1<20&&s2>diff&&s0<noCar){
			strTmp2.Format(_T("车辆数目:  %d"),++car_num);
			IplImage* temPic;
			temPic = cvCreateImage(cvSize(big_frame->width, big_frame->height),big_frame->depth,big_frame->nChannels); 
			cvCopy(big_frame,temPic);
			pPic = cfc.cutRoi(temPic,cvPoint(0,lefttop.y),temPic->width,temPic->height*0.4);

			CopencvtestDlg cpd;
			cpd.searchDirectry();
			//获取进程当前路径
			//char strExePath[_MAX_PATH];
			//GetModuleFileName(NULL,strExePath,_MAX_PATH);
			//PathRemoveFileSpec(strExePath);

			//char subfilename[50]="\\cutPic";					
			//char* filename= strcat(strExePath,subfilename);
			//保存车牌图像到本地文件夹
			CreateDirectory("../Picture/", NULL);
			
			sprintf(img_name,"../Picture/%d.jpg",car_num); 
			cvSaveImage(img_name,pPic);

			///if (ONE_FLOW==TRUE)
			//{
				//Cpp.plateSegmentOne();
				cvNamedWindow("车辆截图",CV_WINDOW_AUTOSIZE);     
				cvShowImage("车辆截图", pPic);   
				cvReleaseImage(&temPic); 
			//} 

			//Cpp.plateSegmentOne();
		
		}
	}
	s0=s1;
	s1=s2;
}
void carDoor2(){
	CarFlowCount cfc;
	lefttop.x = 0;
	lefttop.y = (int)(pFrImg->height*height_scale);
	int thickness=(int)(pFrImg->height*0.02 );//细条粗细
	int sum = 0;//区域像素和
	int sum2=0;
	int count = 0;//区域像素个数
	int count2=0;
	int avg = 0;//区域像素平均值
	int avg2=0;
	uchar tmp=0;
	uchar tmp2 = 0;
	IplImage* img=cvCreateImage(cvSize(frame->width,frame->height),IPL_DEPTH_8U,1);
	uchar* data=(uchar *)pFrImg->imageData;
	int step = pFrImg->widthStep/sizeof(uchar);

	for(int i=lefttop.y;i<(lefttop.y+thickness);i++)
	{
		for(int j=0;j<frame->width/2;j++)
		{ 
			tmp=data[i*step+j];
			sum = sum + tmp;
			count++;
		}
	}
	if(count!=0){	
		avg = sum /count;
	}

	s2 = avg;
	if(s2>diff){//可能有车经过
		if(s2-s1>diff&&s1<20&&s2>diff&&s0<noCar){
			strTmp2.Format(_T("车辆数目:  %d"),++car_num);
			IplImage* temPic;
			temPic = cvCreateImage(cvSize(big_frame->width, big_frame->height),big_frame->depth,big_frame->nChannels); 
			cvCopy(big_frame,temPic);
			pPic = cfc.cutRoi(temPic,cvPoint(0,lefttop.y),temPic->width/2,temPic->height*0.4);


			 //dir_path = "../Picture/";
			//Directory dir;
			//CopencvtestDlg cpd;
			//cpd.searchDirectry();

			CreateDirectory("../Picture/", NULL);

			sprintf(img_name,"../Picture/%d.jpg",car_num); 
			cvSaveImage(img_name,pPic);


			//if (ONE_FLOW==TRUE)
		//	{
				//Cpp.plateSegmentOne();
				cvNamedWindow("左侧车道截图",CV_WINDOW_AUTOSIZE);     
				cvShowImage("左侧车道截图", pPic);   
				cvReleaseImage(&temPic); 

		}
	}
	s0=s1;
	s1=s2;




	for(int i=lefttop.y;i<lefttop.y+thickness;i++)
	{
		for(int j=frame->width/2;j<frame->width;j++)
		{
			tmp2=data[i*step+j];
			sum2 = sum2 + tmp2;
			count2++;
		}
	}
	if(count2!=0){avg2 = sum2/count2;}
	s4 = avg2;
	if(s4>diff){//可能有车经过
		if(s4-s3>diff&&s3<20&&s4>diff&&s5<noCar){
			strTmp2.Format(_T("车辆数目:  %d"),++car_num);
			IplImage* temPic;
			temPic = cvCreateImage(cvSize(big_frame->width, big_frame->height),big_frame->depth,big_frame->nChannels); 
			cvCopy(big_frame,temPic);
			pPic = cfc.cutRoi(temPic,cvPoint(temPic->width/2,lefttop.y),temPic->width/2,temPic->height*0.4);

			//CopencvtestDlg cpd;
			//cpd.searchDirectry();

			CreateDirectory("../Picture/", NULL);

			sprintf(img_name,"../Picture/%d.jpg",car_num); 
			cvSaveImage(img_name,pPic);
			////if (ONE_FLOW==TRUE)
			//{
				//Cpp.plateSegmentOne();
				cvNamedWindow("右侧车道截图",CV_WINDOW_AUTOSIZE);     
				cvShowImage("右侧车道截图", pPic);   
				cvReleaseImage(&temPic); 
		//	} 
		}
	}
	s5=s3;
	s3=s4;
}
void carDoor3(){
	CarFlowCount cfc;
	lefttop.x = 0;
	lefttop.y = (int)(pFrImg->height*height_scale);
	int thickness=(int)(pFrImg->height*0.02 );//细条粗细
	int sum = 0;//区域像素和
	int sum2=0;
	int sum3=0;
	int count = 0;//区域像素个数
	int count2=0;
	int count3=0;
	int avg = 0;//区域像素平均值
	int avg2=0;
	int avg3=0;
	uchar tmp=0;
	uchar tmp2 = 0;
	int tmp3 = 0;
	IplImage* img=cvCreateImage(cvSize(frame->width,frame->height),IPL_DEPTH_8U,1);
	uchar* data=(uchar *)pFrImg->imageData;
	int step = pFrImg->widthStep/sizeof(uchar);

	for(int i=lefttop.y;i<(lefttop.y+thickness);i++)
	{
		for(int j=0;j<frame->width/3;j++)
		{ 
			tmp=data[i*step+j];
			sum = sum + tmp;
			count++;
		}
	}
	if(count!=0){	
		avg = sum /count;
	}
	s2 = avg;
	if(s2>diff){//可能有车经过
		if(s2-s1>diff&&s1<20&&s2>diff&&s0<noCar){
			strTmp2.Format(_T("车辆数目:  %d"),++car_num);
			IplImage* temPic;
			temPic = cvCreateImage(cvSize(big_frame->width, big_frame->height),big_frame->depth,big_frame->nChannels); 
			cvCopy(big_frame,temPic);
			pPic = cfc.cutRoi(temPic,cvPoint(0,lefttop.y),temPic->width/3,temPic->height*0.4);

		//	CopencvtestDlg cpd;
		//	cpd.searchDirectry();

			CreateDirectory("../Picture/", NULL);

			sprintf(img_name,"../Picture/%d.jpg",car_num); 
			cvSaveImage(img_name,pPic);
			//Cpp.plateSegmentOne();
			//if (ONE_FLOW==TRUE)
			//{
				//Cpp.plateSegmentOne();
				cvNamedWindow("左侧车道截图",CV_WINDOW_AUTOSIZE);     
				cvShowImage("左侧车道截图", pPic);   
				cvReleaseImage(&temPic); 
			//} 
		}
	}
	s0=s1;
	s1=s2;




	for(int i=lefttop.y;i<lefttop.y+thickness;i++)
	{
		for(int j=frame->width/3;j<(2*frame->width)/3;j++)
		{
			tmp2=data[i*step+j];
			sum2 = sum2 + tmp2;
			count2++;
		}
	}
	if(count2!=0){avg2 = sum2/count2;}
	s4 = avg2;
	if(s4>diff){//可能有车经过
		if(s4-s3>diff&&s3<20&&s4>diff&&s5<noCar){
			strTmp2.Format(_T("车辆数目:  %d"),++car_num);
			IplImage* temPic;
			temPic = cvCreateImage(cvSize(big_frame->width, big_frame->height),big_frame->depth,big_frame->nChannels); 
			cvCopy(big_frame,temPic);
			pPic = cfc.cutRoi(temPic,cvPoint(temPic->width/3,lefttop.y),(temPic->width)/3,temPic->height*0.4);

		//	CopencvtestDlg cpd;
		//	cpd.searchDirectry();

			CreateDirectory("../Picture/", NULL);

			sprintf(img_name,"../Picture/%d.jpg",car_num); 
			cvSaveImage(img_name,pPic);

			//if (ONE_FLOW==TRUE)
			//{
				//Cpp.plateSegmentOne();
				cvNamedWindow("中间车道截图",CV_WINDOW_AUTOSIZE);     
				cvShowImage("中间车道截图", pPic);   
				cvReleaseImage(&temPic); 
		}
	}
	s5=s3;
	s3=s4;



	for(int i=lefttop.y;i<lefttop.y+thickness;i++)
	{
		for(int j=(2*frame->width)/3;j<frame->width;j++)
		{
			tmp3=data[i*step+j];
			sum3 = sum3 + tmp3;
			count3++;
		}
	}
	if(count3!=0){avg3 = sum3/count3;}
	s7 = avg3;
	if(s7>diff){//可能有车经过
		if(s7-s6>diff&&s6<20&&s7>diff&&s8<noCar){
			strTmp2.Format(_T("车辆数目:  %d"),++car_num);
			IplImage* temPic;
			temPic = cvCreateImage(cvSize(big_frame->width, big_frame->height),big_frame->depth,big_frame->nChannels); 
			cvCopy(big_frame,temPic);
			pPic = cfc.cutRoi(temPic,cvPoint((2*temPic->width)/3,lefttop.y),temPic->width/3,temPic->height*0.4);

		//	CopencvtestDlg cpd;
		//	cpd.searchDirectry();

			CreateDirectory("../Picture/", NULL);

			sprintf(img_name,"../Picture/%d.jpg",car_num); 
			cvSaveImage(img_name,pPic);


			//if (ONE_FLOW==TRUE)
			//{
				//Cpp.plateSegmentOne();
				cvNamedWindow("右侧车道截图",CV_WINDOW_AUTOSIZE);     
				cvShowImage("右侧车道截图", pPic);   
				cvReleaseImage(&temPic); 

		}
	}
	s8=s6;
	s6=s7;
}
void carDoor4(){
	CarFlowCount cfc;
	lefttop.x = 0;
	lefttop.y = (int)(pFrImg->height*height_scale);
	int thickness=(int)(pFrImg->height*0.02 );//细条粗细
	int sum = 0;//区域像素和
	int sum2=0;
	int sum3=0;
	int sum4=0;
	int count = 0;//区域像素个数
	int count2=0;
	int count3=0;
	int count4=0;
	int avg = 0;//区域像素平均值
	int avg2=0;
	int avg3=0;
	int avg4=0;
	uchar tmp=0;
	uchar tmp2 = 0;
	uchar tmp3 = 0;
	uchar tmp4 = 0;
	IplImage* img=cvCreateImage(cvSize(frame->width,frame->height),IPL_DEPTH_8U,1);
	uchar* data=(uchar *)pFrImg->imageData;
	int step = pFrImg->widthStep/sizeof(uchar);

	for(int i=lefttop.y;i<(lefttop.y+thickness);i++)
	{
		for(int j=0;j<frame->width/4;j++)
		{ 
			tmp=data[i*step+j];
			sum = sum + tmp;
			count++;
		}
	}
	if(count!=0){	
		avg = sum /count;
	}
	s2 = avg;
	if(s2>diff){//可能有车经过
		if(s2-s1>diff&&s1<20&&s2>diff&&s0<noCar){
			strTmp2.Format(_T("车辆数目:  %d"),++car_num);
			IplImage* temPic;
			temPic = cvCreateImage(cvSize(big_frame->width, big_frame->height),big_frame->depth,big_frame->nChannels); 
			cvCopy(big_frame,temPic);
			pPic = cfc.cutRoi(temPic,cvPoint(0,lefttop.y),temPic->width/4,temPic->height*0.4);

			CreateDirectory("../Picture/", NULL);

			sprintf(img_name,"../Picture/%d.jpg",car_num); 
			cvSaveImage(img_name,pPic);
			//Cpp.plateSegmentOne();
			//if (ONE_FLOW==TRUE)
		//	{
				//Cpp.plateSegmentOne();
				cvNamedWindow("左侧车道截图",CV_WINDOW_AUTOSIZE);     
				cvShowImage("左侧车道截图", pPic);   
				cvReleaseImage(&temPic); 
			//} 
		}
	}
	s0=s1;
	s1=s2;




	for(int i=lefttop.y;i<lefttop.y+thickness;i++)
	{
		for(int j=frame->width/4;j<(2*frame->width)/4;j++)
		{
			tmp2=data[i*step+j];
			sum2 = sum2 + tmp2;
			count2++;
		}
	}
	if(count2!=0){avg2 = sum2/count2;}
	s4 = avg2;
	if(s4>diff){//可能有车经过
		if(s4-s3>diff&&s3<20&&s4>diff&&s5<noCar){
			strTmp2.Format(_T("车辆数目:  %d"),++car_num);
			IplImage* temPic;
			temPic = cvCreateImage(cvSize(big_frame->width, big_frame->height),big_frame->depth,big_frame->nChannels); 
			cvCopy(big_frame,temPic);
			pPic = cfc.cutRoi(temPic,cvPoint(temPic->width/4,lefttop.y),(temPic->width)/4,temPic->height*0.4);


			CreateDirectory("../Picture/", NULL);

			sprintf(img_name,"../Picture/%d.jpg",car_num); 
			cvSaveImage(img_name,pPic);


			//if (ONE_FLOW==TRUE)
			//{
				//Cpp.plateSegmentOne();
				cvNamedWindow("左中车道截图",CV_WINDOW_AUTOSIZE);     
				cvShowImage("左中车道截图", pPic);   
				cvReleaseImage(&temPic); 
			//} 

		}
	}
	s5=s3;
	s3=s4;



	for(int i=lefttop.y;i<lefttop.y+thickness;i++)
	{
		for(int j=(2*frame->width)/4;j<(3*frame->width)/4;j++)
		{
			tmp3=data[i*step+j];
			sum3 = sum3 + tmp3;
			count3++;
		}
	}
	if(count3!=0){avg3 = sum3/count3;}
	s7 = avg3;
	if(s7>diff){//可能有车经过
		if(s7-s6>diff&&s6<20&&s7>diff&&s8<noCar){
			strTmp2.Format(_T("车辆数目:  %d"),++car_num);
			IplImage* temPic;
			temPic = cvCreateImage(cvSize(big_frame->width, big_frame->height),big_frame->depth,big_frame->nChannels); 
			cvCopy(big_frame,temPic);
			pPic = cfc.cutRoi(temPic,cvPoint((2*temPic->width)/4,lefttop.y),temPic->width/4,temPic->height*0.4);


			CreateDirectory("../Picture/", NULL);

			sprintf(img_name,"../Picture/%d.jpg",car_num); 
			cvSaveImage(img_name,pPic);

		//	if (ONE_FLOW==TRUE)
			//{
				//Cpp.plateSegmentOne();
				cvNamedWindow("右中车道截图",CV_WINDOW_AUTOSIZE);     
				cvShowImage("右中车道截图", pPic);   
				cvReleaseImage(&temPic); 
			//} 
		}
	}
	s8=s6;
	s6=s7;



	for(int i=lefttop.y;i<lefttop.y+thickness;i++)
	{
		for(int j=(2*frame->width)/4;j<(3*frame->width)/4;j++)
		{
			tmp4=data[i*step+j];
			sum4 = sum4 + tmp4;
			count4++;
		}
	}
	if(count4!=0){avg4 = sum4/count4;}
	s10 = avg4;
	if(s10>diff){//可能有车经过
		if(s10-s9>diff&&s9<20&&s10>diff&&s11<noCar){
			strTmp2.Format(_T("车辆数目:  %d"),++car_num);
			IplImage* temPic;
			temPic = cvCreateImage(cvSize(big_frame->width, big_frame->height),big_frame->depth,big_frame->nChannels); 
			cvCopy(big_frame,temPic);
			pPic = cfc.cutRoi(temPic,cvPoint((3*temPic->width)/4,lefttop.y),temPic->width/4,temPic->height*0.4);

			CreateDirectory("../Picture/", NULL);

			sprintf(img_name,"../Picture/%d.jpg",car_num); 
			cvSaveImage(img_name,pPic);


			//if (ONE_FLOW==TRUE)
		//	{
				//Cpp.plateSegmentOne();
				cvNamedWindow("右侧车道截图",CV_WINDOW_AUTOSIZE);     
				cvShowImage("右侧车道截图", pPic);   
				cvReleaseImage(&temPic); 
		//	} 
		}
	}
	s11=s9;
	s9=s10;
}
int CarFlowCount::countCar()
{
	//const char* path = (LPSTR)(LPCTSTR)m_path;
	// TODO: 在此添加控件通知处理程序代码
	//capture = cvCreateFileCapture (path);  //读取视频
	if(pCapture==NULL) {
		//printf("NO capture"); 
		AfxMessageBox("NO capture");//读取不成功，则标识
	};    
	double fps=cvGetCaptureProperty(pCapture, CV_CAP_PROP_FPS );   //cvGetCaptureProperty：获取视频文件的一些属性  其中第二个参数用户确定属性的类型
	vfps = 1000 / fps;    
	return vfps;//计算每帧播放的时间  1000ms = 1s                           	
}
// 在某个控件上显示图像
void CarFlowCount::ShowImage(IplImage *img, UINT ID)
{
	CDC *pDC=GetDlgItem(ID)->GetDC();
	HDC hDC=pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	SetRect(rect,rect.left,rect.top,rect.right,rect.bottom);
	CvvImage cimg;
	cimg.CopyOf(img);
	cimg.DrawToHDC(hDC,&rect);
	ReleaseDC(pDC);
}
//截图，截取指定坐标位置的矩形图片
IplImage* CarFlowCount::cutRoi(IplImage* img, CvPoint pt1, int width,int height)
{
	//定义一个和cvSize（长，宽）大小相同的矩形
	IplImage* src_photo_copy = cvCreateImage(cvSize(width,height),img->depth,img->nChannels);
	//按照条件画一个矩形	
	//cvRectangle(img,pt1,cvPoint(pt1.x+width,pt1.y+height),Scalar(0,0,255),1,8,0);
	CvRect roi = cvRect(pt1.x, pt1.y, width, height); //顶点坐标，矩形长、宽
	cvSetImageROI(img,roi);//在src中截取指定矩形大小的图像
	cvCopyImage(img,src_photo_copy);
	return src_photo_copy;
}
IplImage* CarFlowCount::cvCopyIplImage(IplImage*  srcImg){

	IplImage* dstImg = cvCreateImage(cvGetSize(srcImg),srcImg->depth,1);
    cvCopy(srcImg,dstImg);
	return dstImg;
}
//连通域检测 返回所有检测到的连通域结构vector
vector<Hei_sta> CarFlowCount::find_connected_domain(IplImage* src_image, IplImage* dst_image)  //连通域
{
	int row = src_image->height;
	int col = src_image->width;
	int nConnectNum=0;//连通域数目
	if (src_image->nChannels!=1)
	{
		//MessageBox(_T("不是二值图像！"));
	}

	//封住四周边界
	for (int i=0;i<col;i++)
	{
		((uchar*)(src_image->imageData+0*src_image->widthStep))[i]=0;  //上边界赋值0
		((uchar*)(src_image->imageData+(row-1)*src_image->widthStep))[i]=0;   //下边界赋值0
	}
	for (int i=0;i<row;i++)
	{
		((uchar*)(src_image->imageData+i*src_image->widthStep))[0]=0;   //左边界赋值0
		((uchar*)(src_image->imageData+i*src_image->widthStep))[col-1]=0;  //右边界赋值0
	}

    //浮点图像的建立
	for (int i = 0 ;i<row;i++)
	{
		for (int j = 0 ;j<col;j++)
		{
			((double*)(dst_image->imageData+i*dst_image->widthStep))[j]=((uchar*)(src_image->imageData+i*src_image->widthStep))[j];
			//将src_image按照每个像素一一赋值的方式复制到dst_image中去
		}
	}

	//连通域检测，每个点逐一检测
	int sign_con = 300;//此处可以任意赋值，但考虑到万一图像中连通域过多恰好有标记为255的点，因此设初值大于255
	//sign_con 检测到的连通域被赋值的像素数
	bool con_suc = false;
	for (int i=1;i<row-1;i++)//去掉最上面的一条线和最下面的一条线
	{
		for (int j=1;j<col-1;j++)//不计算最左面的一条线和最右面的一条线
		{
			if (((double*)(dst_image->imageData+i*dst_image->widthStep))[j]==255)
			{
				((double*)(dst_image->imageData+i*dst_image->widthStep))[j]=sign_con;
				con_suc = true;//con_suc 是否是连通域的标志，true 表示当前正在搜索连通域
				while(con_suc)
				{
					con_suc = false;
					for (int i_s=1;i_s<row-1;i_s++)
					{
						for (int j_s=1;j_s<col-1;j_s++)
						{
							if (((double*)(dst_image->imageData+i_s*dst_image->widthStep))[j_s]==255)
							{
								for (int m=-1;m<=1;m++)//若该像素为255，检查上下左右像素值是否为0或者255
								{

									bool flag_bre = false;
									for (int n=-1;n<=1;n++)
									{
										int temp1=((double*)(dst_image->imageData+(i_s+m)*dst_image->widthStep))[j_s + n];
										if ((temp1!=0)&&(temp1!=255))
										{
											((double*)(dst_image->imageData+i_s*dst_image->widthStep))[j_s] = sign_con;
											con_suc = true;
											flag_bre = true;
											break;
										}
									}
									if (flag_bre)
									{
										break;
									}
								}
							}
						}
					}
				}
				sign_con = sign_con + 1;
			}
		}
	}

	//连通域整理
	Hei_sta st_Connect;//连通域
	vector <Hei_sta>  vt_Connect;//智能指针
	vt_Connect.clear();
	IplImage* pImageTemp=cvCopyIplImage(dst_image);
	for (int i=0;i<row;i++)
	{
		for (int j=0;j<col;j++)
		{
			int tmp_value = ((double*)(pImageTemp->imageData+i*pImageTemp->widthStep))[j];//取出图像中的每个像素点进行
			//if (((double*)(dst_image->imageData+i*dst_image->widthStep))[j]>0)
			if(tmp_value>0)
			{
				int left_col = col-1;
				int right_col = 0;
				int nTop_row=row-1;
				int nBottom_row=0;
				for (int m=0;m<row;m++)
				{
					for (int n=0;n<col;n++)
					{
						if (((double*)(pImageTemp->imageData+m*pImageTemp->widthStep))[n]==tmp_value)
						{
							if (n<left_col)
								left_col = n;
							if (n>right_col)
								right_col = n;
							if(m<nTop_row)
								nTop_row=m;
							if(n>nBottom_row)
								nBottom_row=m;
                            ((double*)(pImageTemp->imageData+m*pImageTemp->widthStep))[n]=0;
						}
					}
				}
				st_Connect.width = right_col - left_col+1;  //计算连通域宽度
				st_Connect.height=nBottom_row-nTop_row+1;
				st_Connect.top=nTop_row;
				st_Connect.bottom=nBottom_row;
				st_Connect.left=left_col;
				st_Connect.right=right_col;
				st_Connect.sign=tmp_value;
				vt_Connect.push_back(st_Connect);

			}
		}
	}
	cvReleaseImage(&pImageTemp);
     return vt_Connect;
}
IplImage* CarFlowCount::count_aga()
{
	InitText();	
    big_frame = cvQueryFrame( pCapture );
	
 	if (big_frame==NULL)
 	{
 		flag = 1;
 	}
	else{
	CvSize dst_cvsize;   //目标图像尺寸   
	dst_cvsize.width = big_frame->width * scale;  //目标图像的宽为源图象宽的scale倍  
	dst_cvsize.height = big_frame->height * scale; //目标图像的高为源图象高的scale倍  
	frame = cvCreateImage( dst_cvsize, big_frame->depth,big_frame->nChannels); //构造目标图象  
	cvResize(big_frame, frame, CV_INTER_LINEAR); //缩放源图像到目标图像  

	nFrmNum++;//全局变量  帧数量
	if(nFrmNum == 1)  
	{  
		pBkImg = cvCreateImage(cvSize(frame->width, frame->height),IPL_DEPTH_8U,1);  
		pFrImg = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U,1);  
		pBkMat = cvCreateMat(frame->height, frame->width, CV_32FC1);  
		pFrMat = cvCreateMat(frame->height, frame->width, CV_32FC1);  
		pFrameMat = cvCreateMat(frame->height, frame->width, CV_32FC1);  //开辟矩阵空间
		//转化成单通道图像再处理  
		cvCvtColor(frame, pBkImg, CV_BGR2GRAY); //输入图像。输出图像。转成灰度图像 
		cvCvtColor(frame, pFrImg, CV_BGR2GRAY);  
		cvConvert(pFrImg, pFrameMat);  //
		cvConvert(pFrImg, pFrMat);  //图像类型转换成矩阵类型存放
		cvConvert(pFrImg, pBkMat);  
	}  
	else  
	{  
		//每一帧图像frame      都转换成灰度图像pFrImg
		cvCvtColor(frame, pFrImg, CV_BGR2GRAY);  
		//#define cvConvert( src, dst )  cvConvertScale( (src), (dst), 1, 0 )
		cvConvert(pFrImg, pFrameMat); //图像类型转换成矩阵类型存放 ****视频放完时会在此句出现异常
		//先做高斯滤波，以平滑图像  
		cvSmooth(pFrameMat, pFrameMat, CV_GAUSSIAN, 3, 0, 0);  
		//当前帧跟背景图相减  opencvtest.rc
		cvAbsDiff(pFrameMat, pBkMat, pFrMat);  
		//二值化前景图 
		cvThreshold(pFrMat, pFrImg, threshold_num, 255.0,CV_THRESH_BINARY);  //28这个值可以调节二值化后的图像质量
		//更新背景  
		cvRunningAvg(pFrameMat, pBkMat, 0.1, 0);  
		//将背景转化为图像格式，用以显示  
		cvConvert(pBkMat, pBkImg);  
		//从前景图中截取画线的区域，保存在新图像中，统计连通域的个数


		switch(cardoor){
		case 1:
			{
				carDoor1();
			}
			break;
		case  2:
			{
				carDoor2();

			}

			break;
		case 3:
			{
				carDoor3();
			}
			break;
		case 4:
			{
				carDoor4();
			}
			break;
			
		}
		return pPic;
	}	
	}
}