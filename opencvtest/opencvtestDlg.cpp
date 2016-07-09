#include "stdafx.h"
#include "opencvtest.h"
#include "opencvtestDlg.h"
#include "afxdialogex.h"
#include "cv.h" //  OpenCV 的基本函数头文件  
#include"highgui.h" //OpenCV 的图像显示函数头文件  
#include "CvvImage.h" 
#include "CarFlowCount.h"
#include "CarPlatePosition.h"
#include "SetPreferencesDlg.h"
#include "CarPlateRec2.h"
#include <atltime.h>
#include <opencv2/opencv.hpp>
#include<vector>      //使用Vector向量应包含的头文件
#include <iostream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;

//-----------------------------------------变量的声明----------------------------------------------
extern CString strTmp2;
extern int flag;
int nnPosNum;
float height_xian;
int thickness_scale2;
float thickness=0.0;
int car_num_num;

IplImage* srcPic;//用于   单独车牌定位   的传入图像
extern IplImage* srcPic_One;//用于一键处理才原始图像
extern IplImage* pPic;
extern IplImage* m_img_VG;
IplImage* one_img;

Mat image;//用于遍历文件夹
CString pathVideo;
CString pathPic;
CvCapture* pCapture;
int g_m_run;
IplImage* g_pFrame;
int g_m_int;
bool IsPlaying;
bool IsAviFile;
CRect rect;
CDC* pDC;
HDC hDC;
CWnd* pwnd;
IplImage* cutPic =NULL;//视频截图，1、用于显示截图 2、用于车牌定位函数；
CarFlowCount CFC;
CarPlatePosition CPP;
/***********************************车流统计部分传入的参数********/
extern IplImage* exPic;//车流量统计部分过来的截图，计划是不用了
extern string result_RES2;
extern string dir_path;
extern Directory dir;

/*****************c车牌识别部分传入的外部参数******************************/
extern IplImage* m_img_vp_bluewhite;//用于定位的二值图像
extern IplImage* roiThree;  //原图中裁剪的彩色车牌截图
extern IplImage* vp_images_sign_ref; //字符分割后的车牌图像

/*******************车牌定位单独使用与 一键处理的开关**************************/
//BOOL ONE_PART;
//BOOL ONE_FLOW;
/***************两个滑动条的位置获得的全局数值**************************/
int detctlinePos;
int thicknesslinePos;

