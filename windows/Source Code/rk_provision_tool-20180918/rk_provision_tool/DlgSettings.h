#pragma once
#include ".\settings\SettingBase.h"
#include "afxcmn.h"
#include "DlgItemCmn.h"
#include "afxwin.h"
// CDlgSettings dialog

class CDlgSettings : public CDialog
{
	DECLARE_DYNAMIC(CDlgSettings)

public:
	CDlgSettings(CIniSettingBase &Config, CIniLocalLan &LocalLang, CWnd* pParent = NULL);
	virtual ~CDlgSettings();

// Dialog Data
	enum { IDD = IDD_DLG_SETTINGS };

public:
	BOOL OnUpdateTab(int iCurSel);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CDlgItemCmn m_DlgItemCmn;
	CIniSettingBase &m_Configs;
	CIniLocalLan &m_LocalLan;
	CTabCtrl m_TabItem;
	void OnUpdateCtrl();
	void OnSaveCtrl();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnTcnSelchangingTabItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangeTabItem(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_btnForceMaskrom;
	CEdit m_CeditLoader;
	afx_msg void OnBnClickedCheckForceMaskrom();
	afx_msg void OnBnClickedCheckCompat();
	afx_msg void OnBnClickedCheckReboot();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheckLock();
};
