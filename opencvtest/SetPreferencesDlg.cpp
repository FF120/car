// SetPreferencesDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "opencvtest.h"
#include "SetPreferencesDlg.h"
#include "afxdialogex.h"
#include "iostream"
#include "sstream"
#include <fstream>
#include <stdlib.h>
#include <math.h>


using namespace std;
// SetPreferencesDlg 对话框
CString strFilePath1;  //车牌保存位置
CString strFilePath2;  //结果保存位置
CString strFilePath3;  //截图保存位置

CString pathVideo2;//存放配置文件的路径
/************************DLG中的参数写入到文件中的参数定义，所有全局变量*************************/
//ComBox内容
CString laneText;//车道字符串
CString areaText;//车牌地区字符串
CString searchboxText;//粗定位搜索框字符串

//CStrilBar内容
CString sprpiexlText;//像素差
CString zoomText;//缩放倍数
CString thresholdText;//阈值
CString nocarpiexlText;//无车像素值
CString platethresholdText;//车牌二值化阈值

//自定义路径内容
CString platepathText;//车牌路径
CString resultpathText;//结果路径
CString cutpicpathText;//截图路径

/************************文件中的参数读取到DLG中的参数定义，所有全局变量*************************/
//ComBox内容
string inlaneText;//车道字符串
string inareaText;//车牌地区字符串
string insearchboxText;//粗定位搜索框字符串

//CStrilBar内容
string insprpiexlText;//像素差
string inzoomText;//缩放倍数
string inthresholdText;//阈值
string innocarpiexlText;//无车像素值
string inplatethresholdText;//车牌二值化阈值

//自定义路径内容
string inplatepathText;//车牌路径
string inresultpathText;//结果路径
string incutpicpathText;//截图路径

/***************文件中的参数读取到DLG中的参数定义，所有全局变量为具体的类型************************/
//ComBox内容
int inlaneInt;//车道字符串
string inareaString;//车牌地区字符串
string insearchboxString;//粗定位搜索框字符串

//CStrilBar内容
int insprpiexlInt;//像素差
double inzoomFloat;//缩放倍数
int inthresholdInt;//阈值
int innocarpiexlInt;//无车像素值
int inplatethresholdInt;//车牌二值化阈值

//自定义路径内容
string inplatepathString;//车牌路径
string inresultpathString;//结果路径
string incutpicpathString;//截图路径



IMPLEMENT_DYNAMIC(SetPreferencesDlg, CDialog)

SetPreferencesDlg::SetPreferencesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetPreferencesDlg::IDD, pParent)
{

}

SetPreferencesDlg::~SetPreferencesDlg()
{
}

void SetPreferencesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER2, m_horiScrollbar);
	DDX_Control(pDX, IDC_SLIDER4, m_horiScrollbar_zoom);
	DDX_Control(pDX, IDC_SLIDER06, m_horiScrollbar_threshold);
	DDX_Control(pDX, IDC_SLIDER8, m_horiScrollbar_pixel);
	DDX_Control(pDX, IDC_SLIDER7, m_horiScrollbar_platethros);
	DDX_Control(pDX, IDC_COMBO_LANE, m_combo_lane);
	DDX_Control(pDX, IDC_COMBO_AREA, m_combox_area);
	DDX_Control(pDX, IDC_COMBO_SEARCHBOX, m_combox_searchbox);
	DDX_Control(pDX, IDC_HSCROLL_EDIT, m_sprpiexledit);
	DDX_Control(pDX, IDC_HSCROLL_EDIT4, m_zoomedit);
	DDX_Control(pDX, IDC_HSCROLL_EDIT06, m_thresholdedit);
	DDX_Control(pDX, IDC_HSCROLL_EDIT8, m_nocaredit);
	DDX_Control(pDX, IDC_HSCROLL_EDIT7, m_paltethrestholdedit);
	DDX_Control(pDX, IDC_EDIT_PLATE, m_platepathedit);
	DDX_Control(pDX, IDC_EDIT_RESULT, m_resultpathedit);
	DDX_Control(pDX, IDC_EDIT_CUTPIC, m_cutpicedit);
}