//-------------------------------------------------------------------程序自动生成的代码---------------------------------
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()
// CopencvtestDlg 对话框
CopencvtestDlg::CopencvtestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CopencvtestDlg::IDD, pParent)
	, car_num(_T(""))
	, columns(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
void CopencvtestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ORIGINAL, Picture1);
	DDX_Control(pDX, IDC_LICENSE, frontimg);
	DDV_MaxChars(pDX, car_num, 100);
	DDX_Control(pDX, IDC_LICENSENUM, mMyPicture);
	DDX_Control(pDX, IDC_SLIDER2, m_hor_detectline);
	DDX_Control(pDX, IDC_SLIDER1, m_hor_thicknessline);
	DDX_Control(pDX, IDC_LIST, m_listbox);
}
BEGIN_MESSAGE_MAP(CopencvtestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_BN_CLICKED(IDC_OPENVIDEO, &CopencvtestDlg::OnBnClickedOpenvideo)
	ON_BN_CLICKED(IDC_STARTVIDEO, &CopencvtestDlg::OnBnClickedStartvideo)
	ON_BN_CLICKED(IDC_PAUSE, &CopencvtestDlg::OnBnClickedPause)
	ON_BN_CLICKED(IDC_STOP, &CopencvtestDlg::OnBnClickedStop)
//	ON_BN_CLICKED(IDC_EXIT, &CopencvtestDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_FSTART, &CopencvtestDlg::OnBnClickedFstart)
	ON_COMMAND(ID_32771, OnC1)         // 第一个参数为菜单项ID,第二个参数为事件名称，这里命名为OnC1;
	ON_COMMAND(ID_32781, OnC2) 
	ON_COMMAND(ID_32772, OnOpenVideo) 
	ON_COMMAND(ID_32782, OnOpenPic) 
	ON_COMMAND(ID_32779, OnHelpDlg) 
	ON_COMMAND(ID_32780, OnAboutDlg) 

//	ON_WM_TIMER()
ON_WM_TIMER()
ON_BN_CLICKED(IDC_FSTOP, &CopencvtestDlg::OnBnClickedFstop)
ON_LBN_SELCHANGE(IDC_LIST, &CopencvtestDlg::OnLbnSelchangeList)
ON_BN_CLICKED(IDC_CUTSCREEN, &CopencvtestDlg::OnBnClickedCutscreen)
ON_BN_CLICKED(IDC_LSTART, &CopencvtestDlg::OnBnClickedLstart)
ON_STN_CLICKED(IDC_CARNUM, &CopencvtestDlg::OnStnClickedCarnum)
ON_WM_HSCROLL()
ON_BN_CLICKED(IDC_LRECNOGIZE, &CopencvtestDlg::OnBnClickedLrecnogize)
ON_EN_CHANGE(IDC_EDIT1, &CopencvtestDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()
// CopencvtestDlg 消息处理程序
BOOL CopencvtestDlg::OnInitDialog()
{
	HMENU m_hMenu1;
	CDialogEx::OnInitDialog();
	//最大化显示
	ShowWindow(SW_SHOWMAXIMIZED);
	//添加菜单栏
	//m_hMenu1 =LoadMenu(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MENU1));//
	// ::SetMenu(this->GetSafeHwnd(),m_hMenu1);//添加到对话框
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	// TODO: 在此添加额外的初始化代码

	//GetDlgItem( IDC_OPENVIDEO )->EnableWindow( TRUE );
	GetDlgItem( IDC_STARTVIDEO )->EnableWindow( FALSE );
	GetDlgItem( IDC_PAUSE )->EnableWindow( FALSE );
	GetDlgItem( IDC_STOP )->EnableWindow( FALSE );
	GetDlgItem( IDC_FSTART )->EnableWindow( TRUE );//一键开始
	GetDlgItem( IDC_LSTART )->EnableWindow( FALSE );//车流统计
	GetDlgItem( IDC_LRECNOGIZE )->EnableWindow( FALSE );//车牌识别
	
	
	pwnd = GetDlgItem(IDC_VIDEO);
	pwnd->MoveWindow(30,30,624,436);
	pDC =pwnd->GetDC();
	hDC= pDC->GetSafeHdc();
	pwnd->GetClientRect(&rect);
	Invalidate();
//	m_slider.SetRange(0,100);
//	m_slider.SetTicFreq(10);
	CvSize ImgSize;
	ImgSize.height = 436;
	ImgSize.width = 624;
	g_pFrame = cvCreateImage( ImgSize, IPL_DEPTH_8U, 3 );
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE


	/***********************对检测线位置滚动条进行初始化**********************************/
	m_hor_detectline.SetRange(1,100);
	//设置水平滚动条的初始位置为20
	m_hor_detectline.SetPos(50);

	/***********************对检测线粗细滚动条进行初始化**********************************/
	m_hor_detectline.SetRange(1,100);
	//设置水平滚动条的初始位置为20
	m_hor_detectline.SetPos(5);



}

void CopencvtestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}
// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CopencvtestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}
//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CopencvtestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CopencvtestDlg::OnStnClickedShowimage()
{
	// TODO: 在此添加控件通知处理程序代码
}
//-------------------------------------------------------以上是自动生成代码-------------------------------------
void CopencvtestDlg::cleanArea()
{
	CStatic* pStatic = (CStatic*)GetDlgItem(IDC_VIDEO);
	CRect lRect;
	pStatic->GetClientRect(&lRect);
	pStatic->GetDC()->FillSolidRect(lRect.left ,lRect.top ,lRect.Width(),lRect.Height(), RGB(192, 192, 192));

	pStatic = (CStatic*)GetDlgItem(IDC_PLATE_POS);
	pStatic->GetClientRect(&lRect);	
	pStatic->GetDC()->FillSolidRect(lRect.left ,lRect.top ,lRect.Width(),lRect.Height(), RGB(192, 192, 192));

	pStatic = (CStatic*)GetDlgItem(IDC_PLATE_THRES);
	pStatic->GetClientRect(&lRect);	
	pStatic->GetDC()->FillSolidRect(lRect.left ,lRect.top ,lRect.Width(),lRect.Height(), RGB(192, 192, 192));

	pStatic = (CStatic*)GetDlgItem(IDC_PLATE_THRES_RESULT);
	pStatic->GetClientRect(&lRect);	
	pStatic->GetDC()->FillSolidRect(lRect.left ,lRect.top ,lRect.Width(),lRect.Height(), RGB(192, 192, 192));

	pStatic = (CStatic*)GetDlgItem(IDC_PLATE_CHAR_SEG);
	pStatic->GetClientRect(&lRect);	
	pStatic->GetDC()->FillSolidRect(lRect.left ,lRect.top ,lRect.Width(),lRect.Height(), RGB(192, 192, 192));


	//加载图像的时候就将一个空字符串加到edit中了
	result_RES2=" ";
	LPCTSTR result2= result_RES2.c_str(); 
	SetDlgItemText(IDC_EDIT3,result2);
}
void CopencvtestDlg::ShowImage(IplImage* img, UINT ID)
{
	CDC* pDC = GetDlgItem( ID ) ->GetDC();		
	HDC hDC = pDC ->GetSafeHdc();	
	CRect rect;                                
	GetDlgItem(ID) ->GetClientRect( &rect );   
	int rw = rect.right - rect.left;
	int rh = rect.bottom - rect.top;
	int iw = img->width;
	int ih = img->height;
	int tx = (int)(rw - iw)/2;	
	int ty = (int)(rh - ih)/2;
	SetRect( rect, tx, ty, tx+iw, ty+ih );
	CvvImage cimg;
	cimg.CopyOf( img );				
	cimg.DrawToHDC( hDC, &rect );
	ReleaseDC( pDC );
}

void CopencvtestDlg::DrawPicToHDC(IplImage *img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(img,3);
	cimg.DrawToHDC(hDC,&rect);
	ReleaseDC(pDC);
}


void CopencvtestDlg::ResizeImage(IplImage* img)
{
	int w = img->width;
	int h = img->height;
	int max = (w > h)? w: h;
	float scale = (float) ( (float) max / 624.0f );
	int nw = (int)( w/scale );
	int nh = (int)( h/scale );
	//int tlx = (nw > nh)? 0: (int)(320-nw)/2;
	//int tly = (nw > nh)? (int)(320-nh)/2: 0;
	int tlx = 0;
	int tly = 0;
	cvSetImageROI( g_pFrame, cvRect( tlx, tly, nw, nh) );
	cvResize( img, g_pFrame );//依照一定的尺寸重新估量，改造  //运行一段时间后会出错，，，
	cvResetImageROI( g_pFrame );
}

//添加视频文件
void CopencvtestDlg::OnaddVideo()
{
	// TODO: 在此添加控件通知处理程序代码

	strTmp2=" ";
	SetDlgItemText(IDC_EDIT1,strTmp2);

	CFileDialog dlg(
		TRUE,
		_T("*.avi"), 
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("*.avi|*.avi|*.rmvb|*.rmvb|*.mp4|*.mp4|*.wmv|*.wmv| All Files (*.*) |*.*||"), 
		NULL
		);	
	dlg.m_ofn.lpstrTitle = _T("选取视频文件");	
	if( dlg.DoModal() != IDOK )		
		return;

	pathVideo = dlg.GetPathName();
	cleanArea();

	GetDlgItem( IDC_STARTVIDEO )->EnableWindow( TRUE );
	GetDlgItem( IDC_PAUSE )->EnableWindow( TRUE );
	GetDlgItem( IDC_STOP )->EnableWindow( TRUE );
	GetDlgItem( IDC_LRECNOGIZE )->EnableWindow( FALSE );
	//GetDlgItem( IDC_SLIDER1 )->EnableWindow( TRUE );
}

//添加图像文件
void CopencvtestDlg::OnaddPic()
{
	CFileDialog dlg(TRUE, _T("*.bmp"),"",OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY,"image files (*.bmp; *.jpg) |*.bmp;*.jpg|All Files (*.*)|*.*||",NULL);
	dlg.m_ofn.lpstrTitle = _T("选取图像文件");	
	if( dlg.DoModal() != IDOK )		
		return;

	pathPic = dlg.GetPathName();

	cleanArea();
	//srcPic=cvLoadImage("D:\18.bmp",1);
	srcPic = cvLoadImage((LPSTR)(LPCTSTR)pathPic);
	DrawPicToHDC(srcPic,IDC_VIDEO);



	GetDlgItem( IDC_STARTVIDEO )->EnableWindow( FALSE );
	GetDlgItem( IDC_PAUSE )->EnableWindow( FALSE );
	GetDlgItem( IDC_STOP )->EnableWindow( FALSE );
	//GetDlgItem( IDC_SLIDER1 )->EnableWindow( TRUE );
	GetDlgItem( IDC_FSTART )->EnableWindow( FALSE );
	GetDlgItem( IDC_LSTART )->EnableWindow( FALSE );
	GetDlgItem( IDC_LRECNOGIZE )->EnableWindow( TRUE );
}


void CopencvtestDlg::OnBnClickedStartvideo()
{
	BOOL SWITCH=FALSE;
	nnPosNum=50;
	// TODO: 在此添加控件通知处理程序代码
	if( pathVideo == "" ) 
	{
		MessageBox("请先选择视频文件!");
		return;
	}
	else
	{
		if(!(pCapture = cvCreateFileCapture( pathVideo )))
		{
			MessageBox("打开视频文件失败!");
			return;
		}
	}
	int frames = (int) cvGetCaptureProperty( pCapture, CV_CAP_PROP_FRAME_COUNT );
	//GetDlgItem( IDC_OPENVIDEO )->EnableWindow( FALSE );
	GetDlgItem( IDC_STARTVIDEO )->EnableWindow( FALSE );
	GetDlgItem( IDC_PAUSE )->EnableWindow( TRUE );
	GetDlgItem( IDC_STOP )->EnableWindow( TRUE );
	GetDlgItem( IDC_FSTART )->EnableWindow( FALSE );
	GetDlgItem( IDC_LSTART )->EnableWindow( TRUE );
	
	IplImage *pFrame = NULL;
	cvNamedWindow("video");
	cvResizeWindow("video",1,1);
	HWND hWnd = (HWND) cvGetWindowHandle("video");
	HWND hParent = ::GetParent(hWnd);
	HWND hwnd1=::FindWindow("CVideoPlayerDlg","VideoPlayer");
	::SetParent(hWnd, hwnd1);
	::ShowWindow(hParent, SW_HIDE);
	if( g_pFrame )								
		cvZero( g_pFrame );
	g_m_run = 1;
	if(pCapture)
	{
		cvSetCaptureProperty(pCapture,CV_CAP_PROP_POS_FRAMES,g_m_int);
	}
	while( (g_m_int < frames) & (g_m_run == 1) )
	{ 
		pFrame = cvQueryFrame( pCapture );
		if (pFrame==NULL)
		{
			INT_PTR nRes;
			nRes=MessageBox(_T("视频处理已完成，返回主界面"), _T("车辆识别系统V1.0"), MB_OK | MB_ICONEXCLAMATION );
			if (IDOK == nRes)   
			     // return;   
				// CopencvtestDlg dlg;
				 //dlg.DoModal();
				 //break;
				 KillTimer(1);
				break;

		} 
		else
		{
		
		IplImage* newframe = cvCloneImage(pFrame);  //视频结束这里会出错
		ResizeImage( newframe );
		//ShowImage( pFrame , IDC_VIDEO);
		

		height_xian=(nnPosNum/100.0)*g_pFrame->height;
		thickness = (thickness_scale2/400.0)*g_pFrame->height;
		cvLine(g_pFrame,cvPoint(0,height_xian),cvPoint(650,height_xian),cvScalar(255,0,0),thickness); 
		ShowImage( g_pFrame , IDC_VIDEO);

		if (cvWaitKey(20) == 27) 
			break;
		g_m_int++;
		cvReleaseImage( &newframe );

		}
	}
	if(g_m_int==frames)
	{
		g_m_int=0;
		//m_slider.SetPos(g_m_int);
		g_m_run = 0;
		if( g_pFrame )	
		{
			cvZero( g_pFrame  );
			//cvLine(g_pFrame,cvPoint(0,350),cvPoint(650,350),cvScalar(255,0,0),20); 
			ShowImage( g_pFrame , IDC_VIDEO );
		}
	//	GetDlgItem( IDC_OPENVIDEO )->EnableWindow( FALSE );
		GetDlgItem( IDC_STARTVIDEO )->EnableWindow( FALSE );
		GetDlgItem( IDC_PAUSE )->EnableWindow( TRUE );
		GetDlgItem( IDC_STOP )->EnableWindow( TRUE );
	}	
	cvReleaseCapture(&pCapture);
	cvDestroyWindow("video");
}

void CopencvtestDlg::OnBnClickedPause()
{
	// TODO: 在此添加控件通知处理程序代码
	g_m_run=0;

//	GetDlgItem( IDC_OPENVIDEO )->EnableWindow( FALSE );
	GetDlgItem( IDC_STARTVIDEO )->EnableWindow( TRUE );
	GetDlgItem( IDC_PAUSE )->EnableWindow( FALSE );
	GetDlgItem( IDC_STOP )->EnableWindow( FALSE );
	GetDlgItem( IDC_FSTART )->EnableWindow( FALSE );
	GetDlgItem( IDC_LSTART )->EnableWindow( FALSE );
	KillTimer(1);
}

void CopencvtestDlg::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
	CDC MemDC; 
	CBitmap m_Bitmap1;
	//m_Bitmap1.LoadBitmap(129); 
	MemDC.CreateCompatibleDC(NULL);
	MemDC.SelectObject(&m_Bitmap1);
	pDC->StretchBlt(rect.left,rect.top,rect.Width(),rect.Height(),&MemDC,0,0,48,48,SRCCOPY); 
	m_Bitmap1.DeleteObject();
	MemDC.DeleteDC();

//	GetDlgItem( IDC_OPENVIDEO )->EnableWindow( TRUE );
	GetDlgItem( IDC_STARTVIDEO )->EnableWindow( TRUE );
	GetDlgItem( IDC_PAUSE )->EnableWindow( FALSE );
	GetDlgItem( IDC_STOP )->EnableWindow( FALSE );
	GetDlgItem( IDC_FSTART )->EnableWindow( FALSE );
	GetDlgItem( IDC_LSTART )->EnableWindow( FALSE );

	g_m_run=0;
	g_m_int=0;
	KillTimer(1);
	return;
}


