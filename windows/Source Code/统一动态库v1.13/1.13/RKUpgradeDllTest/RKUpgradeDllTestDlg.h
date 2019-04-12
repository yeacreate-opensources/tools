// RKUpgradeDllTestDlg.h : header file
//

#if !defined(AFX_RKUPGRADEDLLTESTDLG_H__D435CB80_C135_4550_99E5_F0E0D9559DE9__INCLUDED_)
#define AFX_RKUPGRADEDLLTESTDLG_H__D435CB80_C135_4550_99E5_F0E0D9559DE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "UpgradeLibrary.h"
/////////////////////////////////////////////////////////////////////////////
// CRKUpgradeDllTestDlg dialog

class CRKUpgradeDllTestDlg : public CDialog
{
// Construction
	
public:
	CString m_modulePath;
	CString m_LogPath;
  	~CRKUpgradeDllTestDlg();
	CRKUpgradeDllTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRKUpgradeDllTestDlg)
	enum { IDD = IDD_RKUPGRADEDLLTEST_DIALOG };
	CListBox	m_infoListBox;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRKUpgradeDllTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRKUpgradeDllTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpenFw();
	afx_msg void OnBnClickedButtonOpenLoader();
private:
	CUpgradeLibrary *m_dllObject;
	BOOL m_bRun;
	CString m_strFw;
	CString m_strLoader;
	VOID EnableCtrl();
	VOID DisableCtrl();
	BOOL DownloadImage(TCHAR *szDevPath,TCHAR *szImage,unsigned int uiOffset=0);
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonUpgrade();
	BOOL RunProc();
	BOOL WriteMacProc();
	afx_msg void OnBnClickedButtonWriteMac();
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RKUPGRADEDLLTESTDLG_H__D435CB80_C135_4550_99E5_F0E0D9559DE9__INCLUDED_)