BEGIN_MESSAGE_MAP(SetPreferencesDlg, CDialog)
	ON_COMMAND(ID_32781, &SetPreferencesDlg::On32781)
	ON_COMMAND(ID_32771, &SetPreferencesDlg::On32771)
	
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO_LANE, &SetPreferencesDlg::OnCbnSelchangeComboLane)
	ON_CBN_SELCHANGE(IDC_COMBO_AREA, &SetPreferencesDlg::OnCbnSelchangeComboArea)
	ON_CBN_SELCHANGE(IDC_COMBO_SEARCHBOX, &SetPreferencesDlg::OnCbnSelchangeComboSearchbox)
	ON_BN_CLICKED(IDC_BUTTON2, &SetPreferencesDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &SetPreferencesDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &SetPreferencesDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDOK, &SetPreferencesDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON5, &SetPreferencesDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON1, &SetPreferencesDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// SetPreferencesDlg 消息处理程序


void SetPreferencesDlg::On32781()
{
	SetPreferencesDlg spd;
	spd.DoModal();
	// TODO: 在此添加命令处理程序代码
}


void SetPreferencesDlg::On32771()
{
	exit(0);
	// TODO: 在此添加命令处理程序代码
}


void SetPreferencesDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void SetPreferencesDlg::OnCbnSelchangeCombo4()
{
	// TODO: 在此添加控件通知处理程序代码
}

void SetPreferencesDlg::InitText()
{
	ifstream inf;
	//文件中参数读入
	if(pathVideo2==""){
		inf.open("./out.txt");
			
	}
	else{
		inf.open("./out.txt");
	//inf.open(pathVideo2);
	}
	inf>>inlaneText>>inareaText>>insearchboxText>>insprpiexlText>>inzoomText>>inthresholdText>>innocarpiexlText>>inplatethresholdText
		>>inplatepathText>>inresultpathText>>incutpicpathText;

	inlaneInt = atoi(inlaneText.c_str());
	inareaString=inareaText;
	insearchboxString=insearchboxText;
	insprpiexlInt=atoi(insprpiexlText.c_str());
	inzoomFloat=atof(inzoomText.c_str());
	inthresholdInt=atoi(inthresholdText.c_str());
	innocarpiexlInt=atoi(innocarpiexlText.c_str());
	inplatethresholdInt=atoi(inplatethresholdText.c_str());
	inplatepathString=inplatethresholdText;
	inresultpathString=inresultpathText;
	incutpicpathString=incutpicpathText;
	////设置默认路径
	//strFilePath1 = "D://word";
	//strFilePath2 = "D://word";
	//strFilePath3 = "D://word";

	// SetDlgItemText(IDC_EDIT_PLATE, strFilePath1);
	// SetDlgItemText(IDC_EDIT_RESULT, strFilePath2);
	//  SetDlgItemText(IDC_EDIT_CUTPIC, strFilePath3);
	
}


BOOL SetPreferencesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitText();

		/**************************像素差水平滚动条初始化设置****************************************/
	//设置水平滚动条范围为1-100
	m_horiScrollbar.SetRange(1,100);
	//设置水平滚动条的初始位置为20
	m_horiScrollbar.SetPos(insprpiexlInt);
	//在编辑框中显示20
	SetDlgItemInt(IDC_HSCROLL_EDIT,insprpiexlInt);

	/**************************缩放倍数水平滚动条初始化设置****************************************/
	//设置水平滚动条范围为1-100
	m_horiScrollbar_zoom.SetRange(1,100);
	//设置水平滚动条的初始位置为20
	m_horiScrollbar_zoom.SetPos(inzoomFloat);
	//在编辑框中显示20
	SetDlgItemInt(IDC_HSCROLL_EDIT4,inzoomFloat);

	/**************************二值化阈值滚动条初始化设置****************************************/
	//设置水平滚动条范围为1-100
	m_horiScrollbar_threshold.SetRange(1,100);
	//设置水平滚动条的初始位置为20
	m_horiScrollbar_threshold.SetPos(inthresholdInt);
	//在编辑框中显示20
	SetDlgItemInt(IDC_HSCROLL_EDIT06,inthresholdInt);

	/**************************无车像素值滚动条初始化设置****************************************/
	//设置水平滚动条范围为1-100
	m_horiScrollbar_pixel.SetRange(1,100);
	//设置水平滚动条的初始位置为20
	m_horiScrollbar_pixel.SetPos(innocarpiexlInt);
	//在编辑框中显示20
	SetDlgItemInt(IDC_HSCROLL_EDIT8,innocarpiexlInt);

	/**************************车牌二值化阈值滚动条初始化设置****************************************/
	//设置水平滚动条范围为1-100
	m_horiScrollbar_platethros.SetRange(1,100);
	//设置水平滚动条的初始位置为20
	m_horiScrollbar_platethros.SetPos(inplatethresholdInt);
	//在编辑框中显示20
	SetDlgItemInt(IDC_HSCROLL_EDIT7,inplatethresholdInt);

	/******************************车道选择COMBOX初始化设置***************************************/


 	m_combo_lane.InsertString(0,"1");
 	m_combo_lane.InsertString(1,"2");
 	m_combo_lane.InsertString(2,"3");
 	m_combo_lane.InsertString(3,"4");

	LPCSTR inlaneTextLpc = inlaneText.c_str();
	int inlaneIndex = m_combo_lane.FindString(-1,inlaneTextLpc);
	m_combo_lane.SetCurSel(inlaneIndex);
	//m_combo_lane.Text

	/******************************地区选择ComBox初始化设置***************************************/
	m_combox_area.InsertString(0,"津");
	m_combox_area.InsertString(1,"京");
	m_combox_area.InsertString(2,"沪");
	m_combox_area.InsertString(3,"冀");
	m_combox_area.InsertString(4,"黑");
	m_combox_area.InsertString(5,"辽");
	m_combox_area.InsertString(6,"苏");
	m_combox_area.InsertString(7,"贵");

	LPCSTR inareaTextLpc = inareaText.c_str();
	int inareaIndex = m_combox_area.FindString(-1,inareaTextLpc);
	m_combox_area.SetCurSel(inareaIndex);

	/******************************粗定位搜索框大小COMBOX初始化设置***************************************/
	m_combox_searchbox.InsertString(0,"40*10");
	m_combox_searchbox.InsertString(1,"60*15");
	m_combox_searchbox.InsertString(2,"80*20");
	m_combox_searchbox.InsertString(3,"100*25");
	m_combox_searchbox.InsertString(4,"120*30");
	m_combox_searchbox.InsertString(5,"140*35");

	LPCSTR insearchboxTextLpc = insearchboxText.c_str();
	int insearchboxIndex = m_combox_searchbox.FindString(-1,insearchboxTextLpc);
	m_combox_searchbox.SetCurSel(insearchboxIndex);



	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//通过在类的属性框中添加对应事件得到
void SetPreferencesDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/***************************像素差水平滚动条设置*******************************************/
	switch(pScrollBar->GetDlgCtrlID()){
	
	case IDC_SLIDER2:
		{
			int pos = m_horiScrollbar.GetPos();
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
				break;
				//下面的m_horiScrollbar.SetScrollPos(pos);执行时会第二次进入此函数，最终确定滚动块位置，并且会直接到default分支，
				//所以此处设置编辑框中显示数值
			default:
				SetDlgItemInt(IDC_HSCROLL_EDIT,pos);
				return;
			}
			//设置滚动条的位置
			//m_horiScrollbar.SetScrollPos(pos);
			m_horiScrollbar.SetPos(pos);
			break;
			}
	case IDC_SLIDER4:
		/***************************缩放倍数水平滚动条设置*******************************************/
		{
			//获取水平滚动条当前位置
		//int pos = m_horiScrollbar.GetScrollPos();
		int pos_zoom = m_horiScrollbar_zoom.GetPos();
		switch(nSBCode)
		{
			//如果向左滚动一列，则pos减1
		case  SB_LINEUP:
			pos_zoom-=1;
			break;
			//如果向右滚动一列，则pos加1
		case SB_LINEDOWN:
			pos_zoom+=1;
			break;
			//如果向左滚动一页，则pos减10
		case SB_PAGEUP:
			pos_zoom-=10;
			break;
			//如果向右滚动一页，则pos减10
		case SB_PAGEDOWN:
			pos_zoom+=10;
			break;
			//如果滚动到最左端，则pos为1
		case SB_TOP:
			pos_zoom = 1;
			break;
			//如果滚动到最右端，则pos为100
		case SB_BOTTOM:
			pos_zoom = 100;
			break;
			//如果拖动滚动块滚动到指定位置，则pos赋值为npos的值
		case SB_THUMBPOSITION:
			pos_zoom = nPos;
			break;
			//下面的m_horiScrollbar.SetScrollPos(pos);执行时会第二次进入此函数，最终确定滚动块位置，并且会直接到default分支，
			//所以此处设置编辑框中显示数值
		default:
			SetDlgItemInt(IDC_HSCROLL_EDIT4,pos_zoom);
			return;
		}
		//设置滚动条的位置
		//m_horiScrollbar.SetScrollPos(pos);
		m_horiScrollbar_zoom.SetPos(pos_zoom);
		break;
		}
	case IDC_SLIDER06:
		/***************************缩放倍数水平滚动条设置*******************************************/
		{
			//获取水平滚动条当前位置
			//int pos = m_horiScrollbar.GetScrollPos();
			int pos_threshold = m_horiScrollbar_threshold.GetPos();
			switch(nSBCode)
			{
				//如果向左滚动一列，则pos减1
			case  SB_LINEUP:
				pos_threshold-=1;
				break;
				//如果向右滚动一列，则pos加1
			case SB_LINEDOWN:
				pos_threshold+=1;
				break;
				//如果向左滚动一页，则pos减10
			case SB_PAGEUP:
				pos_threshold-=10;
				break;
				//如果向右滚动一页，则pos减10
			case SB_PAGEDOWN:
				pos_threshold+=10;
				break;
				//如果滚动到最左端，则pos为1
			case SB_TOP:
				pos_threshold = 1;
				break;
				//如果滚动到最右端，则pos为100
			case SB_BOTTOM:
				pos_threshold = 100;
				break;
				//如果拖动滚动块滚动到指定位置，则pos赋值为npos的值
			case SB_THUMBPOSITION:
				pos_threshold = nPos;
				break;
				//下面的m_horiScrollbar.SetScrollPos(pos);执行时会第二次进入此函数，最终确定滚动块位置，并且会直接到default分支，
				//所以此处设置编辑框中显示数值
			default:
				SetDlgItemInt(IDC_HSCROLL_EDIT06,pos_threshold);
				return;
			}
			//设置滚动条的位置
			//m_horiScrollbar.SetScrollPos(pos);
			m_horiScrollbar_threshold.SetPos(pos_threshold);
			break;
		}
	case IDC_SLIDER8:
		/***************************缩放倍数水平滚动条设置*******************************************/
		{
			//获取水平滚动条当前位置
			//int pos = m_horiScrollbar.GetScrollPos();
			int pos_pixel = m_horiScrollbar_pixel.GetPos();
			switch(nSBCode)
			{
				//如果向左滚动一列，则pos减1
			case  SB_LINEUP:
				pos_pixel-=1;
				break;
				//如果向右滚动一列，则pos加1
			case SB_LINEDOWN:
				pos_pixel+=1;
				break;
				//如果向左滚动一页，则pos减10
			case SB_PAGEUP:
				pos_pixel-=10;
				break;
				//如果向右滚动一页，则pos减10
			case SB_PAGEDOWN:
				pos_pixel+=10;
				break;
				//如果滚动到最左端，则pos为1
			case SB_TOP:
				pos_pixel = 1;
				break;
				//如果滚动到最右端，则pos为100
			case SB_BOTTOM:
				pos_pixel = 100;
				break;
				//如果拖动滚动块滚动到指定位置，则pos赋值为npos的值
			case SB_THUMBPOSITION:
				pos_pixel = nPos;
				break;
				//下面的m_horiScrollbar.SetScrollPos(pos);执行时会第二次进入此函数，最终确定滚动块位置，并且会直接到default分支，
				//所以此处设置编辑框中显示数值
			default:
				SetDlgItemInt(IDC_HSCROLL_EDIT8,pos_pixel);
				return;
			}
			//设置滚动条的位置
			//m_horiScrollbar.SetScrollPos(pos);
			m_horiScrollbar_pixel.SetPos(pos_pixel);
			break;
		}

	case IDC_SLIDER7:
		/***************************缩放倍数水平滚动条设置*******************************************/
		{
			//获取水平滚动条当前位置
			//int pos = m_horiScrollbar.GetScrollPos();
			int pos_platethros = m_horiScrollbar_platethros.GetPos();
			switch(nSBCode)
			{
				//如果向左滚动一列，则pos减1
			case  SB_LINEUP:
				pos_platethros-=1;
				break;
				//如果向右滚动一列，则pos加1
			case SB_LINEDOWN:
				pos_platethros+=1;
				break;
				//如果向左滚动一页，则pos减10
			case SB_PAGEUP:
				pos_platethros-=10;
				break;
				//如果向右滚动一页，则pos减10
			case SB_PAGEDOWN:
				pos_platethros+=10;
				break;
				//如果滚动到最左端，则pos为1
			case SB_TOP:
				pos_platethros = 1;
				break;
				//如果滚动到最右端，则pos为100
			case SB_BOTTOM:
				pos_platethros = 100;
				break;
				//如果拖动滚动块滚动到指定位置，则pos赋值为npos的值
			case SB_THUMBPOSITION:
				pos_platethros = nPos;
				break;
				//下面的m_horiScrollbar.SetScrollPos(pos);执行时会第二次进入此函数，最终确定滚动块位置，并且会直接到default分支，
				//所以此处设置编辑框中显示数值
			default:
				SetDlgItemInt(IDC_HSCROLL_EDIT7,pos_platethros);
				return;
			}
			//设置滚动条的位置
			//m_horiScrollbar.SetScrollPos(pos);
			m_horiScrollbar_platethros.SetPos(pos_platethros);
			break;
		}


	}
	//获取水平滚动条当前位置
	//int pos = m_horiScrollbar.GetScrollPos();
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


