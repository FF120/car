
// opencvtestDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "cv.h" //  OpenCV 的基本函数头文件  
#include"highgui.h" //OpenCV 的图像显示函数头文件  
#include "mypicture.h"

// CopencvtestDlg 对话框
class CopencvtestDlg : public CDialogEx
{
// 构造
public:
	CopencvtestDlg(CWnd* pParent = NULL);	// 标准构造函数
	void cleanArea();
	void OnaddVideo();
	void OnaddPic();
	void ResizeImage(IplImage* img);
	void ShowImage(IplImage* img, UINT ID);
	void cutToOrignal();
	void DrawPicToHDC(IplImage *img, UINT ID);
	void plateSegment_ALL();
	void searchDirectry();

// 对话框数据
	enum { IDD = IDD_OPENCVTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listbox;
	CStatic Picture1;
	CSliderCtrl m_hor_detectline;
	CSliderCtrl m_hor_thicknessline;


	afx_msg void OnBnClickedOk();
	afx_msg void OnStnClickedShowimage();
	afx_msg void OnBnClickedButton1();
	// 在某个控件上显示图像
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	//菜单响应事件1
	 afx_msg void OnC1();  
	 afx_msg void OnC2();  
	 afx_msg void OnOpenVideo();  
	 afx_msg void OnOpenPic(); 
	 afx_msg void OnHelpDlg(); 
	 afx_msg void OnAboutDlg(); 
	
	CStatic frontimg;
	// 车辆计数
	CString car_num;
	// 截取图像
	void GetImageRect(IplImage* orgImage, IplImage* imgRect, CvRect rectInImage);
	//
	IplImage* CopyIplImageROI(IplImage* pImageSrc,CvRect rectROI);
	int columns;
	afx_msg void OnStnClickedcolumn();
	afx_msg void OnBnClickedChoosefile();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CMyPicture mMyPicture;
	afx_msg void OnStnClickedMypicture();
//	afx_msg void OnBnClickedOpenvideo();
	afx_msg void OnBnClickedStartvideo();
	afx_msg void OnBnClickedPause();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedFstart();

//	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedFstop();
	afx_msg void OnLbnSelchangeList();
	// 菜单栏
	HMENU SetMenu;
	afx_msg void OnBnClickedCutscreen();
	afx_msg void OnBnClickedLstart();
	afx_msg void OnStnClickedCarnum();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedLrecnogize();
	afx_msg void OnEnChangeEdit1();
};
