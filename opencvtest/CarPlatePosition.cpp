#include "StdAfx.h"
#include "CarPlatePosition.h"
#include "CarFlowCount.h"
#include "SetPreferencesDlg.h"
#include "cv.h"
#include "cvaux.h"
#include "CvvImage.h"
#include "cxcore.h"
#include "highgui.h"

using namespace cv;
using namespace std;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
struct VP m_Vp_Region;
//extern IplImage* ;
IplImage* m_img_VG;//用于保存纹理处理结果的图像
IplImage* m_img_blue;//用于HSV图像空间转换及其二值化
IplImage* cutPic_plate;
IplImage* vp_images_bin;
IplImage* m_img_vp_bluewhite;//原图像二值化的最佳效果
IplImage* roiThree;  //原图中裁剪的彩色车牌截图
IplImage* vp_images_sign_ref; //字符分割后的车牌图像
//用于车牌精确定位的参数
int ntop = 0;
int nOther = 0;
int m_iStart_col = 0;
int m_iEnd_col = 0;
int m_iStart_row = 0;
int m_iEnd_row = 0;

//用于RGB图像到HSV图像色彩空间的分离
IplImage* m_img_H;
IplImage* m_img_S;
IplImage* m_img_V;

/*******************车牌定位单独使用与 一键处理的开关**************************/
//extern BOOL ONE_PART;
extern IplImage* one_img;
/********外部全局变量，用于进行单独定位传入图像**********/
extern IplImage* srcPic;//用于 单独使用车牌定位的传入图像
extern IplImage* pPic;//用于一键处理的车牌定位的传入图像
//IplImage* srcPic_One;
//Mat image;//用于一键处理的Mat
CarFlowCount cfc;
SetPreferencesDlg spd;

/***************文件中的参数读取到DLG中的参数定义，所有全局变量为具体的类型************************/
//ComBox内容
extern int inlaneInt;//车道字符串
extern string inareaString;//车牌地区字符串
extern string insearchboxString;//粗定位搜索框字符串

//CStrilBar内容
extern int insprpiexlInt;//像素差
extern double inzoomFloat;//缩放倍数
extern int inthresholdInt;//阈值
extern int innocarpiexlInt;//无车像素值
extern int inplatethresholdInt;//车牌二值化阈值

//自定义路径内容
extern string inplatepathString;//车牌路径
extern string inresultpathString;//结果路径
extern string incutpicpathString;//截图路径

CarPlatePosition::CarPlatePosition(void)
{
}

CarPlatePosition::~CarPlatePosition(void)
{
}