//通过在Dlg中右键单击对应的控件，添加事件处理程序得到
void SetPreferencesDlg::OnCbnSelchangeComboLane()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strlane;
	int nSel;

	//获取组合框控件的列表框中项的索引
	nSel=m_combo_lane.GetCurSel();
	//根据选中项索引获取该项字符串
	m_combo_lane.GetLBText(nSel,strlane);

}


void SetPreferencesDlg::OnCbnSelchangeComboArea()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strarea;
	int nSel;

	//获取组合框控件的列表框中项的索引
	nSel=m_combox_area.GetCurSel();
	//根据选中项索引获取该项字符串
	m_combox_area.GetLBText(nSel,strarea);
}


void SetPreferencesDlg::OnCbnSelchangeComboSearchbox()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strsearchbox;
	int nSel;

	//获取组合框控件的列表框中项的索引
	nSel=m_combox_searchbox.GetCurSel();
	//根据选中项索引获取该项字符串
	m_combox_searchbox.GetLBText(nSel,strsearchbox);
}

//通过Button的属性列表中的“事件控件”添加
void SetPreferencesDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	char szDir[150];//定义一个字符串用来接收文件名

	BROWSEINFO bi;//建立一个BROWSEIFO对象
	ITEMIDLIST* pidl;//创建一个结构指针
	memset(&bi,0,sizeof BROWSEINFO);
	bi.hwndOwner=m_hWnd;
	bi.hwndOwner=this->m_hWnd;//指定父窗口，在对话框显示期间，父窗口将被禁用
	 bi.pszDisplayName=szDir;//pszDisplayNmae：用来保存用户选中的目录字符串的内存地址（缓冲区）
	bi.pidlRoot=NULL;//如果指定为NULL，就以“桌面”为根，pidRoot：ITEMIDLIST结构的地址包含浏览时的初始根目录
	bi.lpszTitle="请选择文件";//这一行将显示在对话框的顶端

	//描述窗口的选项：包括状态文本、显示目录和文件、只返回文件系统目录、返回系统一个节点
	bi.ulFlags=BIF_STATUSTEXT|BIF_BROWSEINCLUDEFILES|BIF_RETURNONLYFSDIRS|BIF_RETURNFSANCESTORS;

	bi.lpfn=NULL;//lpfn：应用程序定义的浏览器对话框回调函数的地址，当对话框中的事件发生时，将调用回调函数。该参数可用为NULL；

	bi.iImage=0;//对话框传递给回调函数的一个参数指针
	bi.lParam=0;//与选中目录相关的图像。该图像将被指定为系统图像列表中的索引值
	pidl=SHBrowseForFolder(&bi);


	if (SHGetPathFromIDList(pidl,szDir))
	{
		 strFilePath1=szDir;
		 SetDlgItemText(IDC_EDIT_PLATE, strFilePath1);
		//return;
	} 
	//else