void CopencvtestDlg::OnBnClickedFstart()
{
	//ONE_FLOW=TRUE;
	//ONE_PART=FALSE;//用于一键处理的开关

	//int vfps = CFC.countCar();
	//AfxMessageBox("哈哈");

	//SetTimer(1,vfps, NULL);  //定时器ID，定时时间，采用消息函数响应定时操作
	searchDirectry();
//	AfxMessageBox("哈哈");
}

void CopencvtestDlg::cutToOrignal()
{
	//AfxMessageBox("哈哈");
	DrawPicToHDC(exPic,IDC_LICENSE);
}

void CopencvtestDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//CarFlowCount cfc;
    cutPic = CFC.count_aga();
	DrawPicToHDC(pPic,IDC_PLATE_POS);
	cutToOrignal();
	//CWnd *pWnd2=GetDlgItem(IDC_CARNUM);//获得对话框句柄
	//pWnd2->SetWindowText(CFC.strTmp2);
	SetDlgItemText(IDC_EDIT1,strTmp2);
	if(flag==1)
	{
		KillTimer(1);
	}
	

	CDialogEx::OnTimer(nIDEvent);
}


void CopencvtestDlg::searchDirectry()
{
	//UpdateData(TRUE);
// 	char szDir[150];//定义一个字符串用来接收文件名
// 	CString strFilePath1;
// 	BROWSEINFO bi;//建立一个BROWSEIFO对象
// 	ITEMIDLIST* pidl;//创建一个结构指针
// 	memset(&bi,0,sizeof BROWSEINFO);
// 	bi.hwndOwner=m_hWnd;
// 	bi.hwndOwner=this->m_hWnd;//指定父窗口，在对话框显示期间，父窗口将被禁用
// 	bi.pszDisplayName=szDir;//pszDisplayNmae：用来保存用户选中的目录字符串的内存地址（缓冲区）
// 	bi.pidlRoot=NULL;//如果指定为NULL，就以“桌面”为根，pidRoot：ITEMIDLIST结构的地址包含浏览时的初始根目录
// 	bi.lpszTitle="请选择批处理车牌路径";//这一行将显示在对话框的顶端
// 
// 	//描述窗口的选项：包括状态文本、显示目录和文件、只返回文件系统目录、返回系统一个节点
// 	bi.ulFlags=BIF_STATUSTEXT|BIF_BROWSEINCLUDEFILES|BIF_RETURNONLYFSDIRS|BIF_RETURNFSANCESTORS;
// 
// 	bi.lpfn=NULL;//lpfn：应用程序定义的浏览器对话框回调函数的地址，当对话框中的事件发生时，将调用回调函数。该参数可用为NULL；
// 
// 	bi.iImage=0;//对话框传递给回调函数的一个参数指针
// 	bi.lParam=0;//与选中目录相关的图像。该图像将被指定为系统图像列表中的索引值
// 	pidl=SHBrowseForFolder(&bi);
// 
// 
//  	if (SHGetPathFromIDList(pidl,szDir))
//  	{
//  		strFilePath1=szDir;
// // 		SetDlgItemText(IDC_EDIT_PLATE, strFilePath1);
//  		//return;
// 		 } 
	//UpdateData(FALSE);


	string dir_path = "C:\\Picture\\";
	//LPCTSTR strFilePath= atoi(strFilePath1); 
	//string strFilePath  = LPCSTR(strFilePath1);
	vector<string> filename = dir.GetListFiles(dir_path,"*.jpg",false);

	for(int i = 0;i<filename.size();i++)
	{
		string fileName = filename[i];
		string fileFullName = dir_path+fileName;
		image = imread(fileFullName,1);

		srcPic = &IplImage(image);


		CPP.plateSegment();

	}

	waitKey(1000);
}