/*************工具函数******************/
//截取单通道图像
IplImage* CarPlatePosition::cut_Roi_single(IplImage* img, CvPoint pt1, int width,int height)
{
	//定义一个和cvSize（长，宽）大小相同的矩形
	IplImage* src_photo_copy = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
	//按照条件画一个矩形	
	//cvRectangle(img,pt1,cvPoint(pt1.x+width,pt1.y+height),Scalar(0,0,255),1,8,0);
	CvRect roi =cvRect(pt1.x, pt1.y, width, height); //顶点坐标，矩形长、宽
	cvSetImageROI(img,roi);//在src中截取指定矩形大小的图像
	cvCopy(img,src_photo_copy);
	cvResetImageROI(img);
	return src_photo_copy;
}
//截取三通道图像
IplImage* CarPlatePosition::cut_Roi_three(IplImage* img, CvPoint pt1, int width,int height)
{
	//定义一个和cvSize（长，宽）大小相同的矩形
	IplImage* src_photo_copy = cvCreateImage(cvSize(width,height),img->depth,img->nChannels);
	//按照条件画一个矩形	
	//cvRectangle(img,pt1,cvPoint(pt1.x+width,pt1.y+height),Scalar(0,0,255),1,8,0);
	CvRect roi =cvRect(pt1.x, pt1.y, width, height); //顶点坐标，矩形长、宽
	cvSetImageROI(img,roi);//在src中截取指定矩形大小的图像
	cvCopy(img,src_photo_copy);
	cvResetImageROI(img);
	return src_photo_copy;
}
//复制图像
IplImage* CarPlatePosition::CopyIplImage(IplImage*  srcImg)
{

	IplImage* dstImg = cvCreateImage(cvGetSize(srcImg),srcImg->depth,srcImg->nChannels);
	cvCopy(srcImg,dstImg);
	return dstImg;
}
//复制图像指定区域
IplImage* CarPlatePosition::CopyIplImageROI(IplImage* m_img_src,CvRect selection)
{
	cvSetImageROI(m_img_src, selection);
	IplImage* img_copy = cvCreateImage(cvGetSize(m_img_src),
		m_img_src->depth,
		m_img_src->nChannels);
	cvCopy(m_img_src, img_copy, NULL);
	cvResetImageROI(m_img_src);
	return img_copy;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
函数工作过程：
传入参数： 源图像的src副本
传出图像：全局图像m_img_VG
返回值：是m_img_VG的副本，暂时觉得没用，回头看
过程：对src副本进行基于纹理的二值化操作，得到纹理二值化图像m_img_VG;
*/
IplImage* CarPlatePosition::texture(IplImage *img_src)    //求纹理特征-纹理特征提取
{
	IplImage* img_gray=cvCreateImage(cvGetSize(img_src),IPL_DEPTH_8U,1);	
	IplImage* p_img_VG=cvCreateImage(cvGetSize(img_gray),IPL_DEPTH_8U,1);
	cvCvtColor(img_src,img_gray,CV_BGR2GRAY);//彩色图像转换为灰度图像
	int row=img_gray->height;
	int col=img_gray->width;
	int m,n;
	float ave_VG=0;
	float sum=0;
	int height=p_img_VG->height;
	int width=p_img_VG->width;

	int step= img_gray->widthStep/sizeof(uchar);   //sizeof(uchar)是1
	uchar* data=(uchar*)img_gray->imageData;

	int step_VG = p_img_VG->widthStep/sizeof(uchar);
	uchar* data_VG=(uchar*)p_img_VG->imageData;

	for(int i=1;i<row-1;i++)   //
		for(int j=1;j<col-1;j++)
		{
			int a,b,c;
			a=((uchar*)(img_gray->imageData+i*img_gray->widthStep))[j];
			b=((uchar*)(img_gray->imageData+i*img_gray->widthStep))[j-1];
			c=((uchar*)(img_gray->imageData+i*img_gray->widthStep))[j+1];
			m=abs(a-b);  //abs:求绝对值
			n=abs(a-c);

			((uchar*)(p_img_VG->imageData+i*p_img_VG->widthStep))[j]= m + n;
		}

		for(int i1=1;i1<row-1;i1++)
			for(int j1=1;j1<col-1;j1++)
				sum=sum+data_VG[i1*step_VG+j1]; //像素和，为求平均值

		ave_VG =2*sum/(row*col);
		for(int i=1;i<row-1;i++)
			for(int j=1;j<col-1;j++)
			{
				if(data_VG[i*step_VG+j]>ave_VG)
					data_VG[i*step_VG+j]=255;
				else
					data_VG[i*step_VG+j]=0;
			}
			m_img_VG=p_img_VG;
			cvSmooth(m_img_VG,m_img_VG,CV_MEDIAN,3);
			cvReleaseImage(&img_gray);
			return p_img_VG;
}

//颜色转换  第一步
//单独测试通过........  感觉颜色转换做的略复杂了   目的为了得到HSV图像的三个通道，H,S,V；
/*
传入图像：src副本
传出图像：全局图像m_img_blue
返回值：无
过程：对src副本进行基于HSV车牌的二值化操作，得到二值化图像m_img_blue
*/
void CarPlatePosition::Color_Trans(IplImage *img_src) //RGB空间转换为HSV（色相H,饱和度S,透明度V）
{
	int row = img_src->height;
	int col = img_src->width;

	m_img_H = cvCreateImage(cvSize(img_src->width,img_src->height),IPL_DEPTH_64F,1);
	m_img_S = cvCreateImage(cvSize(img_src->width,img_src->height),IPL_DEPTH_64F,1);
	m_img_V = cvCreateImage(cvSize(img_src->width,img_src->height),IPL_DEPTH_64F,1);
	m_img_blue=cvCreateImage(cvSize(img_src->width,img_src->height),IPL_DEPTH_8U,1);
	if(img_src->nChannels!=3)
	{
		//MessageBoxA("您选择的不是彩色图像，请选择彩色图像!","");
		exit(1); //程序结束时返回1
	}
	//建立原始图像副本, 程序结束释放
	IplImage* img_src1=cvCreateImage(cvSize(img_src->width,img_src->height),IPL_DEPTH_64F,3);
	int step= img_src->widthStep/sizeof(uchar);
	int step1=img_src1->widthStep/sizeof(double);
	uchar*  data=(uchar*)img_src->imageData;
	double* dataimage=(double*)img_src1->imageData;

	for(int i=0;i<row;i++)//将原图像的值赋值给新建的图像，赋值懂，为什么不用cvCopy？
	{
		for(int j=0;j<col;j++)
		{
			dataimage[i*step1+j*3+0]=data[i*step+j*3+0];
			dataimage[i*step1+j*3+1]=data[i*step+j*3+1];
			dataimage[i*step1+j*3+2]=data[i*step+j*3+2];
		}
	}
	for(int i=0;i<row;i++)
	{
		for(int j=0;j<col;j++)
		{
			double x,y,t,t1;

			y=std::max((double)dataimage[i*step1+j*3+0],(double)dataimage[i*step1+j*3+1]);
			y=std::max((double)y,(double)dataimage[i*step1+j*3+2]);
			x=std::min((double)dataimage[i*step1+j*3+0],(double)dataimage[i*step1+j*3+1]);
			x=std::min((double)x,(double)dataimage[i*step1+j*3+2]);
			((double*)(m_img_V->imageData+i*m_img_V->widthStep))[j]=y/255;
			if(y==0)
				((double*)(m_img_S->imageData+i*m_img_S->widthStep))[j] = 0;
			else
				((double*)(m_img_S->imageData+i*m_img_S->widthStep))[j] = (y-x)/(0.0001+y);//此处避免可能内存读取失败导致y=0的情况
			t=y-x;

			if(t!=0)
			{
				if((((double*)(m_img_S->imageData+i*m_img_S->widthStep))[j]!=0)&&(y == dataimage[i*step1+j*3+0]))
					//double *p = (double*)(S->imageData+i*S->widthStep);
					// if((p[j]!=0)&&(y==dataimage[i*step+j*3+0]))
				{
					t1=dataimage[i*step1+j*3+2]-dataimage[i*step1+j*3+1];
					((double*)(m_img_H->imageData+i*m_img_H->widthStep))[j] = 60*(4+t1/t);
				}
				else if((((double*)(m_img_S->imageData+i*m_img_S->widthStep))[j]!=0)&&(y==dataimage[i*step1+j*3+1]))
				{
					t1=dataimage[i*step1+j*3+0]-dataimage[i*step1+j*3+2];
					((double*)(m_img_H->imageData+i*m_img_H->widthStep))[j] = 60*(2+t1/t);

				}
				else if((((double*)(m_img_S->imageData+i*m_img_S->widthStep))[j]!=0)&&(y==dataimage[i*step1+j*3+2]))
				{
					t1=dataimage[i*step1+j*3+1]-dataimage[i*step1+j*3+0];
					((double*)(m_img_H->imageData+i*m_img_H->widthStep))[j] = 60*(t1/t);
				}
				else
					((double*)(m_img_H->imageData+i*m_img_H->widthStep))[j] = 0;
			}
		}
	}

	for(int i=0;i<row;i++)
		for(int j=0;j<col;j++)
		{
			//m_img_H、m_img_S、m_img_V：都是全局变量，经过色彩空间转换后得到的单通道图像
			double value_H = ((double*)(m_img_H->imageData+i*m_img_H->widthStep))[j];
			double value_S = ((double*)(m_img_S->imageData+i*m_img_S->widthStep))[j];
			double value_V = ((double*)(m_img_V->imageData+i*m_img_V->widthStep))[j];

			/*
			            对HSV空间的图像，按照H通道的颜色进行二值化处理
			*/
			if((value_S>0.1)&&(value_H>200)&&(value_H<250))  //蓝色          针对蓝白车牌的特点定义的参数 0.1  200   250
				((uchar*)(m_img_blue->imageData+i*m_img_blue->widthStep))[j] = 255; //
			else
				((uchar*)(m_img_blue->imageData+i*m_img_blue->widthStep))[j] = 0;
		}
	cvSmooth(m_img_blue,m_img_blue,CV_MEDIAN,3);
//	DrawPicToHDC(m_img_blue,IDC_HSVERZHIHUA);
	cvReleaseImage(&img_src1);
}

//本函数是vp_detect（）函数的子函数，可以不用直接测试，间接测试即可，要找清楚各个函数之间的接口与调用关系，理清局部变量和全局变量等关系
//封装的对一些对图像特征提取的操作，可以认为是vp_detect（）的一部分，所以传参部分都在vp_detect（）就行了；蓝色拍照部分与纹理化的图像记性与运算
//特征图像提取    第三步
/*
传入图像：全局二值图像：m_img_VG   全局二值图像：m_img_blue
传出图像：全局二值图像：m_img_vp_bluewhite
返回值：无
过程：对传入的两幅图像对应位置像素值进行相与操作，得到最终二值化的结果m_img_vp_bluewhite
*/

void CarPlatePosition::vp_blue_white( int istart_row, int istart_col, int iend_row, int iend_col) //若为蓝底白字车牌，进行特征提取
{
	//int m_iCount_bluewhite=0;
	int height = iend_row-istart_row+1;
	int width  = iend_col-istart_col+1;
	IplImage* m_img_b=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
	IplImage* m_img_wl=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);

	cvCopy(m_img_blue,m_img_b);
	cvCopy(m_img_VG,m_img_wl);
	m_img_vp_bluewhite=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);//8位单通道图像
	//	IplImage* m_img_white=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);//创建这样的数据结构，里面没有数据，都是空的
	//	IplImage* m_img_blue=cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);

	int step_vp= m_img_vp_bluewhite->widthStep;
	//int step_w = m_img_white->widthStep;  //白色区域
	//int step_b = m_img_blue->widthStep;   //蓝色区域                  图像一行像素的字节数

	uchar* data_vp=(uchar*)m_img_vp_bluewhite->imageData;
	//uchar* data_w=(uchar*)m_img_white->imageData;
	//uchar* data_b=(uchar*)m_img_blue->imageData;

	for(int i=istart_row;i<=iend_row;i++)
		for(int j=istart_col;j< iend_col;j++)
		{
			if((((uchar*)(m_img_b->imageData+i*m_img_b->widthStep))[j]==255)&&(((uchar*)(m_img_wl->imageData+i*m_img_wl->widthStep))[j]==255))  //将纹理图像与蓝色部分相与，去除噪声，提取纹理特征；在此处操作的时候，函数已经执行完色彩空间转换等操作了
			{
				data_vp[i*step_vp+j]=255;
				//	m_iCount_bluewhite++;
			}
			else
				data_vp[i*step_vp+j]=0;//理论上来说运算结果都是下面的，都是0才对；虽然修改的是data_vp，但是m_img_vp_bluewhite指定的值也会对应改变，因为指向的是同一个地址；
		}
}
//投影函数（vp_dect函数的子函数，具体传入传出参数在函数vp_detect中定义）
void CarPlatePosition::projection(IplImage *img, int istart_row, int iend_row, int istart_col, int iend_col, char s) //按行和列投影，重新估计上下以及左右边缘
{
	IplImage *img1=cvCreateImage(cvGetSize(img),IPL_DEPTH_64F,1);
	int row=img->height;
	int col=img->width;
	//逐个像素点拷贝
	for(int i=0;i<row;i++)
		for(int j=0;j<col;j++)
		{
			((double*)(img1->imageData+i*img1->widthStep))[j]=((uchar*)(img->imageData+i*img->widthStep))[j];
		}

		//cvCopy(img,img1);

		if(s=='h')  //垂直方向
		{
			vector<double> y(row,0.000000);
			//水平方向投影
			for(int i=0;i<row;i++)
			{
				for(int j=istart_col;j<=iend_col;j++)
				{
					if (((double*)(img1->imageData+i*img1->widthStep))[j]==255)
					{
						y.at(i)=y.at(i)+1;
					}
				}
			}

			int tt_limit=iend_row-istart_row+1;
			int t_limit = 0;
			tt_limit=cvRound(tt_limit/3);  //三分之一的高度；cvRound对一个double型的数进行四舍五入,并返回一个整型数
			//上边缘向上搜索，上边缘先向上搜索，如果上面搜索到，就不往下搜索了，避免重复计算或者矛盾

			bool bret1 = false;//单向标志，向上搜索到点就不继续往下搜索了
			if((istart_row-tt_limit)<=0)
				t_limit = 0;
			else
				t_limit = istart_row - tt_limit; //向上三分之一位置处
			int nNum=0;
			for(int i=istart_row;i>=t_limit;i--)
			{
				if(y.at(i)>ntop)  
				{
					istart_row = i;
					bret1  = true;
				}
			}
			m_iStart_row = istart_row;
			if(bret1==false)
				//上边缘向下搜索
			{
				if((istart_row+tt_limit)>row-1)//上边界处理，避免车牌图像位于第一行
					t_limit = row-1;
				else
					t_limit = istart_row+tt_limit;

				for(int i=istart_row;i<=t_limit;i++)
				{
					if(y.at(i)>nOther)  
						istart_row = i;

				}
				m_iStart_row = istart_row;//重置上边缘
			}
			// 下边缘向下搜索

			bool bret2 = false;//单向标志，向上搜索到点就不继续向下搜索了
			if((iend_row+tt_limit)>row-1)
				t_limit = row-1;
			else
				t_limit = iend_row + tt_limit;
			for(int i=iend_row;i<=t_limit;i++)
			{
				if(y.at(i)>nOther)
				{
					iend_row = i;
					bret2 = true;
				}
			}
			m_iEnd_row = iend_row;

			//下边缘向上搜索
			if(bret2==false)
			{
				if((iend_row - tt_limit)<0)
					t_limit = 0;
				else
					t_limit = iend_row - tt_limit;

				for(int i = iend_row;i>=t_limit;i--)
				{
					if(y.at(i)>nOther)
						iend_row = i;
				}
				m_iEnd_row = iend_row;
			}
		}
		if(s=='v')
		{
			vector<double>y(col,0.000000);
			for(int j=0;j<col;j++)
			{
				for(int i=istart_row;i<=iend_row;i++)
				{
					if (((double*)(img1->imageData+i*img1->widthStep))[j]==255) 
					{
						y.at(j)=y.at(j)+1;//显然就是0或者1，二值化的节奏啊
					}
				}
			}

			int tt_limit = iend_col - istart_col + 1;
			int t_limit = 0;
			tt_limit = cvRound(tt_limit/10);

			//左边缘向左搜索
			bool bret1=false;
			if((istart_col-tt_limit)<0)
				t_limit = 0;
			else
				t_limit = istart_col-tt_limit;

			for(int i=istart_col;i>=t_limit;i--)
			{
				if(y.at(i)>nOther)
				{
					istart_col = i;
					bret1 = true;
				}
			}
			m_iStart_col = istart_col;

			if(bret1==false)
				//左边缘向右搜索
			{
				if((istart_col+tt_limit)>col-1)
					t_limit = col-1;
				else
					t_limit = istart_col + tt_limit;

				for(int i = istart_col;i<=t_limit;i++)
				{
					if(y.at(i)>nOther)
					{
						istart_col = i;
					}
				}
				m_iStart_col = istart_col;
			}

			//右边缘向右搜索
			bool bret2 = false;
			if((iend_col+tt_limit)>col-1)
				t_limit = col-1;
			else
				t_limit = iend_col + tt_limit;

			for(int i=iend_col;i<=t_limit;i++)
			{
				if(y.at(i)>nOther)
				{
					iend_col = i;
					bret2  = true;
				}
			}
			m_iEnd_col = iend_col;

			//右边缘向左搜索
			if(bret2==false)
			{
				if((iend_col-tt_limit)<0)
					t_limit=0;
				else
					t_limit=iend_col-tt_limit;
				int t=0;
				for(int i=iend_col;i>=t_limit;i--)
				{
					if(y.at(i)>nOther)
						iend_col = i;
				}
				m_iEnd_col = iend_col;
			}

		}
		cvReleaseImage(&img1);		
}