//	{
//		SHGetPathFromIDList(pidl,szDir);
//	}
//	m_strFileName=szDir;
	UpdateData(FALSE);

}



void SetPreferencesDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	char szDir[150];//定义一个字符串用来接收文件名

	BROWSEINFO bi;//建立一个BROWSEIFO对象
	ITEMIDLIST* pidl;//创建一个结构指针
	memset(&bi,0,sizeof BROWSEINFO);
	bi.hwndOwner=m_hWnd;
	bi.hwndOwner=this->m_hWnd;//指定父窗口，在对话框显示期间，父窗口将被禁用
	bi.pszDisplayName=szDir;//pszDisplayNmae：用来保存用户选中的目录字符串的内存地址（缓冲区）
	bi.pidlRoot=NULL;//如果指定为NULL，就以“桌面”为根，pidRoot：ITEMIDLIST结构的地址包含浏览时的初始根目录
	bi.lpszTitle="请选择文件";//这一行将显示在对话框的顶端

	//描述窗口的选项：包括状态文本、显示目录和文件、只返回文件系统目录、返回系统一个节点
	bi.ulFlags=BIF_STATUSTEXT|BIF_BROWSEINCLUDEFILES|BIF_RETURNONLYFSDIRS|BIF_RETURNFSANCESTORS;

	bi.lpfn=NULL;//lpfn：应用程序定义的浏览器对话框回调函数的地址，当对话框中的事件发生时，将调用回调函数。该参数可用为NULL；

	bi.iImage=0;//对话框传递给回调函数的一个参数指针
	bi.lParam=0;//与选中目录相关的图像。该图像将被指定为系统图像列表中的索引值
	pidl=SHBrowseForFolder(&bi);


	if (SHGetPathFromIDList(pidl,szDir))
	{
		strFilePath2=szDir;
		SetDlgItemText(IDC_EDIT_RESULT, strFilePath2);
		//return;
	} 
	//else
	//	{
	//		SHGetPathFromIDList(pidl,szDir);
	//	}
	//	m_strFileName=szDir;
	UpdateData(FALSE);
}


void SetPreferencesDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	char szDir[150];//定义一个字符串用来接收文件名

	BROWSEINFO bi;//建立一个BROWSEIFO对象
	ITEMIDLIST* pidl;//创建一个结构指针
	memset(&bi,0,sizeof BROWSEINFO);
	bi.hwndOwner=m_hWnd;
	bi.hwndOwner=this->m_hWnd;//指定父窗口，在对话框显示期间，父窗口将被禁用
	bi.pszDisplayName=szDir;//pszDisplayNmae：用来保存用户选中的目录字符串的内存地址（缓冲区）
	bi.pidlRoot=NULL;//如果指定为NULL，就以“桌面”为根，pidRoot：ITEMIDLIST结构的地址包含浏览时的初始根目录
	bi.lpszTitle="请选择文件";//这一行将显示在对话框的顶端

	//描述窗口的选项：包括状态文本、显示目录和文件、只返回文件系统目录、返回系统一个节点
	bi.ulFlags=BIF_STATUSTEXT|BIF_BROWSEINCLUDEFILES|BIF_RETURNONLYFSDIRS|BIF_RETURNFSANCESTORS;

	bi.lpfn=NULL;//lpfn：应用程序定义的浏览器对话框回调函数的地址，当对话框中的事件发生时，将调用回调函数。该参数可用为NULL；

	bi.iImage=0;//对话框传递给回调函数的一个参数指针
	bi.lParam=0;//与选中目录相关的图像。该图像将被指定为系统图像列表中的索引值
	pidl=SHBrowseForFolder(&bi);


	if (SHGetPathFromIDList(pidl,szDir))
	{
		strFilePath3=szDir;
		SetDlgItemText(IDC_EDIT_CUTPIC, strFilePath3);
		//return;
	} 
	//else
	//	{
	//		SHGetPathFromIDList(pidl,szDir);
	//	}
	//	m_strFileName=szDir;
	UpdateData(FALSE);
}