void CopencvtestDlg::OnBnClickedFstop()
{
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(1);
}


void CopencvtestDlg::OnLbnSelchangeList()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CopencvtestDlg::OnBnClickedCutscreen()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CopencvtestDlg::OnBnClickedLstart()
{
	//ONE_FLOW = FALSE;
	int vfps = CFC.countCar();
	//AfxMessageBox("哈哈");
	
	SetTimer(1,vfps, NULL);  //定时器ID，定时时间，采用消息函数响应定时操作
	// TODO: 在此添加控件通知处理程序代码
}
void CopencvtestDlg::OnC1()
{
	OnaddVideo();
}
void CopencvtestDlg::OnC2()
{
	SetPreferencesDlg spd;
	spd.DoModal();
}
void CopencvtestDlg::OnOpenVideo()
{
	KillTimer(1);
	exit(0);
	
}

void CopencvtestDlg::OnHelpDlg()
{
   ShellExecute(NULL,"open","d:\\车流量统计及车牌识别系统使用说明书.htm",NULL,NULL,SW_SHOWNORMAL);

}

void CopencvtestDlg::OnAboutDlg()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();

}

void CopencvtestDlg::OnOpenPic()
{
	//PostMessage(WM_QUIT,0,0);
	OnaddPic();
}

void CopencvtestDlg::OnStnClickedCarnum()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CopencvtestDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	switch(pScrollBar->GetDlgCtrlID()){

	case IDC_SLIDER2:
		{
			int pos = m_hor_detectline.GetPos();
			switch(nSBCode)
			{
				//如果向左滚动一列，则pos减1
			case  SB_LINEUP:
				pos-=1;
				break;
				//如果向右滚动一列，则pos加1
			case SB_LINEDOWN:
				pos+=1;
				break;
				//如果向左滚动一页，则pos减10
			case SB_PAGEUP:
				pos-=10;
				break;
				//如果向右滚动一页，则pos减10
			case SB_PAGEDOWN:
				pos+=10;
				break;
				//如果滚动到最左端，则pos为1
			case SB_TOP:
				pos = 1;
				break;
				//如果滚动到最右端，则pos为100
			case SB_BOTTOM:
				pos = 100;
				break;
				//如果拖动滚动块滚动到指定位置，则pos赋值为npos的值
			case SB_THUMBPOSITION:
				pos = nPos;

				nnPosNum=nPos;
				break;
				//下面的m_horiScrollbar.SetScrollPos(pos);执行时会第二次进入此函数，最终确定滚动块位置，并且会直接到default分支，
				//所以此处设置编辑框中显示数值
			default:
				//SetDlgItemInt(IDC_HSCROLL_EDIT,pos);
				return;
			}
			//设置滚动条的位置
			//m_horiScrollbar.SetScrollPos(pos);
			m_hor_detectline.SetPos(pos);
			detctlinePos = m_hor_detectline.GetPos();
			break;
		}
	case IDC_SLIDER1:
		/***************************缩放倍数水平滚动条设置*******************************************/
		{
			//获取水平滚动条当前位置
			//int pos = m_horiScrollbar.GetScrollPos();
			int pos_thickness = m_hor_thicknessline.GetPos();
			switch(nSBCode)
			{
				//如果向左滚动一列，则pos减1
			case  SB_LINEUP:
				pos_thickness-=1;
				break;
				//如果向右滚动一列，则pos加1
			case SB_LINEDOWN:
				pos_thickness+=1;
				break;
				//如果向左滚动一页，则pos减10
			case SB_PAGEUP:
				pos_thickness-=10;
				break;
				//如果向右滚动一页，则pos减10
			case SB_PAGEDOWN:
				pos_thickness+=10;
				break;
				//如果滚动到最左端，则pos为1
			case SB_TOP:
				pos_thickness = 1;
				break;
				//如果滚动到最右端，则pos为100
			case SB_BOTTOM:
				pos_thickness = 100;
				break;
				//如果拖动滚动块滚动到指定位置，则pos赋值为npos的值
			case SB_THUMBPOSITION:
				pos_thickness = nPos;

				thickness_scale2=nPos;
				break;
				//下面的m_horiScrollbar.SetScrollPos(pos);执行时会第二次进入此函数，最终确定滚动块位置，并且会直接到default分支，
				//所以此处设置编辑框中显示数值
			default:
				//SetDlgItemInt(IDC_HSCROLL_EDIT4,pos_zoom);
				return;
			}
			//设置滚动条的位置
			//m_horiScrollbar.SetScrollPos(pos);
			m_hor_thicknessline.SetPos(pos_thickness);
			thicknesslinePos = m_hor_thicknessline.GetPos();
			break;
		}
	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
}