//定位处理函数，返回定位粗结果 
//主要用于返回定位结果....
VP CarPlatePosition::vp_detect(int W_Width, int W_Height)
{
	VP m_PlateResult;
	//	int vp_class = 0;
	VP m_Vp_Region_bluewhite;

	int row=m_img_VG->height;//row是整个图像的高度，而不是车牌部分车牌的高度
	int col=m_img_VG->width;

	m_Vp_Region_bluewhite.initiapos_row = 0;
	m_Vp_Region_bluewhite.initiapos_col = 0;
	m_Vp_Region_bluewhite.end_row = 0;
	m_Vp_Region_bluewhite.end_col = 0;
	m_Vp_Region_bluewhite.count = 0;

	for(int i=0;i<(row - W_Height);i+=4)
		for(int j=0;j<(col - W_Width);j+=8)
		{
			int count1 = 0;//蓝色图搜索区域内像素总数
			for(int m=0;m<=W_Height;m++)
			{
				for(int n=0;n<=W_Width;n++)
				{
					if(((uchar*)(m_img_vp_bluewhite->imageData+(i+m)*m_img_vp_bluewhite->widthStep))[j+n]==255)
						count1=count1+1;
				}
			}
			//确定了待粗糙截图区域的图像尺寸
			if(count1>m_Vp_Region_bluewhite.count)//保存最大的区域，此处待修改，因为实验中彻底找错的是因为该处，如果保存点数最多的四处或者五处再筛选，效果会好很多，但速度会变慢
			{
				m_Vp_Region_bluewhite.count = count1;
				m_Vp_Region_bluewhite.initiapos_row = i;
				m_Vp_Region_bluewhite.initiapos_col = j;
				m_Vp_Region_bluewhite.end_row = i+W_Height;
				m_Vp_Region_bluewhite.end_col = j+W_Width;
			}
		}
		//搜索最大区域
		m_Vp_Region.initiapos_row = m_Vp_Region_bluewhite.initiapos_row;
		m_Vp_Region.initiapos_col = m_Vp_Region_bluewhite.initiapos_col;
		m_Vp_Region.end_row = m_Vp_Region_bluewhite.end_row;
		m_Vp_Region.end_col = m_Vp_Region_bluewhite.end_col;
		m_Vp_Region.count = m_Vp_Region_bluewhite.count;

		//定义全局二值图像m_img_vp_bluewhite的副本pic
		IplImage* m_pic = cvCreateImage(cvGetSize(m_img_vp_bluewhite),IPL_DEPTH_8U,1);
		cvCopy(m_img_vp_bluewhite,m_pic);

		//////////////////////////////////////////////以下是精确定位部分////////////////////////////////////////////
		int start_row_ref = 0;
		int start_col_ref = 0;
		int end_row_ref = 0;
		int end_col_ref = 0;
		bool bret = true;
		int intertimes = 30;//限定最大迭代次数为30次，避免陷入死循环
		while((bret)&&(intertimes>0))
		{
			intertimes--;
			intertimes = intertimes - 1 ;
			start_row_ref = m_Vp_Region.initiapos_row ;
			end_row_ref = m_Vp_Region.end_row ;
			start_col_ref = m_Vp_Region.initiapos_col ;
			end_col_ref = m_Vp_Region.end_col ;

			projection(m_pic,m_Vp_Region.initiapos_row,m_Vp_Region.end_row,m_Vp_Region.initiapos_col,m_Vp_Region.end_col,'v');//按列投影，重新估计两个垂直边缘
			m_Vp_Region.initiapos_col = m_iStart_col;
			m_Vp_Region.end_col = m_iEnd_col;

			projection(m_pic,m_Vp_Region.initiapos_row,m_Vp_Region.end_row,m_Vp_Region.initiapos_col,m_Vp_Region.end_col,'h');//按行投影，重新估计两个水平边缘，此时使用的垂直边缘为更新后的边缘
			m_Vp_Region.initiapos_row = m_iStart_row;
			m_Vp_Region.end_row = m_iEnd_row;

			bret =((m_Vp_Region.initiapos_row!=start_row_ref)||(m_Vp_Region.end_row!=end_row_ref)||(m_Vp_Region.initiapos_col!=start_col_ref)||(m_Vp_Region.end_col!=end_col_ref));//判断投影前后边缘是否变化，如果变化继续循环，否则终止
		}

		CvPoint p1,p2,p3,p4; //定位精细结果
		p1.x=m_Vp_Region.initiapos_col;
		p1.y=m_Vp_Region.initiapos_row;
		p2.x=m_Vp_Region.end_col;
		p2.y=m_Vp_Region.initiapos_row;
		p3.x=m_Vp_Region.end_col;
		p3.y=m_Vp_Region.end_row;
		p4.x=m_Vp_Region.initiapos_col;
		p4.y=m_Vp_Region.end_row;    

		//一次性进行处理   这块得处理，不能直接把原图像就放这了，应该用个switch开关
		//if (ONE_PART==TRUE)
		//{
			cvRectangle(srcPic,p1,p3,Scalar(0,0,255),1,8,0);  //srcPic没有值
			roiThree = cut_Roi_three(srcPic,p1,p3.x-p1.x,p3.y-p1.y);
		//}
		//else
		//{
			//IplImage* src = cvCreateImage(cvGetSize(one_img),8,3);
		//	cvCopy(one_img,src);
		//	Color_Trans(src);
			//此处放置一键处理的车牌定位相关代码
		//	cvRectangle(one_img,p1,p3,Scalar(0,0,255),1,8,0);
		//	roiThree = cut_Roi_three(one_img,p1,p3.x-p1.x,p3.y-p1.y);
		//}

		m_PlateResult=m_Vp_Region;
		return m_PlateResult;
}
VP CarPlatePosition::plate_segment(IplImage *src_img) //定位函数
{
	int width_big;
	int height_big;
	int width_small;
	int height_small;
	spd.InitText();
	if (insearchboxString=="40*10")
	{
		width_small =40;
		height_small=10;
		width_big=52;
		height_big=13;
		
	} 
	else if(insearchboxString=="60*15")
	{
		width_small =60;
		height_small=15;
		width_big=77;
		height_big=19;
	}
	else if(insearchboxString=="80*20")
	{
		width_small =80;
		height_small=20;
		width_big=102;
		height_big=26;
	}
	else if(insearchboxString=="100*25")
	{
		width_small =100;
		height_small=25;
		width_big=128;
		height_big=32;
	}
	else if(insearchboxString=="120*30")
	{
		width_small =120;
		height_small=30;
		width_big=154;
		height_big=38;
	}
	else if(insearchboxString=="140*35")
	{
		width_small =140;
		height_small=35;
		width_big=180;
		height_big=45;
	}

	m_Vp_Region.initiapos_row = 0;
	m_Vp_Region.initiapos_col = 0;
	m_Vp_Region.count = 0;
	//矩形搜索框大小
	int W_Width;
	int W_Height;
	if (src_img->height<600)
	{
		W_Width = width_small;
		W_Height = height_small;
		ntop=2;
		nOther=2;
	}
	else
	{
		W_Width =width_big;
		W_Height = height_big;
		ntop=30;
		nOther=5;
	}
	return vp_detect(W_Width,W_Height);   
}