void SetPreferencesDlg::OnBnClickedOk()
{

	//ComBox内容写入
	int laneIndex = m_combo_lane.GetCurSel();
	m_combo_lane.GetLBText(laneIndex,laneText);

	int areaIndex=m_combox_area.GetCurSel();	
	m_combox_area.GetLBText(areaIndex,areaText);

	int searchboxIndex =m_combox_searchbox.GetCurSel();
	m_combox_searchbox.GetLBText(searchboxIndex,searchboxText);

	//CStrilBar内容写入
	m_sprpiexledit.GetWindowText(sprpiexlText);
	m_zoomedit.GetWindowText(zoomText);
	m_thresholdedit.GetWindowText(thresholdText);
	m_nocaredit.GetWindowText(nocarpiexlText);
	m_paltethrestholdedit.GetWindowText(platethresholdText);

	//自定义路径内容
	m_platepathedit.GetWindowText(platepathText);
	m_resultpathedit.GetWindowText(resultpathText);
	m_cutpicedit.GetWindowText(cutpicpathText);

	
	ofstream ouf2;
 	ouf2.open("./out.txt",ios::out);
 	ouf2<<laneText<<"\r\n"<<areaText<<"\r\n"<<searchboxText<<"\r\n"<<sprpiexlText<<"\r\n"<<zoomText<<"\r\n"<<thresholdText<<"\r\n"<<nocarpiexlText<<"\r\n"
		<<platethresholdText<<"\r\n"<<platepathText<<"\r\n"<<resultpathText<<"\r\n"<<cutpicpathText;
 	//ouf2<<"\r\n";

	/*WritePrivateProfileSection(_T("windows"),_T("load=c:\\winnt\\notepad.exe"),_T("d://cfc.ini"));
	GetPrivateProfileIntW("system","type",0,"./config.ini");  */
	CDialog::OnOK();
}