void CopencvtestDlg::OnBnClickedLrecnogize()
{
	// TODO: 在此添加控件通知处理程序代码

		//开关为TRUE进行分别处理
		//ONE_PART=TRUE;
		CPP.plateSegment();
		segmentRecognition();

		LPCTSTR result2= result_RES2.c_str(); 
		SetDlgItemText(IDC_EDIT3,result2);

		

		CTime curTime;
		curTime = CTime::GetCurrentTime();
		CString strCurTime,showContent;   
		char carNum[40];
		CString spaces="     ";
		strCurTime.Format(_T("%04d/%02d/%02d %02d:%02d:%02d"), curTime.GetYear(), curTime.GetMonth(), curTime.GetDay(), curTime.GetHour(), curTime.GetMinute(), curTime.GetSecond());
		car_num_num++;
		sprintf(carNum,"%d",car_num_num); 
		showContent = carNum+spaces+strCurTime+spaces+result2;
		m_listbox.InsertString(0,showContent);

		result_RES2="";
		DrawPicToHDC(srcPic,IDC_PLATE_POS);
		DrawPicToHDC(m_img_vp_bluewhite,IDC_PLATE_THRES);
		DrawPicToHDC(roiThree,IDC_PLATE_THRES_RESULT);
		DrawPicToHDC(vp_images_sign_ref,IDC_PLATE_CHAR_SEG);

		GetDlgItem( IDC_LRECNOGIZE )->EnableWindow( FALSE );
		//searchDirectry();
	
}
//虽然定义了，但是存在问题，并未使用；第一种截图直接使用的方式里面的车牌处理方法，不可用
void CopencvtestDlg::plateSegment_ALL()
{
	//开关为TRUE进行分别处理
	//ONE_PART=TRUE;
	CPP.plateSegmentOne();
	segmentRecognition();

	LPCTSTR result2= result_RES2.c_str(); 
	//LPCTSTR result2=(LPCTSTR)result.c_str();
	SetDlgItemText(IDC_EDIT3,result2);
	DrawPicToHDC(srcPic,IDC_PLATE_POS);
	DrawPicToHDC(m_img_vp_bluewhite,IDC_PLATE_THRES);
	DrawPicToHDC(roiThree,IDC_PLATE_THRES_RESULT);
	DrawPicToHDC(vp_images_sign_ref,IDC_PLATE_CHAR_SEG);	
}

void CopencvtestDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