//连通域筛选
bool CarPlatePosition::pre_check(vector <Hei_sta> vt_Connect,IplImage* src_image, IplImage* image_sign, IplImage* dst_image, IplImage* image_sign_ref)
{
	//
	//src_image为二值图像，image_sign为连通域图像
	//本函数通过定位车牌中的字符实现对车牌的反定位
	//1、去除小于平均宽度大于1/7的连通域
	//1、统计通过中心的每个连通域的高度
	//2、去除小于平均高度1/3的连通域

	if (src_image->nChannels!=1)
		exit(1);

	//行列初始化
	int row = src_image->height;
	int col = src_image->width;
	int a=0;
	///////////////////// /*去除高度大于1/7的连通域*/
	for(vector<Hei_sta>::iterator iter=vt_Connect.begin(); iter!=vt_Connect.end(); )
	{
		if( iter->width>col/7.0||iter->height<row/3)
		{
			for (int m=0;m<row;m++)
			{
				for (int n=0;n<col;n++)
				{
					if (((double*)(image_sign->imageData+m*image_sign->widthStep))[n]==iter->sign)
						((double*)(image_sign->imageData+m*image_sign->widthStep))[n]=0;
				}
			}
			iter = vt_Connect.erase(iter);
		}
		else
			iter ++ ;
	}	
	int stastic_count = 7;
	int height_thre = 5;
	int max_top_bot_thre = 30;
	///////////////////// /*去除未通过重心的连通域*/
	int row_center = 0;
	int col_center = 0;
	int pos_count = 0;

	for (int i=0;i<row;i++)
	{
		for (int j=0;j<col;j++)
		{
			if (((double*)(image_sign->imageData+i*image_sign->widthStep))[j]>0)
			{
				row_center = row_center + i;
				col_center = col_center + j;
				pos_count = pos_count + 1;
			}
		}
	}

	if ((row_center==0)||(pos_count==0))
	{
		dst_image = NULL;
		image_sign_ref = NULL;
		return false;
	}
	row_center = row_center/pos_count;  //行的中心
	col_center = col_center/pos_count;  //列的中心
	//将图像置0
	cvZero(dst_image);
	cvZero(image_sign_ref);
	for (int j=0;j<col;j++)
	{
		if (((double*)(image_sign->imageData+row_center*image_sign->widthStep))[j]>0)
		{
			int tmp_sign = ((double*)(image_sign->imageData+row_center*image_sign->widthStep))[j];  //中心线上的像素
			for (int m = 0;m<row;m++)
			{
				for (int n=0;n<col;n++)
				{
					if (((double*)(image_sign->imageData+m*image_sign->widthStep))[n]==tmp_sign)//搜索过中心点的连通域
					{
						((uchar*)(dst_image->imageData+m*dst_image->widthStep))[n]=((uchar*)(src_image->imageData+m*src_image->widthStep))[n];
						((double*)(image_sign_ref->imageData+m*image_sign_ref->widthStep))[n]=((double*)(image_sign->imageData+m*image_sign->widthStep))[n];
					}
				}
			}
		}
	}
	return true;
}