void SetPreferencesDlg::OnBnClickedButton5()
{
	// TODO: 将当前配置保存为配置文件
	CFileDialog fileDlg(FALSE);              //构造一个文件另存为对话框
	fileDlg.m_ofn.lpstrTitle="保存";
	fileDlg.m_ofn.lpstrFilter="Text Files(*.txt)\0*.txt\0ALL Files(*.*)\0*.*\0\0";
	fileDlg.m_ofn.lpstrDefExt="txt";		  //定义缺省文件扩展名
	if(IDOK==fileDlg.DoModal())              //打开文件另存为对话框
	{
		////CFile file(fileDlg.GetFileName(),CFile::modeCreate | CFile::modeWrite);           //以写的方式创建新文件
		//CFile file(fileDlg.GetFileName(),CFile::modeCreate | CFile::modeWrite);           //以写的方式创建新

		//ComBox内容写入
		int laneIndex = m_combo_lane.GetCurSel();
		m_combo_lane.GetLBText(laneIndex,laneText);

		int areaIndex=m_combox_area.GetCurSel();	
		m_combox_area.GetLBText(areaIndex,areaText);

		int searchboxIndex =m_combox_searchbox.GetCurSel();
		m_combox_searchbox.GetLBText(searchboxIndex,searchboxText);

		//CStrilBar内容写入
		m_sprpiexledit.GetWindowText(sprpiexlText);
		m_zoomedit.GetWindowText(zoomText);
		m_thresholdedit.GetWindowText(thresholdText);
		m_nocaredit.GetWindowText(nocarpiexlText);
		m_paltethrestholdedit.GetWindowText(platethresholdText);

		//自定义路径内容
		m_platepathedit.GetWindowText(platepathText);
		m_resultpathedit.GetWindowText(resultpathText);
		m_cutpicedit.GetWindowText(cutpicpathText);



		ofstream ouf2;
		string path = fileDlg.GetFolderPath()+"/"+fileDlg.GetFileName();
		ouf2.open(path,ios::out);
		ouf2<<laneText<<"\r\n"<<areaText<<"\r\n"<<searchboxText<<"\r\n"<<sprpiexlText<<"\r\n"<<zoomText<<"\r\n"<<thresholdText<<"\r\n"<<nocarpiexlText<<"\r\n"
			<<platethresholdText<<"\r\n"<<platepathText<<"\r\n"<<resultpathText<<"\r\n"<<cutpicpathText;
		ouf2.close();
		CDialog::OnOK();
	}
	
}


