// DllToolSampleDlg.h : header file
//

#if !defined(AFX_DLLTOOLSAMPLEDLG_H__68A2F7E9_EE28_47FA_BFA7_C3AA28AEC1BE__INCLUDED_)
#define AFX_DLLTOOLSAMPLEDLG_H__68A2F7E9_EE28_47FA_BFA7_C3AA28AEC1BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Label.h"
#include "cmLog.h"
#include "cmIniFile.h"
#include "cmMultiLanguage.h"
#include "cmPath.h"
#include "cmFile.h"
#include "cmNumString.h"
#include "cmCommonDlg.h"
#include "cmStrCode.h"
using namespace cm;

typedef enum{EM_ACTION_NONE=0,
EM_ACTION_READ_SN,
EM_ACTION_WRITE_SN,
EM_ACTION_READ_MAC,
EM_ACTION_WRITE_MAC,
EM_ACTION_READ_BT,
EM_ACTION_WRITE_BT,
EM_ACTION_READ_IMEI,
EM_ACTION_WRITE_IMEI,
EM_ACTION_READ_ALL,
EM_ACTION_WRITE_ALL,
EM_ACTION_READ_CUSTOM,
EM_ACTION_WRITE_CUSTOM
}EM_ACTION;
/////////////////////////////////////////////////////////////////////////////
// CDllToolSampleDlg dialog

class CDllToolSampleDlg : public CDialog
{
public:
	void ScanDeviceProc();
	BOOL WriteDataProc();
// Construction
public:
	CDllToolSampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDllToolSampleDlg)
	enum { IDD = IDD_DLLTOOLSAMPLE_DIALOG };
	CLabel	m_labelDevice;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDllToolSampleDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDllToolSampleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnBtnSnWrite();
	afx_msg void OnBtnImeiWrite();
	afx_msg void OnBtnMacWrite();
	afx_msg void OnBtnBtWrite();
	afx_msg void OnBtnCustomBrowse();
	afx_msg void OnBtnCustomWrite();
	afx_msg void OnBtnAllWrite();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_strModulePath;
	CString m_strLogPath;
	
	CString m_LangPath;
	cmIniFile m_iniConfig;
	cmMultiLanguage *m_pLangObject;
	LANG_PROP_VECTOR m_langProp;
	CHAR m_curLang;
	int  m_emDeviceType;
	BOOL m_bSupportFullUsb;
	BOOL m_bLogOn;
	int  m_mscVid;
	int  m_mscPid;
	int  m_mscTimeout;
	int  m_rockusbTimeout;
	BOOL m_bUpgradeDllInitOK;
	BOOL m_bTerminated;
	UINT m_nDeviceCount;
	BOOL m_bExistMsc;
	BOOL m_bExistAdb;

	CWinThread *m_pScanThreadObject;
	CEvent *m_pScanEventObject;
	CCriticalSection m_csScanLock;
	EM_ACTION m_curAction;
	CFont m_font;
	BYTE m_sn[60];
	BYTE m_mac[6];
	BYTE m_bt[6];
	BYTE m_imei[15];
	BYTE m_custom[512];
	UINT m_uiCustomOffset;
	UINT m_uiCustomLen;
	STRUCT_SEC3_DATA m_all;
	BOOL LoadConfig();
	CString GetLocalString(CString strKey);
	VOID DisableCtrl();
	VOID EnableCtrl();
	BOOL HexStrToBytes(CString strHex,PBYTE pBuf,int &nBufLen);
	BOOL GetSnData();
	BOOL GetMacData();
	BOOL GetBtData();
	BOOL GetImeiData();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLLTOOLSAMPLEDLG_H__68A2F7E9_EE28_47FA_BFA7_C3AA28AEC1BE__INCLUDED_)