//字符分割函数
//此处的传入参数是车牌图像  车牌的彩色图像 roiThress图像
void  CarPlatePosition::PlateCharSegment(IplImage *pImagePlate)
{
	IplImage* m_img_plate = cvCreateImage(cvGetSize(pImagePlate),pImagePlate->depth,pImagePlate->nChannels);
	int thresthold=inplatethresholdInt*2.5;//从文件读入车牌二值化的阈值
	//得到矩形区域：m_Vp_Region
	if (pImagePlate->width<500)//车牌图像
		m_img_plate=CopyIplImage(pImagePlate);
	else
	{
		CvPoint p1,p2,p3,p4;
		p1.x=m_Vp_Region.initiapos_col;
		p1.y=m_Vp_Region.initiapos_row;
		p2.x=m_Vp_Region.end_col;
		p2.y=m_Vp_Region.initiapos_row;
		p3.x=m_Vp_Region.end_col;
		p3.y=m_Vp_Region.end_row;
		p4.x=m_Vp_Region.initiapos_col;
		p4.y=m_Vp_Region.end_row;
		int vp_height = p4.y-p1.y;
		int vp_width = p3.x-p4.x;	
		CvRect selection;
		selection.x = p1.x;
		selection.y = p1.y;
		selection.width = p2.x-p1.x;
		selection.height = p4.y-p1.y;
		//m_img_plate= CopyIplImageROI(src,selection);	//此处的m_img_src应该为最原始的图像图像
		//if (ONE_PART==TRUE)
		//{
			m_img_plate= CopyIplImageROI(srcPic,selection);
		//} 
// 		else
// 		{
// 			IplImage* src = cvCreateImage(cvGetSize(one_img),8,3);
// 			cvCopy(one_img,src);
// 			//此处放置一键处理的相关代码
// 			m_img_plate= CopyIplImageROI(src,selection);
// 		}		
	} 

	IplImage* vp_images = m_img_plate;
	IplImage* vp_images_gray = cvCreateImage(cvGetSize(vp_images),IPL_DEPTH_8U,1);
	cvCvtColor(vp_images,vp_images_gray,CV_BGR2GRAY);//vp_images为源图像，vp_images_gray为目的图像

	vp_images_bin = cvCreateImage(cvGetSize(vp_images),IPL_DEPTH_8U,1);
	cvThreshold(vp_images_gray,vp_images_bin,thresthold,255,CV_THRESH_BINARY);

	//拷贝二值图像
	//m_PlateResult.pImagePlateBin=CopyIplImage(vp_images_bin);

	IplImage* vp_images_precise = cvCreateImage(cvGetSize(vp_images_bin),IPL_DEPTH_8U,1);
	IplImage* vp_images_sign = cvCreateImage(cvGetSize(vp_images_bin),IPL_DEPTH_64F,1);      //连通域标记图像        
    vp_images_sign_ref = cvCreateImage(cvGetSize(vp_images_bin),IPL_DEPTH_64F,1);
	//连通域搜索 返回连通域结构
	vector <Hei_sta> vt_ConnectDom=cfc.find_connected_domain( vp_images_bin,vp_images_sign); 

	pre_check(vt_ConnectDom,vp_images_bin,vp_images_sign,vp_images_precise,vp_images_sign_ref);
}