void SetPreferencesDlg::OnBnClickedButton1()
{
	// TODO:
	CFileDialog dlg(
		TRUE,
		_T("*.txt"), 
		NULL,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T(" All Files (*.*) |*.*||"), 
		NULL
		);	
	dlg.m_ofn.lpstrTitle = _T("选取配置文件");	
	if( dlg.DoModal() != IDOK ){
		 	return;
	}		
	pathVideo2 = dlg.GetPathName();
	SetDlgItemText(IDC_EDIT_PLATE2, pathVideo2);	
	ifstream inf(pathVideo2);

	inf>>inlaneText>>inareaText>>insearchboxText>>insprpiexlText>>inzoomText>>inthresholdText>>innocarpiexlText>>inplatethresholdText
		>>inplatepathText>>inresultpathText>>incutpicpathText;

	inlaneInt = atoi(inlaneText.c_str());
	inareaString=inareaText;
	insearchboxString=insearchboxText;
	insprpiexlInt=atoi(insprpiexlText.c_str());
	inzoomFloat=atof(inzoomText.c_str());
	inthresholdInt=atoi(inthresholdText.c_str());
	innocarpiexlInt=atoi(innocarpiexlText.c_str());
	inplatethresholdInt=atoi(inplatethresholdText.c_str());
	inplatepathString=inplatethresholdText;
	inresultpathString=inresultpathText;
	incutpicpathString=incutpicpathText;



	/**************************像素差水平滚动条初始化设置****************************************/
	//设置水平滚动条范围为1-100
	m_horiScrollbar.SetRange(1,100);
	//设置水平滚动条的初始位置为20
	m_horiScrollbar.SetPos(insprpiexlInt);
	//在编辑框中显示20
	SetDlgItemInt(IDC_HSCROLL_EDIT,insprpiexlInt);

	/**************************缩放倍数水平滚动条初始化设置****************************************/
	//设置水平滚动条范围为1-100
	m_horiScrollbar_zoom.SetRange(1,100);
	//设置水平滚动条的初始位置为20
	m_horiScrollbar_zoom.SetPos(inzoomFloat);
	//在编辑框中显示20
	SetDlgItemInt(IDC_HSCROLL_EDIT4,inzoomFloat);

	/**************************二值化阈值滚动条初始化设置****************************************/
	//设置水平滚动条范围为1-100
	m_horiScrollbar_threshold.SetRange(1,100);
	//设置水平滚动条的初始位置为20
	m_horiScrollbar_threshold.SetPos(inthresholdInt);
	//在编辑框中显示20
	SetDlgItemInt(IDC_HSCROLL_EDIT06,inthresholdInt);

	/**************************无车像素值滚动条初始化设置****************************************/
	//设置水平滚动条范围为1-100
	m_horiScrollbar_pixel.SetRange(1,100);
	//设置水平滚动条的初始位置为20
	m_horiScrollbar_pixel.SetPos(innocarpiexlInt);
	//在编辑框中显示20
	SetDlgItemInt(IDC_HSCROLL_EDIT8,innocarpiexlInt);

	/**************************车牌二值化阈值滚动条初始化设置****************************************/
	//设置水平滚动条范围为1-100
	m_horiScrollbar_platethros.SetRange(1,100);
	//设置水平滚动条的初始位置为20
	m_horiScrollbar_platethros.SetPos(inplatethresholdInt);
	//在编辑框中显示20
	SetDlgItemInt(IDC_HSCROLL_EDIT7,inplatethresholdInt);

	/******************************车道选择COMBOX初始化设置***************************************/


	m_combo_lane.InsertString(0,"1");
	m_combo_lane.InsertString(1,"2");
	m_combo_lane.InsertString(2,"3");
	m_combo_lane.InsertString(3,"4");

	LPCSTR inlaneTextLpc = inlaneText.c_str();
	int inlaneIndex = m_combo_lane.FindString(-1,inlaneTextLpc);
	m_combo_lane.SetCurSel(inlaneIndex);
	//m_combo_lane.Text

	/******************************地区选择ComBox初始化设置***************************************/
	m_combox_area.InsertString(0,"津");
	m_combox_area.InsertString(1,"京");
	m_combox_area.InsertString(2,"沪");
	m_combox_area.InsertString(3,"冀");
	m_combox_area.InsertString(4,"黑");
	m_combox_area.InsertString(5,"辽");
	m_combox_area.InsertString(6,"苏");
	m_combox_area.InsertString(7,"贵");

	LPCSTR inareaTextLpc = inareaText.c_str();
	int inareaIndex = m_combox_area.FindString(-1,inareaTextLpc);
	m_combox_area.SetCurSel(inareaIndex);

	/******************************粗定位搜索框大小COMBOX初始化设置***************************************/
	m_combox_searchbox.InsertString(0,"40*10");
	m_combox_searchbox.InsertString(1,"60*15");
	m_combox_searchbox.InsertString(2,"80*20");
	m_combox_searchbox.InsertString(3,"100*25");
	m_combox_searchbox.InsertString(4,"120*30");
	m_combox_searchbox.InsertString(5,"140*35");

	LPCSTR insearchboxTextLpc = insearchboxText.c_str();
	int insearchboxIndex = m_combox_searchbox.FindString(-1,insearchboxTextLpc);
	m_combox_searchbox.SetCurSel(insearchboxIndex);
}
