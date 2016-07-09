#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "resource.h"


// SetPreferencesDlg 对话框

class SetPreferencesDlg  : public CDialog
{
	DECLARE_DYNAMIC(SetPreferencesDlg)

public:
	SetPreferencesDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SetPreferencesDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void On32781();
	afx_msg void On32771();
	// 选项卡
	void InitText();
	CTabCtrl m_tabcontrol;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeCombo4();
	virtual BOOL OnInitDialog();
	CSliderCtrl m_horiScrollbar;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl m_horiScrollbar_zoom;
	afx_msg void OnNMCustomdrawSlider4(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_horiScrollbar_threshold;
	CSliderCtrl m_horiScrollbar_pixel;
	CSliderCtrl m_horiScrollbar_platethros;
	CComboBox m_combo_lane;
	afx_msg void OnCbnSelchangeComboLane();
	CComboBox m_combox_area;
	afx_msg void OnCbnSelchangeComboArea();
	CComboBox m_combox_searchbox;
	afx_msg void OnCbnSelchangeComboSearchbox();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedOk();
	CEdit m_sprpiexledit;
	CEdit m_zoomedit;
	CEdit m_thresholdedit;
	CEdit m_nocaredit;
	CEdit m_paltethrestholdedit;
	CEdit m_platepathedit;
	CEdit m_resultpathedit;
	CEdit m_cutpicedit;
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton1();
};