void CarPlatePosition::plateSegment()
{
	//图像进行色彩空间转换
	IplImage* src_color_trans = cvCreateImage(cvGetSize(srcPic),8,3);
	cvCopy(srcPic,src_color_trans);
	Color_Trans(src_color_trans);

	//图像进行纹理处理
	IplImage* src_texture = cvCreateImage(cvGetSize(srcPic),8,3);
	cvCopy(srcPic,src_texture);
	texture(src_texture);

	//蓝白车牌识别车牌区域
	int row=m_img_VG->height;//row是整个图像的高度，而不是车牌部分车牌的高度
	int col=m_img_VG->width;
	vp_blue_white(0,0,row-1,col-1); 

	plate_segment(srcPic);

	PlateCharSegment(roiThree);
}

void CarPlatePosition::plateSegmentOne()
{
	// TODO: 在此添加控件通知处理程序代码
	//导入图片
// 	string dir_path = "D:\cutpic\\";
// 	Directory dir;
// 	vector<string> filename = dir.GetListFiles(dir_path,"*.jpg",false);
// 
// 	for(int i = 0;i<filename.size();i++)
// 	{
// 		string fileName = filename[i];
// 		string fileFullName = dir_path+fileName;
// 		image = imread(fileFullName,1);
		//Mat转化成IplImage

		//srcPic_One = &IplImage(image);

		IplImage* src_color_trans = cvCreateImage(cvSize(pPic->width,pPic->height),pPic->depth,pPic->nChannels);
		cvCopy(pPic,src_color_trans);
		cvShowImage("xxx",src_color_trans);
		Color_Trans(src_color_trans);

		IplImage* src_texture = cvCreateImage(cvGetSize(pPic),8,3);
		cvCopy(pPic,src_texture);
		texture(src_texture);

		//车牌精确定位
		int row=m_img_VG->height;//row是整个图像的高度，而不是车牌部分车牌的高度
		int col=m_img_VG->width;
		vp_blue_white(0,0,row-1,col-1); 

		plate_segment(pPic);

		PlateCharSegment(roiThree);

// 		waitKey(1000);
// 	}
}